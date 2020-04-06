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


// Main initialization of memory
void memoryInit() {
    // Print kernel memory info
    consolePrintFmt("Kernel at 0x%08X -- 0x%08X: %d KB in memory\n",
                    kernStart, kernEnd, (kernEnd - kernStart + 1023) / 1024);

    // Detect total memory and the number of pages in need
    memoryDetect();
    consolePrintFmt("Available physical memory: %d KB = %d MB\n",
                    totalMem, (totalMem + 1023) / 1024);

    pageDirInit();
    pageInit();
}


//
// Physical Page Management
//


// Initialize pageInfoList
void pageInit() {
    // Create page info list
    pageInfoArray = (struct PageInfo *) bootAlloc(sizeof(struct PageInfo) * nPages);
    memoryZero(pageInfoArray, sizeof(struct PageInfo) * nPages);

    // 1. Mark all pages below 1MB as used
    size_t i;
    for (i = 0; i < EXTPHYSMEM / PGSIZE; ++i)
        pageInfoArray[i].refCount = 1;

    // 2. Mark pages used by kernel as used
    size_t nextFreePage = (physaddr_t) PHY_ADDR(bootAlloc(0)) / PGSIZE;
    for (; i < nextFreePage; ++i)
        pageInfoArray[i].refCount = 1;

    // 3. Mark remaining pages as free
    for (; i < nPages; ++i) {
        pageInfoArray[i].refCount = 0;
        pageInfoArray[i].nextFree = pageFreeList;
        pageFreeList = pageInfoArray + i;
    }

    // Do some test
    assert(phyToPage(0xb8000)->refCount > 0);
    assert(phyToPage(PHY_ADDR(pageInfoArray))->refCount > 0);
    assert(phyToPage((totalMem - 4) * 1024u)->refCount == 0);
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
        memoryZero(va, PGSIZE);
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

// Initialize kernelPageDir
void pageDirInit() {
    // Create kernel page dir
    kernelPageDir = (pde_t *) bootAlloc(PGSIZE);
    memoryZero(kernelPageDir, PGSIZE);
    // Map itself to va: UVPT (user virtual page table)
    kernelPageDir[PDX(UVPT)] = PHY_ADDR(kernelPageDir) | PTE_U | PTE_P;
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
        pte_t *pageTable = KERN_ADDR(PTE_ADDR(*pde));
        return pageTable + ptx;
    } else if (!create) {
        return NULL;
    } else {
        // Allocate a new page table page's info
        struct PageInfo *pp = pageAlloc(true);
        if (pp == NULL) { return NULL; }    // Allocation fails
        pp->refCount++;

        // TODO: virtual here?
        memoryZero(pageToKernV(pp), PGSIZE); // Clear the real page

        // Actually insert the page table page into pageDir+idx (*pde)
        *pde = pageToPhy(pp) | PTE_P | PTE_U | PTE_W; // Set permissive flags

        tlbInvalidate(pageDir, (void *) va);

        pte_t *pageTable = KERN_ADDR(PTE_ADDR(*pde));
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


// Adopted from xv6/JOS
//
// Invalidate a TLB entry, but only if the page tables being
// edited are the ones currently in use by the processor.
//
void tlbInvalidate(pde_t *pageDir, void *va) {
    // Flush the entry only if we're modifying the current address space.
    // For now, there is only one address space, so always invalidate.
    invlpg(va);
}