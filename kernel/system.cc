//
// Created by Bugen Zhao on 2020/3/27.
//

#include <x86.h>
#include <stdio.h>
#include "system.h"

namespace system {
    [[noreturn]] void reboot() {
        x86::outb(0x92, 0x1);
        while (1);
    }

    static const char *cpuStr() {
        // https://wiki.osdev.org/Talk:CPUID
        // https://www.sandpile.org/x86/cpuid.htm#level_8000_0002h
        static char str[64] = "BugenX86CPU     ";
        auto p = reinterpret_cast<int *>(str);
        asm volatile ("cpuid":"=b"(p[0]),  "=d"(p[1]),  "=c"(p[2])              :"a"(0x80000000)); // vendor
        asm volatile ("cpuid":"=a"(p[4]),  "=b"(p[5]),  "=c"(p[6]),  "=d"(p[7]) :"a"(0x80000002)); // name
        asm volatile ("cpuid":"=a"(p[8]),  "=b"(p[9]),  "=c"(p[10]), "=d"(p[11]):"a"(0x80000003));
        asm volatile ("cpuid":"=a"(p[12]), "=b"(p[13]), "=c"(p[14]), "=d"(p[15]):"a"(0x80000003));
        return str;
    }

    void cpuInfo() {
        console::out::print("%s\n", cpuStr());
    }
}