cmake_minimum_required(VERSION 3.13)
project(Litchi C ASM_NASM)
set(LITCHI_BIN LITCHI.LIT)

set(CMAKE_VERBOSE_MAKEFILE 1)

# C
set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_FLAGS "-c -Wall -m32 -ggdb -gstabs+ -nostdinc -fno-builtin -fno-stack-protector")
# NASM (Cmake's nasm module is buggy. We cannot set correct object format only by setting CMAKE_ASM_NASM_OBJECT_FORMAT.)
set(CMAKE_ASM_NASM_COMPILER nasm)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf)
set(CMAKE_ASM_NASM_FLAGS "-g -F stabs")
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> <FLAGS> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")
# Link
set(CMAKE_C_LINK_EXECUTABLE "ld <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
set(LD_SCRIPT ${PROJECT_SOURCE_DIR}/src/scripts/kernel.ld)

# Sources
aux_source_directory(src/kernel KERNEL_SRC)
aux_source_directory(src/libs LIBS_SRC)
aux_source_directory(src/tests TESTS_SRC)
set(ASM_SRC src/loader/nloader.asm)
# Include
include_directories(src/include src/tests)

# Target
add_executable(${LITCHI_BIN} ${KERNEL_SRC} ${LIBS_SRC} ${TESTS_SRC} ${ASM_SRC})
set_target_properties(${LITCHI_BIN} PROPERTIES LINK_FLAGS "-T ${LD_SCRIPT} -m elf_i386 -nostdlib")

# Image
set(IMG ${PROJECT_SOURCE_DIR}/out/litchi.img)
set(TMP_IMG /tmp/litchi.img)
set(GRUB_IMG ${PROJECT_SOURCE_DIR}/grub.img)
set(FLOPPY /mnt/litchi)
add_custom_target(image
        COMMAND cp -v ${GRUB_IMG} ${TMP_IMG}
        COMMAND sudo mkdir -p ${FLOPPY}
        COMMAND sudo mount -o loop ${TMP_IMG} ${FLOPPY}
        COMMAND sudo cp -v ${LITCHI_BIN} ${FLOPPY}
        COMMAND sleep 1
        COMMAND sudo umount ${FLOPPY}
        COMMAND cp -v ${TMP_IMG} ${IMG}
        COMMAND cp -v ${PROJECT_BINARY_DIR}/${LITCHI_BIN} ${PROJECT_SOURCE_DIR}/out/${LITCHI_BIN}
        DEPENDS ${LITCHI_BIN}
)