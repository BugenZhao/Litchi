//
// Created by Bugen Zhao on 2020/2/21.
//

#include "test.h"
#include "assert.h"
#include "console.h"
#include "printl.h"
#include "string.h"
#include "types.h"

void test_all() {
    test_console();
    test_mem();
    test_str();
    test_printl();
}

static void test_mem() {
    console_print_str_c("Testing mem funcs...\n", BLACK, GREEN);
    uint16_t a[8];
    uint32_t b[5];
    b[4] = 0x88888888;
    memset(a, 0x10, sizeof(a));
    assert(a[0] == 0x1010 && a[7] == 0x1010, "memset");
    memcpy(b, a, sizeof(a));
    assert(b[0] == 0x10101010 && b[3] == 0x10101010 && b[4] == 0x88888888, "memcpy");
}

static void test_str() {
    console_print_str_c("Testing str funcs...\n", BLACK, GREEN);
    assert(strlen("BugenZhao") == 9, "strlen");
    char a[] = "bz\0h";
    char b[] = "bz";
    char c[] = "by";
    char d[] = "b";
    assert(strcmp(d, c) < 0 && strcmp(c, b) < 0 && strcmp(b, a) == 0, "strcmp");
    char e[20] = "bugenz\0hao";
    char f[20] = "litchi\0os";
    assert(strcmp(strcpy(f, e), "bugenz") == 0, "strcpy");
    char g[30] = "bugen \0zhao";
    char h[20] = "litchi\0 os";
    assert(strcmp(strcat(g, h), "bugen litchi") == 0, "strcat");
}

static void test_console() {
    console_print_str_c("Testing console...\n", BLACK, GREEN);
    console_print_oct(0x00000000);
    console_put_char(' ');
    console_print_oct(0xffffeeee);
    console_put_char(' ');
    console_print_oct(0x1234ABCD);
    console_put_char(' ');
    console_print_dec(0xffffffff);
    console_put_char(' ');
    console_print_dec(12345678);
    console_put_char('\n');
}

static void test_printl() {
    console_print_str_c("Testing printl...\n", BLACK, GREEN);
    static char format0[] = "c:%c, s:%s, d:%d %d %d, i:%i, o:%o, x:%x, X:%X, u:%u, %z%%%z\n";
    static char answer[]  = "c:c, s:Litchi, d:0 -2147483648 -2147483647, i:2147483647, o:010, "
                           "x:0x12ab, X:0X34CD, u:4294967295, %z%%%z\n";
    char buffer[100];
    sprintl(buffer, format0, 'c', "Litchi\0OS", 0, 0x80000000, 0x80000001, 0x7fffffff, 8, 0x12ab,
            0x34cd, 0xffffffff);
    printl_c(BLACK, BCYAN, answer);
    printl_c(BLACK, BPURPLE, format0, 'c', "Litchi\0OS", 0, 0x80000000, 0x80000001, 0x7fffffff, 8,
             0x12ab, 0x34cd, 0xffffffff);
    assert(strcmp(buffer, answer), "printl");
}