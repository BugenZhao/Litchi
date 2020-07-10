#include <include/mmu.h>
#include <include/memlayout.h>

__attribute__((__aligned__(PGSIZE)))
pde_t entry_pgdir_super[NPDENTRIES] = {
        // Map VA's [0, 4MB) to PA's [0, 4MB), for some use in entry.S
        // PTE_PS: super page
        [0] = (0) | PTE_P | PTE_W | PTE_PS,
        // Map VA's [KERNBASE, KERNBASE+4MB) to PA's [0, 4MB), for kernel before paging initialized
        [KERNBASE >> PDXSHIFT] = (0) | PTE_P | PTE_W | PTE_PS,
};
