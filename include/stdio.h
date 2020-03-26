//
// Created by Bugen Zhao on 2020/3/26.
//

#ifndef LITCHI_STDIO_H
#define LITCHI_STDIO_H

#include <include/types.h>
#include <kernel/console.h>
#include <include/vargs.h>
#include <include/color.h>

// Generic printFmt oriented putChar func pointer
typedef void (*_gePutCharFunction)(int, void *);

// printf.c

static void _geConsolePutChar(int c, int *cnt);

int consolePrintFmtVa(const char *fmt, va_list ap);

int consolePrintFmt(const char *fmt, ...);

void consoleErrorPrintFmtVa(const char *fmt, va_list ap);

void consoleErrorPrintFmt(const char *fmt, ...);


// printfmt.c

void _gePrintNumber(_gePutCharFunction putChar, void *putdat, unsigned long long num, unsigned base, int width,
                    int paddingChar, bool capital, enum color_t fore, enum color_t back);

void _gePrintFmtVa(_gePutCharFunction putChar, void *putdat, const char *fmt, va_list ap, enum color_t defFore,
                   enum color_t defBack);

void _gePrintFmt(_gePutCharFunction putChar, void *putdat, const char *fmt, ...);


#endif //LITCHI_STDIO_H
