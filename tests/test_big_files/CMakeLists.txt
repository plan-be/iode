enable_testing()

add_executable(test_cpp_big_files test_big_files.cpp)
target_link_libraries(test_cpp_big_files PUBLIC gtest_main iode_cpp_api)
target_link_options(test_cpp_big_files PUBLIC $<$<CXX_COMPILER_ID:MSVC>:/FORCE:MULTIPLE>)

include(GoogleTest)
gtest_discover_tests(test_cpp_big_files)
