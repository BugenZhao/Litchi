//
// Created by Bugen Zhao on 2020/3/26.
//

#include <include/panic.hpp>
#include <include/vargs.hpp>
#include <include/stdio.hpp>
#include "kdebug.hpp"
#include <include/x64.h>

void _kernelPanic(const char *file, int line, const char *fmt, ...) {
    static bool PANIC = false;
    if (PANIC) goto spin;
    else PANIC = true;

    va_list ap;

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

namespace std {
    void __throw_bad_function_call() {
        kernelPanic("bad function call");
    }
}