//
// Created by Bugen Zhao on 7/26/20.
//

#include <kernel/mp.hh>
#include <include/stdio.hh>
#include <include/string.hh>
#include <kernel/vmem.hh>

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
}

namespace mp {
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
    void init() {
        console::out::print("Initializing multi-processor...\n");

        auto fp = searchFP();
        if (fp == nullptr) {
            console::out::print("No multi-processor support.\n");
            return;
        }

        // get Configuration Table
        auto conf = (ConfTable *) KERN_ADDR(fp->configuration_table);
        assert(conf != nullptr);                            // no default configuration support
        assert(mem::cmp(conf->signature, "PCMP", 4) == 0);  // signature
        assert(checksumOk(conf, conf->length));             // base checksum
        assert(checksumOk((uint8_t *) conf + conf->length, conf->extended_table_length));   // extended checksum

        char oem[9] = {};
        mem::copy(oem, conf->oem_id, sizeof(conf->oem_id));
        console::out::print("  MP Conf Table OEM: %s\n", oem);

        // **After** the table, there are entry_count entries describing more information about the system
        uintptr_t entries = (uintptr_t) conf + sizeof(ConfTable);
        ProcessorAPIC *proc;
        console::out::print("  Found CPU");
        for (int i = 0; i < conf->entry_count; ++i) {
            switch (*(APICType *) entries) {
                case MPPROC:    // processor entry type
                    proc = (ProcessorAPIC *) entries;
                    console::out::print(" %d", proc->local_apic_id);
                    entries += sizeof(ProcessorAPIC);
                    break;
                default:
                    entries += 8;
                    break;
            }
        }
        console::out::print("\n  %<Done\n", WHITE);
    }
}