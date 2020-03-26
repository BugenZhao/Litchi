//
// Created by Bugen Zhao on 2020/3/25.
//

#ifndef LITCHI_STRING_H
#define LITCHI_STRING_H

#include <include/types.h>

void *memorySet(void *dest, uint8_t val, size_t count);

void *memoryCopy(void *dest, const void *src, size_t count);

void memoryZero(void *dest, size_t count);

size_t stringLength(const char *str);

char *stringAppend(char *dest, const char *src);

char *stringCopy(char *dest, const char *src);

int stringCompare(const char *lhs, const char *rhs);


static struct memory_p {
    void *(*set)(void *dest, uint8_t val, size_t count);

    void *(*copy)(void *dest, const void *src, size_t count);

    void (*zero)(void *dest, size_t count);
} Memory = {
        .set = memorySet,
        .copy = memoryCopy,
        .zero = memoryZero
};


static struct string_p {
    size_t (*length)(const char *str);

    char *(*append)(char *dest, const char *src);

    char *(*copy)(char *dest, const char *src);

    int (*compare)(const char *lhs, const char *rhs);
} String = {
        .length=stringLength,
        .append=stringAppend,
        .copy=stringCopy,
        .compare=stringCompare
};

#endif //LITCHI_STRING_H
