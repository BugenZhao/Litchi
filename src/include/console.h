//
// Created by Bugen Zhao on 2020/2/20.
//

#ifndef LITCHI_CONSOLE_H
#define LITCHI_CONSOLE_H

#include "types.h"

typedef uint16_t *vram_t;

static vram_t  vram = (vram_t) 0xb8000;
static uint8_t cr   = 0;
static uint8_t cc   = 0;

void bz_write_pos(void *_vram, int8_t row, int8_t col, const char *str, color_t color);

uint16_t get_attr(char c, color_t back, color_t fore);

void console_clear_c(color_t back, color_t cursor);

void console_clear();

void console_scroll();

void console_put_char_c(char c, color_t back, color_t fore);

void console_put_char(char c);

void console_print_str_c(const char *str, color_t back, color_t fore);

void console_print_str(const char *str);

void console_print_oct();

void console_print_dec();

void move_cursor();

#endif // LITCHI_CONSOLE_H
