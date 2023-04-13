# CMakeList.txt : CMake project for scr4, include source and define
# project specific logic here.
#

file(WRITE "exedir.txt" "${CMAKE_BINARY_DIR}/")
#set(ENV{VS_BIN_DIR} "${CMAKE_BINARY_DIR}/")

add_subdirectory(test_c_api)
add_subdirectory(test_cpp_api)
