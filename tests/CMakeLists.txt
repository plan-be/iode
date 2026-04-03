string(TOLOWER ${CMAKE_BUILD_TYPE} build_mode)

if(NOT SKBUILD)
    message(STATUS "\n---- TESTS ----\n")

    set(test_c_api "test_${build_mode}_c_api")
    add_subdirectory(test_c_api)

    set(test_cpp_api "test_${build_mode}_cpp_api")
    add_subdirectory(test_cpp_api)
    
    set(test_big_files "test_${build_mode}_big_files")
    add_subdirectory(test_big_files)

    add_custom_target(test_all)
    add_dependencies(test_all ${test_c_api} ${test_cpp_api} ${test_big_files})
endif()
