//
// Created by Bugen Zhao on 2020/3/27.
//

#ifndef LITCHI_KERNEL_PCKBD_H
#define LITCHI_KERNEL_PCKBD_H

#include <kernel/knlast.inc>
#include <include/types.h>

extern uint8_t shiftcode[256];
extern uint8_t togglecode[256];
extern uint8_t normalmap[256];
extern uint8_t shiftmap[256];
extern uint8_t ctlmap[256];
extern uint8_t *charcode[4];

// kernel/pckbd.c
#ifdef __cplusplus
extern "C" {
#endif

void kbdInit(void);

int kbdGetNextChar(void);

#ifdef __cplusplus
}

namespace kbd {
    inline static void init() {
        kbdInit();
    }

    inline static int getNextChar() {
        return kbdGetNextChar();
    }
}
#endif

#endif //LITCHI_KERNEL_PCKBD_H
