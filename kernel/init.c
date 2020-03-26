//
// Created by Bugen Zhao on 2020/3/25.
//

#include <include/string.h>
#include <kernel/console.h>

static const char *HELLO_STR = "Hello, Litchi!\r\n(C) Bugen Zhao 2020";

static void hello();

void i386InitLitchi(void) {
    // Variable 'edata' and 'end' are from link scripts.
    extern char edata[], end[];

    // Before doing anything else, complete the ELF loading process.
    // Clear the uninitialized global data (BSS) section of our program.
    // This ensures that all static/global variables start out zero.
    memorySet(edata, 0, end - edata);
    consoleInit();
    hello();
}

void hello() {
    size_t i;
    size_t len;
    len = stringLength(HELLO_STR);

    for (i = 0; i < len; i++) {
        cgaPutChar(HELLO_STR[i], (DARK_GRAY + i) % 8 + 8, BLACK);
    }
}