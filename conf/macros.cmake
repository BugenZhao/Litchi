# Get the length of source path to minimize macro __FILE__ by using __FILE__ + SOURCE_PATH_FILE
# https://stackoverflow.com/questions/8487986/file-macro-shows-full-path
string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_DIR_OFFSET)
string(LENGTH "${CMAKE_BINARY_DIR}/" BINARY_DIR_OFFSET)
add_definitions("-DSOURCE_DIR_OFFSET=${SOURCE_DIR_OFFSET}")
add_definitions("-DBINARY_DIR_OFFSET=${BINARY_DIR_OFFSET}")

# Provide embedded ELF file name
add_definitions("-DEMBUSER_ELF=\"$<TARGET_FILE:${EMBUSER_ELF}>\"")

# Provide time
string(TIMESTAMP TIMESEED "%y%m%d%H%M%S")
string(TIMESTAMP BUILD_TIME "%b %d %Y, %H:%M:%S")
string(TIMESTAMP YEAR "%Y")
add_definitions("-DTIMESEED=${TIMESEED}")
add_definitions("-DBUILD_TIME=\"${BUILD_TIME}\"")
add_definitions("-DYEAR=${YEAR}")

# Provide compiler
add_definitions("-DCOMPILER=\"${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} on ${CMAKE_SYSTEM}\"")