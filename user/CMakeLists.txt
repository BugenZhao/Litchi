set(USER_LD_SCRIPT ${PROJECT_SOURCE_DIR}/scripts/user.ld)

file(GLOB USER_SRCS RELATIVE ${CMAKE_SOURCE_DIR} "user/*.cc")
set(USER_LIB_SRCS
        user/entry/uentry.S
        user/entry/umain.cc
        user/lib/printf.cc)

foreach(file_path ${USER_SRCS})
    string( REPLACE ".cc" ".elf" new_name ${file_path} )
    get_filename_component(target ${new_name} NAME)
    add_executable(${target} ${file_path} ${USER_LIB_SRCS})
    set_target_properties(${target} PROPERTIES LINK_FLAGS "-Wl,-m,elf_i386,-T,${USER_LD_SCRIPT} -nostdlib")
    set_target_properties(${target} PROPERTIES COMPILE_FLAGS "-O0")
    target_link_libraries(${target} ${LITCHI_LIB} -static-libgcc -lgcc)
    add_asm_dump(${target})
endforeach()