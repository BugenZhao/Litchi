//
// Created by Bugen Zhao on 7/13/20.
//

#include "vmem.hpp"
#include <include/stdio.hpp>
#include <include/string.hpp>
#include "task.hh"

extern PseudoDesc pseudoDesc; // kernel/gdt.c

namespace task {
    Task *Task::array = nullptr;
    Task *Task::freeList = nullptr;

    // alloc task array, called by vmem::init()
    void alloc() {
        using namespace vmem;
        Task::array = bootAllocCount<Task>(Task::maxCount);
        mem::clear(Task::array, sizeof(Task) * Task::maxCount);

        // map the task array to read-only user space
        pgdir::staticMap(kernelPageDir,
                         UENVS,
                         ROUNDUP(sizeof(Task) * Task::maxCount, PGSIZE),
                         PHY_ADDR(Task::array),
                         PTE_U);
    }

    void init() {
        console::out::print("Initializing tasks...");
        // vmem part should have allocated task array
        assert(Task::array != nullptr);

        // construct free list
        for (int i = Task::maxCount - 1; i >= 0; i--) {
            Task::array[i].nextFree = Task::freeList;
            Task::freeList = Task::array + i;
        }

        initPerCpu();
        console::out::print("%<Done\n", WHITE);
    }

    void initPerCpu() {
        // now switch gdt from bootstrap gdt to new one, for use of privilege level
        // see kernel/gdt.c
        x86::lgdt(&pseudoDesc);

        // set segment register (selector) to the correct descriptor

        // the kernel never uses GS or FS, so we leave those set to the user data segment.
        // LS 2 bit: Requested Privilege Level
        asm volatile("movw %%ax,%%gs" : : "a" (GD_UD|3));
        asm volatile("movw %%ax,%%fs" : : "a" (GD_UD|3));
        // the kernel does use ES, DS, and SS.
        asm volatile("movw %%ax,%%es" : : "a" (GD_KD));
        asm volatile("movw %%ax,%%ds" : : "a" (GD_KD));
        asm volatile("movw %%ax,%%ss" : : "a" (GD_KD));

        // perform a long jump to set new CS selector
        asm volatile("ljmp %0,$1f\n 1:\n" : : "i" (GD_KT));
    }
}