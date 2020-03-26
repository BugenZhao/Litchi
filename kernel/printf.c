//
// Created by Bugen Zhao on 2020/3/26.
//

#include <include/stdio.h>
#include <include/assert.h>

// Generic printFmt oriented console putChar
static inline void _geConsolePutChar(int c, int *cnt) {
    consolePutChar(c);
    *cnt++;
}

// Console vargs printFmt
int consolePrintFmtVa(const char *fmt, va_list ap) {
    int cnt = 0;
    _gePrintFmtVa((_gePutCharFunction) _geConsolePutChar, &cnt, fmt, ap, DEF_FORE, DEF_BACK);
    return cnt;
}

// Console printFmt with color extension
// "%<" -> foreground color, "%>" -> background color
int consolePrintFmt(const char *fmt, ...) {
    va_list ap;
    int cnt;
    va_start(ap, fmt);
    cnt = consolePrintFmtVa(fmt, ap);
    va_end(ap);
    return cnt;
}

// Console error vargs printFmt
void consoleErrorPrintFmtVa(const char *fmt, va_list ap) {
    int cnt = 0;
    _gePrintFmtVa((_gePutCharFunction) _geConsolePutChar, &cnt, fmt, ap, LIGHT_RED, DEF_BACK);
}

// Console printFmt
void consoleErrorPrintFmt(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    consoleErrorPrintFmtVa(fmt, ap);
    va_end(ap);
}