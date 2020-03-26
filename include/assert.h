//
// Created by Bugen Zhao on 2020/3/26.
//

#ifndef LITCHI_ASSERT_H
#define LITCHI_ASSERT_H

void _kernelPanic(const char *file, int line, const char *fmt, ...) __attribute__((noreturn));

#define kernelPanic(...) _kernelPanic(__FILE__, __LINE__, __VA_ARGS__)

void _kernelWarning(const char *file, int line, const char *fmt, ...);

#define kernelWarning(...) _kernelWarning(__FILE__, __LINE__, __VA_ARGS__)

#define assert(x) do{ if (!(x)) kernelPanic("ASSERTION FAILED: %s", #x); } while (0)

#define static_assert(x) switch (x) case 0: case (x)

#endif //LITCHI_ASSERT_H
