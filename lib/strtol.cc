//
// Created by Bugen Zhao on 2020/4/8.
//

#include <include/string.hpp>
#include <include/ctype.h>

namespace str {
    // Convert a string to a long integer
    // TODO: deal with out-of-range exception
    long toLong(const char *ptr, int base) {
        const char *str = ptr;
        int c;
        bool neg = false;

        do {
            c = *str++;
        } while (isWhiteSpace(c));

        if (c == '-') {
            neg = true;
            c = *str++;
        } else if (c == '+') {
            neg = false;
            c = *str++;
        }

        if ((base == 0 || base == 16) && c == '0' && (*str == 'x' || *str == 'X')) {
            // hex
            c = str[1];
            str += 2;
            base = 16;
        } else if (base == 0) {
            // oct or dec
            if (c == '0') base = 8;
            else base = 10;
        }

        long long acc = 0;
        for (;; c = *str++) {
            if (isDigit(c)) c -= '0';
            else if (isCapital(c)) c -= 'A' - 10;
            else if (isLittle(c)) c -= 'a' - 10;
            else break;
            if (c >= base) break;

            acc = acc * base + c;
        }

        if (neg) acc = -acc;
        return (long) acc;
    }
}