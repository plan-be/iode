# CMakeList.txt : CMake project for scr4, include source and define
# project specific logic here.
#

add_executable(tests "tests.cpp" "tests.h")

target_link_libraries(tests iodeapi scr4iode)
target_include_directories(tests PUBLIC "../scr4" "../api")

