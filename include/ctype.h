//
// Created by Bugen Zhao on 2020/3/26.
//

#ifndef LITCHI_CTYPE_H
#define LITCHI_CTYPE_H

#include <include/types.h>

#define isDigit(ch) (('0' <= (ch)) && ((ch) <= '9'))
#define isSimpleChar(ch) ((0x20 <= (ch)) && ((ch) <= 0x7e))

#endif //LITCHI_CTYPE_H
