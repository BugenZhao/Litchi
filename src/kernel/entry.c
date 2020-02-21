//
// Created by Bugen Zhao on 2020/2/20.
//

#include "console.h"
#include "types.h"

int kernel_entry() {
    const char *msg = "\nHello, Litchi!!\n\nThis is the loader of Bugen's tiny OS -- Litchi OS.\n"
                      "Version v0.2.2, Build 200220\n\n(C) BugenZhao 2020\n";
    console_clear_c(BLACK, BYELLOW);
    console_print_str_c(msg, BLACK, BYELLOW);

    console_print_oct(0x00000000);
    console_put_char(' ');
    console_print_oct(0xffffeeee);
    console_put_char(' ');
    console_print_oct(0x1234ABCD);
    console_put_char('\n');

    console_print_dec(0xffffffff);
    console_put_char(' ');
    console_print_dec(12345678);
    console_put_char('\n');

    for (int i = 0; i < 16; ++i) {
        console_print_str_c("TEST\n",BLACK,BGREEN);
    }

    return 0;
}
