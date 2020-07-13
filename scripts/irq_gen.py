# .globl irq_handler_common
#
# .globl irq_3
# irq_3:
#     [pushl $0]
#     pushl $3
#     jmp irq_handler_common
#

with open("../kernel/irqentry.S", "w") as fp:
    o = """#include <include/memlayout.h>
# IRQ general handler, call trap()

.globl irq_handler_common
irq_handler_common:
    pushl %ds           # save tf, see trap::Frame
    pushl %es
    pushal
    movw $GD_KD, %ax    # set kernel data segment
    movw %ax, %ds
    movw %ax, %es
    pushl %esp          # above esp => the trap frame
    call trap           # will not return
    """

    o += """

# IRQ handlers

# irq_3:
#    [pushl $0]         # CPU may push a error code, push this 0 (or not) to keep the same format
#     pushl $3          # IRQ number, for common handler use
#     jmp irq_handler_common

"""

    for i in range(256):
        o += f".globl irq_{i}\nirq_{i}:\n"
        if i not in [8, 10, 11, 12, 13, 14, 17]:
            o += f"    pushl $0\n"
        o += f"    pushl ${i}\n"
        o += "    jmp irq_handler_common\n\n"

    o += """
# Interrupt Vector Table

.data
.globl ivt
ivt:
"""

    for i in range(256):
        o += f"    .long irq_{i}\n"

    fp.write(o)

