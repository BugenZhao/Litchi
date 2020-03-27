//
// Created by Bugen Zhao on 2020/3/25.
//

#include <include/string.h>
#include <include/stdio.h>
#include <kernel/console.h>
#include <kernel/version.h>
#include <include/assert.h>

extern int monitor();

void i386InitLitchi(void) {
    // Clear .BSS section
    // Variable 'edata' and 'end' are from link scripts.
    extern char edata[], end[];
    memorySet(edata, 0, end - edata);

    // Init the console for I/O and print welcome message
    consoleInit();
    consolePrintFmt("%<%s%c %<%s!\n%<Kernel version %s\n(C) BugenZhao %d%03x\n\n",
                    YELLOW, "Hello", ',', LIGHT_MAGENTA, "Litchi", WHITE, LITCHI_VERSION, 2, 0x20);

    // Go to the monitor
    int errno = monitor();
    kernelPanic("Monitor dead with errno %d", errno);
}