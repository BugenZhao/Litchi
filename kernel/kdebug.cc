//
// Created by Bugen Zhao on 2020/4/6.
//

#include "kdebug.h"
#include <x86.h>
#include <stdio.h>

namespace kdebug {
    void backtrace() {
        uint32_t ebp, eip;
        ebp = read_ebp();

        console::err::printFmt("Backtrace:\n  ebp\t\t\beip\t\t\bargs\n");
        while (ebp) {
            eip = *(uintptr_t *) (ebp + 4);
            console::err::printFmt("  %08X  %08X  %08X %08X %08X %08X %08X\n",
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
}