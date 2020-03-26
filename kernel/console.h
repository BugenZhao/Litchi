//
// Created by Bugen Zhao on 2020/3/26.
//

#ifndef LITCHI_CONSOLE_H
#define LITCHI_CONSOLE_H

#ifndef LITCHI_KERNEL
#error "User programs are not allowed to #include Litchi kernel headers."
#endif

#include <include/types.h>
#include <include/color.h>

#define MONO_BASE   0x3B4
#define MONO_BUF    0xB0000
#define CGA_BASE    0x3D4
#define CGA_BUF     0xB8000

#define CRT_ROWS    25
#define CRT_COLS    80
#define CRT_SIZE    (CRT_ROWS * CRT_COLS)

static void cgaInit(void);

void cgaPutChar(int c, enum color_t foreColor, enum color_t backColor);

void consoleInit(void);

void consolePutChar(int c);

#endif //LITCHI_CONSOLE_H
