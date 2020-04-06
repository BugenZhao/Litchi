//
// Created by Bugen Zhao on 2020/4/6.
//

#ifndef LITCHI_PMAP_H
#define LITCHI_PMAP_H

#include <kernel/knlast.inc>
#include <include/x86.h>
#include <include/assert.h>
#include <include/memlayout.h>

extern size_t nPages;
extern struct PageInfo *pageInfoArray;

// Adopted from JOS/xv6

#define    MC_NVRAM_START    0xe    /* start of NVRAM: offset 14 */
#define    MC_NVRAM_SIZE    50    /* 50 bytes of NVRAM */

/* NVRAM bytes 7 & 8: base memory size */
#define NVRAM_BASELO    (MC_NVRAM_START + 7)    /* low byte; RTC off. 0x15 */
#define NVRAM_BASEHI    (MC_NVRAM_START + 8)    /* high byte; RTC off. 0x16 */

/* NVRAM bytes 9 & 10: extended memory size (between 1MB and 16MB) */
#define NVRAM_EXTLO    (MC_NVRAM_START + 9)    /* low byte; RTC off. 0x17 */
#define NVRAM_EXTHI    (MC_NVRAM_START + 10)    /* high byte; RTC off. 0x18 */

/* NVRAM bytes 38 and 39: extended memory size (between 16MB and 4G) */
#define NVRAM_EXT16LO    (MC_NVRAM_START + 38)    /* low byte; RTC off. 0x34 */
#define NVRAM_EXT16HI    (MC_NVRAM_START + 39)    /* high byte; RTC off. 0x35 */

#define    IO_RTC        0x070        /* RTC port */

static unsigned mc146818_read(unsigned reg) {
    outb(IO_RTC, reg);
    return inb(IO_RTC + 1);
}

static int nvram_read(int r) {
    return mc146818_read(r) | (mc146818_read(r + 1) << 8);
}

// End


void memoryInit();

// Convert a kernel virtual address to physical
#define PHY_ADDR(kernva) _paddr(__RFILE__, __LINE__, kernva)

static inline physaddr_t _paddr(const char *file, int line, void *kernva) {
    if ((uintptr_t) kernva < KERNBASE)
        _kernelPanic(file, line, "PADDR: cannot convert 0x%08lX to pa", kernva);
    return (physaddr_t) kernva - KERNBASE;
}

// Convert a physical address to kernel virtual address
#define KERN_ADDR(pa) _kaddr(__RFILE__, __LINE__, pa)

static inline void *_kaddr(const char *file, int line, physaddr_t pa) {
    // Out of available memory
    if (PGNUM(pa) >= nPages || nPages == 0)
        _kernelPanic(file, line, "KADDR: cannot convert 0x%08lX to kva", pa);
    return (void *) (pa + KERNBASE);
}

// Convert a PageInfo to physical address
static inline physaddr_t pageToPhy(struct PageInfo *pp) {
    // Shift by 12
    return (pp - pageInfoArray) << PGSHIFT;
}

// Convert a physical address to its PageInfo
static inline struct PageInfo *phyToPage(physaddr_t pa) {
    if (PGNUM(pa) >= nPages || nPages == 0)
        kernelPanic("phyToPage: cannot convert 0x%08lX to PageInfo", pa);
    return pageInfoArray + PGNUM(pa);
}

// Convert a PageInfo to kernel virtual address
static inline void *pageToKernV(struct PageInfo *pp) {
    return KERN_ADDR(pageToPhy(pp));
}

#endif //LITCHI_PMAP_H
