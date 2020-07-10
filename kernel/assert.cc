//
// Created by Bugen Zhao on 2020/3/26.
//

#include <assert.h>
#include <vargs.h>
#include <stdio.h>
#include "console.h"
#include "kdebug.h"

static bool PANIC = false;

void _kernelPanic(const char *file, int line, const char *fmt, ...) {
    va_list ap;
    if (PANIC) goto spin;
    else PANIC = true;

    // Be extra sure that the machine is in as reasonable state
    asm volatile("cli; cld");

    va_start(ap, fmt);
    console::err::printFmt("********\nKERNEL PANIC AT %s:%d =>\n  ", file, line);
    console::err::printFmtVa(fmt, ap);
    console::err::printFmt("\n");
    kdebug::backtrace();
    console::err::printFmt("********\n");
    va_end(ap);

    spin:
    asm volatile("hlt");
    goto spin;
}


void _kernelWarning(const char *file, int line, const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    console::err::printFmt("********\nKERNEL WARNING AT %s:%d =>\n  ", file, line);
    console::err::printFmtVa(fmt, ap);
    console::err::printFmt("\n********\n");
    va_end(ap);
}