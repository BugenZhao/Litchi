//
// Created by Bugen Zhao on 2020/4/6.
//

#include "pmap.h"
#include <mmu.h>
#include <stdio.h>
#include <x86.h>
#include <memlayout.h>
#include <string.h>

size_t totalMem = 0;
size_t nPages = 0;
size_t nPagesBaseMem = 0;

pte_t *kernelPageDir;

// PageInfo
struct PageInfo *pageInfoArray;
static struct PageInfo *pageFreeList;

// Kernel memory boundary from linker
extern char kernStart[], kernEnd[];

static bool bootAllocForbidden = false;

// Allocate after 'kernEnd'
static void *bootAlloc(size_t size) {
    static char *next = NULL;
    // First time -> set next at kernEnd
    if (next == NULL) next = ROUNDUP((char *) kernEnd, PGSIZE);
    if (size == 0) return next;

    assert(bootAllocForbidden == false);
    char *result = next;
    next = ROUNDUP(next + size, PGSIZE);
    return result;
}

// Detect available memory from NVRAM
static void vmemoryDetect() {
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


// Main initialization of memory
void vmemoryInit() {
    // Detect total memory and the number of pages in need
    vmemoryDetect();
    console::out::print("Available physical memory: %d KB = %d MB\n",
                        totalMem, (totalMem + 1023) / 1024);

    console::out::print("Setting up virtual memory...");
    // Allocate space for kernelPageDir
    pageDirAlloc();

    // Build pageInfoArray for all pages
    pageInit();
    bootAllocForbidden = true;

    // Setup and load kernelPageDir
    pageDirSetup();
    console::out::print("Done\n");

    // Print kernel memory info
    char *kernEndCurrent = (char *)bootAlloc(0);
    console::out::print("Kernel at 0x%08X->0x%08X->0x%08X: %d KB in memory\n",
                        kernStart, kernEnd, kernEndCurrent, (kernEndCurrent - kernStart + 1023) / 1024);
}


//
// Physical Page Management
//


// Initialize pageInfoList
void pageInit() {
    // Create page info list
    pageInfoArray = (struct PageInfo *) bootAlloc(sizeof(struct PageInfo) * nPages);
    mem::clear(pageInfoArray, sizeof(struct PageInfo) * nPages);

    // 1. Mark all pages below 1MB as used
    size_t i;
    for (i = 0; i < EXTPHYSMEM / PGSIZE; ++i)
        pageInfoArray[i].refCount = 1;

    // 2. Mark pages where kernel resides as used
    size_t nextFreePage = (physaddr_t) PHY_ADDR(bootAlloc(0)) / PGSIZE;
    for (; i < nextFreePage; ++i)
        pageInfoArray[i].refCount = 1;

    size_t j;
    // 3. Mark [4MB, total] as free
    for (j = 4 * 1024 * 1024 / PGSIZE; j < nPages; ++j) {
        pageInfoArray[j].refCount = 0;
        pageInfoArray[j].nextFree = pageFreeList;
        pageFreeList = pageInfoArray + j;
    }

    // CRITICAL:
    // 4. Mark [nextFreePage * PGSIZE, 4MB) as free **AT LAST**,
    //    since we cannot touch higher memory now when we are using entry_pgdir
    for (; i < 4 * 1024 * 1024 / PGSIZE; ++i) {
        pageInfoArray[i].refCount = 0;
        pageInfoArray[i].nextFree = pageFreeList;
        pageFreeList = pageInfoArray + i;
    }

    // Do some test
    assert(phyToPage(0xb8000)->refCount > 0);
    assert(phyToPage(PHY_ADDR(pageInfoArray))->refCount > 0);
    assert(phyToPage((totalMem - 4) * 1024u)->refCount == 0);
    // Make sure we are about to alloc page below 4MB !!!
    assert(PDX(pageToPhy(pageFreeList)) == 0);


}


// Allocate a physical page
struct PageInfo *pageAlloc(bool zero) {
    struct PageInfo *pp = pageFreeList;
    if (pp == NULL) {
        // Out of free memory
        return NULL;
    }
    pageFreeList = pageFreeList->nextFree;
    pp->nextFree = NULL;
    if (zero) {
        void *va = pageToKernV(pp);
        mem::clear(va, PGSIZE);
    }
    return pp;
}


// Free a physical page
void pageFree(struct PageInfo *pp) {
    if (pp == NULL) return;
    if (pp->refCount != 0 || pp->nextFree != NULL) {
        kernelPanic("pageFree: cannot free page #%d", pp - pageInfoArray);
    }
    pp->nextFree = pageFreeList;
    pageFreeList = pp;
}

// Dec refCount and free it if refCount is 0
void pageDecRef(struct PageInfo *pp) {
    if (pp == NULL) return;
    if (--pp->refCount <= 0) pageFree(pp);
}


//
// Virtual Memory
//

// Allocate kernelPageDir
void pageDirAlloc() {
    // Create kernel page dir
    kernelPageDir = (pde_t *) bootAlloc(PGSIZE);
    mem::clear(kernelPageDir, PGSIZE);
    // Map itself to va: UVPT (user virtual page table)
    kernelPageDir[PDX(UVPT)] = PHY_ADDR(kernelPageDir) | PTE_U | PTE_P;

//    printFmt("%08X %08X\n", UVPT, PHY_ADDR(kernelPageDir));
}

// Find the page table entry of va
// If not exist but create == true, func will create one page table / page dir entry
pte_t *pageDirFindPte(pde_t *pageDir, const void *va, bool create) {
    uint32_t pdx = PDX(va);     // Page directory index
    uint32_t ptx = PTX(va);     // Page table index
    pde_t *pde = pageDir + pdx; // Page directory entry

    if ((*pde) & PTE_P) {
        // Entry present
        // Clear the least significant 10 bits (flags) to get the pt address,
        //  then convert it to virtual address in order to access
        pte_t *pageTable = (pte_t *)KERN_ADDR(PTE_ADDR(*pde));
        return pageTable + ptx;
    } else if (!create) {
        return NULL;
    } else {
        // Allocate a new page table page's info
        struct PageInfo *pp = pageAlloc(true);
        if (pp == NULL) { return NULL; }    // Allocation fails
        pp->refCount++;

        // TODO: virtual here?
        mem::clear(pageToKernV(pp), PGSIZE); // Clear the real page

        // Actually insert the page table page into pageDir+idx (*pde)
        *pde = pageToPhy(pp) | PTE_P | PTE_U | PTE_W; // Set permissive flags

        tlbInvalidate(pageDir, (void *) va);

        pte_t *pageTable = (pte_t *)KERN_ADDR(PTE_ADDR(*pde));
        return pageTable + ptx;
    }
}


// Lookup physical page mapped at va, and store pte if pteStore is not NULL
struct PageInfo *pageDirFindInfo(pde_t *pageDir, const void *va, pte_t **pteStore) {
    pte_t *pte = pageDirFindPte(pageDir, va, 0);
    // No page or page not present
    if (pte == NULL || !((*pte) & PTE_P)) return NULL;
    if (pteStore) *pteStore = pte;
    // Get pa from pte, then get page info from pa
    return phyToPage(PTE_ADDR(*pte));
}


// Remove physical page mapped at va
void pageDirRemove(pde_t *pageDir, void *va) {
    pte_t *pte;
    struct PageInfo *pp = pageDirFindInfo(pageDir, va, &pte);
    if (pp && (*pte) & PTE_P) {
        // Dec the ref, and mark the physical page free if necessary
        pageDecRef(pp);
        // Clear page table entry
        *pte = 0;
        // Invalidate TLB since we removed an entry
        tlbInvalidate(pageDir, va);
    }
}


// Map the physical page 'pp' at va, with permission bits 'perm'
int pageDirInsert(pde_t *pageDir, struct PageInfo *pp, void *va, int perm) {
    pte_t *pte = pageDirFindPte(pageDir, va, 1);
    if (pte == NULL) { return -1; }

    // First, inc refCount of pp
    pp->refCount++;
    if ((*pte) & PTE_P) {
        // Entry already present
        // Corner-case won't cause bug since we've already inc the ref count,
        //  then it (pp) won't be freed.
        pageDirRemove(pageDir, va);
    }
    *pte = pageToPhy(pp) | perm | PTE_P;
    // Invalidate TLB since we may have updated an entry
    tlbInvalidate(pageDir, va);
    return 0;
}

// Statically map [va, va+size) to [pa, pa+size)
// It will not make changes on pageInfoArray
static void bootMap(pde_t *pageDir, uintptr_t va, size_t size, physaddr_t pa, int perm) {
    uint32_t offset = 0;
    // There might be multiple pte's to find and set
    while (offset < size) {
        // Get the page table entry
        pte_t *pte = pageDirFindPte(pageDir, (const void *) (va + offset), 1);
        // Actually set the pte to map the page
        *pte = (pa + offset) | PTE_P | perm;
        tlbInvalidate(pageDir, (void *) (va + offset));
        offset += PGSIZE;
    }
}

static void pageDirSetup() {
    extern char bootstack[], bootstacktop[]; // from entry.S

    // Map pageInfoArray at va UPAGES for user (read-only)
    bootMap(kernelPageDir, UPAGES, PTSIZE, PHY_ADDR(pageInfoArray), PTE_U);
    // Map kernel stack at va KSTACKTOP-KSTKSIZE
    bootMap(kernelPageDir, KSTACKTOP - KSTKSIZE, KSTKSIZE, PHY_ADDR(bootstack), PTE_W);
    // Map all of physical memory in [0, 2^32 - KERNBASE) at va KERNBASE
    bootMap(kernelPageDir, KERNBASE, ~KERNBASE, 0, PTE_W);

    // Actually load page dir
    lcr3(PHY_ADDR(kernelPageDir));

    // Set other cr0 flags
    uint32_t cr0 = rcr0();
    cr0 |= CR0_PE | CR0_PG | CR0_AM | CR0_WP | CR0_NE | CR0_MP;
    cr0 &= ~(CR0_TS | CR0_EM);
    lcr0(cr0);

    // Do some checks
    assert(*(uint64_t *) UPAGES == *(uint64_t *) pageInfoArray);
    assert(*(uint32_t *) (KSTACKTOP - (bootstacktop - (char *) &cr0)) == cr0);

    struct PageInfo *pp0, *pp1;
    void *va2gb = (void *) 0x80000000;

    assert(pp0 = pageAlloc(false));
    mem::set(pageToKernV(pp0), 0x18, PGSIZE);
    pageDirInsert(kernelPageDir, pp0, va2gb, PTE_W);
    assert(pp0->refCount == 1);
    assert(*(int64_t *) va2gb == 0x1818181818181818);

    assert(pp1 = pageAlloc(false));
    mem::set(pageToKernV(pp1), 0x10, PGSIZE);
    pageDirInsert(kernelPageDir, pp1, va2gb, PTE_W);
    assert(pp0->refCount == 0);
    assert(pp0->nextFree != NULL);
    assert(*(int64_t *) va2gb == 0x1010101010101010);
    pageDirRemove(kernelPageDir, va2gb);
    assert(pp1->refCount == 0);
    assert(pp1->nextFree != NULL);
}


// Adopted from xv6/JOS
// Invalidate a TLB entry, but only if the page tables being
// edited are the ones currently in use by the processor.
//
void tlbInvalidate(pde_t *pageDir, void *va) {
    // Flush the entry only if we're modifying the current address space.
    // For now, there is only one address space, so always invalidate.
    invlpg(va);
}


// Show memory map at va: [beginV, endV]
void vmemoryShow(pte_t *pageDir, void *beginV, void *endV) {
    beginV = ROUNDDOWN(beginV, PGSIZE);
    endV = ROUNDDOWN(endV, PGSIZE);
    void *va;
    char flagsBuf[13];
    console::out::print("VIRTUAL     PHYSICAL  RC  FLAGS\n");
    for (va = beginV; va <= endV; va += PGSIZE) {
        console::out::print("%08lX -> ", va);
        pte_t *pte = pageDirFindPte(pageDir, va, 0);
        if (pte == NULL) console::err::print("<NOT MAPPED>\n");
        else {
            physaddr_t phy = PTE_ADDR(*pte);
            if (PGNUM(phy) >= nPages)
                console::err::print("<NOT EXIST>\n");
            else
                console::out::print("%08lX  %2d  %s\n", phy, phyToPage(phy)->refCount,
                                    pageFlagsToStr(pte, flagsBuf));
        }
    }
}

// Show memory map at va: [beginV, endV]
void vmemoryDumpV(pte_t *pageDir, void *beginV, void *endV) {
    beginV = ROUNDDOWN(beginV, 16);
    endV = ROUNDUP(endV, 16);
    console::out::print("VIRTUAL   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
    for (void *row = beginV; row <= endV; row += 16) {
        console::out::print("%08lX", row);
        pte_t *pte = pageDirFindPte(pageDir, row, 0);
        if (pte == NULL) {
            console::err::print("  <INVALID>\n");
            return;
        } else if (!(*pte & PTE_P)) {
            console::err::print("  <NOT PRESENT>\n");
            return;
        }
        for (uint8_t *va = static_cast<uint8_t *>(row); (void *) va <= row + 15; ++va) {
            console::out::print(" %02X", (uint32_t) *va);
        }
        console::out::print("\n");
    }
}

// Show memory map at pa: [beginP, endP]
void vmemoryDumpP(pte_t *pageDir, physaddr_t beginP, physaddr_t endP) {
    beginP = ROUNDDOWN(beginP, 16);
    endP = ROUNDUP(endP, 16);
    const size_t _totalMem = totalMem * 1024u;
    console::out::print("PHYSICAL  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
    for (physaddr_t row = beginP; row <= endP; row += 16) {
        console::out::print("%08lX", row);
        for (physaddr_t pa = row; pa <= row + 15; ++pa) {
            if (pa >= _totalMem) {
                console::err::print("  <INVALID>\n");
                return;
            }
            console::out::print(" %02X", (uint32_t) (*(uint8_t *) KERN_ADDR(pa)));
        }
        console::out::print("\n");
    }
}


char *pageFlagsToStr(pte_t *entry, char *buf) {
    if (entry == NULL) return NULL;
    static const char *str[] = {"_________SR_", "AVLGPDACTUWP"};
    int i;
    for (i = 0; i < 12; ++i) {
        buf[i] = str[((*entry) >> (11 - i)) & 0x1][i];
    }
    buf[i] = '\0';
    return buf;
}