# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.16)

project ("iode")

# Include sub-projects.
add_subdirectory ("scr4")
add_subdirectory ("api")
add_subdirectory ("tests")