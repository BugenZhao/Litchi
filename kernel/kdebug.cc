//
// Created by Bugen Zhao on 2020/4/6.
//

#include "kdebug.hpp"
#include <include/x86.h>
#include <include/stdio.hpp>
#include <include/memlayout.h>

namespace kdebug {
    void backtrace() {
        uint32_t ebp, eip;
        ebp = x86::read_ebp();

        console::err::print("Backtrace:\n  ebp\t\t\beip\t\t\bargs\n");
        int depth = 5;
        while (ebp && ebp + 24 < KSTACKTOP && depth--) {
            eip = *(uintptr_t *) (ebp + 4);
            console::err::print("  %08X  %08X  %08X %08X %08X %08X %08X\n",
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