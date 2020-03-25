//
// Created by Bugen Zhao on 2020/3/25.
//

#include <include/string.h>
#include <kernel/console.h>
#include <include/memlayout.h>

static const char *HELLO_STR = "Hello, Litchi!       (C) Bugen Zhao 2020";

static void hello();

void litchi_i386_init(void) {
    extern char edata[], end[];

    // Before doing anything else, complete the ELF loading process.
    // Clear the uninitialized global data (BSS) section of our program.
    // This ensures that all static/global variables start out zero.
    memset(edata, 0, end - edata);
    hello();
}

void hello() {
    size_t i;
    size_t len;
    uint16_t *vmem;

    vmem = (uint16_t *) (KERNBASE + CGA_BUF);
    len = strlen(HELLO_STR);

    for (i = 0; i < len; i++) {
        vmem[10 * CRT_COLS + i] = 0x0700 | HELLO_STR[i];
    }
    vmem[11 * CRT_COLS - 1] = 0x0c00 | 'D';
}