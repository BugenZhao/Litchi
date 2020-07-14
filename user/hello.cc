//
// Created by Bugen Zhao on 7/13/20.
//

int umain(int, char **) {
    asm volatile ("int $88");   // syscall

    int sum = 0;
    for (int va = 0x100; va < 0x200; ++va)
        sum += *(int *)(va);    // page fault
    *(int *)(0x100) = sum;

    return 0;
}