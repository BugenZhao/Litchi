set(KERNEL_SRC
        kernel/entry.S
        kernel/init.cc
        kernel/entrypgdir.c
        kernel/console.cc
        kernel/printf.cc
        kernel/panic.cc
        kernel/pckbd.c
        kernel/readline.cc
        kernel/monitor.cc
        kernel/system.cc
        kernel/kdebug.cc
        kernel/vmem.cc
        kernel/color.c
        kernel/task.cc
        kernel/task.hh
        kernel/gdt.c
        kernel/trap.cc
        kernel/embuser.S
        kernel/irqentry.S)

# Target - kernel
add_executable(${LITCHI_ELF} ${KERNEL_SRC})
set_target_properties(${LITCHI_ELF} PROPERTIES LINK_FLAGS "-Wl,-m,elf_i386,-T,${LD_SCRIPT} -nostdlib")
# ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_FLAGS} ${COMPILE_FLAGS} -o foo.o -c foo.cc
# Use -Og for a better kernel debugging experience
set_target_properties(${LITCHI_ELF} PROPERTIES COMPILE_FLAGS "-Og -DLITCHI_KERNEL")
target_link_libraries(${LITCHI_ELF} ${LITCHI_LIB} -static-libgcc -lgcc)
add_asm_dump(${LITCHI_ELF})