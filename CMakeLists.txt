# BUGEN'S LITCHI CMAKE CONFIGURATION

cmake_minimum_required(VERSION 3.13)

# Make CMake verbose or not
set(CMAKE_VERBOSE_MAKEFILE 0)

# Set NONE to avoid CMake choosing compilers itself
project(Litchi LANGUAGES NONE)

# Toolchains prefix
set(TP i386-litchi-elf-)
set(QEMU qemu-system-i386)

# Object name constants
set(LITCHI_ELF litchi.elf)
set(BOOT_ELF boot.elf)
set(BOOT_BIN boot.bin)
set(IMG ${PROJECT_SOURCE_DIR}/out/litchi.img)

# Dump dir
set(ASM_DUMP_DIR ${PROJECT_BINARY_DIR}/asm_dumps)
file(MAKE_DIRECTORY ${ASM_DUMP_DIR})
macro(add_asm_dump TARGET)
    add_custom_command(
            TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --cyan --bold --progress-dir=${PROJECT_BINARY_DIR}/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1)  "Generating ASM dump of ${TARGET}"
            COMMAND ${CMAKE_OBJDUMP} -S $<TARGET_FILE:${TARGET}> > ${ASM_DUMP_DIR}/${TARGET}.asm
            VERBATIM)
    set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${ASM_DUMP_DIR}/${TARGET}.asm)
endmacro(add_asm_dump)

# Flag constants
set(C_OR_ASM_FLAGS "-pipe -ffreestanding -fno-builtin -nostdlib -MD -fno-omit-frame-pointer -std=gnu17 -static -Wall -Wno-format -Wno-unused -Werror -m32 -fno-tree-ch -fno-stack-protector -DJOS_KERNEL -DLITCHI_KERNEL -g -ggdb")
set(CXX_FLAGS "-pipe -ffreestanding -fno-builtin -nostdlib -MD -fno-omit-frame-pointer -std=gnu++17 -static -Wall -Wno-pointer-arith -Werror -Wno-format -Wno-unused -m32 -fno-tree-ch -fno-stack-protector -fno-rtti -fno-exceptions -fno-unwind-tables -DJOS_KERNEL -DLITCHI_KERNEL -g -ggdb")

# Other constants
set(LD_SCRIPT ${CMAKE_SOURCE_DIR}/scripts/kernel.ld)

# Get the length of source path to minimize macro __FILE__ by using __FILE__ + SOURCE_PATH_FILE
# https://stackoverflow.com/questions/8487986/file-macro-shows-full-path
string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")

# CMake will try to compile and run a simple test program, but it is to fail when cross-compiling
# After setting this, CMake will try to compile a static library and do not run it
# https://stackoverflow.com/questions/53633705/cmake-the-c-compiler-is-not-able-to-compile-a-simple-test-program
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# C
set(CMAKE_C_COMPILER ${TP}gcc)
set(CMAKE_C_FLAGS ${C_OR_ASM_FLAGS})

# CXX
set(CMAKE_CXX_COMPILER ${TP}g++)
set(CMAKE_CXX_FLAGS ${CXX_FLAGS})

# ASM
set(CMAKE_ASM_COMPILER ${TP}gcc)
set(CMAKE_ASM_FLAGS ${C_OR_ASM_FLAGS})

# Objdump
set(CMAKE_OBJDUMP ${TP}objdump)

# gcc linker fix
# https://stackoverflow.com/questions/54482519/avoid-cmake-to-add-the-flags-search-paths-first-and-headerpad-max-install-name
set(HAVE_FLAG_SEARCH_PATHS_FIRST 0)
set(CMAKE_C_LINK_FLAGS "")
set(CMAKE_CXX_LINK_FLAGS "")

# Link - use ld
#set(CMAKE_C_LINK_EXECUTABLE "${TP}ld -o <TARGET> <LINK_FLAGS> <OBJECTS> <LINK_LIBRARIES>")
#set(CMAKE_CXX_LINK_EXECUTABLE "${TP}ld -o <TARGET> <LINK_FLAGS> <OBJECTS> <LINK_LIBRARIES>")

# Now we can enable languages
enable_language(C ASM CXX)


# Include
include_directories(.)

# Sources
set(BOOT_SRC
        boot/boot.S
        boot/main.c)

set(LIB_SRC
        lib/string.cc
        lib/printfmt.cc
        lib/strtol.cc
        lib/ctype.cc)

set(KERNEL_SRC
        ${LIB_SRC}
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
        kernel/gdt.c)

# Target - kernel
add_executable(${LITCHI_ELF} ${KERNEL_SRC} ${LIBS_SRC})
set_target_properties(${LITCHI_ELF} PROPERTIES LINK_FLAGS "-Wl,-m,elf_i386,-T,${LD_SCRIPT} -nostdlib")
# ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_FLAGS} ${COMPILE_FLAGS} -o foo.o -c foo.cc
# Use -Og for a better kernel debugging experience
set_target_properties(${LITCHI_ELF} PROPERTIES COMPILE_FLAGS "-Og")
target_link_libraries(${LITCHI_ELF} -static-libgcc -lgcc)
add_asm_dump(${LITCHI_ELF})

# Target - boot
add_executable(${BOOT_ELF} ${BOOT_SRC})
set_target_properties(${BOOT_ELF} PROPERTIES LINK_FLAGS "-Wl,-m,elf_i386,-Ttext,0x7C00,-N,-e,start -nostdlib")
# Use optimization to limit the binary size to 512 bytes
set_target_properties(${BOOT_ELF} PROPERTIES COMPILE_FLAGS "-O1")
# post-build command: objcopy (binary, only .text)
add_custom_command(
        TARGET ${BOOT_ELF}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --cyan --bold --progress-dir=${PROJECT_BINARY_DIR}/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1)  "Objcopy from ${BOOT_ELF} to ${BOOT_BIN}"
        COMMAND ${TP}objcopy -S -O binary -j .text ${PROJECT_BINARY_DIR}/${BOOT_ELF} ${PROJECT_BINARY_DIR}/${BOOT_BIN}
)
add_asm_dump(${BOOT_ELF})

# Target - Image
add_custom_target(image
        COMMAND dd if=/dev/zero of=${IMG} bs=512 count=2880 >/dev/null 2>&1
        COMMAND printf '\\x55\\xaa' | dd of=${IMG} bs=1 seek=510 conv=notrunc >/dev/null 2>&1
        COMMAND dd if=${PROJECT_BINARY_DIR}/${BOOT_BIN} of=${IMG} conv=notrunc >/dev/null 2>&1
        COMMAND dd if=${PROJECT_BINARY_DIR}/${LITCHI_ELF} of=${IMG} seek=1 conv=notrunc >/dev/null 2>&1
        DEPENDS ${LITCHI_ELF} ${BOOT_ELF})

# Options for qemu and gdb
set(QEMU_OPTIONS -drive file=${IMG},index=0,media=disk,format=raw -m 256 -serial mon:stdio)
set(GDB_PORT 10188)
set(QEMU_GDB_OPTIONS -gdb tcp::${GDB_PORT} -S) # -S: stop at entry

add_custom_target(qemu-nox-gdb
        COMMAND ${QEMU} -nographic ${QEMU_OPTIONS} ${QEMU_GDB_OPTIONS}
        DEPENDS image)

add_custom_target(qemu-gdb
        COMMAND ${QEMU} ${QEMU_OPTIONS} ${QEMU_GDB_OPTIONS}
        DEPENDS image)

add_custom_target(qemu
        COMMAND ${QEMU} ${QEMU_OPTIONS}
        DEPENDS image)

add_custom_target(cgdb-cli
        COMMAND cgdb -n -x .gdbinit.cli)