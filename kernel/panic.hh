//
// Created by Bugen Zhao on 2020/3/26.
//

#ifndef LITCHI_PANIC_HH
#define LITCHI_PANIC_HH

#include <kernel/knlast.inc>

void _kernelPanic(const char *file, int line, const char *fmt, ...) __attribute__((noreturn));

// Reduce the length of __FILE__
// For the macro SOURCE_PATH_SIZE, refer to CMakeLists.txt
#define __RFILE__ ((const char *)(__FILE__) + SOURCE_DIR_OFFSET)

#define kernelPanic(...) _kernelPanic(__RFILE__, __LINE__, __VA_ARGS__)

void _kernelWarning(const char *file, int line, const char *fmt, ...);

#define kernelWarning(...) _kernelWarning(__RFILE__, __LINE__, __VA_ARGS__)

#define assert(x) do{ if (!(x)) kernelPanic("ASSERTION FAILED: %s", #x); } while (0)

#define assertStatic(x) switch (x) case 0: case (x)

#endif //LITCHI_PANIC_HH
