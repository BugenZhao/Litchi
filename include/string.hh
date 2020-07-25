//
// Created by Bugen Zhao on 2020/3/25.
//

#ifndef LITCHI_STRING_HH
#define LITCHI_STRING_HH

#include <include/types.h>

namespace mem {
    void *set(void *dest, uint8_t val, size_t count);

    void *copy(void *dest, const void *src, size_t count);

    void *move(void *dest, const void *src, size_t count);

    void clear(void *dest, size_t count);
}

namespace str {
    size_t count(const char *str);

    char *append(char *dest, const char *src);

    char *copy(char *dest, const char *src, size_t count = SIZE_MAX);

    int cmp(const char *lhs, const char *rhs);

    int cmpCase(const char *lhs, const char *rhs);

    char *find(const char *str, char c);

    int split(char *str, const char *delimiters, char **resultBuf, size_t bufCount, bool ignoreQuo);

    int splitWs(char *str, char **resultBuf, size_t bufCount);

    long toLong(const char *ptr, int base);
}

#endif //LITCHI_STRING_HH
