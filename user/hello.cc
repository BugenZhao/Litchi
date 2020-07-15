//
// Created by Bugen Zhao on 7/13/20.
//

#include <include/stdio.hpp>
#include <include/string.hpp>
#include <include/memlayout.h>
#include <user/stdlib.hh>

using namespace console::out;

void pageFault1() {     // low address
    int sum = 0;
    for (uint64_t va = 0x100; va < 0x200; ++va)
        sum += *(int *) (va);
    *(int *) (0x100) = sum;
}

void pageFault2() {     // higher address
    static int a = 5;
    for (int i = 0; i < 1 << 18; ++i) {
        print("%d", *(&a + i));
    }
}

void pageFault3() {     // read kernel address space
    print("%c", *((char *) KERNBASE + 0x110000));
}

void pageFault4() {     // cheat kernel to r/w privileged memory
    syscall(ksyscall::Num::putString, KERNBASE + 0x110000, 10);
}

int main(int, char **) {
    asm volatile ("int3");
    print("%<Hello from user!!!!\n", LIGHT_GREEN);
    pageFault4();
    return 0;
}