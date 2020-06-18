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


// ANSI color codes for serial output
// https://en.wikipedia.org/wiki/ANSI_escape_code#Colors
static const char *ansiForeCode[] = {
        [BLACK] = "30",
        [RED] = "31",
        [GREEN] = "32",
        [BROWN] = "33",
        [BLUE] = "34",
        [MAGENTA] = "35",
        [CYAN] = "36",
        [GRAY] = "37",
        [DARK_GRAY] = "90",
        [LIGHT_RED] = "91",
        [LIGHT_GREEN] = "92",
        [YELLOW] = "93",
        [LIGHT_BLUE] = "94",
        [LIGHT_MAGENTA] = "95",
        [LIGHT_CYAN] = "96",
        [WHITE] = "97"
};

static const char *ansiBackCode[] = {
        [BLACK] = "40",
        [RED] = "41",
        [GREEN] = "42",
        [BROWN] = "43",
        [BLUE] = "44",
        [MAGENTA] = "45",
        [CYAN] = "46",
        [GRAY] = "47",
        [DARK_GRAY] = "100",
        [LIGHT_RED] = "101",
        [LIGHT_GREEN] = "102",
        [YELLOW] = "103",
        [LIGHT_BLUE] = "104",
        [LIGHT_MAGENTA] = "105",
        [LIGHT_CYAN] = "106",
        [WHITE] = "107"
};

// Generate a colored char for cga output
#define COLOR_CHAR(c, fore, back) ( ((c)&0xffu) | ((fore)<<8u) | ((back)<<12u) )

// Import fore color from ch
#define FORE_COLOR(ch) ((enum color_t) ((ch>>8u) & 0xfu))
// Import back color from ch
#define BACK_COLOR(ch) ((enum color_t) ((ch>>12u) & 0xfu))

#endif //LITCHI_COLOR_H
