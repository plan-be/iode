# CMakeList.txt : CMake project for iodecmd


# NOTE: use PRIVATE to NOT propagate compile options to other lib/executable depending on iode_c_api.
#       Option -Zp1 makes compilation of the Google Test lib to fail
file(INSTALL ${PROJECT_SOURCE_DIR}/iode.msg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(iodecmd "iodecmd.c")
target_compile_options(iodecmd PRIVATE -Zp1)
target_link_libraries(iodecmd PUBLIC scr4iode iode_c_api)
target_include_directories(iodecmd PUBLIC "../api")
