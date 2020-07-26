//
// Created by Bugen Zhao on 2020/3/27.
//

#ifndef LITCHI_STDIN_HH
#define LITCHI_STDIN_HH

#include <include/vargs.hh>

// readline.cc

namespace console::in {
    constexpr int READLINE_BUF_SIZE = 4096;

    char getChar();

    char *readline(const char *promptFmt = nullptr, ...);

    char *readlineVa(const char *promptFmt, va_list ap);
}


#endif //LITCHI_STDIN_HH
