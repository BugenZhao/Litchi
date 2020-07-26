//
// Created by Bugen Zhao on 2020/3/27.
//

#ifndef LITCHI_STDOUT_HH
#define LITCHI_STDOUT_HH

#include <include/types.h>
#include <include/vargs.hh>
#include <include/color.h>
#include <functional>

// kernel/printf.cc
// user/lib/printf.cc
// kernel/console.cc

namespace console {
    namespace out {
        void putChar(int c);

        int printVa(const char *fmt, va_list ap);

        int print(const char *fmt, ...);
    }

    namespace err {
        void printVa(const char *fmt, va_list ap);

        void print(const char *fmt, ...);
    }
}


// lib/printfmt.c

namespace console::ge {
    // Generic printFmt oriented putChar func pointer
    using PutCharFunc = std::function<void(int c)>;

    void printNumber(PutCharFunc putChar, unsigned long long num, unsigned base, int width, int paddingChar,
                     bool capital, enum color_t fore, enum color_t back);

    void printFmtVa(PutCharFunc putChar, const char *fmt, va_list ap, enum color_t defFore, enum color_t defBack);

    void printFmt(PutCharFunc putChar, const char *fmt, ...);
}


#endif //LITCHI_STDOUT_HH
