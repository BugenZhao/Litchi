//
// Created by Bugen Zhao on 2020/3/27.
//

#include <include/stdio.h>
#include <include/ctype.h>
#include <include/assert.h>
#include <include/vargs.h>

#define READLINE_BUF_SIZE 8192
static char buf[READLINE_BUF_SIZE];

// Read line from console
char *consoleReadline(const char *promptFmt, ...) {
    char *ret;
    va_list ap;
    va_start(ap, promptFmt);
    ret = consoleReadlineVa(promptFmt, ap);
    va_end(ap);
    return ret;
}

char *consoleReadlineVa(const char *promptFmt, va_list ap) {
    int c;
    int idx = 0;

    if (promptFmt) consolePrintFmtVa(promptFmt, ap);
    while (true) {
        c = consoleGetChar();
        if (c < 0) {
            kernelPanic("Illegal input: 0x%x", c);
        } else if (isSimpleChar(c) && idx < READLINE_BUF_SIZE - 1) {
            consolePutChar(c);
            buf[idx++] = c;
        } else if (c == '\b' && idx > 0) {
            consolePutChar(c);
            idx--;
        } else if (c == '\r' || c == '\n') {
            consolePutChar('\n');
            buf[idx] = '\0';
            break;
        }
    }

    return buf;
}