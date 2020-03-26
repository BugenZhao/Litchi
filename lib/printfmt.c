//
// Created by Bugen Zhao on 2020/3/26.
//

#include <include/stdio.h>
#include <include/ctype.h>
#include <include/string.h>

static unsigned long long _getUIntVa(va_list *ap, int longFlag) {
    unsigned long long num;

    if (longFlag == 0) num = va_arg(*ap, unsigned int);
    else if (longFlag == 1) num = va_arg(*ap, unsigned long);
    else num = va_arg(*ap, unsigned long long);

    return num;
}

static long long _getIntVa(va_list *ap, int longFlag) {
    unsigned long long num;

    if (longFlag == 0) num = va_arg(*ap, int);
    else if (longFlag == 1) num = va_arg(*ap, long);
    else num = va_arg(*ap, long long);

    return num;
}

void _gePrintNumber(_gePutCharFunction putChar, void *putdat, unsigned long long num, unsigned base, int width,
                    int paddingChar, bool capital) {
    if (num >= base) {
        _gePrintNumber(putChar, putdat, num / base, base, width - 1, paddingChar, capital);
    } else {
        // Print padding char
        while (--width > 0) putChar(paddingChar, putdat);
    }
    if (capital) putChar("0123456789ABCDEF"[num % base], putdat);
    else putChar("0123456789abcdef"[num % base], putdat);
}

void _gePrintFmtVa(_gePutCharFunction putChar, void *putdat, const char *fmt, va_list ap) {
    int ch;
    unsigned long long num;
    char paddingChar;
    char *str;
    int width;
    int longFlag;
    bool capital;
    size_t len;

    while (1) {
        while ((ch = *(fmt++)) != '%') {
            if (ch == '\0') return;
            else putChar(ch, putdat);
        }
        // *fmt is now '%'

        paddingChar = ' ';
        width = -1;
        longFlag = 0;
        capital = false;

        reswitch:
        switch ((ch = *(fmt++))) {
            // Flags:
            case '0':
                paddingChar = '0';
                goto reswitch;

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                for (width = 0;; fmt++) {
                    width = 10 * width + (ch - '0');
                    ch = *fmt;
                    if (!isDigit(ch)) break;
                }
                goto reswitch;

                // How long?
            case 'l':
                longFlag++;
                goto reswitch;

                // Number types
            case 'u':
                num = _getUIntVa(&ap, longFlag);
                _gePrintNumber(putChar, putdat, num, 10, width, paddingChar, capital);
                break;
            case 'd':
                num = _getIntVa(&ap, longFlag);
                if ((long long) num < 0) {
                    putChar('-', putdat);
                    num = -(long long) num;
                }
                _gePrintNumber(putChar, putdat, num, 10, width, paddingChar, capital);
                break;
            case 'X':
                capital = true;
            case 'x':
                num = _getUIntVa(&ap, longFlag);
                _gePrintNumber(putChar, putdat, num, 16, width, paddingChar, capital);
                break;
            case 'O':
                capital = true;
            case 'o':
                num = _getUIntVa(&ap, longFlag);
                _gePrintNumber(putChar, putdat, num, 8, width, paddingChar, capital);
                break;

                // Other types
            case 's':
                str = va_arg(ap, char *);
                if (str == NULL) str = "(null)";
                len = stringLength(str);
                for (width -= len; width > 0; width--) putChar(paddingChar, putdat);
                for (str; *str != '\0'; str++) putChar(*str, putdat);
                break;
            case 'c':
                putChar(va_arg(ap, int), putdat); // Char in va_list takes 4 Bytes
                break;

                // "%%"
            case '%':
                putChar(ch, putdat);
                break;
                // "%??"
            default:
                putChar('%', putdat);
                putChar(ch, putdat);
        }

    }
}

void _gePrintFmt(_gePutCharFunction putChar, void *putdat, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    _gePrintFmtVa(putChar, putdat, fmt, ap);
    va_end(ap);
}