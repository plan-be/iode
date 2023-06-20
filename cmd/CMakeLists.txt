# CMakeList.txt : CMake project for ${IODE_CLI_TARGET}


# NOTE: use PRIVATE to NOT propagate compile options to other lib/executable depending on iode_c_api.
#       Option -Zp1 makes compilation of the Google Test lib to fail
file(INSTALL ${PROJECT_SOURCE_DIR}/iode.msg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(${IODE_CLI_TARGET} "iodecmd.c")
target_compile_options(${IODE_CLI_TARGET} PRIVATE -Zp1)
target_link_libraries(${IODE_CLI_TARGET} PUBLIC iode_scr4 iode_c_api)
target_include_directories(${IODE_CLI_TARGET} PUBLIC "../api")
