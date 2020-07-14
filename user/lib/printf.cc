
#include <include/stdout.hpp>
#include <user/stdlib.hh>

namespace console {
    // Generic printFmt oriented console putChar [USER]
    static inline void _geUserConsolePutChar(int c, int *cnt) {
        if ((c & 0x00ff)) {
            syscall(ksyscall::SyscallType::putChar, c);
            if (cnt) *cnt++;
        }
    }

    namespace out {
        // Console vargs printFmt
        int printVa(const char *fmt, va_list ap) {
            int cnt = 0;
            _gePrintFmtVa((_gePutCharFunction) _geUserConsolePutChar, &cnt, fmt, ap, DEF_FORE, DEF_BACK);
            return cnt;
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