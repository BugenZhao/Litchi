//
// Created by Bugen Zhao on 2020/4/6.
//

#include <kernel/pmap.h>
#include <include/mmu.h>
#include <include/stdio.h>
#include <include/x86.h>

size_t totalMem = 0;
size_t nPages = 0;

static void memoryDetect() {
    size_t baseMem, extMem, ext16Mem;

    // Get memory count from NVRAM in KB
    baseMem = nvram_read(NVRAM_BASELO);          // <1MB
    extMem = nvram_read(NVRAM_EXTLO);            // 1~16MB
    ext16Mem = nvram_read(NVRAM_EXT16LO) * 64;   // >16MB

    if (ext16Mem) totalMem = 16 * 1024 + ext16Mem;
    else if (extMem) totalMem = 1 * 1024 + extMem;
    else totalMem = baseMem;

    // Calculate number of pages
    nPages = totalMem / (PGSIZE / 1024);
}

void memoryInit() {
    // Print kernel memory info
    extern char kernStart[], kernEnd[];
    consolePrintFmt("Kernel at 0x%08X -- 0x%08X: %d KB in memory\n",
                    kernStart, kernEnd, (kernEnd - kernStart + 1023) / 1024);

    // Detect total memory and the number of pages in need
    memoryDetect();
    consolePrintFmt("Available physical memory: %d KB = %d MB\n",
                    totalMem, (totalMem + 1023) / 1024);
}