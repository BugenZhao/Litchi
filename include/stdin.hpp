//
// Created by Bugen Zhao on 2020/3/27.
//

#ifndef LITCHI_STDIN_HPP
#define LITCHI_STDIN_HPP

#include <kernel/pckbd.h>
#include <include/vargs.hpp>

// readline.cc

namespace console::in {
    constexpr int READLINE_BUF_SIZE = 4096;

    char getChar();

    char *readline(const char *promptFmt = nullptr, ...);

    char *readlineVa(const char *promptFmt, va_list ap);
}


#endif //LITCHI_STDIN_HPP
