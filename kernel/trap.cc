//
// Created by Bugen Zhao on 7/13/20.
//

#include <include/stdio.hpp>
#include <include/x86.h>
#include "trap.hh"
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
}

namespace trap {
    [[noreturn]] void trap(Frame *tf) {
        using namespace task;

        // interrupts should be disabled
        assert(!(x86::read_eflags() & FL_IF));

        if ((tf->cs & 0b11) == 3) { // trapped from user mode, must be Task::current
            Task::current->trapFrame = *tf;   // update tf
            tf = &Task::current->trapFrame;   // use the one on kernel stack to avoid problems
            print("[%08x] Back to kernel: Trap %u\n", Task::current->id, tf->trapType);
        } else {
            print("Kernel Trap %u\n", tf->trapType);
        }

        // dispatch to trap handlers
        tf->dispatch();

        // run again
        assert(Task::current);
        Task::current->run();
    }
}

namespace trap {
    namespace handler {
        void pageFault(Frame *tf);

        uint32_t syscall(Frame *tf);
    }

    void Frame::dispatch() {
        switch (trapType) {
            case TrapType::pageFault:
                handler::pageFault(this);
                break;
            case TrapType::syscall:
                handler::syscall(this);
                break;

            case TrapType::divide:
            case TrapType::debug:
            case TrapType::nmi:
            case TrapType::breakpoint:
            case TrapType::overflow:
            case TrapType::bound:
            case TrapType::illegalOp:
            case TrapType::device:
            case TrapType::doubleFault:
            case TrapType::coprocessor:
            case TrapType::invalidTss:
            case TrapType::segmentNP:
            case TrapType::stack:
            case TrapType::gpFault:
            case TrapType::fpError:
            case TrapType::alignment:
            case TrapType::machineCheck:
            case TrapType::simdError:
            default:
                if ((this->cs & 0b11) == 0) {
                    kernelPanic("Unhandled trap %u from kernel\n", trapType);
                } else {
                    print("[%08x] Unhandled trap %u\n", task::Task::current->id, trapType);
                    task::Task::current->destroy(true); // will trap into monitor
                }
        }
    }
}

namespace trap::handler {
    void pageFault(Frame *tf) {
        // get fault virtual address
        auto faultVa = x86::rcr2();

        if ((tf->cs & 0b11) == 0) {
            kernelPanic("Unhandled page fault (%08x) from kernel\n", faultVa);
        } else {
            print("[%08x] Unhandled page fault (%08x)\n", task::Task::current->id, faultVa);
            task::Task::current->destroy(true); // will trap into monitor
        }
    }

    uint32_t syscall(Frame *tf) {
//        kernelPanic("[%08x] Unhandled syscall \n", task::Task::current->id);
        monitor::main(tf);
        return 0;
    }
}