//
// Created by Bugen Zhao on 2020/3/27.
//

#include <include/x64.h>
#include <include/stdio.hpp>
#include "system.hpp"
#include <tuple>

namespace sys {
    void reboot() {
        asm volatile ("cli");
        x64::outb(0x64, 0xfe);
        while (true);
    }

    void shutdown() {
        asm volatile ("cli");
        x64::outw(0xB004, 0x2000);  // bochs
        x64::outw(0x604, 0x2000);   // qemu
        x64::outw(0x4004, 0x3400);  // vbox
        while (true);
    }

    static const auto cpuStr() {
        // https://wiki.osdev.org/Talk:CPUID
        // https://www.sandpile.org/x86/cpuid.htm#level_8000_0002h
        static char str[64] = "BugenX86CPU!\0";
        auto p = reinterpret_cast<int *>(str);
        asm volatile ("cpuid":"=b"(p[0]),  "=d"(p[1]),  "=c"(p[2])              :"a"(0x80000000)); // vendor
        asm volatile ("cpuid":"=a"(p[4]),  "=b"(p[5]),  "=c"(p[6]),  "=d"(p[7]) :"a"(0x80000002)); // name
        asm volatile ("cpuid":"=a"(p[8]),  "=b"(p[9]),  "=c"(p[10]), "=d"(p[11]):"a"(0x80000003));
        asm volatile ("cpuid":"=a"(p[12]), "=b"(p[13]), "=c"(p[14]), "=d"(p[15]):"a"(0x80000004));
        return std::make_tuple(str, str + 16);
    }

    int cpuInfo(int, char **) {
        auto[vendor, name] = cpuStr();
        console::out::print("Vendor: %s\n", vendor);
        console::out::print("Name:   %s\n", name);
        return 0;
    }
}
