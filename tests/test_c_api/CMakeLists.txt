if(BUILD_TESTING)

add_executable(test_c_api pch.cpp test_period.cpp test_sample.cpp test_c_api.cpp test_subsets.cpp 
                                  test_equation.cpp test_identity.cpp test_scalar.cpp test_table.cpp 
                                  test_kdb_comments.cpp test_kdb_equations.cpp test_kdb_identities.cpp 
                                  test_kdb_lists.cpp test_kdb_scalars.cpp test_kdb_tables.cpp 
                                  test_kdb_variables.cpp)
target_link_libraries(test_c_api PUBLIC gtest_main iode_cpp_api)
target_link_options(test_c_api PUBLIC $<$<CXX_COMPILER_ID:MSVC>:/FORCE:MULTIPLE>)

include(GoogleTest)
gtest_discover_tests(test_c_api)
endif()
