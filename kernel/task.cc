//
// Created by Bugen Zhao on 7/13/20.
//

#include "vmem.hpp"
#include <include/stdio.hpp>
#include <include/string.hpp>
#include <include/random.hh>
#include "task.hh"
#include <include/elf.h>
#include "monitor.hpp"

using namespace console::out;

extern PseudoDesc gdtPD; // kernel/gdt.c

namespace task {
    Task *Task::array = nullptr;
    Task *Task::freeList = nullptr;
    Task *Task::current = nullptr;

    // alloc task array, called by vmem::init()
    void allocArray() {
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
        print("Initializing tasks...");
        // vmem part should have allocated task array
        assert(Task::array != nullptr);

        // construct free list
        for (int i = Task::maxCount - 1; i >= 0; i--) {
            Task::array[i].nextFree = Task::freeList;
            Task::freeList = Task::array + i;
        }

        initPerCpu();
        print("%<Done\n", WHITE);
    }

    void initPerCpu() {
        // now switch gdt from bootstrap gdt to new one, for use of privilege level
        // see kernel/gdt.c
        x86::lgdt(&gdtPD);

        // set segment register (selector) to the correct descriptor

        // the kernel never uses GS or FS, so we leave those set to the user data segment.
        // LS 2 bit: Requested Privilege Level
        asm volatile("movw %%ax,%%gs" : : "a" (GD_UD | 3));
        asm volatile("movw %%ax,%%fs" : : "a" (GD_UD | 3));
        // the kernel does use ES, DS, and SS.
        asm volatile("movw %%ax,%%es" : : "a" (GD_KD));
        asm volatile("movw %%ax,%%ds" : : "a" (GD_KD));
        asm volatile("movw %%ax,%%ss" : : "a" (GD_KD));

        // perform a long jump to set new CS selector
        asm volatile("ljmp %0,$1f\n 1:\n" : : "i" (GD_KT));
    }
}

namespace task {
    std::tuple<Task *, Result> Task::alloc(taskid_t parentId) {
        // find a free slot
        Task *task = freeList;
        if (task == nullptr)
            return {nullptr, Result::noFreeTask};
        freeList = task->nextFree;

        // allocate page dir
        if (Result r; (r = task->setupMemory()) != Result::ok)
            return {nullptr, r};

        // allocate a random id, ending up with the #
        uint32_t rand = random::rand() << 16 | random::rand();
        task->id = (rand & ~(maxCount - 1)) | (task - array);

        // set other fields
        task->parentId = parentId;
        task->status = TaskStatus::ready;
        task->type = TaskType::user;

        // clear the trap frame
        task->trapFrame.clear();
        // set segment descriptors to users', 3: RPL, see: initPerCpu()
        task->trapFrame.ds = GD_UD | 3;
        task->trapFrame.es = GD_UD | 3;
        task->trapFrame.cs = GD_UT | 3;
        task->trapFrame.ss = GD_UD | 3;
        // set user stack
        task->trapFrame.esp = USTACKTOP; // user stack top

        // eip will be set after binary loaded

        print("[%08x] Allocated task\n", task->id);
        return {task, Result::ok};
    }

    Result Task::setupMemory() {
        // allocate a page for the task's page dir
        auto page = vmem::PageInfo::alloc(true);
        if (page == nullptr) return Result::noMemory;

        // simply duplicate the kernel's
        this->pageDir = static_cast<pde_t *>(page->toKernV());
        mem::copy(page->toKernV(), vmem::kernelPageDir, PGSIZE);
        page->refCount += 1;

        // again, make a recursive map to the page dir itself
        this->pageDir[PDX(UVPT)] = PHY_ADDR(this->pageDir) | PTE_U | PTE_P;

        return Result::ok;
    }

    void Task::free() {
        if (this == current) x86::lcr3(PHY_ADDR(vmem::kernelPageDir));

        // free all pages, from 0x0 to UTOP
        for (int pdx = 0; pdx < (int) PDX(UTOP); ++pdx) {
            auto pde = this->pageDir[pdx];  // find the page dir entry
            if (!(pde & PTE_P)) continue;   // page table not present

            auto pa = PTE_ADDR(pde);
            auto pageTable = static_cast<pte_t *>(KERN_ADDR(pa));
            for (int ptx = 0; ptx < NPTENTRIES; ++ptx) {
                auto pte = pageTable[ptx];  // find the page table entry
                if (pte & PTE_P) // remove the page from page dir, and free the info if needed
                    vmem::pgdir::remove(this->pageDir, PGADDR(pdx, ptx, 0));
            }

            // free the page table itself
            pageDir[pdx] = 0;
            vmem::PageInfo::fromPhy(pa)->decRef();
        }

        // free the page dir itself
        vmem::PageInfo::fromPhy(PHY_ADDR(pageDir))->decRef();

        // free the Task
        status = TaskStatus::free;
        nextFree = freeList;
        freeList = this;

        print("[%08x] Freed task\n", this->id);
    }

    void Task::loadElf(uint8_t *binary) {
        auto elf = (Elf *) binary;
        if (elf->e_magic != ELF_MAGIC)
            kernelPanic("bad elf binary");

        x86::lcr3(PHY_ADDR(this->pageDir));

        // copy the binary
        auto ph = (Proghdr *) (binary + elf->e_phoff);  // find the elf header
        auto eph = ph + elf->e_phnum;                   // end of the header
        for (; ph < eph; ++ph) {
            if (ph->p_type != ELF_PROG_LOAD || ph->p_memsz == 0) continue;  // not to load

            // allocate pages
            this->regionAlloc((void *) ph->p_va, ph->p_memsz);
            // copy text/data, only copy filesz length
            auto va = binary + ph->p_offset;
//            print("VA %08x\n", va);
            mem::copy((void *) ph->p_va, va, ph->p_filesz);
        }

        x86::lcr3(PHY_ADDR(vmem::kernelPageDir));

        // allocate stack (PGSIZE)
        this->regionAlloc((void *) (USTACKTOP - PGSIZE), PGSIZE);
        // set eip to elf entry point
        this->trapFrame.eip = elf->e_entry;

        print("[%08x] Loaded elf\n", this->id);
    }

    // allocate zero-ed pages from _va to _va + len, into this->pageDir
    void Task::regionAlloc(void *_va, size_t len) {
        char *va = static_cast<char *>(_va);
        auto vaLo = ROUNDDOWN(va, PGSIZE);
        auto vaHi = ROUNDUP(va + len, PGSIZE);
        if (vaLo >= vaHi) kernelPanic("bad va");

        // allocate pages
        for (auto vaIt = vaLo; vaIt < vaHi; vaIt += PGSIZE) {
            auto page = vmem::PageInfo::alloc(true);
            if (page == nullptr) kernelPanic("out of memory");
//            print("PA %08x\n", page->toPhy());
            auto result = vmem::pgdir::insert(this->pageDir, page, va, PTE_U | PTE_W);
            if (result != Result::ok) kernelPanic("insert page failed");
        }
    }

    std::tuple<Task *, Result> Task::create(uint8_t *binary, TaskType type) {
        // allocate a free task
        auto[task, r] = alloc(0);
        if (r != Result::ok) return {nullptr, r};
        // set up vm
        r = task->setupMemory();
        if (r != Result::ok) return {nullptr, r};
        // load elf
        task->loadElf(binary);

        print("[%08x] Created task\n", task->id);
        return {task, Result::ok};
    }

    void Task::run() {
        if (this != current) {
            // context switching
            if (current != nullptr && current->status == TaskStatus::running)
                current->status = TaskStatus::ready;
            this->status = TaskStatus::running;
            current = this;

            x86::lcr3(PHY_ADDR(this->pageDir));
        }

        print("[%08x] Goodbye, kernel!\n", id);
        this->trapFrame.pop();
    }

    void Task::destroy(bool fromKernel) {
        this->free();
        print("%<[%08x] Destroyed by %s\n", WHITE, id, fromKernel ? "kernel" : "user");
        monitor::main();
    }
}