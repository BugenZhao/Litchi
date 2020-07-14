//
// Created by Bugen Zhao on 2020/3/27.
//

#ifndef LITCHI_MONITOR_HPP
#define LITCHI_MONITOR_HPP

#include <kernel/knlast.inc>
#include "trap.hh"


namespace monitor {
    using FuncType = int (int argc, char **argv, trap::Frame *tf);

    FuncType help;

    [[noreturn]] int main(trap::Frame *tf = nullptr);
}

#endif //LITCHI_MONITOR_HPP
