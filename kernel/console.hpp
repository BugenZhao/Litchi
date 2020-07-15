//
// Created by Bugen Zhao on 2020/3/26.
//

#ifndef LITCHI_CONSOLE_HPP
#define LITCHI_CONSOLE_HPP

#include <kernel/knlast.inc>

#include <include/types.h>
#include <include/color.h>

namespace console {
    namespace cga {
        static void init(void);

        static void putChar(int c, enum color_t defForeColor, enum color_t defBackColor);

        static void clear(void);
    }

    void init(void);

    int clear(void);

    namespace out { void putChar(int c); }

    namespace in { int getChar(void); }
}

#endif //LITCHI_CONSOLE_HPP
