//
// Created by Bugen Zhao on 2020/2/21.
//

// Naive basic string library

#include <include/string.h>
#include <include/types.h>
#include <include/ctype.h>


inline void *memorySet(void *dest, uint8_t val, size_t count) {
    uint8_t *_dest = (uint8_t *) dest;
    while (count--)
        *(_dest++) = val;
    return dest;
}

inline void *memoryCopy(void *dest, const void *src, size_t count) {
    uint8_t *_dest = (uint8_t *) dest;
    uint8_t *_src = (uint8_t *) src;
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

int stringCaseCompare(const char *lhs, const char *rhs) {
    char a, b;
    while (*lhs && *rhs) {
        a = isCapital(*lhs) ? ((*lhs) - 'A' + 'a') : *lhs;
        b = isCapital(*rhs) ? ((*rhs) - 'A' + 'a') : *rhs;
        if (a != b) return a - b;
        lhs++, rhs++;
    }
    return *lhs - *rhs;
}

// Return the pointer to specific character in string
char *stringFindChar(const char *str, char c) {
    for (; *str; str++)
        if (*str == c) return (char *) str;
    return NULL;
}

// Split string by any char in delimiters, return the count of substrings
// Note: str will be split up to (bufCount - 1) pieces,
//  since the last char * will always be NULL
int stringSplit(char *str, const char *delimiters, char **resultBuf, size_t bufCount, bool ignoreQuo) {
    if (str == NULL) return -1;
    int count = 0;
    int quo = 0;

    while (count < bufCount - 1) {
        while (*str && stringFindChar(delimiters, *str) != NULL) *str++ = '\0';
        if (*str == '\0') break;
        resultBuf[count++] = str;
        while (*str && (quo || stringFindChar(delimiters, *str) == NULL)) {
            if (!ignoreQuo && *str == '\"') {
                if (!quo) resultBuf[count - 1] = str + 1;
                quo = !quo;
                *str = '\0';
            }
            str++;
        }
    }

    if (quo) return -1;

    resultBuf[count] = NULL;
    return count;
}

// Split string by any whitespaces, return the count of substrings
// Note: str will be split up to (bufCount - 1) pieces,
//  since the last char * will always be NULL
inline int stringSplitWS(char *str, char **resultBuf, size_t bufCount) {
    return stringSplit(str, WHITESPACE, resultBuf, bufCount, true);
}