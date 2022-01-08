


add_library(iodecppapi STATIC "iode_cpp_api.cpp" "iode_cpp_api.h" "common.h" "utils.h" "KDB/kdb_abstract.h" "KDB/kdb_comments.h" "KDB/kdb_equations.h" "period.h" "sample.h")

# Compiler options
add_compile_options(-Zi -Zp1)
add_compile_definitions(DOS VC WINDOWS_IGNORE_PACKING_MISMATCH WINDOWS DOSW32 REALD SCRPROTO IODEWIN NOEMS NOEMF)

target_link_libraries(iodecppapi PUBLIC iodeapi)