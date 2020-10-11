//
// Created by Bugen Zhao on 2020/3/25.
//

#include <include/string.hh>
#include <include/stdio.hh>
#include <kernel/panic.hh>
#include <kernel/console.hh>
#include <kernel/version.h>
#include <kernel/vmem.hh>
#include <kernel/monitor.hh>
#include <kernel/task.hh>
#include <kernel/mp.hh>

using namespace console::out;

extern "C" {
/// The entry of litchi kernel for i386 arch
void i386InitLitchi(void) {
    // Clear .BSS section
    extern uint8_t edata[], end[];
    mem::set(edata, 0, end - edata);

    // Init the console for I/O, and print welcome message
    console::init();
    print("%<Hello, %<Litchi!\n%<Kernel v%s\n(C) BugenZhao %d\n\n",
          YELLOW, LIGHT_MAGENTA, WHITE, LITCHI_VERSION_TIME, YEAR);

    // Init memory
    vmem::init();

    // Init task
    task::init();

    extern uint8_t embUserElf[];
    print("  User ELF [%<%s%<] is embedded at %p\n",
          WHITE, (char *) EMBUSER_ELF + BINARY_DIR_OFFSET, DEF_FORE,
          embUserElf);

    // Init trap
    trap::init();

    mp::init();

    // Go to the monitor
    monitor::main();
    kernelPanic("Monitor died");
}
}
