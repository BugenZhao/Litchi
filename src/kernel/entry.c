//
// Created by Bugen Zhao on 2020/2/20.
//

#include "console.h"
#include "types.h"
#include "test.h"

int kernel_entry() {
    const char *msg = "\nHello, Litchi!!\n\nThis is the loader of Bugen's tiny OS -- Litchi OS.\n"
                      "Version v0.2.3, Build 200220\n\n(C) BugenZhao 2020\n";
    console_clear_c(BLACK, BYELLOW);
    console_print_str_c(msg, BLACK, BYELLOW);

    test_all();

    return 0;
}
