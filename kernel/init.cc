//
// Created by Bugen Zhao on 2020/3/25.
//

#include <include/string.hpp>
#include <include/stdio.hpp>
#include <include/panic.hpp>
#include "console.hpp"
#include "version.h"
#include "vmem.hpp"
#include "monitor.hpp"
#include "task.hh"

extern "C" {
void i386InitLitchi(void) {
    // Clear .BSS section
    extern char edata[], end[];
    mem::set(edata, 0, end - edata);

    // Init the console for I/O, and print welcome message
    console::init();
    console::out::print("%<%s%c %<%s!\n%<Kernel version %s\n(C) BugenZhao %d%03x\n\n",
                        YELLOW, "Hello", ',', LIGHT_MAGENTA, "Litchi", WHITE, LITCHI_VERSION, 2, 0x20);

    // Init memory
    vmem::init();

    // Init task
    task::init();

    int loop = 10;
    while (loop--) {
        console::out::print("%r\n", std::get<1>(task::Task::alloc(0)));
    }

    // Go to the monitor
    int result = monitor::main();
    kernelPanic("Monitor dead with errno %d", result);
}
}
