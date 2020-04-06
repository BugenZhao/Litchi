//
// Created by Bugen Zhao on 2020/4/6.
//

#include <kernel/pmap.h>
#include <include/mmu.h>
#include <include/stdio.h>
#include <include/x86.h>
#include <include/memlayout.h>
#include <include/string.h>

size_t totalMem = 0;
size_t nPages = 0;
size_t nPagesBaseMem = 0;

pte_t *kernelPageDir;

// PageInfo
struct PageInfo *pageInfoArray;
static struct PageInfo *pageFreeList;

// Kernel memory boundary from linker
extern char kernStart[], kernEnd[];

// Allocate after 'kernEnd'
static void *bootAlloc(size_t size) {
    static char *next = NULL;
    // First time -> set next at kernEnd
    if (next == NULL) next = ROUNDUP((char *) kernEnd, PGSIZE);

    char *result = next;
    next = ROUNDUP(next + size, PGSIZE);
    return result;
}

// Detect available memory from NVRAM
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
    nPagesBaseMem = baseMem / (PGSIZE / 1024);
}

// Initialize kernelPageDir
void pageDirInit() {
    // Create kernel page dir
    kernelPageDir = (pde_t *) bootAlloc(PGSIZE);
    memoryZero(kernelPageDir, PGSIZE);
    // Map itself to va: UVPT (user virtual page table)
    kernelPageDir[PDX(UVPT)] = PHY_ADDR(kernelPageDir) | PTE_U | PTE_P;
}

// Initialize pageInfoList
void pageInfoInit() {
    // Create page info list
    pageInfoArray = (struct PageInfo *) bootAlloc(sizeof(struct PageInfo) * nPages);
    memoryZero(pageInfoArray, sizeof(struct PageInfo) * nPages);

    // 1. Mark all pages below 1MB as used
    size_t i;
    for (i = 0; i < EXTPHYSMEM / PGSIZE; ++i)
        pageInfoArray[i].refCount = 1;

    // 2. Mark pages used by kernel as used
    size_t nextFreePage = (physaddr_t) PHY_ADDR(bootAlloc(0)) / PGSIZE;
    for (i; i < nextFreePage; ++i)
        pageInfoArray[i].refCount = 1;

    // 3. Mark remaining pages as free
    for (i; i < nPages; ++i) {
        pageInfoArray[i].refCount = 0;
        pageInfoArray[i].nextFree = pageFreeList;
        pageFreeList = pageInfoArray + i;
    }

    // Do some test
    assert(phyToPage(0xb8000)->refCount > 0);
    assert(phyToPage(PHY_ADDR(pageInfoArray))->refCount > 0);
    assert(phyToPage((totalMem - 4) * 1024u)->refCount == 0);
}

// Initialize memory
void memoryInit() {
    // Print kernel memory info
    consolePrintFmt("Kernel at 0x%08X -- 0x%08X: %d KB in memory\n",
                    kernStart, kernEnd, (kernEnd - kernStart + 1023) / 1024);

    // Detect total memory and the number of pages in need
    memoryDetect();
    consolePrintFmt("Available physical memory: %d KB = %d MB\n",
                    totalMem, (totalMem + 1023) / 1024);

    pageDirInit();
    pageInfoInit();
}