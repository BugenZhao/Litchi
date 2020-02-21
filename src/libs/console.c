//
// Created by Bugen Zhao on 2020/2/20.
//

#include "console.h"
#include "common.h"

inline void bz_write_pos(void *_vram, int8_t row, int8_t col, const char *str, color_t color) {
    uint8_t *__vram = _vram;
    uint16_t offset = (80 * row + col) * 2;
    while (*str != '\0') {
        if (*str == '\n') {
            offset = (offset / 160 + 1) * 160;
            str++;
            continue;
        }
        *(__vram + (offset++)) = *(str++);
        *(__vram + (offset++)) = color;
    }
}

inline uint16_t get_attr(char c, color_t back, color_t fore) {
    return ((back << 4U | fore) << 8U) | c;
}

void console_clear_c(color_t back, color_t cursor) {
    uint16_t blank = get_attr(' ', back, cursor);
    for (int i = 0; i < 80 * 25; ++i) {
        vram[i] = blank;
    }
    cc = 0, cr = 0;
    move_cursor();
}

void console_clear() {
    console_clear_c(BLACK, WHITE);
}

void console_scroll() {
    uint16_t blank = get_attr(' ', BLACK, WHITE);
    if (cr >= 25) {
        for (int i = 0; i < 24 * 80; ++i) {
            vram[i] = vram[i + 80];
        }
        for (int i = 24 * 80; i < 25 * 80; ++i) {
            vram[i] = blank;
        }
        cr = 24;
    }
}

void console_put_char_c(char c, color_t back, color_t fore) {
    uint16_t attr = get_attr(c, back, fore);
    if (c == '\r')
        cc = 0;
    else if (c == '\n')
        cc = 0, cr++;
    else
        vram[80 * cr + cc] = attr, cc++;

    if (cc >= 80) {
        cc -= 80;
        cr++;
    }
    console_scroll();
    move_cursor();
}

void console_put_char(char c) {
    console_put_char_c(c, BLACK, WHITE);
}

void console_print_str_c(const char *str, color_t back, color_t fore) {
    while (*str) {
        console_put_char_c(*(str++), back, fore);
    }
}

void console_print_str(const char *str) {
    console_print_str_c(str, BLACK, WHITE);
}

void console_print_oct(uint32_t num) {
    char fs[8];
    int  i = 0;
    while (i != 8) {
        uint8_t tmp = num & 0xfU;
        fs[i++]     = (tmp >= 10) ? 'A' + tmp - 10 : '0' + tmp;
        num >>= 4U;
    }
    while (i--)
        console_put_char(fs[i]);
}

void console_print_dec(uint32_t num) {
    if (!num) {
        console_put_char('0');
        return;
    }
    char fs[10];
    int  i = 0;
    while (num) {
        uint8_t tmp = num % 10;
        fs[i++]     = '0' + tmp;
        num /= 10;
    }
    while (i--)
        console_put_char(fs[i]);
}

void move_cursor() {
    uint16_t pos = 80 * cr + cc;
    out_byte(0x3d4, 14);
    out_byte(0x3d5, (uint8_t)(pos >> 8U));
    out_byte(0x3d4, 15);
    out_byte(0x3d5, (uint8_t) pos);
}