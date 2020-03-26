//
// Created by Bugen Zhao on 2020/3/25.
//

#include <include/string.h>
#include <kernel/console.h>
#include <include/stdio.h>


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
    consoleClear();
    consolePrintFmt("%<%s%c %<%s!\n%<BugenZhao %d%03x",
                    YELLOW, "Hello", ',', LIGHT_MAGENTA, "Litchi", DEF_FORE, 2, 0x20);
}