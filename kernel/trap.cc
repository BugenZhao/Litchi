//
// Created by Bugen Zhao on 7/13/20.
//

#include <include/stdio.hpp>
#include <include/x86.h>
#include "trap.hh"
#include "monitor.hpp"
#include "task.hh"
#include "ksyscall.hh"

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

        // allow some traps be called by user => dpl = 3
        for (auto no: {(int) Type::syscall,
                       (int) Type::breakpoint,
                       (int) Type::debug}) {
            SETGATE(idt[no], 1, GD_KT, ivt[no], 3);
        }

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

            if (tf->trapType != Type::syscall)
                print("[%08x] Back to kernel: Trap [%s]\n", Task::current->id, describe(tf->trapType));
        } else {
            print("Kernel Trap [%s]\n", describe(tf->trapType));
        }

        // dispatch to trap handlers
        tf->dispatch();

        // run again
        assert(Task::current);
        Task::current->run(tf->trapType != Type::syscall);
    }
}

namespace trap {
    namespace handler {
        void pageFault(Frame *tf);

        uint32_t syscall(Frame *tf);

        void debug(Frame *tf);
    }

    void Frame::dispatch() {
        switch (trapType) {
            case Type::pageFault:
                handler::pageFault(this);
                break;
            case Type::syscall:
                this->regs.eax = handler::syscall(this);
                break;

            case Type::debug:
            case Type::breakpoint:
                handler::debug(this);
                break;

            case Type::divide:
            case Type::nmi:
            case Type::overflow:
            case Type::bound:
            case Type::invalidOp:
            case Type::device:
            case Type::doubleFault:
            case Type::coprocessor:
            case Type::invalidTss:
            case Type::segmentNP:
            case Type::stack:
            case Type::gpFault:
            case Type::fpError:
            case Type::alignment:
            case Type::machineCheck:
            case Type::simdError:
            default:
                if ((this->cs & 0b11) == 0) {
                    // Unhandled trap from kernel => BUG
                    kernelPanic("Unhandled trap [%s] from kernel\n", describe(trapType));
                } else {
                    // Unhandled trap from user
                    print("[%08x] Unhandled trap [%s]\n", task::Task::current->id, describe(trapType));
                    monitor::main(&task::Task::current->trapFrame);     // Break
                    // task::Task::current->destroy(true);              // or destroy
                }
        }
    }
}

namespace trap::handler {
    void pageFault(Frame *tf) {
        // get fault virtual address
        auto faultVa = x86::rcr2();

        if ((tf->cs & 0b11) == 0) {
            kernelPanic("Unhandled page fault (%p) from kernel\n", faultVa);
        } else {
            print("[%08x] Unhandled page fault (at %p) [%s, %s, %s]\n",
                  task::Task::current->id, faultVa,
                  tf->err & 4 ? "user" : "kernel",
                  tf->err & 2 ? "write" : "read",
                  tf->err & 1 ? "protection" : "not-present");
            task::Task::current->destroy(true); // will trap into monitor
        }
    }

    uint32_t syscall(Frame *tf) {
        return ksyscall::main(static_cast<ksyscall::Num>(tf->regs.eax),
                              tf->regs.edx, tf->regs.ecx, tf->regs.ebx, tf->regs.edi, tf->regs.esi);
    }

    void debug(Frame *tf) {
        print("eip = %p\n", tf->eip);
        monitor::main(tf);
    }
}
