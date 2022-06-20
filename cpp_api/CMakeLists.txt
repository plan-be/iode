
add_library(iodecppapi STATIC "iode_cpp_api.cpp" "iode_cpp_api.h" 
                              "common.h" "utils.h" 
                              "iode_exceptions.h"
                              "period.h" "sample.h"
                              "super.h" "super.cpp"
                              "equation.h" "equation.cpp"
                              "table.h" "table.cpp"
                              "simulation.h" "simulation.cpp"
                              "KDB/kdb_global.h" "KDB/kdb_global.cpp"
                              "KDB/kdb_template.h"
                              "KDB/kdb_abstract.h" "KDB/kdb_abstract.cpp" 
                              "KDB/kdb_comments.h" "KDB/kdb_comments.cpp" 
                              "KDB/kdb_equations.h" "KDB/kdb_equations.cpp"
                              "KDB/kdb_identities.h" "KDB/kdb_identities.cpp"
                              "KDB/kdb_lists.h" "KDB/kdb_lists.cpp"
                              "KDB/kdb_scalars.h" "KDB/kdb_scalars.cpp"
                              "KDB/kdb_tables.h" "KDB/kdb_tables.cpp" 
                              "KDB/kdb_variables.h" "KDB/kdb_variables.cpp"
                              "legacy/s_iode.c" "legacy/sb_gui.c" "legacy/b_legacy.c")

set_source_files_properties(equation.cpp table.cpp PROPERTIES COMPILE_FLAGS -Zp1)
target_link_libraries(iodecppapi PUBLIC iodeapi)
