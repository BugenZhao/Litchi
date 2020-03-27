//
// Created by Bugen Zhao on 2020/3/27.
//

#include <include/pckbd.h>
#include <include/x86.h>
#include <include/stdout.h>

// Read status and data from keyboard register
static int _kbdProcessData(void) {
    // Adopted from JOS/xv6
    int c;
    uint8_t stat, data;
    static uint32_t shift;

    // Read keyboard status
    stat = inb(KBSTATP);
    // Read data in keyboard buffer
    if ((stat & KBS_DIB) == 0)
        return -1;
    // Ignore data from mouse.
    if (stat & KBS_TERR)
        return -1;

    // Read keyboard data
    data = inb(KBDATAP);

    if (data == 0xE0) {
        // E0 escape character
        shift |= E0ESC;
        return 0;
    } else if (data & 0x80) {
        // Key released
        data = (shift & E0ESC ? data : data & 0x7F);
        shift &= ~(shiftcode[data] | E0ESC);
        return 0;
    } else if (shift & E0ESC) {
        // Last character was an E0 escape; or with 0x80
        data |= 0x80;
        shift &= ~E0ESC;
    }

    shift |= shiftcode[data];
    shift ^= togglecode[data];

    c = charcode[shift & (CTL | SHIFT)][data];
    if (shift & CAPSLOCK) {
        if ('a' <= c && c <= 'z')
            c += 'A' - 'a';
        else if ('A' <= c && c <= 'Z')
            c += 'a' - 'A';
    }

    // Process special keys
    // Ctrl-Alt-Del: reboot
    if (!(~shift & (CTL | ALT)) && c == KEY_DEL) {
        consoleErrorPrintFmt("Rebooting!\n");
        outb(0x92, 0x3); // courtesy of Chris Frost
    }

    return c;
}

// Init keyboard buffer (unnecessary since static) and try to read
void kbdInit(void) {
    kbdBuffer.readPos = 0;
    kbdBuffer.writePos = 0;
    consolePrintFmt("Keyboard initializing...");
    // Try to read a char
    _kbdProcessData();
    consolePrintFmt("Done\n\n");
}

// Fake keyboard interrupt currently ~
static void kbdIntr(void) {
    int c;
    while (true) {
        c = _kbdProcessData();
        if (c == -1) return;
        else if (c == 0) continue;
        kbdBuffer.buffer[kbdBuffer.writePos] = c;
        kbdBuffer.writePos = (kbdBuffer.writePos + 1) % KBD_BUF_SIZE;
    }
}

// Get next char from keyboard, 0 if none
int kbdGetNextChar(void) {
    int c;
    kbdIntr();
    if (kbdBuffer.writePos != kbdBuffer.readPos) {
        c = kbdBuffer.buffer[kbdBuffer.readPos];
        kbdBuffer.readPos = (kbdBuffer.readPos + 1) % KBD_BUF_SIZE;
        return c;
    }
    // No new char from keyboard
    return 0;
}