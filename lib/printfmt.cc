//
// Created by Bugen Zhao on 2020/3/26.
//

#include <include/stdio.h>
#include <include/ctype.h>
#include <include/string.h>
#include <include/color.h>

constexpr char NULL_FMT[] = "(null)";

// Get unsigned int in different length from va_list
static unsigned long long _getUIntVa(va_list *ap, int longFlag) {
    unsigned long long num;

    if (longFlag == 0) num = va_arg(*ap, unsigned int);
    else if (longFlag == 1) num = va_arg(*ap, unsigned long);
    else num = va_arg(*ap, unsigned long long);

    return num;
}

// Get signed int in different length from va_list
static long long _getIntVa(va_list *ap, int longFlag) {
    unsigned long long num;

    if (longFlag == 0) num = va_arg(*ap, int);
    else if (longFlag == 1) num = va_arg(*ap, long);
    else num = va_arg(*ap, long long);

    return num;
}

// Generic print number with color extension
void _gePrintNumber(_gePutCharFunction putChar, void *putdat, unsigned long long num, unsigned base, int width,
                    int paddingChar, bool capital, enum color_t fore, enum color_t back) {
    if (num >= base) {
        _gePrintNumber(putChar, putdat, num / base, base, width - 1, paddingChar, capital, fore, back);
    } else {
        // Print padding char
        while (--width > 0) putChar(COLOR_CHAR(paddingChar, fore, back), putdat);
    }
    if (capital) putChar(COLOR_CHAR("0123456789ABCDEF"[num % base], fore, back), putdat);
    else putChar(COLOR_CHAR("0123456789abcdef"[num % base], fore, back), putdat);
}

// Generic vargs printFmt with color extension
void _gePrintFmtVa(_gePutCharFunction putChar, void *putdat, const char *fmt, va_list ap, enum color_t defFore,
                   enum color_t defBack) {

#define putColorChar(c, fore, back, p) do { putChar(COLOR_CHAR(c, fore, back), p); } while (0)

    int ch;
    unsigned long long num;
    char paddingChar;
    char *str;
    int width;
    int longFlag;
    bool capital;
    size_t len;
    enum color_t fore = defFore, back = defBack;

    while (1) {
        while ((ch = *(fmt++)) != '%') {
            if (ch == '\0')
                return;
            else
                putColorChar(ch, fore, back, putdat);
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
                _gePrintNumber(putChar, putdat, num, 10, width, paddingChar, capital, fore, back);
                break;
            case 'd':
                num = _getIntVa(&ap, longFlag);
                if ((long long) num < 0) {
                    putColorChar('-', fore, back, putdat);
                    num = -(long long) num;
                }
                _gePrintNumber(putChar, putdat, num, 10, width, paddingChar, capital, fore, back);
                break;
            case 'X':
                capital = true;
            case 'x':
                num = _getUIntVa(&ap, longFlag);
                _gePrintNumber(putChar, putdat, num, 16, width, paddingChar, capital, fore, back);
                break;
            case 'O':
                capital = true;
            case 'o':
                num = _getUIntVa(&ap, longFlag);
                _gePrintNumber(putChar, putdat, num, 8, width, paddingChar, capital, fore, back);
                break;

                // Other types
            case 's':
                str = va_arg(ap, char *);
                if (str == NULL) str = const_cast<char *>(NULL_FMT);
                len = str::count(str);
                for (width -= len; width > 0; width--) putColorChar(paddingChar, fore, back, putdat);
                for (str; *str != '\0'; str++) putColorChar(*str, fore, back, putdat);
                break;
            case 'c':
                putColorChar(va_arg(ap,
                                     int), fore, back, putdat); // Char in va_list takes 4 Bytes
                break;

                // Colors
            case '<':
                fore = (color_t) va_arg(ap, int);
                break;
            case '>':
                back = (color_t) va_arg(ap, int);
                break;

                // "%%"
            case '%':
                putColorChar(ch, fore, back, putdat);
                break;

                // "%??"
            default:
                putColorChar('%', fore, back, putdat);
                putColorChar(ch, fore, back, putdat);
                break;
        }
    }

#undef putColorChar
}

// Generic printFmt with color extension
void _gePrintFmt(_gePutCharFunction putChar, void *putdat, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    _gePrintFmtVa(putChar, putdat, fmt, ap, DEF_FORE, DEF_BACK);
    va_end(ap);
}