//
// Created by Bugen Zhao on 2020/2/20.
//

#include "console.h"
#include "types.h"

int kernel_entry() {
    uint8_t *   vram  = (uint8_t *) 0xB8000;
    const char *msg =
        "Hello, Litchi!\n\nThis is the loader of Bugen's tiny OS -- Litchi OS.\n"
        "Version v0.2.0, Build 200220\n\n(C) BugenZhao 2020";
    print_pos(vram, 4, 0, msg, BYELLOW);

    return 0;
}
