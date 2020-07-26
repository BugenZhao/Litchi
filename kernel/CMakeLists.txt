set(LD_SCRIPT "${CMAKE_SOURCE_DIR}/scripts/kernel.ld")

set(KERNEL_SRC
        entry.S
        init.cc
        entrypgdir.c
        console.cc
        printf.cc
        panic.cc
        pckbd.c
        readline.cc
        monitor.cc
        system.cc
        kdebug.cc
        vmem.cc
        task.cc
        task.hh
        gdt.c
        trap.cc
        embuser.S
        irqentry.S
        ksyscall.cc
        mp.cc)

# Target - kernel
add_executable(${LITCHI_ELF} ${KERNEL_SRC})
set_target_properties(${LITCHI_ELF} PROPERTIES LINK_FLAGS "-Wl,-m,elf_i386,-T,${LD_SCRIPT} -nostdlib")
# ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_FLAGS} ${COMPILE_FLAGS} -o foo.o -c foo.cc
# Use -Og for a better kernel debugging experience
set_target_properties(${LITCHI_ELF} PROPERTIES COMPILE_FLAGS "-Og -DLITCHI_KERNEL")
target_link_libraries(${LITCHI_ELF} ${LITCHI_LIB} -static-libgcc -lgcc)
add_asm_dump(${LITCHI_ELF})

# Litchi.elf includes and thus depends on embUserElf
# WARNING: not always working, remember to rebuild every time after updating
add_dependencies(${LITCHI_ELF} ${EMBUSER_ELF})