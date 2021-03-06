//
// Created by Bugen Zhao on 2020/3/26.
//

#include <include/stdio.hh>
#include <include/ctype.h>
#include <include/string.hh>
#include <include/color.h>
#include <include/result.hh>

static inline constexpr const char *describe(Result result) {
    switch (result) {
        case Result::ok:
            return "OK";
        case Result::unknownError:
            return "Unknown Error";
        case Result::invalidArg:
            return "Invalid Argument";
        case Result::noMemory:
            return "No Memory";
        case Result::noFreeTask:
            return "No Free Task";
        case Result::fuck:
            return "Fucking Error";
        case Result::tooManyArgs:
            return "Too Many Arguments";
        case Result::badSyntax:
            return "Bad Syntax";
        case Result::badCommand:
            return "Bad Command";
        case Result::noSuchTask:
            return "No Such Task";
    }
    return "Undefined Error";
}

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

namespace console::ge {
// Generic print number with color extension
    void printNumber(PutCharFunc putChar, unsigned long long num, unsigned base, int width, int paddingChar,
                     bool capital, enum color_t fore, enum color_t back) {
        if (num >= base) {
            printNumber(putChar, num / base, base, width - 1, paddingChar, capital, fore, back);
        } else {
            // Print padding char
            while (--width > 0) putChar(COLOR_CHAR(paddingChar, fore, back));
        }
        if (capital) putChar(COLOR_CHAR("0123456789ABCDEF"[num % base], fore, back));
        else putChar(COLOR_CHAR("0123456789abcdef"[num % base], fore, back));
    }

// Generic vargs printFmt with color extension
    void
    printFmtVa(PutCharFunc putChar, const char *fmt, va_list ap, enum color_t defFore, enum color_t defBack) {

#define putColorChar(c, fore, back) do { putChar(COLOR_CHAR(c, fore, back)); } while (0)

        int ch;
        unsigned long long num;
        char paddingChar;
        char *str;
        Result result;
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
                    putColorChar(ch, fore, back);
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
                    printNumber(putChar, num, 10, width, paddingChar, capital, fore, back);
                    break;
                case 'd':
                    num = _getIntVa(&ap, longFlag);
                    if ((long long) num < 0) {
                        putColorChar('-', fore, back);
                        num = -(long long) num;
                    }
                    printNumber(putChar, num, 10, width, paddingChar, capital, fore, back);
                    break;
                case 'X':
                    capital = true;
                case 'x':
                    num = _getUIntVa(&ap, longFlag);
                    printNumber(putChar, num, 16, width, paddingChar, capital, fore, back);
                    break;
                case 'O':
                    capital = true;
                case 'o':
                    num = _getUIntVa(&ap, longFlag);
                    printNumber(putChar, num, 8, width, paddingChar, capital, fore, back);
                    break;

                    // Other types
                case 's':
                    str = va_arg(ap, char *);
                    if (str == NULL) str = const_cast<char *>(NULL_FMT);
                    len = str::count(str);
                    for (width -= len; width > 0; width--) putColorChar(paddingChar, fore, back);
                    for (str; *str != '\0'; str++) putColorChar(*str, fore, back);
                    break;
                case 'p':
                    num = va_arg(ap, uintptr_t);
                    printNumber(putChar, num, 16, 8, '0', false, fore, back);
                    break;
                case 'P':
                    num = va_arg(ap, uintptr_t);
                    printNumber(putChar, num, 16, 8, '0', true, fore, back);
                    break;
                case 'r':
                    result = va_arg(ap, Result);
                    num = static_cast<int>(result);
                    str = const_cast<char *>(describe(result));
                    len = str::count(str);
                    for (width -= len; width > 0; width--) putColorChar(paddingChar, fore, back);
                    for (str; *str != '\0'; str++) putColorChar(*str, fore, back);
                    if (num != 0) {
                        for (auto c: " [E") putColorChar(c, fore, back);
                        printNumber(putChar, num, 10, 0, 0, false, fore, back);
                        putColorChar(']', fore, back);
                    }
                    break;
                case 'c':
                    putColorChar(va_arg(ap, int), fore, back); // Char in va_list takes 4 Bytes
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
                    putColorChar(ch, fore, back);
                    break;

                    // "%??"
                default:
                    putColorChar('%', fore, back);
                    putColorChar(ch, fore, back);
                    break;
            }
        }

#undef putColorChar
    }

// Generic printFmt with color extension
    void printFmt(PutCharFunc putChar, const char *fmt, ...) {
        va_list ap;
        va_start(ap, fmt);
        printFmtVa(putChar, fmt, ap, DEF_FORE, DEF_BACK);
        va_end(ap);
    }
}