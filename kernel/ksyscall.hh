//
// Created by Bugen Zhao on 7/14/20.
//

#ifndef LITCHI_KERNEL_KSYSCALL_HH
#define LITCHI_KERNEL_KSYSCALL_HH

#include <include/syscall.hh>

namespace ksyscall {
    int64_t main(Num type,
                 uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5);
}

namespace ksyscall {
    void putChar(int c);

    void putString(const uint16_t *str, size_t count);

    void exit();
}

#endif //LITCHI_KERNEL_KSYSCALL_HH
