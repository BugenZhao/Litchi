//
// Created by Bugen Zhao on 7/14/20.
//

#ifndef LITCHI_USER_STDLIB_HH
#define LITCHI_USER_STDLIB_HH

#include <include/types.h>
#include <include/syscall.hh>
#include <include/trap.hh>

static inline int32_t syscall(ksyscall::Num type,
                              uint32_t a1 = 0, uint32_t a2 = 0, uint32_t a3 = 0, uint32_t a4 = 0, uint32_t a5 = 0) {
    int32_t ret;

    // move type into: eax
    // move arguments into: edx, ecx, ebx, edi, esi
    asm volatile("int %1\n"
    : "=a" (ret)
    : "i" (trap::Type::syscall),
    "a" (type),
    "d" (a1),
    "c" (a2),
    "b" (a3),
    "D" (a4),
    "S" (a5)
    : "cc", "memory");

    return ret;
}

#endif //LITCHI_USER_STDLIB_HH
