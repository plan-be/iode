


add_library(iodecppapi STATIC "iode_cpp_api.cpp" "iode_cpp_api.h" "common.h" "utils.h" "period.h" "sample.h" 
                              "KDB/kdb_abstract.h" "KDB/kdb_comments.h" "KDB/kdb_equations.h" "KDB/kdb_identities.h" "KDB/kdb_lists.h" "KDB/kdb_scalars.h"
                               "legacy/s_iode.c" "legacy/sb_gui.c" "legacy/b_legacy.c" "legacy/w_wrt.c" "legacy/w_wrtv.c")

# Compiler options
add_compile_options(-Zi -Zp1)
add_compile_definitions(DOS VC WINDOWS_IGNORE_PACKING_MISMATCH WINDOWS DOSW32 REALD SCRPROTO IODEWIN NOEMS NOEMF)

target_link_libraries(iodecppapi PUBLIC iodeapi)