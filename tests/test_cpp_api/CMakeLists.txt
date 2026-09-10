if(BUILD_TESTING)
add_executable(${test_cpp_api} pch.cpp)

# Precompiled headers for faster compilation
target_precompile_headers(${test_cpp_api} PRIVATE pch.h)
target_link_libraries(${test_cpp_api} PUBLIC gtest_main iode_cpp_api)
target_link_options(${test_cpp_api} PUBLIC $<$<CXX_COMPILER_ID:MSVC>:/FORCE:MULTIPLE>)

message(STATUS "new TESTS target: ${test_cpp_api}")

include(GoogleTest)
gtest_discover_tests(${test_cpp_api})
endif()
