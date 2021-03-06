//
// Created by Bugen Zhao on 7/14/20.
//

#ifndef LITCHI_INCLUDE_SYSCALL_HH
#define LITCHI_INCLUDE_SYSCALL_HH

#include <include/types.h>

namespace ksyscall {
    enum struct Num : uint32_t {
        putChar,
        putString,
        getChar,
        exit
    };
}


#endif //LITCHI_INCLUDE_SYSCALL_HH
