//
// Created by Bugen Zhao on 7/13/20.
//

#include <include/trap.hh>
#include <include/stdio.hpp>
#include <include/x86.h>
#include "monitor.hpp"
#include "task.hh"

using namespace console::out;

// see irqentry.S
extern uint32_t ivt[];
// see gdt.c
extern struct SegDesc gdt[];

namespace trap {
    // Interrupt DESCRIPTOR Table <= generate from ivt
    GateDesc idt[256] = {{0}};
    // for load idt
    PseudoDesc idtPD = {
            sizeof(idt) - 1, (uint32_t) idt
    };

    // Task State, if trapped, CPU will automatically load ss, esp from it
    // we need to set them to a new, real "kernel stack" (instead of current bootstrap stack),
    // then describe it in the last segment descriptor(GD_TSS0) in GDT,
    // finally, load the descriptor number to "Task Register"
    TaskState kernelTS;

    void init() {
        print("Initializing traps...");

        for (int i = 0; i < 256; ++i) {
            // gate  : interrupt descriptor
            // istrap: trap or fault (different behaviors on restart), see IA32 manual p5-6 [ignored]
            // sel   : GD_KT,  irq's cs
            // offset: ivt[i], irq's ip
            // dpl   : privilege, all traps but our system call should not be directly called by user
            SETGATE(idt[i], 0, GD_KT, ivt[i], 0);
        }

        // allow syscall trap be called by user => dpl = 3
        auto no = (int) TrapType::syscall;
        SETGATE(idt[no], 1, GD_KT, ivt[no], 3);

        // set and load tss, ivt per CPU
        initPerCpu();

        print("%<Done\n", WHITE);
    }

    void initPerCpu() {
        // see comments around the definition of kernelTS
        // Note: for time-slicing multitasking, the stack(esp0) should be switched every context switching
        kernelTS.ts_ss0 = GD_KD;
        kernelTS.ts_esp0 = KSTACKTOP;
        kernelTS.ts_iomb = sizeof(TaskState);   // just ignored, http://forum.osdev.org/viewtopic.php?t=13678

        // specifying the TSS' address as "base",
        // TSS' size as "limit",
        // 0x89 (Present|Executable|Accessed) as "access byte"
        // and 0x40 (Size-bit) as "flags". In the TSS itself
        // https://wiki.osdev.org/TSS
        gdt[GD_TSS0 >> 3] = SEG16(STS_T32A, (uint32_t) &kernelTS, sizeof(TaskState) - 1, 0);
        gdt[GD_TSS0 >> 3].sd_s = 0;

        // load
        x86::ltr(GD_TSS0);
        x86::lidt(&idtPD);
    }

    void trap(Frame *tf) {
        console::out::print("[%08x] Back to kernel: Trap %d\n", task::Task::current->id, tf->trapNumber);
        monitor::main(tf);
        kernelPanic("");
    }
}