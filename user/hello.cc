//
// Created by Bugen Zhao on 7/13/20.
//

int umain(int, char **) {
    asm volatile ("int $88");   // syscall
    return 0;
}