//
// Created by Bugen Zhao on 2020/3/27.
//

#ifndef LITCHI_STDIN_H
#define LITCHI_STDIN_H

#include <include/pckbd.h>
#include <include/vargs.h>

// readline.c

char *consoleReadline(const char *promptFmt, ...);

char *consoleReadlineVa(const char *promptFmt, va_list ap);


#endif //LITCHI_STDIN_H
