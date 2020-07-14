//
// Created by Bugen Zhao on 7/13/20.
//

#ifndef LITCHI_INCLUDE_TRAP_HH
#define LITCHI_INCLUDE_TRAP_HH

#include <include/string.hpp>
#include <include/panic.hpp>
#include <include/mmu.h>
#include <include/memlayout.h>

namespace trap {
    enum class TrapType : uint32_t {
        divide = 0,
        debug,
        nmi,
        breakpoint,
        overflow,
        bound,
        illegalOp,
        device,
        doubleFault,
        coprocessor,    // reserved
        invalidTss,     // invalid task switch segment
        segmentNP,      // segment not present
        stack,
        gpFault,        // general protection fault
        pageFault,
        /* reserved, */
        fpError = 16,
        alignment,
        machineCheck,
        simdError,

        syscall = 88
    };
}

#endif //LITCHI_INCLUDE_TRAP_HH
