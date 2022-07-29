enable_testing()

add_executable(test_c_api pch.cpp test_c_api.cpp)

target_link_libraries(test_c_api PUBLIC gtest_main iodeapi)
target_link_options(test_c_api PUBLIC $<$<CXX_COMPILER_ID:MSVC>:/FORCE:MULTIPLE>)

file(INSTALL ${PROJECT_SOURCE_DIR}/iode.msg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

include(GoogleTest)
gtest_discover_tests(test_c_api)