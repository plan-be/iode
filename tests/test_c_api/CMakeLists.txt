enable_testing()

add_executable(test_c_api pch.cpp test_c_api.cpp)
target_link_libraries(test_c_api PUBLIC gtest_main iode_c_api)
target_link_options(test_c_api PUBLIC $<$<CXX_COMPILER_ID:MSVC>:/FORCE:MULTIPLE>)

include(GoogleTest)
gtest_discover_tests(test_c_api)