//
// Created by Bugen Zhao on 2020/4/8.
//

#include <include/ctype.h>

bool isWhiteSpace(char ch) {
    const char *whiteSpace = WHITESPACE;
    while (*whiteSpace) {
        if (ch == *whiteSpace) return true;
        whiteSpace++;
    }
    return false;
}