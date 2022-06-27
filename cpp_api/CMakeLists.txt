
add_library(iodecppapi STATIC "iode_cpp_api.cpp" "iode_cpp_api.h" 
                              "common.h" "utils.h" 
                              "period.h" "sample.h"
                              "super.h" "super.cpp"
                              "equation.h" "equation.cpp"
                              "table.h" "table.cpp" 
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

# NOTE: use PRIVATE to NOT propagate compile options to other lib/executable depending on iodeapi.
#       Option -Zp1 makes compilation of the Google Test lib to fail
target_compile_options(iodecppapi PRIVATE -Zp1)
target_link_libraries(iodecppapi PUBLIC iodeapi)
