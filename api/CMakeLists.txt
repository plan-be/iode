# CMakeList.txt : CMake project for api, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.16)

project ("iodeapi")

# Add source to this project's executable.
add_library (iodeapi STATIC "api.cpp" "api.h")

target_link_libraries(iodeapi PUBLIC "scr4iode")
target_include_directories(iodeapi PUBLIC "../scr4")

# TODO: Add tests and install targets if needed.
