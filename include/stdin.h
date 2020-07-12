//
// Created by Bugen Zhao on 2020/3/27.
//

#ifndef LITCHI_STDIN_H
#define LITCHI_STDIN_H

#include <include/pckbd.h>
#include <include/vargs.h>

// readline.c

namespace console::in {
    char *readline(const char *promptFmt, ...);

    char *readlineVa(const char *promptFmt, va_list ap);
}


#endif //LITCHI_STDIN_H
