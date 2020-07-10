//
// Created by Bugen Zhao on 2020/3/26.
//

#ifndef LITCHI_CONSOLE_H
#define LITCHI_CONSOLE_H

#include <kernel/knlast.inc>

#include <include/types.h>
#include <include/color.h>

#define MONO_BASE   0x3B4
#define MONO_BUF    0xB0000
#define CGA_BASE    0x3D4
#define CGA_BUF     0xB8000

#define CRT_ROWS    25
#define CRT_COLS    80
#define CRT_SIZE    (CRT_ROWS * CRT_COLS)

namespace console {
    namespace cga {
        static void init(void);

        static void putChar(int c, enum color_t defForeColor, enum color_t defBackColor);

        static void clear(void);
    }

    void init(void);

    void putChar(int c);

    int getChar(void);

    int clear(void);
}

#endif //LITCHI_CONSOLE_H
