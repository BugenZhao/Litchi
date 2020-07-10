//
// Created by Bugen Zhao on 2020/3/27.
//

#include <include/x86.h>
#include <kernel/system.h>

[[noreturn]] void reboot(void) {
    outb(0x92, 0x1);
    while(1);
}