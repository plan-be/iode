enable_testing()

add_executable(test_cpp_api pch.cpp test_utils.cpp test_lec.cpp 
               test_equation.cpp test_identity.cpp test_scalar.cpp test_table.cpp test_computed_table.cpp 
               test_report.cpp test_simulation.cpp test_estimation.cpp 
               test_kdb_comments.cpp test_kdb_equations.cpp test_kdb_identities.cpp test_kdb_lists.cpp  
               test_kdb_scalars.cpp test_kdb_tables.cpp test_kdb_variables.cpp test_kdb_global.cpp)

target_link_libraries(test_cpp_api PUBLIC gtest_main iode_cpp_api)
target_link_options(test_cpp_api PUBLIC $<$<CXX_COMPILER_ID:MSVC>:/FORCE:MULTIPLE>)

include(GoogleTest)
gtest_discover_tests(test_cpp_api)
