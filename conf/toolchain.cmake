# Toolchains prefix
set(TP i386-litchi-elf-)
set(QEMU qemu-system-i386)

# CMake will try to compile and run a simple test program, but it is to fail when cross-compiling
# After setting this, CMake will try to compile a static library and do not run it
# https://stackoverflow.com/questions/53633705/cmake-the-c-compiler-is-not-able-to-compile-a-simple-test-program
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# Flags
set(C_OR_ASM_FLAGS "-ffreestanding -fno-builtin -nostdlib -fno-omit-frame-pointer -static -Wall -Wno-format -Wno-unused -Wno-pointer-arith -Werror -m32 -fno-tree-ch -fno-stack-protector -g -ggdb")
set(CXX_FLAGS "-ffreestanding -fno-builtin -nostdlib -fno-omit-frame-pointer -std=gnu++20 -static -Wall -Wno-pointer-arith -Werror -Wno-format -Wno-unused -m32 -fno-tree-ch -fno-stack-protector -fno-rtti -fno-exceptions -fno-unwind-tables -g -ggdb")

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

