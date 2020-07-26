//
// Created by Bugen Zhao on 2020/3/26.
//

#include <include/stdio.hh>
#include <kernel/panic.hh>
#include <include/cstdout.h>
#include <kernel/console.hh>


namespace console {
    namespace out {
        // Console vargs printFmt
        int printVa(const char *fmt, va_list ap) {
            int cnt = 0;
            auto putChar = [&cnt](int c) {
                out::putChar(c);
                if (c & 0x00ff) cnt++;
            };
            ge::printFmtVa(putChar, fmt, ap, DEF_FORE, DEF_BACK);
            return cnt;
        }

        // Console printFmt with color extension
        // "%<" -> foreground color, "%>" -> background color
        int print(const char *fmt, ...) {
            va_list ap;
            va_start(ap, fmt);
            int cnt = printVa(fmt, ap);
            va_end(ap);
            return cnt;
        }
    }

    namespace err {
        // Console error vargs printFmt
        void printVa(const char *fmt, va_list ap) {
            ge::printFmtVa(out::putChar, fmt, ap, LIGHT_RED, DEF_BACK);
        }

        // Console printFmt
        void print(const char *fmt, ...) {
            va_list ap;
            va_start(ap, fmt);
            printVa(fmt, ap);
            va_end(ap);
        }
    }

}

extern "C" {
int consoleOutPrintC(const char *fmt, ...) {
    va_list ap;
    int cnt;
    va_start(ap, fmt);
    cnt = console::out::printVa(fmt, ap);
    va_end(ap);
    return cnt;
}

void consoleErrPrintC(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    console::err::printVa(fmt, ap);
    va_end(ap);
}
}
