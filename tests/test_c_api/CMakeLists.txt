
file(INSTALL ${PROJECT_SOURCE_DIR}/iode.msg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(test1 "test1.c")
target_link_libraries(test1 iode_c_api iode_scr4)
target_include_directories(test1 PUBLIC "../../scr4" "../../api")

add_executable(test1cpp "test1cpp.cpp")
target_link_libraries(test1cpp iode_c_api iode_scr4)
target_include_directories(test1cpp PUBLIC "../../scr4" "../../api")

enable_testing()

add_executable(test_c_api pch.cpp test_c_api.cpp)
target_link_libraries(test_c_api PUBLIC gtest_main iode_c_api)
target_link_options(test_c_api PUBLIC $<$<CXX_COMPILER_ID:MSVC>:/FORCE:MULTIPLE>)

include(GoogleTest)
gtest_discover_tests(test_c_api)