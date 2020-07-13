//
// Created by Bugen Zhao on 7/13/20.
//

#include <include/memlayout.h>

// https://wiki.osdev.org/Segmentation
// https://wiki.osdev.org/GDT
// https://wiki.osdev.org/GDT_Tutorial
// https://stackoverflow.com/questions/9113310/segment-selector-in-ia-32

// kernel GDT, for the need of
// - privilege protection for user tasks
// - trap & system call, loading kernel stack by TSS0 segment
struct SegDesc gdt[] = {
        // descriptor: type | base | limit | privilege

        // 0x0 - unused (always faults -- for trapping NULL far pointers)
        SEG_NULL,
        // 0x8 - kernel code segment
        [GD_KT >> 3] = SEG(STA_X | STA_R, 0x0, 0xffffffff, 0),
        // 0x10 - kernel data segment
        [GD_KD >> 3] = SEG(STA_W, 0x0, 0xffffffff, 0),
        // 0x18 - user code segment
        [GD_UT >> 3] = SEG(STA_X | STA_R, 0x0, 0xffffffff, 3),
        // 0x20 - user data segment
        [GD_UD >> 3] = SEG(STA_W, 0x0, 0xffffffff, 3),
        // 0x28 - tss, initialized in trap::initPercpu()
        [GD_TSS0 >> 3] = SEG_NULL
};


// for loading the gdt through lgdt
struct PseudoDesc pseudoDesc = {
        sizeof(gdt) - 1,
        (uintptr_t) gdt
};