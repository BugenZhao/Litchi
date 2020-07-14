//
// Created by Bugen Zhao on 7/14/20.
//

#ifndef LITCHI_INCLUDE_SYSCALL_HH
#define LITCHI_INCLUDE_SYSCALL_HH

#include <include/types.h>

namespace ksyscall {
    enum struct SyscallType : uint32_t {
        putChar,
        putString,
        exit
    };
}


#endif //LITCHI_INCLUDE_SYSCALL_HH
