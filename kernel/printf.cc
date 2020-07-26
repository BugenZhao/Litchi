//
// Created by Bugen Zhao on 2020/3/26.
//

#include <include/stdio.hh>
#include <kernel/panic.hh>
#include <include/cstdout.h>
#include <kernel/console.hh>


namespace console {
    // Generic printFmt oriented console putChar
    static inline void _geConsolePutChar(int c, int *cnt) {
        if ((c & 0x00ff)) {
            out::putChar(c);
            if (cnt) *cnt++;
        }
    }

    namespace out {
        // Console vargs printFmt
        int printVa(const char *fmt, va_list ap) {
            int cnt = 0;
            _gePrintFmtVa((_gePutCharFunction) _geConsolePutChar, &cnt, fmt, ap, DEF_FORE, DEF_BACK);
            return cnt;
        }

        // Console printFmt with color extension
        // "%<" -> foreground color, "%>" -> background color
        int print(const char *fmt, ...) {
            va_list ap;
            int cnt;
            va_start(ap, fmt);
            cnt = printVa(fmt, ap);
            va_end(ap);
            return cnt;
        }
    }

    namespace err {
        // Console error vargs printFmt
        void printVa(const char *fmt, va_list ap) {
            int cnt = 0;
            _gePrintFmtVa((_gePutCharFunction) _geConsolePutChar, &cnt, fmt, ap, LIGHT_RED, DEF_BACK);
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
int consolePrintFmtC(const char *fmt, ...) {
    va_list ap;
    int cnt;
    va_start(ap, fmt);
    cnt = console::out::printVa(fmt, ap);
    va_end(ap);
    return cnt;
}

void consoleErrorPrintFmtC(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    console::err::printVa(fmt, ap);
    va_end(ap);
}
}

void filePrintFmtVa(int fd, const char *fmt, va_list ap) {
    kernelPanic("Not implemented");
}

void filePrintFmt(int fd, const char *fmt, ...) {
    kernelPanic("Not implemented");
}

