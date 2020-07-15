with open("../kernel/irqentry.S", "w") as fp:
    o = """#include <include/memlayout.h>
# IRQ general handler, call trap()

#define PUSHA               \\
    subq $120,%rsp;         \\
    movq %rax,112(%rsp);    \\
    movq %rbx,104(%rsp);    \\
    movq %rcx,96(%rsp);     \\
    movq %rdx,88(%rsp);     \\
    movq %rbp,80(%rsp);     \\
    movq %rdi,72(%rsp);     \\
    movq %rsi,64(%rsp);     \\
    movq %r8,56(%rsp);      \\
    movq %r9,48(%rsp);      \\
    movq %r10,40(%rsp);     \\
    movq %r11,32(%rsp);     \\
    movq %r12,24(%rsp);     \\
    movq %r13,16(%rsp);     \\
    movq %r14,8(%rsp);      \\
    movq %r15,0(%rsp);

.globl irq_handler_common
irq_handler_common:
    subq $24, %rsp
    movq %rax, 0(%rsp)  # save %rax first
    xorq %rax, %rax
    movw %ds, %ax
    movq %rax, 16(%rsp) # save tf, see trap::Frame
    movw %es, %ax
    movq %rax, 8(%rsp)
    popq %rax           # restore %rax
    PUSHA
    xorq %rax, %rax
    movw $GD_KD, %ax    # set kernel data segment
    movw %ax, %ds
    movw %ax, %es
    movq %rsp, %rdi     # Frame => the first arg
    call trap           # will not return
    """

    o += """

# IRQ handlers

# irq_3:
#    [pushq $0]         # CPU may push a error code, push this 0 (or not) to keep the same format
#     pushq $3          # IRQ number, for common handler use
#     jmp irq_handler_common

"""

    for i in range(256):
        o += f".globl irq_{i}\nirq_{i}:\n"
        if i not in [8, 10, 11, 12, 13, 14, 17]:
            o += f"    pushq $0\n"
        o += f"    pushq ${i}\n"
        o += "    jmp irq_handler_common\n\n"

    o += """
# Interrupt Vector Table

.data
.globl ivt
ivt:
"""

    for i in range(256):
        o += f"    .quad irq_{i}\n"

    fp.write(o)

