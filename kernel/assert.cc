//
// Created by Bugen Zhao on 2020/3/26.
//

#include <include/assert.h>
#include <include/vargs.h>
#include <include/stdio.h>
#include "kdebug.h"

static bool PANIC = false;

void _kernelPanic(const char *file, int line, const char *fmt, ...) {
    va_list ap;
    if (PANIC) goto spin;
    else PANIC = true;

    // Be extra sure that the machine is in as reasonable state
    asm volatile("cli; cld");

    va_start(ap, fmt);
    console::err::print("********\nKERNEL PANIC AT %s:%d =>\n  ", file, line);
    console::err::printVa(fmt, ap);
    console::err::print("\n");
    kdebug::backtrace();
    console::err::print("********\n");
    va_end(ap);

    spin:
    asm volatile("hlt");
    goto spin;
}


void _kernelWarning(const char *file, int line, const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    console::err::print("********\nKERNEL WARNING AT %s:%d =>\n  ", file, line);
    console::err::printVa(fmt, ap);
    console::err::print("\n********\n");
    va_end(ap);
}