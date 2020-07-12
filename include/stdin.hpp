//
// Created by Bugen Zhao on 2020/3/27.
//

#ifndef LITCHI_STDIN_HPP
#define LITCHI_STDIN_HPP

#include <include/pckbd.h>
#include <include/vargs.hpp>

// readline.cc

namespace console::in {
    char *readline(const char *promptFmt, ...);

    char *readlineVa(const char *promptFmt, va_list ap);
}


#endif //LITCHI_STDIN_HPP
