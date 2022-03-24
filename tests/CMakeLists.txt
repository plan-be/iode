# CMakeList.txt : CMake project for scr4, include source and define
# project specific logic here.
#

add_executable(test1 "test1.c")
target_link_libraries(test1 iodeapi scr4iode)
target_include_directories(test1 PUBLIC "../scr4" "../api")
target_compile_options(test1 PRIVATE -Zp1)

add_subdirectory(test_c_api)
add_subdirectory(test_cpp_api)
