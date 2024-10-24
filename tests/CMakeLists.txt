# CMakeList.txt : CMake project for scr4, include source and define
# project specific logic here.
#

if(NOT SKBUILD)
    add_subdirectory(test_c_api)
    add_subdirectory(test_cpp_api)
endif()

add_subdirectory(test_python)
