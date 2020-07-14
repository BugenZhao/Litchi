//
// Created by Bugen Zhao on 2020/3/27.
//

#ifndef LITCHI_MONITOR_HPP
#define LITCHI_MONITOR_HPP

#include <kernel/knlast.inc>
#include "trap.hh"
#include <include/result.hh>


namespace monitor {
    [[noreturn]] int main(trap::Frame *tf = nullptr);
}

#endif //LITCHI_MONITOR_HPP
