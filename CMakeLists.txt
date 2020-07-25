# BUGEN'S LITCHI CMAKE CONFIGURATION

cmake_minimum_required(VERSION 3.13)
set(CMAKE_VERBOSE_MAKEFILE 0)

# Set NONE to avoid CMake choosing compilers itself
project(Litchi LANGUAGES NONE)
include(conf/toolchain.cmake)

# Now we can enable languages
enable_language(C ASM CXX)

# Object names
set(LITCHI_ELF litchi.elf)
set(BOOT_ELF boot.elf)
set(BOOT_BIN boot.bin)
set(LITCHI_LIB litchicc)
set(IMG "${PROJECT_SOURCE_DIR}/image/litchi.img")
set(EMBUSER_ELF hello.elf)  # Embedded user elf binary, see kernel/embuser.S

# CMake macro: add_asm_dump
include(conf/dump.cmake)

# Provide define's to sources
include(conf/macros.cmake)

# Include
include_directories(.)

# Sources
add_subdirectory(lib)
add_subdirectory(user)
add_subdirectory(boot)
add_subdirectory(kernel)

# Image, qemu, gdb...
include(conf/ptargets.cmake)
