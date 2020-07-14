//
// Created by Bugen Zhao on 7/14/20.
//

#include "ksyscall.hh"
#include "console.hpp"
#include "task.hh"

namespace ksyscall {
    int32_t main(SyscallType type, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5) {
        switch (type) {
            case SyscallType::putChar:
                putChar(a1);
                return 0;
            case SyscallType::exit:
                exit();
                return 0;
        }
        return 0;
    }
}

namespace ksyscall {
    void putChar(int c) {
        if (c & 0x00ff) {
            console::out::putChar(c);
        }
    }

    void exit() {
        task::Task::current->destroy(false);
    }
}