//
// Created by Bugen Zhao on 2020/2/21.
//

#include "printl.h"
#include "assert.h"
#include "string.h"

void printl(const char *format, ...) {
    static char buffer[0x200];
    va_list      args;
    int          i;
    va_start(args, format);
    i = vsprintl((char *) buffer, format, args);
    va_end(args);
    buffer[i] = 0;
    console_print_str((const char *) buffer);
}

void printl_c(color_t back, color_t fore, const char *format, ...) {
    static char buffer[0x200];
    va_list      args;
    int          i;
    va_start(args, format);
    i = vsprintl((char *) buffer, format, args);
    va_end(args);
    buffer[i] = 0;
    console_print_str_c((const char *) buffer, back, fore);
}

void sprintl(char *buffer, const char *format, ...) {
    va_list args;
    int     i;
    va_start(args, format);
    i = vsprintl((char *) buffer, format, args);
    va_end(args);
    buffer[i] = 0;
}

// c s di o xX u
static int vsprintl(char *buffer, const char *format, va_list args) {
    char *str = buffer;

    char *  s;
    int     i;
    uint8_t flags;

    for (; *format; ++format) {
        flags = 0;
        if (*format != '%') {
            *str++ = *format;
            continue;
        }
        ++format;
        switch (*format) {
        case 'c':
            *str++ = (unsigned char) va_arg(args, int);
            break;
        case 's':
            s = va_arg(args, char *);
            while (*s)
                *str++ = *s++;
            break;
        case 'd':
        case 'i':
            flags |= SIGNED;
        case 'u':
            i   = va_arg(args, unsigned int);
            str = itoa(i, str, 10, flags);
            while (*str)
                str++;
            break;
        case 'o':
            *(str++) = '0';
            i        = va_arg(args, unsigned int);
            str      = itoa(i, str, 8, flags);
            while (*str)
                str++;
            break;
        case 'x':
            flags |= SMALL;
            *(str++) = '0';
            *(str++) = 'x';
            i        = va_arg(args, unsigned int);
            str      = itoa(i, str, 16, flags);
            while (*str)
                str++;
            break;
        case 'X':
            *(str++) = '0';
            *(str++) = 'X';
            i        = va_arg(args, unsigned int);
            str      = itoa(i, str, 16, flags);
            while (*str)
                str++;
            break;
        default:
            if (*format != '%') *str++ = '%';
            if (*format)
                *str++ = *format;
            else
                --format;
        }
    }

    *str = 0;
    return str - buffer;
}

static char *itoa(int value, char *str, int base, uint8_t flags) {
    size_t   i = 0;
    size_t   j = 0;
    char     tmp[40];
    unsigned uvalue;

    const char *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if (IS_SMALL(flags)) digits = "0123456789abcdefghijklmnopqrstuvwxyz";
    if (IS_SIGNED(flags)) {
        if (value == 0x80000000) {
            // BUGGY...
            strcpy(str, "-2147483648");
            str[11] = 0;
            return str;
        } else if (value < 0) {
            value    = -value;
            str[i++] = '-';
        } else if (value == 0) {
            str[i++] = '0';
        }
        while (value) {
            tmp[j++] = digits[value % base];
            value /= base;
        }
        while (j--)
            str[i++] = tmp[j];

        str[i] = 0;
    } else {
        uvalue = (unsigned) value;
        if (uvalue == 0) {
            str[i++] = '0';
        }
        while (uvalue) {
            tmp[j++] = digits[uvalue % base];
            uvalue /= base;
        }
        while (j--)
            str[i++] = tmp[j];

        str[i] = 0;
    }

    return str;
}