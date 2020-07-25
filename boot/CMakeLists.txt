set(BOOT_SRC
        boot.S
        main.c)

# Target - boot
add_executable(${BOOT_ELF} ${BOOT_SRC})
set_target_properties(${BOOT_ELF} PROPERTIES LINK_FLAGS "-Wl,-m,elf_i386,-Ttext,0x7C00,-N,-e,start -nostdlib")
# Use optimization to limit the binary size to 512 bytes
set_target_properties(${BOOT_ELF} PROPERTIES COMPILE_FLAGS "-O1 -DLITCHI_KERNEL")
# post-build command: objcopy (binary, only .text)
add_custom_command(
        TARGET ${BOOT_ELF}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --cyan --bold --progress-dir=${PROJECT_BINARY_DIR}/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1)  "Objcopy from ${BOOT_ELF} to ${BOOT_BIN}"
        COMMAND ${TP}objcopy -S -O binary -j .text ${PROJECT_BINARY_DIR}/boot/${BOOT_ELF} ${PROJECT_BINARY_DIR}/boot/${BOOT_BIN}
)
add_asm_dump(${BOOT_ELF})
set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${PROJECT_BINARY_DIR}/boot/${BOOT_BIN}")
