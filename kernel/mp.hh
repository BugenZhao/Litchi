//
// Created by Bugen Zhao on 7/26/20.
//

#ifndef LITCHI_KERNEL_MP_HH
#define LITCHI_KERNEL_MP_HH

#include <include/types.h>
#include <kernel/task.hh>

namespace mp {
    struct CPUInfo {
        enum struct Status : uint32_t {
            unused = 0,
            started,
            halted
        };

        uint8_t id;
        volatile Status status;
        task::Task *current;
        TaskState ts;
    };

    constexpr auto MAX_CPU_COUNT = 32;

    extern bool isMP;
    extern int cpuCount;
    extern CPUInfo cpus[MAX_CPU_COUNT];
    extern CPUInfo *bootstrapCPU;
}

namespace mp::lapic {
    extern physaddr_t lapicPhy;
    extern volatile uint32_t *lapicKern;
}

namespace mp {
    void init();
}

namespace mp::lapic {
    void init();

    void startAP(uint8_t apicid, uint32_t addr);
}

namespace mp {
    CPUInfo *thisCPU();
}

#endif //LITCHI_KERNEL_MP_HH
