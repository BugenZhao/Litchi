//
// Created by Bugen Zhao on 2020/3/27.
//

#include <x86.h>
#include "system.h"

namespace system {
    [[noreturn]] void reboot(void) {
        outb(0x92, 0x1);
        while(1);
    }
}