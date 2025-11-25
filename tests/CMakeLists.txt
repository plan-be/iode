# CMakeList.txt : CMake project for scr4, include source and define
# project specific logic here.
#

if(NOT SKBUILD)
    add_subdirectory(test_c_api)
    add_subdirectory(test_cpp_api)
    add_subdirectory(test_big_files)

    add_custom_target(test_all)
    add_dependencies(test_all test_c_api test_cpp_api test_cpp_big_files)
endif()
