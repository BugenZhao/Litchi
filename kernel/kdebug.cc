//
// Created by Bugen Zhao on 2020/4/6.
//

#include "kdebug.hpp"
#include <include/x64.h>
#include <include/stdio.hpp>
#include <include/memlayout.h>

namespace kdebug {
    void backtrace() {
//        uint64_t rbp, rip;
//        rbp = x64::read_rbp();
//
//        console::err::print("Backtrace:\n  rbp\t\t\brip\t\t\bargs\n");
//        int depth = 5;
//        while (rbp && rbp + 24 < KSTACKTOP && depth--) {
//            rip = *(uintptr_t *) (rbp + 4);
//            console::err::print("  %08X  %08X  %08X %08X %08X %08X %08X\n",
//                                rbp,
//                                rip,
//                                *(uintptr_t *) (rbp + 8),
//                                *(uintptr_t *) (rbp + 12),
//                                *(uintptr_t *) (rbp + 16),
//                                *(uintptr_t *) (rbp + 20),
//                                *(uintptr_t *) (rbp + 24));
//            rbp = *(uintptr_t *) (rbp);
//        }
    }
}