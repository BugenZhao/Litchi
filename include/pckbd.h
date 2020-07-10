//
// Created by Bugen Zhao on 2020/3/27.
//

#ifndef LITCHI_PCKBD_H
#define LITCHI_PCKBD_H

#include "include/types.h"

extern uint8_t shiftcode[256];
extern uint8_t togglecode[256];
extern uint8_t normalmap[256];
extern uint8_t shiftmap[256];
extern uint8_t ctlmap[256];
extern uint8_t *charcode[4];

#define KBD_BUF_SIZE 512
static struct {
    uint8_t buffer[KBD_BUF_SIZE];
    uint32_t readPos;
    uint32_t writePos;
} kbdBuffer;

// kernel/pckbd.c
#ifdef __cplusplus
extern "C" {
#endif

void kbdInit(void);

int kbdGetNextChar(void);

#ifdef __cplusplus
}
#endif

#endif //LITCHI_PCKBD_H
