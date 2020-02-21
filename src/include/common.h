//
// Created by Bugen Zhao on 2020/2/21.
//

#ifndef LITCHI_COMMON_H
#define LITCHI_COMMON_H

#include "types.h"

// 端口读字节
inline void out_byte(uint16_t port, uint8_t byte);

// 端口写字节
inline uint8_t in_byte(uint16_t port);

// 端口写字
inline uint16_t in_word(uint16_t port);

#endif // LITCHI_COMMON_H
