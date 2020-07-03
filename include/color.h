//
// Created by Bugen Zhao on 2020/3/26.
//

#ifndef LITCHI_COLOR_H
#define LITCHI_COLOR_H

#ifdef __cplusplus
extern "C" {
#endif

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


// ANSI color codes for serial output
// https://en.wikipedia.org/wiki/ANSI_escape_code#Colors
extern const char *ansiForeCode[];
extern const char *ansiBackCode[];

// Generate a colored char for cga output
#define COLOR_CHAR(c, fore, back) ( ((c)&0xffu) | ((fore)<<8u) | ((back)<<12u) )

// Import fore color from ch
#define FORE_COLOR(ch) ((enum color_t) ((ch>>8u) & 0xfu))
// Import back color from ch
#define BACK_COLOR(ch) ((enum color_t) ((ch>>12u) & 0xfu))

#ifdef __cplusplus
}
#endif

#endif //LITCHI_COLOR_H
