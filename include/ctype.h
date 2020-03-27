//
// Created by Bugen Zhao on 2020/3/26.
//

#ifndef LITCHI_CTYPE_H
#define LITCHI_CTYPE_H

#include <include/types.h>

inline bool isDigit(int ch) {
    return '0' <= ch && ch <= '9';
}

inline bool isSimpleChar(int ch) {
    // from space to '~'
    return 0x20 <= ch && ch <= 0x7e;
}

#endif //LITCHI_CTYPE_H
