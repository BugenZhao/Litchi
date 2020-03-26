# BUGEN'S LITCHI CMAKE CONFIGURATION

cmake_minimum_required(VERSION 3.13)

# Set NONE to avoid CMake choosing compilers itself
project(Litchi LANGUAGES NONE)

# Toolchains prefix
set(TP i386-jos-elf-)
set(QEMU qemu-system-i386)

# Object name constants
set(LITCHI_BIN litchi)
set(BOOT_BIN boot)
set(IMG ${PROJECT_SOURCE_DIR}/out/litchi.img)

# Flag constants
set(C_OR_ASM_FLAGS "-pipe -nostdinc -fno-builtin -MD -fno-omit-frame-pointer -std=gnu99 -static -Wall -Wno-format -Wno-unused -Werror -gstabs -m32 -fno-tree-ch -fno-stack-protector -DJOS_KERNEL -DLITCHI_KERNEL -gstabs")

# Other constants
set(LD_SCRIPT ${CMAKE_SOURCE_DIR}/scripts/kernel.ld)

# Make CMake verbose or not
set(CMAKE_VERBOSE_MAKEFILE 1)

# BUGENC:
# CMake will try to compile a simple test program, but it is to fail when cross-compiling
# After setting this, CMake will try to compile a static library and avoid running it
# https://stackoverflow.com/questions/53633705/cmake-the-c-compiler-is-not-able-to-compile-a-simple-test-program
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# C
set(CMAKE_C_COMPILER ${TP}gcc)
set(CMAKE_C_FLAGS ${C_OR_ASM_FLAGS})
# Find libgcc.a for operations like '%' or '/'
execute_process(COMMAND ${CMAKE_C_COMPILER} --print-libgcc-file-name OUTPUT_VARIABLE LIBGCC OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "libgcc.a found.")

# ASM
set(CMAKE_ASM_COMPILER ${TP}gcc)
set(CMAKE_ASM-ATT_COMPILER ${TP}gcc)
set(CMAKE_ASM_FLAGS ${C_OR_ASM_FLAGS})

# Objdump
set(CMAKE_OBJDUMP ${TP}objdump)

# Link
set(CMAKE_C_LINK_EXECUTABLE "${TP}ld -o <TARGET> <LINK_FLAGS> <OBJECTS> <LINK_LIBRARIES>")
#set(CMAKE_C_LINK_EXECUTABLE "${TP}ld -o <TARGET> <LINK_FLAGS> <OBJECTS> <LINK_LIBRARIES> -b binary")

# NASM - just for 0x55AA !?
set(NASM_COMPILER nasm)

# Now we can enable languages
enable_language(C ASM ASM-ATT)


# Include
include_directories(.)

# Sources
set(BOOT_SRC
        boot/boot.S
        boot/main.c)

set(LIB_SRC
        lib/string.c
        lib/printfmt.c)

set(KERNEL_SRC
        kernel/entry.S
        kernel/init.c
        kernel/entrypgdir.c
        kernel/console.c
        kernel/printf.c
        kernel/assert.c
        ${LIB_SRC})


# Target - kernel
add_executable(${LITCHI_BIN} ${KERNEL_SRC} ${LIBS_SRC})
set_target_properties(${LITCHI_BIN} PROPERTIES LINK_FLAGS "-m elf_i386 -T ${LD_SCRIPT} -nostdlib")
    # ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_FLAGS} ${COMPILE_FLAGS} -o foo.o -c foo.cc
    # Use -Og for a better kernel debugging experience
set_target_properties(${LITCHI_BIN} PROPERTIES COMPILE_FLAGS "-Og")
target_link_libraries(${LITCHI_BIN} ${LIBGCC})

# Target - boot
add_executable(${BOOT_BIN} ${BOOT_SRC})
set_target_properties(${BOOT_BIN} PROPERTIES LINK_FLAGS "-Ttext 0x7C00 -m elf_i386 -N -e start")
    # Use optimization to limit the binary size to 512 bytes
set_target_properties(${BOOT_BIN} PROPERTIES COMPILE_FLAGS "-O1")
    # post-build command: objcopy (binary, only .text)
add_custom_command(
        TARGET ${BOOT_BIN}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E rename ${PROJECT_BINARY_DIR}/${BOOT_BIN} ${PROJECT_BINARY_DIR}/${BOOT_BIN}.out
        COMMAND ${TP}objcopy -S -O binary -j .text ${PROJECT_BINARY_DIR}/${BOOT_BIN}.out ${PROJECT_BINARY_DIR}/${BOOT_BIN}
        COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --cyan "Objcopy done!"
)

# Target - Image
add_custom_target(image
        COMMAND ${NASM_COMPILER} ${PROJECT_SOURCE_DIR}/bootable/bootable.nasm -o ${PROJECT_BINARY_DIR}/bootable
        COMMAND dd if=/dev/zero of=${IMG} bs=512 count=2880
        COMMAND dd if=${PROJECT_BINARY_DIR}/bootable of=${IMG} conv=notrunc
        COMMAND dd if=${PROJECT_BINARY_DIR}/${BOOT_BIN} of=${IMG} conv=notrunc
        COMMAND dd if=${PROJECT_BINARY_DIR}/${LITCHI_BIN} of=${IMG} seek=1 conv=notrunc
        DEPENDS ${LITCHI_BIN} ${BOOT_BIN})

# Options for qemu and gdb
set(QEMU_OPTIONS -drive file=${IMG},index=0,media=disk,format=raw -serial mon:stdio)
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