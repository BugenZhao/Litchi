set(USER_LD_SCRIPT ${PROJECT_SOURCE_DIR}/scripts/user.ld)

file(GLOB_RECURSE USER_SRCS RELATIVE ${CMAKE_SOURCE_DIR} "user/*")

foreach(file_path ${USER_SRCS})
    string( REPLACE ".cc" ".elf" new_name ${file_path} )
    get_filename_component(target ${new_name} NAME)
    add_executable( ${target} ${file_path})
    set_target_properties(${target} PROPERTIES LINK_FLAGS "-Wl,-m,elf_i386,-T,${USER_LD_SCRIPT} -nostdlib")
    set_target_properties(${target} PROPERTIES COMPILE_FLAGS "-Og")
    target_link_libraries(${target} ${LITCHI_LIB} -static-libgcc -lgcc)
    add_asm_dump(${target})
endforeach()