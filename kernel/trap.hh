//
// Created by Bugen Zhao on 7/14/20.
//

#ifndef LITCHI_KERNEL_TRAP_HH
#define LITCHI_KERNEL_TRAP_HH

#include <include/trap.hh>

namespace trap {
    struct Registers {
        /* registers as pushed by pusha */
        uint32_t edi;
        uint32_t esi;
        uint32_t ebp;
        uint32_t oesp;          /* Useless */
        uint32_t ebx;
        uint32_t edx;
        uint32_t ecx;
        uint32_t eax;
    } __attribute__((packed));

    struct Frame {
        /* manually saved */
        struct Registers regs;  /* pusha */
        uint16_t es;            /* movL */
        uint16_t padding1;
        uint16_t ds;            /* movL */
        uint16_t padding2;
        Type trapType;    /* push imm */

        /* auto saved by x86 hardware */
        uint32_t err;
        uintptr_t eip;          /* iret */
        uint16_t cs;
        uint16_t padding3;
        uint32_t eflags;

        /* below here only when crossing rings, such as from user to kernel */
        uintptr_t esp;
        uint16_t ss;
        uint16_t padding4;

        inline void clear() {
            mem::clear(this, sizeof(*this));
        }

        // pop the trap frame and goto user mode
        [[noreturn]] inline void pop() {
            asm volatile (
            "movl %0, %%esp\n"      // move tf to stack
            "popal\n"
            "popl %%es\n"           // must be LONG
            "popl %%ds\n"
            "addl $0x8, %%esp\n"    // skip trap number and err
            "iret\n"                // IRET!!!
            :
            :"g"(this)              // put this to any General reg
            :"memory"               // do not reorder
            );

            kernelPanic("failed to pop and iret trap frame %08x", this);
        }

        void dispatch();
    } __attribute__((packed));

    static inline constexpr const char *describe(Type type) {
        switch (type) {
            case Type::divide:
                return "Divide Error";
            case Type::debug:
                return "Debug";
            case Type::nmi:
                return "Non-Maskable Interrupt";
            case Type::breakpoint:
                return "Breakpoint";
            case Type::overflow:
                return "Overflow";
            case Type::bound:
                return "BOUND Range Exceed";
            case Type::invalidOp:
                return "Invalid Opcode";
            case Type::device:
                return "Device Not Available";
            case Type::doubleFault:
                return "Double Fault";
            case Type::coprocessor:
                return "Coprocessor Segment";
            case Type::invalidTss:
                return "Invalid TSS";
            case Type::segmentNP:
                return "Segment Not Present";
            case Type::stack:
                return "Stack Fault";
            case Type::gpFault:
                return "General Protection";
            case Type::pageFault:
                return "Page Fault";
            case Type::fpError:
                return "x87 FPU Floating-Point Error";
            case Type::alignment:
                return "Alignment Check";
            case Type::machineCheck:
                return "Machine Check";
            case Type::simdError:
                return "SIMD Floating-Point";
            case Type::syscall:
                return "Litchi Syscall";
        }
        return "Unknown Trap";
    }
}

namespace trap {
    void init();

    void initPerCpu();

    extern "C" { [[noreturn]] void trap(Frame *tf); }
}

#endif //LITCHI_KERNEL_TRAP_HH
