//
// Created by Bugen Zhao on 2020/2/21.
//

// Naive basic string library

#include <include/string.h>
#include <include/types.h>

inline void *memorySet(void *dest, uint8_t val, size_t count) {
    uint8_t *_dest = (uint8_t *) dest;
    while (count--)
        *(_dest++) = val;
    return dest;
}

inline void *memoryCopy(void *dest, const void *src, size_t count) {
    uint8_t *_dest = (uint8_t *) dest;
    uint8_t *_src  = (uint8_t *) src;
    while (count--)
        *(_dest++) = *(_src++);
    return dest;
}

inline void memoryZero(void *dest, size_t count) {
    memorySet(dest, 0, count);
}

inline size_t stringLength(const char *str) {
    size_t len = 0;
    while (*(str++))
        len++;
    return len;
}

inline char *stringAppend(char *dest, const char *src) {
    size_t len = stringLength(dest);
    stringCopy(dest + len, src);
    return dest;
}

inline char *stringCopy(char *dest, const char *src) {
    char *ret = dest;
    while (*src)
        *(dest++) = *(src++);
    *dest = 0;
    return ret;
}

inline int stringCompare(const char *lhs, const char *rhs) {
    while (*lhs && *rhs && *lhs == *rhs)
        lhs++, rhs++;
    return *lhs - *rhs;
}