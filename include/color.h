//
// Created by Bugen Zhao on 2020/3/26.
//

#ifndef LITCHI_COLOR_H
#define LITCHI_COLOR_H

enum color_t {
    BLACK = 0b0000,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    GRAY,
    DARK_GRAY,
    LIGHT_BLUE,
    LIGHT_GREEN,
    LIGHT_CYAN,
    LIGHT_RED,
    LIGHT_MAGENTA,
    YELLOW,
    WHITE,
    COLOR_COUNT,
    DEF_FORE = GRAY,
    DEF_BACK = BLACK
};

#define COLOR_CHAR(c, fore, back) ( ((c)&0xffu) | ((fore)<<8u) | ((back)<<12u) )

#endif //LITCHI_COLOR_H
