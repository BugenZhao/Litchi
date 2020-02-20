//
// Created by Bugen Zhao on 2020/2/20.
//

#include "print.h"

inline void print_pos(void *_vram, int8_t row, int8_t col, const char *str,
                      color_t color) {
    uint8_t *vram   = _vram;
    uint16_t offset = (80 * row + col) * 2;
    while (*str != '\0') {
        if (*str == '\n') {
            offset = (offset / 160 + 1) * 160;
            str++;
            continue;
        }
        *(vram + (offset++)) = *(str++);
        *(vram + (offset++)) = color;
    }
}
