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
set(LITCHI_LIB litchicc)
set(IMG ${PROJECT_SOURCE_DIR}/out/litchi.img)

# Embedded user elf binary, see kernel/embuser.S
set(EMBUSER_ELF hello.elf)

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

# Flags
set(C_OR_ASM_FLAGS "-ffreestanding -fno-builtin -nostdlib -fno-omit-frame-pointer -std=gnu11 -static -Wall -Wno-format -Wno-unused -Werror -m32 -fno-tree-ch -fno-stack-protector -g -ggdb")
set(CXX_FLAGS "-ffreestanding -fno-builtin -nostdlib -fno-omit-frame-pointer -std=gnu++20 -static -Wall -Wno-pointer-arith -Werror -Wno-format -Wno-unused -m32 -fno-tree-ch -fno-stack-protector -fno-rtti -fno-exceptions -fno-unwind-tables -g -ggdb")

# Other constants
set(LD_SCRIPT ${CMAKE_SOURCE_DIR}/scripts/kernel.ld)

# Get the length of source path to minimize macro __FILE__ by using __FILE__ + SOURCE_PATH_FILE
# https://stackoverflow.com/questions/8487986/file-macro-shows-full-path
string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")
add_definitions("-DEMBUSER_ELF=\"${PROJECT_BINARY_DIR}/${EMBUSER_ELF}\"")

# CMake will try to compile and run a simple test program, but it is to fail when cross-compiling
# After setting this, CMake will try to compile a static library and do not run it
# https://stackoverflow.com/questions/53633705/cmake-the-c-compiler-is-not-able-to-compile-a-simple-test-program
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# C
set(CMAKE_C_COMPILER ${TP}gcc)
set(CMAKE_C_FLAGS ${C_OR_ASM_FLAGS})
set(CMAKE_C_STANDARD 11)

# CXX
set(CMAKE_CXX_COMPILER ${TP}g++)
set(CMAKE_CXX_FLAGS ${CXX_FLAGS})
set(CMAKE_CXX_STANDARD 20)

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
set(CMAKE_AR ${TP}ar)
set(CMAKE_RANLIB ${TP}ranlib)
#set(CMAKE_C_LINK_EXECUTABLE "${TP}ld -o <TARGET> <LINK_FLAGS> <OBJECTS> <LINK_LIBRARIES>")
#set(CMAKE_CXX_LINK_EXECUTABLE "${TP}ld -o <TARGET> <LINK_FLAGS> <OBJECTS> <LINK_LIBRARIES>")

# Now we can enable languages
enable_language(C ASM CXX)


# Include
include_directories(.)

# Sources
include(lib/CMakeLists.txt)
include(user/CMakeLists.txt)
include(boot/CMakeLists.txt)
include(kernel/CMakeLists.txt)

# Litchi.elf includes and thus depends on embUserElf
# WARNING: not always working, remember to rebuild every time after updating
add_dependencies(${LITCHI_ELF} ${EMBUSER_ELF})

# Target - Image
add_custom_target(image
        COMMAND dd if=/dev/zero of=${IMG} bs=512 count=2880 >/dev/null 2>&1
        COMMAND printf '\\x55\\xaa' | dd of=${IMG} bs=1 seek=510 conv=notrunc >/dev/null 2>&1
        COMMAND dd if=${PROJECT_BINARY_DIR}/${BOOT_BIN} of=${IMG} conv=notrunc >/dev/null 2>&1
        COMMAND dd if=${PROJECT_BINARY_DIR}/${LITCHI_ELF} of=${IMG} seek=1 conv=notrunc >/dev/null 2>&1
        DEPENDS ${LITCHI_ELF} ${BOOT_ELF})

# Options for qemu and gdb
set(QEMU_OPTIONS -cpu Haswell -drive file=${IMG},index=0,media=disk,format=raw -m 256 -serial mon:stdio)
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