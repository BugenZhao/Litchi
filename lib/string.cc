//
// Created by Bugen Zhao on 2020/2/21.
//

// Naive basic string library

#include <include/string.hh>
#include <include/types.h>
#include <include/ctype.h>


namespace mem {
    void *set(void *dest, uint8_t val, size_t count) {
        uint8_t *_dest = (uint8_t *) dest;
        while (count--)
            *(_dest++) = val;
        return dest;
    }

    void *copy(void *dest, const void *src, size_t count) {
        uint8_t *_dest = (uint8_t *) dest;
        uint8_t *_src = (uint8_t *) src;
        while (count--)
            *(_dest++) = *(_src++);
        return dest;
    }

    void *move(void *dest, const void *src, size_t count) {
        // https://www.student.cs.uwaterloo.ca/~cs350/common/os161-src-html/doxygen/html/memmove_8c_source.html
        size_t i;
        /*
        * If the buffers don't overlap, it doesn't matter what direction
        * we copy in. If they do, it does, so just assume they always do.
        * We don't concern ourselves with the possibility that the region
        * to copy might roll over across the top of memory, because it's
        * not going to happen.
        *
        * If the destination is above the source, we have to copy
        * back to front to avoid overwriting the data we want to
        * copy.
        *
        *      dest:       dddddddd
        *      src:    ssssssss   ^
        *              |   ^  |___|
        *              |___|
        *
        * If the destination is below the source, we have to copy
        * front to back.
        *
        *      dest:   dddddddd
        *      src:    ^   ssssssss
        *              |___|  ^   |
        *                     |___|
        */
        if ((uintptr_t) dest < (uintptr_t) src) {
            /*
            * As author/maintainer of libc, take advantage of the
            * fact that we know memcpy copies forwards.
            */
            return mem::copy(dest, src, count);
        }
        /*
        * Copy by words in the common case. Look in memcpy.c for more
        * information.
        */
        if ((uintptr_t) dest % sizeof(long) == 0 &&
            (uintptr_t) src % sizeof(long) == 0 &&
            count % sizeof(long) == 0) {
            long *d = static_cast<long *>(dest);
            const long *s = static_cast<const long *>(src);
            /*
            * The reason we copy index i-1 and test i>0 is that
            * i is unsigned -- so testing i>=0 doesn't work.
            */
            for (i = count / sizeof(long); i > 0; i--) {
                d[i - 1] = s[i - 1];
            }
        } else {
            char *d = static_cast<char *>(dest);
            const char *s = static_cast<const char *>(src);
            for (i = count; i > 0; i--) {
                d[i - 1] = s[i - 1];
            }
        }
        return dest;
    }

    void clear(void *dest, size_t count) {
        set(dest, 0, count);
    }
}

namespace str {
    size_t count(const char *str) {
        size_t len = 0;
        while (*(str++))
            len++;
        return len;
    }

    char *append(char *dest, const char *src) {
        size_t len = count(dest);
        copy(dest + len, src);
        return dest;
    }

    char *copy(char *dest, const char *src, size_t count) {
        char *ret = dest;
        while (*src && count--)
            *(dest++) = *(src++);
        *dest = 0;
        return ret;
    }

    int cmp(const char *lhs, const char *rhs) {
        while (*lhs && *rhs && *lhs == *rhs)
            lhs++, rhs++;
        return *lhs - *rhs;
    }

    int cmpCase(const char *lhs, const char *rhs) {
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
    char *find(const char *str, char c) {
        for (; *str; str++)
            if (*str == c) return (char *) str;
        return nullptr;
    }

    // Split string by any char in delimiters, return the count of substrings
    // Note: str will be split up to (bufCount - 1) pieces,
    //  since the last char * will always be NULL
    int split(char *str, const char *delimiters, char **resultBuf, size_t bufCount, bool ignoreQuo) {
        if (str == NULL) return -1;
        size_t count = 0;
        int quo = 0;

        while (count < bufCount - 1) {
            while (*str && find(delimiters, *str) != NULL) *str++ = '\0';
            if (*str == '\0') break;
            resultBuf[count++] = str;
            while (*str && (quo || find(delimiters, *str) == NULL)) {
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
    int splitWs(char *str, char **resultBuf, size_t bufCount) {
        return split(str, WHITESPACE, resultBuf, bufCount, true);
    }
}

void * __builtin_memmove(void *dest, const void *src, size_t count) {
    return mem::move(dest, src, count);
}
