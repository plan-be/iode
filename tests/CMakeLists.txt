# CMakeList.txt : CMake project for scr4, include source and define
# project specific logic here.
#

add_executable(tests "tests.cpp" "tests.h")
target_link_libraries(tests iodeapi scr4iode)
target_include_directories(tests PUBLIC "../scr4" "../api")

add_compile_options(-Zi -Zp1)
add_compile_definitions(WINDOWS_IGNORE_PACKING_MISMATCH)

add_executable(test1 "test1.c")
target_link_libraries(test1 iodeapi scr4iode)
target_include_directories(test1 PUBLIC "../scr4" "../api")
