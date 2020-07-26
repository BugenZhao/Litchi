//
// Created by Bugen Zhao on 7/25/20.
//

#include <include/stdio.hh>
#include <user/include/stdlib.hh>
#include <include/ctype.h>

namespace console::in {
    char getChar() {
        return syscall(ksyscall::Num::getChar);
    }
}

namespace console::in {
    static char buf[READLINE_BUF_SIZE];    // not safe

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

        if (promptFmt) out::printVa(promptFmt, ap);
        while (true) {
            c = getChar();
            if (isSimpleChar(c) && idx < READLINE_BUF_SIZE - 1) {
                out::putChar(c);
                buf[idx++] = c;
            } else if (c == '\b' && idx > 0) {
                out::putChar(c);
                idx--;
            } else if (c == '\r' || c == '\n') {
                out::putChar('\n');
                buf[idx] = '\0';
                break;
            }
        }

        return buf;
    }
}