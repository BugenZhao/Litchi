//
// Created by Bugen Zhao on 2020/3/25.
//

#ifndef LITCHI_STRING_H
#define LITCHI_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <include/types.h>

void *memorySet(void *dest, uint8_t val, size_t count);

void *memoryCopy(void *dest, const void *src, size_t count);

void memoryZero(void *dest, size_t count);

size_t stringLength(const char *str);

char *stringAppend(char *dest, const char *src);

char *stringCopy(char *dest, const char *src);

int stringCompare(const char *lhs, const char *rhs);

int stringCaseCompare(const char *lhs, const char *rhs);

char *stringFindChar(const char *str, char c);

int stringSplit(char *str, const char *delimiters, char **resultBuf, size_t bufCount, bool ignoreQuo);

int stringSplitWS(char *str, char **resultBuf, size_t bufCount);

long stringToLong(const char *ptr, int base);

#ifdef __cplusplus
}
#endif

#endif //LITCHI_STRING_H
