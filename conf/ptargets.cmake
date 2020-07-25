# Target - Image
add_custom_target(image
        COMMAND dd if=/dev/zero of=${IMG} bs=512 count=2880 >/dev/null 2>&1
        COMMAND printf '\\x55\\xaa' | dd of=${IMG} bs=1 seek=510 conv=notrunc >/dev/null 2>&1
        COMMAND dd if="${PROJECT_BINARY_DIR}/boot/${BOOT_BIN}" of=${IMG} conv=notrunc >/dev/null 2>&1
        COMMAND dd if="${PROJECT_BINARY_DIR}/kernel/${LITCHI_ELF}" of=${IMG} seek=1 conv=notrunc >/dev/null 2>&1
        DEPENDS ${LITCHI_ELF} ${BOOT_ELF})
set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${IMG}")

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

add_custom_target(qemu-nox
        COMMAND ${QEMU} -nographic ${QEMU_OPTIONS}
        DEPENDS image)

add_custom_target(qemu
        COMMAND ${QEMU} ${QEMU_OPTIONS}
        DEPENDS image)

add_custom_target(cgdb-cli
        COMMAND cgdb -n -x .gdbinit.cli)
