//
// Created by Bugen Zhao on 2020/3/27.
//

#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <vargs.h>
#include "console.h"

#define READLINE_BUF_SIZE 8192
namespace console::in {
    static char buf[READLINE_BUF_SIZE];

    // Read line from console
    char *readline(const char *promptFmt, ...) {
        char *ret;
        va_list ap;
        va_start(ap, promptFmt);
        ret = readlineVa(promptFmt, ap);
        va_end(ap);
        return ret;
    }

    char *readlineVa(const char *promptFmt, va_list ap) {
        int c;
        int idx = 0;

        if (promptFmt) out::printFmtVa(promptFmt, ap);
        while (true) {
            c = getChar();
            if (c < 0) {
                kernelPanic("Illegal input: 0x%x", c);
            } else if (isSimpleChar(c) && idx < READLINE_BUF_SIZE - 1) {
                putChar(c);
                buf[idx++] = c;
            } else if (c == '\b' && idx > 0) {
                putChar(c);
                idx--;
            } else if (c == '\r' || c == '\n') {
                putChar('\n');
                buf[idx] = '\0';
                break;
            }
        }

        return buf;
    }
}

