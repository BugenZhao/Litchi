//
// Created by Bugen Zhao on 2020/3/26.
//

#include <include/x86.h>
#include <include/memlayout.h>
#include <include/string.hh>
#include <kernel/pckbd.h>
#include "console.hpp"

namespace console {
    namespace serial {
        /***** Serial I/O *****/
        // Adopted from JOS

#define COM1           0x3F8
#define COM_RX         0       // In:  Receive buffer (DLAB=0)
#define COM_TX         0       // Out: Transmit buffer (DLAB=0)
#define COM_DLL        0       // Out: Divisor Latch Low (DLAB=1)
#define COM_DLM        1       // Out: Divisor Latch High (DLAB=1)
#define COM_IER        1       // Out: Interrupt Enable Register
#define COM_IER_RDI    0x01    // Enable receiver data interrupt
#define COM_IIR        2       // In: Interrupt ID Register
#define COM_FCR        2       // Out: FIFO Control Register
#define COM_LCR        3       // Out: Line Control Register
#define COM_LCR_DLAB   0x80    // Divisor latch access bit
#define COM_LCR_WLEN8  0x03    // Wordlength: 8 bits
#define COM_MCR        4       // Out: Modem Control Register
#define COM_MCR_RTS    0x02    // RTS complement
#define COM_MCR_DTR    0x01    // DTR complement
#define COM_MCR_OUT2   0x08    // Out2 complement
#define COM_LSR        5       // In: Line Status Register
#define COM_LSR_DATA   0x01    // Data available
#define COM_LSR_TXRDY  0x20    // Transmit buffer avail
#define COM_LSR_TSRE   0x40    // Transmitter off

        constexpr bool colored = false;

        // Stupid I/O delay routine necessitated by historical PC design flaws
        static void delay(void) {
            x86::inb(0x84);
            x86::inb(0x84);
            x86::inb(0x84);
            x86::inb(0x84);
        }

        static bool exists;

        static void init(void) {
            // Turn off the FIFO
            x86::outb(COM1 + COM_FCR, 0);

            // Set speed; requires DLAB latch
            x86::outb(COM1 + COM_LCR, COM_LCR_DLAB);
            x86::outb(COM1 + COM_DLL, (uint8_t) (115200 / 9600));
            x86::outb(COM1 + COM_DLM, 0);

            // 8 data bits, 1 stop bit, parity off; turn off DLAB latch
            x86::outb(COM1 + COM_LCR, COM_LCR_WLEN8 & ~COM_LCR_DLAB);

            // No modem controls
            x86::outb(COM1 + COM_MCR, 0);
            // Enable rcv interrupts
            x86::outb(COM1 + COM_IER, COM_IER_RDI);

            // Clear any preexisting overrun indications and interrupts
            // Serial port doesn't exist if COM_LSR returns 0xFF
            exists = (x86::inb(COM1 + COM_LSR) != 0xFF);
            (void) x86::inb(COM1 + COM_IIR);
            (void) x86::inb(COM1 + COM_RX);
        }

        static inline void sout(int c) {
            // int i;
            // for (i = 0;
            //      !(inb(COM1 + COM_LSR) & COM_LSR_TXRDY) && i < 12800;
            //      i++)
            //     delay();
            x86::outb(COM1 + COM_TX, c);
        }

        static void putChar(int c, enum color_t defForeColor, enum color_t defBackColor) {
            if (exists) {
                if (colored) {
                    enum color_t foreColor = FORE_COLOR(c) ? FORE_COLOR(c) : defForeColor;
                    enum color_t backColor = BACK_COLOR(c) ? BACK_COLOR(c) : defBackColor;
                    const char *foreCode = ansiForeCode[foreColor];
                    const char *backCode = ansiBackCode[backColor];
                    sout('\x1b');
                    sout('[');
                    while (*foreCode) sout(*(foreCode++));
                    sout(';');
                    while (*backCode) sout(*(backCode++));
                    sout('m');
                }
                if ((c & 0xff) == '\b') {
                    sout(c);
                    sout(' ');
                    sout(c);
                } else {
                    sout(c);
                }

                // sout('\x1b');
                // sout('[');
                // sout('3');
                // sout('9');
                // sout('m');
            }
        }
    }

    namespace cga {
        /***** Text-mode CGA/VGA display output *****/

        static unsigned addr_6845;
        static uint16_t *buffer;
        static uint16_t pos;

        // CGA initialization, adopted from JOS/xv6
        static void init(void) {
            volatile uint16_t *cp;
            uint16_t was;
            unsigned npos;

            cp = (uint16_t *) (KERNBASE + CGA_BUF);
            was = *cp;
            *cp = (uint16_t) 0xA55A;
            if (*cp != 0xA55A) {
                cp = (uint16_t *) (KERNBASE + MONO_BUF);
                addr_6845 = MONO_BASE;
            } else {
                *cp = was;
                addr_6845 = CGA_BASE;
            }

            // Get cursor position
            x86::outb(addr_6845, 14);
            npos = x86::inb(addr_6845 + 1) << 8;
            x86::outb(addr_6845, 15);
            npos |= x86::inb(addr_6845 + 1);

            buffer = (uint16_t *) cp;
            pos = npos;
        }

        // Refresh CGA cursor to next text position
        static inline void cursorRefresh(void) {
            x86::outb(addr_6845, 14);
            x86::outb(addr_6845 + 1, pos >> 8);
            x86::outb(addr_6845, 15);
            x86::outb(addr_6845 + 1, pos);
        }

        // Put char into cga. Explicit colors will be used only if no color info in c
        static void putChar(int c, enum color_t defForeColor, enum color_t defBackColor) {
            if ((c & 0xff00) == 0) // no color in c
                c = COLOR_CHAR(c, defForeColor, defBackColor);

            // Print the character
            switch (c & 0xff) {
                case '\b':
                    if (pos > 0) buffer[--pos] = (c & ~0xff) | ' ';
                    break;
                case '\t':
                    putChar(' ', defForeColor, defBackColor);
                    putChar(' ', defForeColor, defBackColor);
                    putChar(' ', defForeColor, defBackColor);
                    putChar(' ', defForeColor, defBackColor);
                    break;
                case '\n':
                    pos += CRT_COLS;
                    // LF only
                case '\r':
                    pos = pos / CRT_COLS * CRT_COLS;
                    break;
                default:
                    buffer[pos++] = c;
            }

            // Scroll the screen
            if (pos == CRT_SIZE) {
                for (int i = 0; i < CRT_ROWS - 1; ++i) {
                    mem::copy(buffer + i * CRT_COLS, buffer + (i + 1) * CRT_COLS, CRT_COLS * sizeof(uint16_t));
                }
                for (int j = CRT_SIZE - CRT_COLS; j < CRT_SIZE; ++j) {
                    buffer[j] = (c & ~0xff) | ' ';
                }
                pos -= CRT_COLS;
            }

            // Move the cursor
            cursorRefresh();
        }

        // CGA clear screen
        static void clear(void) {
            for (int i = 0; i < CRT_SIZE; ++i) {
                buffer[i] = (DEF_FORE << 8) | ' ';
            }
            pos = 0;
            cursorRefresh();
        }
    }

    // Console I/O initialization
    void init(void) {
        // Output
        console::cga::init();
        serial::init();
        // Input
        kbd::init();
    }

    // Console clear screen
    int clear(void) {
        console::cga::clear();
        return 0;
    }

    namespace out {
        // Put a char to console
        void putChar(int c) {
            cga::putChar(c, DEF_FORE, DEF_BACK);
            serial::putChar(c, DEF_FORE, DEF_BACK);
        }
    }

    namespace in {
        // Get a non-zero char from console
        int getChar() {
            int c;
            while ((c = kbd::getNextChar()) == 0);
            return c;
        }
    }
}
