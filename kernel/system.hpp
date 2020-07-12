//
// Created by Bugen Zhao on 2020/3/27.
//

#ifndef LITCHI_SYSTEM_HPP
#define LITCHI_SYSTEM_HPP

#include <kernel/knlast.inc>

namespace sys {
    [[noreturn]] void reboot();

    int cpuInfo(int, char **);
}

#endif //LITCHI_SYSTEM_HPP
