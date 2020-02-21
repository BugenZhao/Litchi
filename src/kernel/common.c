//
// Created by Bugen Zhao on 2020/2/21.
//

#include "common.h"

inline void out_byte(uint16_t port, uint8_t byte) {
    asm volatile("outb %1, %0" ::"dN"(port), "a"(byte)); // 输入约束, "a" 代表 %rax
}

inline uint8_t in_byte(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "dN"(port)); // 输入约束, "=a"
    return ret;
}

inline uint16_t in_word(uint16_t port) {
    uint16_t ret;
    asm volatile("inw %1, %0" : "=a"(ret) : "dN"(port));
    return ret;
}
