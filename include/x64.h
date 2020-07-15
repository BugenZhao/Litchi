// Adopted from JOS/xv6

#ifndef LITCHI_X64_H
#define LITCHI_X64_H

#include <include/types.h>

#ifdef __cplusplus
namespace x64 {
#endif
    static inline void
    breakpoint(void)
    {
        asm volatile("int3");
    }

    static inline uint8_t
    inb(int port)
    {
        uint8_t data;
        asm volatile("inb %w1,%0" : "=a" (data) : "d" (port));
        return data;
    }

    static inline void
    insb(int port, void *addr, int cnt)
    {
        asm volatile("cld\n\trepne\n\tinsb"
                 : "=D" (addr), "=c" (cnt)
                 : "d" (port), "0" (addr), "1" (cnt)
                 : "memory", "cc");
    }

    static inline uint16_t
    inw(int port)
    {
        uint16_t data;
        asm volatile("inw %w1,%0" : "=a" (data) : "d" (port));
        return data;
    }

    static inline void
    insw(int port, void *addr, int cnt)
    {
        asm volatile("cld\n\trepne\n\tinsw"
                 : "=D" (addr), "=c" (cnt)
                 : "d" (port), "0" (addr), "1" (cnt)
                 : "memory", "cc");
    }

    static inline uint32_t
    inl(int port)
    {
        uint32_t data;
        asm volatile("inl %w1,%0" : "=a" (data) : "d" (port));
        return data;
    }

    static inline void
    insl(int port, void *addr, int cnt)
    {
        asm volatile("cld\n\trepne\n\tinsl"
                 : "=D" (addr), "=c" (cnt)
                 : "d" (port), "0" (addr), "1" (cnt)
                 : "memory", "cc");
    }

    static inline void
    outb(int port, uint8_t data)
    {
        asm volatile("outb %0,%w1" : : "a" (data), "d" (port));
    }

    static inline void
    outsb(int port, const void *addr, int cnt)
    {
        asm volatile("cld\n\trepne\n\toutsb"
                 : "=S" (addr), "=c" (cnt)
                 : "d" (port), "0" (addr), "1" (cnt)
                 : "cc");
    }

    static inline void
    outw(int port, uint16_t data)
    {
        asm volatile("outw %0,%w1" : : "a" (data), "d" (port));
    }

    static inline void
    outsw(int port, const void *addr, int cnt)
    {
        asm volatile("cld\n\trepne\n\toutsw"
                 : "=S" (addr), "=c" (cnt)
                 : "d" (port), "0" (addr), "1" (cnt)
                 : "cc");
    }

    static inline void
    outsl(int port, const void *addr, int cnt)
    {
        asm volatile("cld\n\trepne\n\toutsl"
                 : "=S" (addr), "=c" (cnt)
                 : "d" (port), "0" (addr), "1" (cnt)
                 : "cc");
    }

    static inline void
    outl(int port, uint32_t data)
    {
        asm volatile("outl %0,%w1" : : "a" (data), "d" (port));
    }

    static inline void
    invlpg(void *addr)
    {
        asm volatile("invlpg (%0)" : : "r" (addr) : "memory");
    }

    static inline void
    lidt(void *p)
    {
        asm volatile("lidt (%0)" : : "r" (p));
    }

    static inline void
    lgdt(void *p)
    {
        asm volatile("lgdt (%0)" : : "r" (p));
    }

    static inline void
    lldt(uint16_t sel)
    {
        asm volatile("lldt %0" : : "r" (sel));
    }

    static inline void
    ltr(uint16_t sel)
    {
        asm volatile("ltr %0" : : "r" (sel));
    }

    static inline void
    lcr0(uint64_t val)
    {
        asm volatile("movq %0,%%cr0" : : "r" (val));
    }

    static inline uint64_t
    rcr0(void)
    {
        uint64_t val;
        asm volatile("movq %%cr0,%0" : "=r" (val));
        return val;
    }

    static inline uint64_t
    rcr2(void)
    {
        uint64_t val;
        asm volatile("movq %%cr2,%0" : "=r" (val));
        return val;
    }

    static inline void
    lcr3(uint64_t val)
    {
        asm volatile("movq %0,%%cr3" : : "r" (val));
    }

    static inline uint64_t
    rcr3(void)
    {
        uint64_t val;
        asm volatile("movq %%cr3,%0" : "=r" (val));
        return val;
    }

    static inline void
    lcr4(uint64_t val)
    {
        asm volatile("movq %0,%%cr4" : : "r" (val));
    }

    static inline uint64_t
    rcr4(void)
    {
        uint64_t cr4;
        asm volatile("movq %%cr4,%0" : "=r" (cr4));
        return cr4;
    }

    static inline void
    tlbflush(void)
    {
        uint64_t cr3;
        asm volatile("movq %%cr3,%0" : "=r" (cr3));
        asm volatile("movq %0,%%cr3" : : "r" (cr3));
    }

    static inline uint64_t
    read_rflags(void)
    {
        uint64_t rflags;
        asm volatile("pushfq; popq %0" : "=r" (rflags));
        return rflags;
    }

    static inline void
    write_rflags(uint64_t rflags)
    {
        asm volatile("pushq %0; popfq" : : "r" (rflags));
    }

    static inline uint64_t
    read_rbp(void)
    {
        uint64_t rbp;
        asm volatile("movq %%rbp,%0" : "=r" (rbp));
        return rbp;
    }

    static inline uint64_t
    read_rsp(void)
    {
        uint64_t rsp;
        asm volatile("movq %%rsp,%0" : "=r" (rsp));
        return rsp;
    }

    static inline uint64_t
    read_tsc(void)
    {
        uint64_t tsc;
        asm volatile("rdtsc" : "=A" (tsc));
        return tsc;
    }

    static inline uint32_t
    xchg(volatile uint32_t *addr, uint32_t newval)
    {
        uint32_t result;

        // The + in "+m" denotes a read-modify-write operand.
        asm volatile("lock; xchgl %0, %1"
                 : "+m" (*addr), "=a" (result)
                 : "1" (newval)
                 : "cc");
        return result;
    }

    static inline uint32_t rdrand() {
        // only supported on newer x86 CPUs
        uint32_t r;
        asm volatile ("rdrand %%eax":"=a"(r));
        return r;
    }
#ifdef __cplusplus
}
#endif

#endif //LITCHI_X64_H
