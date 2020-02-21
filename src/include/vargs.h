//
// Created by Bugen Zhao on 2020/2/21.
//

#ifndef LITCHI_VARGS_H
#define LITCHI_VARGS_H

typedef __builtin_va_list va_list;

#define va_start(ap, last)         (__builtin_va_start(ap, last))
#define va_arg(ap, type)           (__builtin_va_arg(ap, type))
#define va_end(ap)

#endif // LITCHI_VARGS_H
