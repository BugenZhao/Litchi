//
// Created by Bugen Zhao on 2020/2/20.
//

#include "console.h"
#include "printl.h"
#include "test.h"
#include "types.h"

int kernel_entry() {
    static const char *msg =
        "\nHello, Litchi!!\n\nThis is the kernel of Bugen's tiny OS -- Litchi OS.\n"
        "Version v0.%d.%d, Build 20%o\n\n(C) %s 2020\n";
    console_clear_c(BLACK, BYELLOW);
    printl_c(BLACK, BYELLOW, msg, 2, 5, 0x91, "BugenZhao");

    test_all();

    return 0;
}
