//
// Created by Bugen Zhao on 7/26/20.
//

#include <kernel/mp.hh>
#include <include/stdio.hh>
#include <include/string.hh>
#include <kernel/vmem.hh>
#include <include/trap.hh>

namespace mp {
    struct FPStruct;
    struct ConfTable;
    struct ProcessorAPIC;
    struct IOAPIC;

    struct FPStruct {
        char signature[4];              // "_MP_"
        physaddr_t configuration_table;
        uint8_t length;                 // In 16 bytes (e.g. 1 = 16 bytes, 2 = 32 bytes)
        uint8_t mp_specification_revision;
        uint8_t checksum;               // This value should make all bytes in the table equal 0 when added together
        uint8_t default_configuration;  // If this is not zero then configuration_table should be
        // ignored and a default configuration should be loaded instead
        uint32_t features;              // If bit 7 is then the IMCR is present and PIC mode is being used, otherwise
        // virtual wire mode is; all other bits are reserved
    } __attribute__((__packed__));

    struct ConfTable {
        char signature[4];          // "PCMP"
        uint16_t length;
        uint8_t mp_specification_revision;
        uint8_t checksum;           // Again, the byte should be all bytes in the table add up to 0
        char oem_id[8];
        char product_id[12];
        physaddr_t oem_table;
        uint16_t oem_table_size;
        uint16_t entry_count;       // This value represents how many entries are following this table
        physaddr_t lapic_address;   // This is the memory mapped address of the local APICs
        uint16_t extended_table_length;
        uint8_t extended_table_checksum;
        uint8_t reserved;
    } __attribute__((__packed__));

    enum APICType : uint8_t {
        MPPROC,     // One per processor
        MPBUS,      // One per bus
        MPIOAPIC,   // One per I/O APIC
        MPIOINTR,   // One per bus interrupt source
        MPLINTR     // One per system interrupt source
    };

    // sizeof == 20
    struct ProcessorAPIC {
        APICType type; // Always 0
        uint8_t local_apic_id;
        uint8_t local_apic_version;
        uint8_t flags; // If bit 0 is clear then the processor must be ignored
        // If bit 1 is set then the processor is the bootstrap processor
        uint32_t signature;
        uint32_t feature_flags;
        uint64_t reserved;
    } __attribute__((__packed__));

    // sizeof == 8
    struct IOAPIC {
        APICType type; // Always 2
        uint8_t id;
        uint8_t version;
        uint8_t flags; // If bit 0 is set then the entry should be ignored
        uint32_t address; // The memory mapped address of the IO APIC is memory
    } __attribute__((__packed__));

    template<typename T>
    static bool checksumOk(const T *addr, size_t len = sizeof(T)) {
        uint8_t sum = 0;
        for (size_t i = 0; i < len; ++i) {
            sum += ((uint8_t *) addr)[i];
        }
        return sum == 0;
    }

    // Search for the Floating Pointer Structure
    static FPStruct *searchFP() {
        // Search FPStruct in [addr, addr+len)
        auto search = [](physaddr_t addr, size_t len) -> FPStruct * {
            auto end = addr + len;
            for (; addr < end; addr += sizeof(FPStruct)) {
                auto fp = static_cast<FPStruct *>(KERN_ADDR(addr));
                // check signature and checksum
                if (mem::cmp(fp->signature, "_MP_", 4) == 0 && checksumOk(fp)) {
                    console::out::print("  Found FPStruct at %p\n", fp);
                    return fp;
                }
            }
            return nullptr;
        };

        // 1. Check the first KB of the Extended BIOS Data Area (EBDA)
        constexpr uint16_t bdaSeg = 0x40;
        auto bda = (uint8_t *) KERN_ADDR(bdaSeg << 4);
        if (auto ebdaSeg = *(uint16_t *) (bda + 0x0e); ebdaSeg) {   // EBDA exists
            auto ebdaPhy = ebdaSeg << 4;
            if (auto fp = search(ebdaPhy, 1024); fp) return fp;
        }

        // 2. No EBDA, check the last KB of system base memory
        auto phy = *(uint16_t *) (bda + 0x13) * 1024;
        if (auto fp = search(phy - 1024, 1024); fp) return fp;

        // 3. Check the BIOS read-only memory space between 0xe0000 and 0xfffff
        return search(0xe0000, 0xfffff - 0xe0000);
    }
}

namespace mp {
    int cpuCount = 0;
    CPUInfo cpus[MAX_CPU_COUNT] = {};
    CPUInfo *bootstrapCPU = nullptr;
    bool isMP = false;
}

namespace mp {
    void init() {
        console::out::print("Initializing multi-processor...\n");

        auto fp = searchFP();
        if (fp == nullptr) {
            console::out::print("No multi-processor support.\n");
            cpuCount = 1;
            cpus[0].id = 0;
            cpus[0].status = CPUInfo::Status::started;
            bootstrapCPU = &cpus[0];
            return;
        }

        // get Configuration Table
        auto conf = (ConfTable *) KERN_ADDR(fp->configuration_table);
        assert(conf != nullptr);                            // no default configuration support
        assert(mem::cmp(conf->signature, "PCMP", 4) == 0);  // signature
        assert(checksumOk(conf, conf->length));             // base checksum
        assert(checksumOk((uint8_t *) conf + conf->length, conf->extended_table_length));   // extended checksum

        // APIC OEM ID
        char oem[9] = {};
        mem::copy(oem, conf->oem_id, sizeof(conf->oem_id));
        console::out::print("  MP Conf Table OEM: %s\n", oem);

        // save local APIC MMIO physical address
        lapic::lapicPhy = conf->lapic_address;

        // **After** the table, there are entry_count entries describing more information about the system
        uintptr_t entries = (uintptr_t) conf + sizeof(ConfTable);
        ProcessorAPIC *proc;
        bool isBootstrap;
        console::out::print("  Found CPU");

        for (int i = 0; i < conf->entry_count; ++i) {
            switch (*(APICType *) entries) {
                case MPPROC:    // processor entry type
                    proc = (ProcessorAPIC *) entries;
                    isBootstrap = proc->flags & 0x02;

                    if (cpuCount < MAX_CPU_COUNT) { // add this CPU
                        cpus[cpuCount].id = cpuCount;
                        console::out::print(" %d%s", proc->local_apic_id, isBootstrap ? "[B]" : "");
                        if (isBootstrap) bootstrapCPU = &cpus[cpuCount];
                        cpuCount++;
                    }

                    entries += sizeof(ProcessorAPIC);
                    break;

                default:        // other apic entry
                    entries += 8;
                    break;
            }
        }

        bootstrapCPU->status = CPUInfo::Status::started;
        isMP = cpuCount > 1;

        lapic::init();

        console::out::print("\n  %<Done\n", WHITE);
    }
}


// Adopted from JOS

namespace mp::lapic {
// Local APIC registers, divided by 4 for use as uint32_t[] indices.
#define ID      (0x0020/4)   // ID
#define VER     (0x0030/4)   // Version
#define TPR     (0x0080/4)   // Task Priority
#define EOI     (0x00B0/4)   // EOI
#define SVR     (0x00F0/4)   // Spurious Interrupt Vector
#define ENABLE     0x00000100   // Unit Enable
#define ESR     (0x0280/4)   // Error Status
#define ICRLO   (0x0300/4)   // Interrupt Command
#define INIT       0x00000500   // INIT/RESET
#define STARTUP    0x00000600   // Startup IPI
#define DELIVS     0x00001000   // Delivery status
#define ASSERT     0x00004000   // Assert interrupt (vs deassert)
#define DEASSERT   0x00000000
#define LEVEL      0x00008000   // Level triggered
#define BCAST      0x00080000   // Send to all APICs, including self.
#define OTHERS     0x000C0000   // Send to all APICs, excluding self.
#define BUSY       0x00001000
#define FIXED      0x00000000
#define ICRHI   (0x0310/4)   // Interrupt Command [63:32]
#define TIMER   (0x0320/4)   // Local Vector Table 0 (TIMER)
#define X1         0x0000000B   // divide counts by 1
#define PERIODIC   0x00020000   // Periodic
#define PCINT   (0x0340/4)   // Performance Counter LVT
#define LINT0   (0x0350/4)   // Local Vector Table 1 (LINT0)
#define LINT1   (0x0360/4)   // Local Vector Table 2 (LINT1)
#define ERROR   (0x0370/4)   // Local Vector Table 3 (ERROR)
#define MASKED     0x00010000   // Interrupt masked
#define TICR    (0x0380/4)   // Timer Initial Count
#define TCCR    (0x0390/4)   // Timer Current Count
#define TDCR    (0x03E0/4)   // Timer Divide Configuration

    physaddr_t lapicPhy;
    volatile uint32_t *lapicKern;   // VOLATILE! memory-mapped I/O: different contents for different CPU

    // (memory-mapped I/O) write to local APIC's register of current CPU
    static void lapicw(int index, int value) {
        lapicKern[index] = value;
        lapicKern[ID];  // wait for write to finish, by reading
    }

    void init() {
        using trap::Type;

        // no MP support
        if (!lapicPhy) return;

        // lapicPhy is the physical address of the LAPIC's 4K MMIO
        // region.  Map it in to virtual memory so we can access it.
        lapicKern = static_cast<volatile uint32_t *>(vmem::pgdir::mmioMap(lapicPhy, 4096));

        // Enable local APIC; set spurious interrupt vector.
        lapicw(SVR, ENABLE | (uint32_t) Type::hwSpurious);

        // The timer repeatedly counts down at bus frequency
        // from lapic[TICR] and then issues an interrupt.
        // If we cared more about precise timekeeping,
        // TICR would be calibrated using an external time source.
        lapicw(TDCR, X1);
        lapicw(TIMER, PERIODIC | (uint32_t) Type::hwTimer);
        lapicw(TICR, 10000000);

        // Leave LINT0 of the BSP enabled so that it can get
        // interrupts from the 8259A chip.
        //
        // According to Intel MP Specification, the BIOS should initialize
        // BSP's local APIC in Virtual Wire Mode, in which 8259A's
        // INTR is virtually connected to BSP's LINTIN0. In this mode,
        // we do not need to program the IOAPIC.
        if (mp::thisCPU() != mp::bootstrapCPU)
            lapicw(LINT0, MASKED);

        // Disable NMI (LINT1) on all CPUs
        lapicw(LINT1, MASKED);

        // Disable performance counter overflow interrupts
        // on machines that provide that interrupt entry.
        if (((lapicKern[VER] >> 16) & 0xFF) >= 4)
            lapicw(PCINT, MASKED);

        // Map error interrupt to IRQ_ERROR.
        lapicw(ERROR, (uint32_t) Type::hwError);

        // Clear error status register (requires back-to-back writes).
        lapicw(ESR, 0);
        lapicw(ESR, 0);

        // Ack any outstanding interrupts.
        lapicw(EOI, 0);

        // Send an Init Level De-Assert to synchronize arbitration ID's.
        lapicw(ICRHI, 0);
        lapicw(ICRLO, BCAST | INIT | LEVEL);
        while (lapicKern[ICRLO] & DELIVS);

        // Enable interrupts on the APIC (but not on the processor).
        lapicw(TPR, 0);
    }

// Acknowledge interrupt.
    void lapic_eoi() {
        if (lapicKern)
            lapicw(EOI, 0);
    }

// Spin for a given number of microseconds.
// On real hardware would want to tune this dynamically.
    static void microdelay(int us) {
    }

// Start additional processor running entry code at addr.
// See Appendix B of MultiProcessor Specification.
    void startAP(uint8_t apicid, uint32_t addr) {
        using namespace x86;

        int i;
        uint16_t *wrv;

        // "The BSP must initialize CMOS shutdown code to 0AH
        // and the warm reset vector (DWORD based at 40:67) to point at
        // the AP startup code prior to the [universal startup algorithm]."
        outb(IO_RTC, 0xF);  // offset 0xF is shutdown code
        outb(IO_RTC + 1, 0x0A);
        wrv = (uint16_t *) KERN_ADDR(0x40 << 4 | 0x67);  // Warm reset vector
        wrv[0] = 0;
        wrv[1] = addr >> 4;

        // "Universal startup algorithm."
        // Send INIT (level-triggered) interrupt to reset other CPU.
        lapicw(ICRHI, apicid << 24);
        lapicw(ICRLO, INIT | LEVEL | ASSERT);
        microdelay(200);
        lapicw(ICRLO, INIT | LEVEL);
        microdelay(100);    // should be 10ms, but too slow in Bochs!

        // Send startup IPI (twice!) to enter code.
        // Regular hardware is supposed to only accept a STARTUP
        // when it is in the halted state due to an INIT.  So the second
        // should be ignored, but it is part of the official Intel algorithm.
        // Bochs complains about the second one.  Too bad for Bochs.
        for (i = 0; i < 2; i++) {
            lapicw(ICRHI, apicid << 24);
            lapicw(ICRLO, STARTUP | (addr >> 12));
            microdelay(200);
        }
    }

    void lapic_ipi(int vector) {
        lapicw(ICRLO, OTHERS | FIXED | vector);
        while (lapicKern[ICRLO] & DELIVS);
    }
}

namespace mp {
    CPUInfo *thisCPU() {
        using namespace lapic;
        int num = lapicKern == nullptr ? 0 : lapicKern[ID] >> 24;
        return &cpus[num];
    }
}