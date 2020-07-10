//
// Created by Bugen Zhao on 2020/3/26.
//

#ifndef LITCHI_CTYPE_H
#define LITCHI_CTYPE_H

#include <include/types.h>

#define isDigit(ch) (('0' <= (ch)) && ((ch) <= '9'))
#define isSimpleChar(ch) ((0x20 <= (ch)) && ((ch) <= 0x7e))
#define isCapital(ch) (('A' <= (ch)) && ((ch) <= 'Z'))
#define isLittle(ch) (('a' <= (ch)) && ((ch) <= 'z'))
#define isAlphabet(ch) (isCapital(ch) || isLittle(ch))

constexpr char WHITESPACE[] = " \t\n\v\f\r";

bool isWhiteSpace(char ch);

#endif //LITCHI_CTYPE_H
