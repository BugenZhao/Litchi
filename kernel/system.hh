//
// Created by Bugen Zhao on 2020/3/27.
//

#ifndef LITCHI_SYSTEM_HH
#define LITCHI_SYSTEM_HH

#include <kernel/knlast.inc>

namespace sys {
    [[noreturn]] void reboot();

    [[noreturn]] void shutdown();

    int cpuInfo(int, char **);
}

#endif //LITCHI_SYSTEM_HH
