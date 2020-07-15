//
// Created by Bugen Zhao on 7/14/20.
//

#ifndef LITCHI_USER_STDLIB_HH
#define LITCHI_USER_STDLIB_HH

#include <include/types.h>
#include <include/syscall.hh>
#include <include/trap.hh>

static inline int64_t syscall(ksyscall::Num type,
                              uint64_t a1 = 0, uint64_t a2 = 0, uint64_t a3 = 0, uint64_t a4 = 0, uint64_t a5 = 0) {
    int64_t ret;

    // move type into: rax
    // move arguments into: rdx, rcx, rbx, r10, r8
    register uint64_t r10 asm("r10") = a4;
    register uint64_t r8 asm("r8") = a5;
    asm volatile("int %1\n"
    : "=a" (ret)
    : "i" (trap::Type::syscall),
    "a" (type),
    "d" (a1),
    "c" (a2),
    "b" (a3),
    "r" (r10),
    "r" (r8)
    : "cc", "memory");

    return ret;
}

#endif //LITCHI_USER_STDLIB_HH
