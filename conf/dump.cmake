# Dump dir
set(ASM_DUMP_DIR "${PROJECT_BINARY_DIR}/asm_dumps")
file(MAKE_DIRECTORY ${ASM_DUMP_DIR})
macro(add_asm_dump TARGET)
    add_custom_command(
            TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --cyan --bold --progress-dir="${PROJECT_BINARY_DIR}/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1)  "Generating ASM dump of ${TARGET}"
            COMMAND ${CMAKE_OBJDUMP} -S $<TARGET_FILE:${TARGET}> > "${ASM_DUMP_DIR}/${TARGET}.asm"
            VERBATIM)
    set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${ASM_DUMP_DIR}/${TARGET}.asm")
endmacro(add_asm_dump)
