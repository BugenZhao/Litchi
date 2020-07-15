//
// Created by Bugen Zhao on 7/14/20.
//

#ifndef LITCHI_KERNEL_TRAP_HH
#define LITCHI_KERNEL_TRAP_HH

#include <include/trap.hh>

namespace trap {
    struct Registers {
        /* registers as pushed by pusha */
        uint64_t r15;
        uint64_t r14;
        uint64_t r13;
        uint64_t r12;
        uint64_t r11;
        uint64_t r10;
        uint64_t r9;
        uint64_t r8;
        uint64_t rsi;
        uint64_t rdi;
        uint64_t rbp;
        uint64_t rdx;
        uint64_t rcx;
        uint64_t rbx;
        uint64_t rax;
    } __attribute__((packed));

    struct Frame {
        /* manually saved */
        struct Registers regs;  /* pusha */
        uint16_t es;            /* movL */
        uint16_t padding11;
        uint32_t padding12;
        uint16_t ds;            /* movL */
        uint16_t padding21;
        uint32_t padding22;
        Type trapType;          /* push imm */

        /* auto saved by x86 hardware */
        uint64_t err;
        uintptr_t rip;          /* iret */
        uint16_t cs;
        uint16_t padding31;
        uint32_t padding32;
        uint64_t rflags;

        /* below here only when crossing rings, such as from user to kernel */
        uintptr_t rsp;
        uint16_t ss;
        uint16_t padding41;
        uint32_t padding42;

        inline void clear() {
            mem::clear(this, sizeof(*this));
        }

        // pop the trap frame and goto user mode
        [[noreturn]] inline void pop() {
            asm volatile (
            "movq %0, %%rsp\n"      // move stack pointer to tf
            "movq 0(%%rsp), %%r15\n"
            "movq 8(%%rsp), %%r14\n"
            "movq 16(%%rsp), %%r13\n"
            "movq 24(%%rsp), %%r12\n"
            "movq 32(%%rsp), %%r11\n"
            "movq 40(%%rsp), %%r10\n"
            "movq 48(%%rsp), %%r9\n"
            "movq 56(%%rsp), %%r8\n"
            "movq 64(%%rsp), %%rsi\n"
            "movq 72(%%rsp), %%rdi\n"
            "movq 80(%%rsp), %%rbp\n"
            "movq 88(%%rsp), %%rdx\n"
            "movq 96(%%rsp), %%rcx\n"
            "movq 104(%%rsp), %%rbx\n"
            "movq 112(%%rsp), %%rax\n"
            "addq $120,%%rsp\n"
            "movw (%%rsp), %%es\n"  // 8 byte
            "movw 8(%%rsp), %%ds\n"
            "addq $16, %%rsp\n"
            "addq $16, %%rsp\n"     // skip trap number and err
            "iretq\n"               // IRET!!!
            :
            :"g"(this)              // put this to any General reg
            :"memory"               // do not reorder
            );

            kernelPanic("failed to pop and iret trap frame %p", this);
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
