//
// Created by Bugen Zhao on 2020/4/6.
//

#include <kernel/kdebug.h>
#include <include/x86.h>
#include <include/stdio.h>

void backtracePrint() {
    uint32_t ebp, eip;
    ebp = read_ebp();

    consoleErrorPrintFmt("Backtrace:\n  ebp\t\t\beip\t\t\bargs\n");
    while (ebp) {
        eip = *(uintptr_t *) (ebp + 4);
        consoleErrorPrintFmt("  %08X  %08X  %08X %08X %08X %08X %08X\n",
                             ebp,
                             eip,
                             *(uintptr_t *) (ebp + 8),
                             *(uintptr_t *) (ebp + 12),
                             *(uintptr_t *) (ebp + 16),
                             *(uintptr_t *) (ebp + 20),
                             *(uintptr_t *) (ebp + 24));
        ebp = *(uintptr_t *) (ebp);
    }
}