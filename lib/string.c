//
// Created by Bugen Zhao on 2020/2/21.
//

#include <include/string.h>
#include <include/types.h>

inline void *memset(void *dest, uint8_t val, size_t count) {
    uint8_t *_dest = (uint8_t *) dest;
    while (count--)
        *(_dest++) = val;
    return dest;
}

inline void *memcpy(void *dest, const void *src, size_t count) {
    uint8_t *_dest = (uint8_t *) dest;
    uint8_t *_src  = (uint8_t *) src;
    while (count--)
        *(_dest++) = *(_src++);
    return dest;
}

inline void bzero(void *dest, size_t count) {
    memset(dest, 0, count);
}

inline size_t strlen(const char *str) {
    size_t len = 0;
    while (*(str++))
        len++;
    return len;
}

inline char *strcat(char *dest, const char *src) {
    size_t len = strlen(dest);
    strcpy(dest + len, src);
    return dest;
}

inline char *strcpy(char *dest, const char *src) {
    char *ret = dest;
    while (*src)
        *(dest++) = *(src++);
    *dest = 0;
    return ret;
}

inline int strcmp(const char *lhs, const char *rhs) {
    while (*lhs && *rhs && *lhs == *rhs)
        lhs++, rhs++;
    return *lhs - *rhs;
}