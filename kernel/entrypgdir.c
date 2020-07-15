#include <include/mmu.h>
#include <include/memlayout.h>

__attribute__((__aligned__(PGSIZE)))
pde_t entry_pde[NPDPENTRIES] = {
        [0] = 0 + PTE_P + PTE_W + PTE_PS,               // [0, 2MB) super page
        [1] = (1 << PDXSHIFT) + PTE_P + PTE_W + PTE_PS, // [2, 4MB) super page

//        [PDX(KERNBASE)] = 0 + PTE_P + PTE_W + PTE_PS,
};

__attribute__((__aligned__(PGSIZE)))
pdpe_t entry_pdp[NPDPENTRIES] = {
        [0] = ((uintptr_t) entry_pde) + PTE_P + PTE_W,
        [PDPX(KERNBASE)] = ((uintptr_t) entry_pde) + PTE_P + PTE_W,
};

__attribute__((__aligned__(PGSIZE)))
pml4e_t entry_pml4[NPML4ENTRIES] = {
        // Map VA's [0, 512GB) to PA's [0, 512GB), for some use in entry.S
        [0] = ((uintptr_t) entry_pdp) + PTE_P + PTE_W,
        // Map VA's [KERNBASE, KERNBASE+512GB) to PA's [0, 512G), for kernel before paging initialized
        [PML4X(KERNBASE)] = ((uintptr_t) entry_pdp) + PTE_P + PTE_W,
};
