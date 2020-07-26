//
// Created by Bugen Zhao on 2020/3/27.
//

#ifndef LITCHI_MONITOR_HH
#define LITCHI_MONITOR_HH

#include <kernel/knlast.inc>
#include <kernel/trap.hh>
#include <include/result.hh>


namespace monitor {
    [[noreturn]] int main(trap::Frame *tf = nullptr);
}

#endif //LITCHI_MONITOR_HH
