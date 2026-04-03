if(BUILD_TESTING)
add_executable(${test_big_files} test_big_files.cpp)
target_link_libraries(${test_big_files} PUBLIC gtest_main iode_cpp_api)
target_link_options(${test_big_files} PUBLIC $<$<CXX_COMPILER_ID:MSVC>:/FORCE:MULTIPLE>)

message(STATUS "new TESTS target: ${test_big_files}")

include(GoogleTest)
gtest_discover_tests(${test_big_files})
endif()
