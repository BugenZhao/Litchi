#include <include/stdout.hh>
#include <user/include/stdlib.hh>

namespace console::out {
    void putChar(int c) {
        syscall(ksyscall::Num::putChar, c);
    }
}

namespace console {
    struct PrintBuf {
        static constexpr int maxCount = 256;
        int idx = 0;
        int count = 0;
        uint16_t buf[maxCount];

        inline void flush() {
            if (idx != 0) {
                syscall(ksyscall::Num::putString, (uint32_t) buf, idx);
                idx = 0;
            }
        }

        inline void putChar(int c) {
            if (c & 0x00ff) {
                buf[idx++] = (uint16_t) (c & 0xffff);
                if (idx == maxCount - 1) flush();
                count++;
            }
        }

        inline void clear() {
            idx = 0;
            count = 0;
        }
    };

    static inline void _geUserConsolePutChar(int c, PrintBuf *printBuf) {
        printBuf->putChar(c);
    }

    namespace out {
        // Console vargs printFmt
        int printVa(const char *fmt, va_list ap) {
            struct PrintBuf printBuf;
            ge::printFmtVa([&printBuf](int c) { printBuf.putChar(c); }, fmt, ap, DEF_FORE, DEF_BACK);
            printBuf.flush();   // flush the buffer
            return printBuf.count;
        }

        // Console printFmt with color extension
        // "%<" -> foreground color, "%>" -> background color
        int print(const char *fmt, ...) {
            va_list ap;
            va_start(ap, fmt);
            int cnt = printVa(fmt, ap);
            va_end(ap);
            return cnt;
        }
    }

    namespace err {
        // Console error vargs printFmt
        void printVa(const char *fmt, va_list ap) {
            struct PrintBuf printBuf;
            ge::printFmtVa([&printBuf](int c) { printBuf.putChar(c); }, fmt, ap, LIGHT_RED, DEF_BACK);
            printBuf.flush();   // flush the buffer
        }

        // Console printFmt
        void print(const char *fmt, ...) {
            va_list ap;
            va_start(ap, fmt);
            printVa(fmt, ap);
            va_end(ap);
        }
    }
}
