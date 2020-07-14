//
// Created by Bugen Zhao on 7/13/20.
//

#include <include/stdio.hpp>

void pageFault() {
    int sum = 0;
    for (int va = 0x100; va < 0x200; ++va)
        sum += *(int *) (va);    // page fault
    *(int *) (0x100) = sum;
}

int umain(int, char **) {
    asm volatile ("int3");
    console::out::print("%<Hello from user!!!!\n", LIGHT_GREEN);
    return 0;
}