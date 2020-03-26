//
// Created by Bugen Zhao on 2020/3/26.
//

#ifndef LITCHI_STDIO_H
#define LITCHI_STDIO_H

#include <include/types.h>
#include <kernel/console.h>
#include <include/vargs.h>

typedef void (*_gePutCharFunction)(int, void *);

// printf.c

// Generic consolePutChar function for printFormat
static void _geConsolePutChar(int c, int *cnt);

// For va_list
int consolePrintFmtVa(const char *fmt, va_list ap);

int consolePrintFmt(const char *fmt, ...);


// printfmt.c

void _gePrintNumber(_gePutCharFunction putChar, void *putdat, unsigned long long num, unsigned base, int width,
                    int paddingChar, bool capital);

void _gePrintFmtVa(_gePutCharFunction putChar, void *putdat, const char *fmt, va_list ap);

void _gePrintFmt(_gePutCharFunction putChar, void *putdat, const char *fmt, ...);


#endif //LITCHI_STDIO_H
