//
// Created by Bugen Zhao on 7/26/20.
//

#include <include/stdio.hh>

namespace std {
    [[noreturn]] void __throw_bad_function_call() {
        console::err::print("Exception: bad_function_call\n");
        while (true);
    }
}