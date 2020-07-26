//
// Created by Bugen Zhao on 2020/4/6.
//

#include <include/mmu.h>
#include <include/stdio.hh>
#include <include/x86.h>
#include <include/memlayout.h>
#include <include/string.hh>
#include <kernel/vmem.hh>
#include <kernel/task.hh>

// Kernel memory boundary from linker
extern char kernStart[], kernEnd[];
extern char bootstack[], bootstacktop[]; // from entry.S


// Global data
namespace vmem {
    size_t totalMem = 0;
    size_t nPages = 0;
    size_t nPagesBaseMem = 0;
    pte_t *kernelPageDir;
}

// PageInfo
namespace vmem {
    PageInfo *PageInfo::array = nullptr;
    PageInfo *PageInfo::freeList = nullptr;

    void PageInfo::decRef() {
        if (--refCount <= 0) this->free();
    }

    void PageInfo::free() {
        if (refCount != 0 || nextFree != nullptr) {
            kernelPanic("pageFree: cannot free page #%d", this - PageInfo::array);
        }
        nextFree = PageInfo::freeList;
        PageInfo::freeList = this;
    }

    PageInfo *PageInfo::alloc(bool zero) {
        struct PageInfo *pp = PageInfo::freeList;
        if (pp == nullptr) {
            // Out of free memory
            return nullptr;
        }
        PageInfo::freeList = PageInfo::freeList->nextFree;
        pp->nextFree = nullptr;
        if (zero) {
            void *va = pp->toKernV();
            mem::clear(va, PGSIZE);
        }
        return pp;
    }

    int PageInfo::freeCount() {
        auto p = freeList;
        int count = 0;
        while (p) {
            count++;
            p = p->nextFree;
        }
        return count;
    }
}


// Global functions
namespace vmem {
    static bool bootAllocForbidden = false;

    // Allocate after 'kernEnd'
    void *bootAlloc(size_t size) {
        static char *next = nullptr;
        // First time -> set next at kernEnd
        if (next == nullptr) next = ROUNDUP((char *) kernEnd, PGSIZE);
        if (size == 0) return next;

        assert(!bootAllocForbidden);
        char *result = next;
        next = ROUNDUP(next + size, PGSIZE);
        return result;
    }

    // Detect available memory from NVRAM
    static void detect() {
        size_t baseMem, extMem, ext16Mem;

        // Get memory count from NVRAM in KB
        baseMem = nvram::read(NVRAM_BASELO);        // <1MB
        extMem = nvram::read(NVRAM_EXTLO);          // 1~16MB
        ext16Mem = nvram::read(NVRAM_EXT16LO) * 64; // >16MB

        if (ext16Mem) totalMem = 16 * 1024 + ext16Mem;
        else if (extMem) totalMem = 1 * 1024 + extMem;
        else totalMem = baseMem;

        // Calculate number of pages
        nPages = totalMem / (PGSIZE / 1024);
        nPagesBaseMem = baseMem / (PGSIZE / 1024);
    }

    // Main initialization of memory
    void init() {
        console::out::print("Initializing virtual memory...\n");

        // Detect total memory and the number of pages in need
        detect();
        console::out::print("  Available: %d KB = %d MB\n",
                            totalMem, (totalMem + 1023) / 1024);

        // Allocate space for kernelPageDir
        pgdir::alloc();

        // Build PageInfo::array for all pages
        page::init();
        task::allocArray();
        bootAllocForbidden = true;

        // Setup and load kernelPageDir
        pgdir::init();

        // Print kernel memory info
        char *kernEndCurrent = (char *) bootAlloc(0);
        console::out::print("  Kernel at %p->%p->%p: %d KB in memory\n",
                            kernStart, kernEnd, kernEndCurrent, (kernEndCurrent - kernStart + 1023) / 1024);

        console::out::print("  %<Done\n", WHITE);
    }
}


// Physical Page Management
namespace vmem::page {
    // Initialize pageInfoList
    void init() {
        // Create page info list
        PageInfo::array = (struct PageInfo *) bootAlloc(sizeof(struct PageInfo) * nPages);
        mem::clear(PageInfo::array, sizeof(struct PageInfo) * nPages);

        // 1. Mark all pages below 1MB as used
        size_t i;
        for (i = 0; i < EXTPHYSMEM / PGSIZE; ++i)
            PageInfo::array[i].refCount = 1;

        // 2. Mark pages where kernel resides as used
        size_t nextFreePage = (physaddr_t) PHY_ADDR(bootAlloc(0)) / PGSIZE;
        for (; i < nextFreePage; ++i)
            PageInfo::array[i].refCount = 1;

        size_t j;
        // 3. Mark [4MB, total] as free
        for (j = 4 * 1024 * 1024 / PGSIZE; j < nPages; ++j) {
            PageInfo::array[j].refCount = 0;
            PageInfo::array[j].nextFree = PageInfo::freeList;
            PageInfo::freeList = PageInfo::array + j;
        }

        // CRITICAL:
        // 4. Mark [nextFreePage * PGSIZE, 4MB) as free **AT LAST**,
        //    since we cannot touch higher memory now when we are using entry_pgdir
        for (; i < 4 * 1024 * 1024 / PGSIZE; ++i) {
            PageInfo::array[i].refCount = 0;
            PageInfo::array[i].nextFree = PageInfo::freeList;
            PageInfo::freeList = PageInfo::array + i;
        }

        // Do some test
        assert(PageInfo::fromPhy(0xb8000)->refCount > 0);
        assert(PageInfo::fromPhy(PHY_ADDR(PageInfo::array))->refCount > 0);
        assert(PageInfo::fromPhy((totalMem - 4) * 1024u)->refCount == 0);
        // Make sure we are about to alloc page below 4MB !!!
        assert(PDX(PageInfo::freeList->toPhy()) == 0);
    }
}


// Virtual Memory
namespace vmem::pgdir {
    // Allocate kernelPageDir
    void alloc() {
        // Create kernel page dir
        kernelPageDir = (pde_t *) bootAlloc(PGSIZE);
        mem::clear(kernelPageDir, PGSIZE);

        // Map itself to va: UVPT (user virtual page table)
        kernelPageDir[PDX(UVPT)] = PHY_ADDR(kernelPageDir) | PTE_U | PTE_P;

        //    printFmt("%p %p\n", UVPT, PHY_ADDR(kernelPageDir));
    }

    // Find the page table entry of va, but won't do any mapping
    // If not exist but create == true, func will create one page table / page dir entry
    pte_t *findPte(pde_t *pageDir, const void *va, bool create) {
        uint32_t pdx = PDX(va);     // Page directory index
        uint32_t ptx = PTX(va);     // Page table index
        pde_t *pde = pageDir + pdx; // Page directory entry

        if ((*pde) & PTE_P) {
            // Entry present
            // Clear the least significant 10 bits (flags) to get the pt address,
            //  then convert it to virtual address in order to access
            pte_t *pageTable = (pte_t *) KERN_ADDR(PTE_ADDR(*pde));
            return pageTable + ptx;
        } else if (!create) {
            return nullptr;
        } else {
            // Allocate a new **PAGE TABLE** page's info
            struct PageInfo *pp = PageInfo::alloc(true);
            if (pp == nullptr) { return nullptr; }    // Allocation fails
            pp->refCount++;

            mem::clear(pp->toKernV(), PGSIZE); // Clear the real page

            // Actually insert the page table page into pageDir+idx (*pde)
            *pde = pp->toPhy() | PTE_P | PTE_U | PTE_W; // Set permissive flags

            invalidateTLB(pageDir, (void *) va);

            pte_t *pageTable = (pte_t *) KERN_ADDR(PTE_ADDR(*pde));
            return pageTable + ptx;
        }
    }

    // Lookup physical page mapped at va
    std::tuple<PageInfo *, pte_t *> findInfo(pde_t *pageDir, const void *va) {
        pte_t *pte = findPte(pageDir, va, 0);
        // No page or page not present
        if (pte == nullptr || !((*pte) & PTE_P)) return {nullptr, nullptr};
        // Get pa from pte, then get page info from pa
        return {PageInfo::fromPhy(PTE_ADDR(*pte)), pte };
    }

    // Remove physical page mapped at va
    void remove(pde_t *pageDir, void *va) {
        auto [pp, pte] = findInfo(pageDir, va);
        if (pp && (*pte) & PTE_P) {
            // Dec the ref, and mark the physical page free if necessary
            pp->decRef();
            // Clear page table entry
            *pte = 0;
            // Invalidate TLB since we removed an entry
            invalidateTLB(pageDir, va);
        }
    }

    // Map the physical page 'pp' at va, with permission bits 'perm'
    Result insert(pde_t *pageDir, struct PageInfo *pp, void *va, int perm) {
        pte_t *pte = findPte(pageDir, va, 1);
        if (pte == nullptr) return Result::fuck;

        // First, inc refCount of pp
        pp->refCount++;
        if ((*pte) & PTE_P) {
            // Entry already present
            // Corner-case won't cause bug since we've already inc the ref count,
            //  then it (pp) won't be freed.
            remove(pageDir, va);
        }
        *pte = pp->toPhy() | perm | PTE_P;
        // Invalidate TLB since we may have updated an entry
        invalidateTLB(pageDir, va);
        return Result::ok;
    }

    // Statically map [va, va+size) to [pa, pa+size)
    // It will not make changes on PageInfo::array
    void staticMap(pde_t *pageDir, uintptr_t va, size_t size, physaddr_t pa, int perm) {
        uint32_t offset = 0;
        // There might be multiple pte's to find and set
        while (offset < size) {
            // Get the page table entry
            pte_t *pte = findPte(pageDir, (const void *) (va + offset), 1);
            // Actually set the pte to map the page
            *pte = (pa + offset) | PTE_P | perm;
            invalidateTLB(pageDir, (void *) (va + offset));
            offset += PGSIZE;
        }
    }

    static void init() {
        // Map PageInfo::array at va UPAGES for user (read-only)
        staticMap(kernelPageDir, UPAGES, PTSIZE, PHY_ADDR(PageInfo::array), PTE_U);
        // Map kernel stack at va KSTACKTOP-KSTKSIZE
        staticMap(kernelPageDir, KSTACKTOP - KSTKSIZE, KSTKSIZE, PHY_ADDR(bootstack), PTE_W);
        // Map all of physical memory in [0, 2^32 - KERNBASE) at va KERNBASE
        staticMap(kernelPageDir, KERNBASE, ~KERNBASE, 0, PTE_W);

        // Actually load page dir
        x86::lcr3(PHY_ADDR(kernelPageDir));

        // Set other cr0 flags
        uint32_t cr0 = x86::rcr0();
        cr0 |= CR0_PE | CR0_PG | CR0_AM | CR0_WP | CR0_NE | CR0_MP;
        cr0 &= ~(CR0_TS | CR0_EM);
        x86::lcr0(cr0);

        // Do some checks
        assert(*(uint64_t *) UPAGES == *(uint64_t *) PageInfo::array);
        assert(*(uint32_t *) (KSTACKTOP - (bootstacktop - (char *) &cr0)) == cr0);

        struct PageInfo *pp0, *pp1;
        void *va2gb = (void *) 0x80000000;

        assert(pp0 = PageInfo::alloc(false));
        mem::set(pp0->toKernV(), 0x18, PGSIZE);
        insert(kernelPageDir, pp0, va2gb, PTE_W);
        assert(pp0->refCount == 1);
        assert(*(int64_t *) va2gb == 0x1818181818181818);

        assert(pp1 = PageInfo::alloc(false));
        mem::set(pp1->toKernV(), 0x10, PGSIZE);
        insert(kernelPageDir, pp1, va2gb, PTE_W);
        assert(pp0->refCount == 0);
        assert(pp0->nextFree != nullptr);
        assert(*(int64_t *) va2gb == 0x1010101010101010);
        remove(kernelPageDir, va2gb);
        assert(pp1->refCount == 0);
        assert(pp1->nextFree != nullptr);
    }

    // Adopted from xv6/JOS
    // Invalidate a TLB entry, but only if the page tables being
    // edited are the ones currently in use by the processor.
    //
    void invalidateTLB(pde_t *pageDir, void *va) {
        // Flush the entry only if we're modifying the current address space.
        // For now, there is only one address space, so always invalidate.
        x86::invlpg(va);
    }


    // some user task may trick the kernel, using syscall to access kernel space
    // use this to check if [va, va + size) all satisfy the perm
    bool userCheck(pde_t *pageDir, const void *va, size_t size, int perm) {
        perm = perm | PTE_P;
        auto va_it = (uint32_t) va;

        for (; va_it < (uint32_t) va + size; va_it += PGSIZE) {
            // out of user limit
            if ((uintptr_t) va_it >= ULIM) goto bad;
            // perm not satisfied
            pte_t *pte = findPte(pageDir, (const void *) va_it, false);
            if (pte == NULL) goto bad;
            if (!(*pte & perm)) goto bad;
        }
        return true;
        bad:
        return false;
    }
}


// Utilities
namespace vmem::utils {
    // Show memory map at va: [beginV, endV]
    void show(pte_t *pageDir, void *beginV, void *endV) {
        beginV = ROUNDDOWN(beginV, PGSIZE);
        endV = ROUNDDOWN(endV, PGSIZE);
        char flagsBuf[13];
        console::out::print("VIRTUAL     PHYSICAL  RC  FLAGS\n");
        for (uintptr_t va = (uintptr_t) beginV; va <= (uintptr_t) endV; va += PGSIZE) {
            console::out::print("%p -> ", va);
            pte_t *pte = pgdir::findPte(pageDir, (void *) va, 0);
            if (pte == nullptr) console::err::print("<NOT MAPPED>\n");
            else {
                physaddr_t phy = PTE_ADDR(*pte);
                if (PGNUM(phy) >= nPages)
                    console::err::print("<NOT EXIST>\n");
                else
                    console::out::print("%p  %2d  %s\n", phy, PageInfo::fromPhy(phy)->refCount,
                                        flagStr(pte, flagsBuf));
            }
        }
    }

    // Show memory map at va: [beginV, endV]
    void dumpV(pte_t *pageDir, void *beginV, void *endV) {
        beginV = ROUNDDOWN(beginV, 16);
        endV = ROUNDUP(endV, 16);
        console::out::print("VIRTUAL   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
        for (void *row = beginV; row <= endV; row = (void *) ((uintptr_t) row + 16)) {
            console::out::print("%p", row);
            pte_t *pte = pgdir::findPte(pageDir, row, 0);
            if (pte == nullptr) {
                console::err::print("  <INVALID>\n");
                return;
            } else if (!(*pte & PTE_P)) {
                console::err::print("  <NOT PRESENT>\n");
                return;
            }
            for (uint8_t *va = static_cast<uint8_t *>(row); (void *) va <= (void *) ((uintptr_t) row + 15); ++va) {
                console::out::print(" %02X", (uint32_t) *va);
            }
            console::out::print("\n");
        }
    }

    // Show memory map at pa: [beginP, endP]
    void dumpP(pte_t *pageDir, physaddr_t beginP, physaddr_t endP) {
        using namespace console;

        beginP = ROUNDDOWN(beginP, 16);
        endP = ROUNDUP(endP, 16);
        const size_t _totalMem = totalMem * 1024u;
        out::print("PHYSICAL  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
        for (physaddr_t row = beginP; row <= endP; row = (physaddr_t) ((uintptr_t) row + 16)) {
            out::print("%p", row);
            for (physaddr_t pa = row; pa <= row + 15; ++pa) {
                if (pa >= _totalMem) {
                    err::print("  <INVALID>\n");
                    return;
                }
                out::print(" %02X", (uint32_t) (*(uint8_t *) KERN_ADDR(pa)));
            }
            out::print("\n");
        }
    }

    char *flagStr(pte_t *entry, char *buf) {
        if (entry == nullptr) return nullptr;
        static const char *str[] = {"_________SR_", "AVLGPDACTUWP"};
        int i;
        for (i = 0; i < 12; ++i) {
            buf[i] = str[((*entry) >> (11 - i)) & 0x1][i];
        }
        buf[i] = '\0';
        return buf;
    }
}
