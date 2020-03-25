//
// Created by Bugen Zhao on 2020/2/21.
//

#ifndef LITCHI_STRING_H
#define LITCHI_STRING_H

#include "types.h"

void *memset(void *dest, uint8_t val, size_t count);

void *memcpy(void *dest, const void *src, size_t count);

void bzero(void *dest, size_t count);

size_t strlen(const char *str);

char *strcat(char *dest, const char *src);

char *strcpy(char *dest, const char *src);

int strcmp(const char *lhs, const char *rhs);

#endif // LITCHI_STRING_H
