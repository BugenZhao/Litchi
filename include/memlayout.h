// Adopted from JOS/xv6

#ifndef JOS_INC_MEMLAYOUT_H
#define JOS_INC_MEMLAYOUT_H

#ifndef __ASSEMBLER__
#include <include/types.h>
#include <include/mmu.h>
#endif /* not __ASSEMBLER__ */

/*
 * This file contains definitions for memory management in our OS,
 * which are relevant to both the kernel and user-mode software.
 */

// Global descriptor numbers
#define GD_KT     0x08     // kernel text
#define GD_KD     0x10     // kernel data
#define GD_UT     0x18     // user text
#define GD_UD     0x20     // user data
#define GD_TSS0   0x28     // Task segment selector for CPU 0
#define GD_TSS0H  0x30     // FOR LONG MODE tss pointer, see gdt.c

/*
 * Virtual memory map:                                Permissions
 *                                                    kernel/user
 *
 *     HUGE -------->  +------------------------------+
 *                     |                              | RW/--
 *                     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *                     :              .               :
 *                     :              .               :
 *                     :              .               :
 *                     |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~| RW/--
 *                     |                              | RW/--
 *                     |   Remapped Physical Memory   | RW/--
 *                     |                              | RW/--
 *    KERNBASE, ---->  +------------------------------+ 0x1018_00000000     --+
 *    KSTACKTOP        |     CPU0's Kernel Stack      | RW/--  KSTKSIZE       |
 *                     | - - - - - - - - - - - - - - -|                       |
 *                     |      Invalid Memory (*)      | --/--  KSTKGAP        |
 *                     +------------------------------+                       |
 *                     |     CPU1's Kernel Stack      | RW/--  KSTKSIZE       |
 *                     | - - - - - - - - - - - - - - -|                     PTSIZE
 *                     |      Invalid Memory (*)      | --/--  KSTKGAP        |
 *                     +------------------------------+                       |
 *                     :              .               :                       |
 *                     :              .               :                       |
 *    MMIOLIM ------>  +------------------------------+ 0x1017_ffc00000     --+
 *                     |       Memory-mapped I/O      | RW/--  PTSIZE
 * ULIM, MMIOBASE -->  +------------------------------+ 0x1017_ff800000
 *                     |  Cur. Page Table (User R-)   | R-/R-  PTSIZE
 *    UVPT      ---->  +------------------------------+ 0x1017_ff400000
 *                     |           RO ENVS            | R-/R-  PTSIZE
 * UTOP,UENVS ------>  +------------------------------+ 0x1017_ff000000
 *                     :              .               :
 *    UPAGES    ---->  +------------------------------+ 0x1010_00000000
 *                     :              .               :
 * UXSTACKTOP ------>  +------------------------------+ 0xfffff000
 *                     |     User Exception Stack     | RW/RW  PGSIZE
 *                     +------------------------------+ 0xffffe000
 *                     |       Empty Memory (*)       | --/--  PGSIZE
 *    USTACKTOP  --->  +------------------------------+ 0xffffd000
 *                     |      Normal User Stack       | RW/RW  PGSIZE
 *                     +------------------------------+ ...
 *                     |                              |
 *                     |                              |
 *                     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *                     .                              .
 *                     .                              .
 *                     .                              .
 *                     |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
 *                     |     Program Data & Heap      |
 *    UTEXT -------->  +------------------------------+ 0x01000000
 *    PFTEMP ------->  |       Empty Memory (*)       |        2 * PTSIZE
 *                     |                              |
 *    UTEMP -------->  +------------------------------+ 0x00800000      --+
 *                     |       Empty Memory (*)       |                   |
 *                     | - - - - - - - - - - - - - - -|                   |
 *                     |  User STAB Data (optional)   |              2 * PTSIZE
 *    USTABDATA ---->  +------------------------------+ 0x00400000        |
 *                     |       Empty Memory (*)       |                   |
 *    0 ------------>  +------------------------------+                 --+
 *
 * (*) Note: The kernel ensures that "Invalid Memory" is *never* mapped.
 *     "Empty Memory" is normally unmapped, but user programs may map pages
 *     there if desired.  JOS user programs map pages temporarily at UTEMP.
 */


// All physical memory mapped at this address
#define	KERNBASE	0x101800000000
#define KERNLIM     0x101900000000

// At IOPHYSMEM (640K) there is a 384K hole for I/O.  From the kernel,
// IOPHYSMEM can be addressed at KERNBASE + IOPHYSMEM.  The hole ends
// at physical address EXTPHYSMEM
#define IOPHYSMEM	0x0A0000
#define EXTPHYSMEM	0x100000

// Kernel stack.
#define KSTACKTOP	KERNBASE
#define KSTKSIZE	(16*PGSIZE)   		// size of a kernel stack
#define KSTKGAP		(8*PGSIZE)   		// size of a kernel stack guard

// Memory-mapped IO.
#define MMIOLIM		(KSTACKTOP - PTSIZE)
#define MMIOBASE	(MMIOLIM - PTSIZE)

#define ULIM		(MMIOBASE)

/*
 * User read-only mappings! Anything below here til UTOP are readonly to user.
 * They are global pages mapped in at env allocation time.
 */

// User read-only virtual page table (see 'uvpt' below)
#define UVPT		(ULIM - PTSIZE)
// Read-only copies of the Page structures
#define UPAGES		0x101000000000
// Read-only copies of the global env structures
#define UENVS		(UVPT - PTSIZE)

/*
 * Top of user VM. User can manipulate VA from UTOP-1 and down!
 */

// Top of user-accessible VM
#define UTOP		UENVS

// Top of one-page user exception stack
#define UXSTACKTOP	0xfffff000
// Next page left invalid to guard against exception stack overflow; then:
// Top of normal user stack
#define USTACKTOP	(UXSTACKTOP - 2*PGSIZE)

// Where user programs generally begin
#define UTEXT		(4*PTSIZE)

// Used for temporary page mappings.  Typed 'void*' for convenience
#define UTEMP		((void*) (2*PTSIZE))
// Used for temporary page mappings for the user page-fault handler
// (should not conflict with other temporary page mappings)
#define PFTEMP		(UTEMP + 2 * PTSIZE - PGSIZE)
// The location of the user-level STABS data structure
#define USTABDATA	PTSIZE

#ifndef __ASSEMBLER__

typedef uint64_t pml4e_t;   // Page Map Level 4 Entry
typedef uint64_t pdpe_t;    // Page Dir Pointer Entry
typedef uint64_t pde_t;     // Page Dir         Entry
typedef uint64_t pte_t;     // Page Table       Entry

#endif /* !__ASSEMBLER__ */
#endif /* !JOS_INC_MEMLAYOUT_H */
