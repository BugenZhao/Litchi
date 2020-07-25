set(LIB_SRC
        string.cc
        printfmt.cc
        strtol.cc
        ctype.cc
        random.cc
        color.c)

add_library(${LITCHI_LIB} STATIC ${LIB_SRC})
#set_target_properties(${LITCHI_LIB} PROPERTIES LINK_FLAGS "-Wl,-m,elf_i386,-T,${USER_LD_SCRIPT} -nostdlib")
set_target_properties(${LITCHI_LIB} PROPERTIES COMPILE_FLAGS "-Og")
target_link_libraries(${LITCHI_LIB} -static-libgcc -lgcc)
add_asm_dump(${LITCHI_LIB})