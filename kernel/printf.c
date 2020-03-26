//
// Created by Bugen Zhao on 2020/3/26.
//

#include <include/stdio.h>


static inline void _geConsolePutChar(int c, int *cnt) {
    consolePutChar(c);
    *cnt++;
}

int consolePrintFmtVa(const char *fmt, va_list ap) {
    int cnt = 0;
    _gePrintFmtVa((_gePutCharFunction) _geConsolePutChar, &cnt, fmt, ap);
    return cnt;
}

int consolePrintFmt(const char *fmt, ...) {
    va_list ap;
    int cnt;
    va_start(ap, fmt);
    cnt = consolePrintFmtVa(fmt, ap);
    va_end(ap);
    return cnt;
}