//
// Created by Bugen Zhao on 2020/2/21.
//

#ifndef LITCHI_PRINTL_H
#define LITCHI_PRINTL_H

#include "console.h"
#include "types.h"
#include "vargs.h"

// Flags
#define SMALL 0b00000001U
#define SIGNED 0b00000010U

#define IS_SMALL(x) (x & SMALL)
#define IS_SIGNED(x) (x & SIGNED)

void printl(const char *format, ...);

void printl_c(color_t back, color_t fore, const char *format, ...);

void sprintl(char *buffer, const char *format, ...);

static int vsprintl(char *buffer, const char *format, va_list args);

static char *itoa(int value, char *str, int base, uint8_t flags);

#endif // LITCHI_PRINTL_H
