//
// Created by Bugen Zhao on 7/14/20.
//

#include "ksyscall.hh"
#include "console.hpp"
#include "task.hh"
#include "vmem.hpp"
#include <include/stdio.hpp>

namespace ksyscall {
    int64_t main(Num type, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5) {
        switch (type) {
            case Num::putChar:
                putChar(a1);
                return 0;
            case Num::putString:
                putString((const uint16_t *) a1, a2);
                return 0;
            case Num::exit:
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

    void putString(const uint16_t *str, size_t count) {
        // check if user is cheating us with our privilege
        if (!vmem::pgdir::userCheck(task::Task::current->pageDir, str, count, PTE_U)) {
            console::out::print("[%08x] Invalid memory access in [%p, %p)\n",
                                task::Task::current->id, str, str + count);
            task::Task::current->destroy(true);
        }
        // put all chars
        for (size_t i = 0; i < count; ++i) console::out::putChar(str[i]);
    }

    void exit() {
        task::Task::current->destroy(false);
    }
}