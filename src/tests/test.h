//
// Created by Bugen Zhao on 2020/2/21.
//

#ifndef LITCHI_TEST_H
#define LITCHI_TEST_H

#include "printl.h"

#define test(cond, msg)                                                                            \
    do {                                                                                           \
        if (!(cond)) {                                                                               \
            printl_c(BLACK, RED, "%s failed\n", msg);                                              \
        } else {                                                                                   \
            printl_c(BLACK, BGREEN, "%s passed\n", msg);                                           \
        }                                                                                          \
    } while (0)

void test_all();

static void test_mem();

static void test_str();

static void test_console();

static void test_printl();

#endif // LITCHI_TEST_H
