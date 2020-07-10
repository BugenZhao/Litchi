//
// Created by Bugen Zhao on 2020/3/26.
//

#include <include/assert.h>
#include <include/vargs.h>
#include <include/stdio.h>
#include <kernel/console.h>
#include <kernel/kdebug.h>

static bool PANIC = false;

void _kernelPanic(const char *file, int line, const char *fmt, ...) {
    va_list ap;
    if (PANIC) goto spin;
    else PANIC = true;

    // Be extra sure that the machine is in as reasonable state
    asm volatile("cli; cld");

    va_start(ap, fmt);
    consoleErrorPrintFmt("********\nKERNEL PANIC AT %s:%d =>\n  ", file, line);
    consoleErrorPrintFmtVa(fmt, ap);
    consoleErrorPrintFmt("\n");
    backtracePrint();
    consoleErrorPrintFmt("********\n");
    va_end(ap);

    spin:
    asm volatile("hlt");
    goto spin;
}


void _kernelWarning(const char *file, int line, const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    consoleErrorPrintFmt("********\nKERNEL WARNING AT %s:%d =>\n  ", file, line);
    consoleErrorPrintFmtVa(fmt, ap);
    consoleErrorPrintFmt("\n********\n");
    va_end(ap);
}