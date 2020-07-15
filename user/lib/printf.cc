#include <include/stdout.hpp>
#include <user/stdlib.hh>

namespace console {
    struct PrintBuf {
        static constexpr int maxCount = 256;
        int idx = 0;
        int count = 0;
        uint16_t buf[maxCount];

        inline void flush() {
            if (idx != 0) {
                syscall(ksyscall::Num::putString, (uint64_t) buf, idx);
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
            _gePrintFmtVa((_gePutCharFunction) _geUserConsolePutChar, &printBuf, fmt, ap, DEF_FORE, DEF_BACK);
            printBuf.flush();   // flush the buffer
            return printBuf.count;
        }

        // Console printFmt with color extension
        // "%<" -> foreground color, "%>" -> background color
        int print(const char *fmt, ...) {
            va_list ap;
            int cnt;
            va_start(ap, fmt);
            cnt = printVa(fmt, ap);
            va_end(ap);
            return cnt;
        }
    }
}