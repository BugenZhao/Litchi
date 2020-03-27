//
// Created by Bugen Zhao on 2020/3/26.
//

#include <include/x86.h>
#include <include/memlayout.h>
#include <include/string.h>
#include <kernel/console.h>
#include <include/pckbd.h>

// Stupid I/O delay routine necessitated by historical PC design flaws
static void
delay(void) {
    inb(0x84);
    inb(0x84);
    inb(0x84);
    inb(0x84);
}

/***** Text-mode CGA/VGA display output *****/

static unsigned addr_6845;
static uint16_t *crtBuffer; // CRT vram
static uint16_t crtPos;     // The next position to put char

// CGA initialization, adopted from JOS/xv6
static void cgaInit(void) {
    volatile uint16_t *cp;
    uint16_t was;
    unsigned pos;

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
    outb(addr_6845, 14);
    pos = inb(addr_6845 + 1) << 8;
    outb(addr_6845, 15);
    pos |= inb(addr_6845 + 1);

    crtBuffer = (uint16_t *) cp;
    crtPos = pos;
}

// Refresh CGA cursor to next text position
static inline void cgaCursorRefresh(void) {
    outb(addr_6845, 14);
    outb(addr_6845 + 1, crtPos >> 8);
    outb(addr_6845, 15);
    outb(addr_6845 + 1, crtPos);
}

// Put char into cga. Explicit colors will be used only if no color info in c
static void cgaPutChar(int c, enum color_t defForeColor, enum color_t defBackColor) {
    if ((c & 0xff00) == 0) // no color in c
        c = COLOR_CHAR(c, defForeColor, defBackColor);

    // Print the character
    switch (c & 0xff) {
        case '\b':
            if (crtPos > 0) crtBuffer[--crtPos] = (c & ~0xff) | ' ';
            break;
        case '\t':
            cgaPutChar(' ', defForeColor, defBackColor);
            cgaPutChar(' ', defForeColor, defBackColor);
            cgaPutChar(' ', defForeColor, defBackColor);
            cgaPutChar(' ', defForeColor, defBackColor);
            break;
        case '\n':
            crtPos += CRT_COLS;
            // LF only
        case '\r':
            crtPos = crtPos / CRT_COLS * CRT_COLS;
            break;
        default:
            crtBuffer[crtPos++] = c;
    }

    // Scroll the screen
    if (crtPos == CRT_SIZE) {
        for (int i = 0; i < CRT_ROWS - 1; ++i) {
            memoryCopy(crtBuffer + i * CRT_COLS, crtBuffer + (i + 1) * CRT_COLS, CRT_COLS * sizeof(uint16_t));
        }
        for (int j = CRT_SIZE - CRT_COLS; j < CRT_SIZE; ++j) {
            crtBuffer[j] = (c & ~0xff) | ' ';
        }
        crtPos -= CRT_COLS;
    }

    // Move the cursor
    cgaCursorRefresh();
}

// CGA clear screen
static void cgaClear(void) {
    for (int i = 0; i < CRT_SIZE; ++i) {
        crtBuffer[i] = (DEF_FORE << 8) | ' ';
    }
    crtPos = 0;
    cgaCursorRefresh();
}

// Console I/O initialization
void consoleInit(void) {
    // Litchi will only print to display now
    cgaInit();
    kbdInit();
}

// Put a char to console
void consolePutChar(int c) {
    cgaPutChar(c, DEF_FORE, DEF_BACK);
}

// Get a non-zero char from console
int consoleGetChar(void) {
    int c;
    while ((c = kbdGetNextChar()) == 0);
    return c;
}

// Console clear screen
int consoleClear(void) {
    cgaClear();
    return 0;
}