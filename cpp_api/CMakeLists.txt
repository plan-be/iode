

add_library(iodecppapi STATIC "cpp_api.cpp" "cpp_api.h")


# Compiler options
add_compile_options(-Zi -Zp1)
add_compile_definitions(DOS VC WINDOWS_IGNORE_PACKING_MISMATCH WINDOWS DOSW32 REALD SCRPROTO IODEWIN NOEMS NOEMF)

target_link_libraries(iodecppapi PUBLIC iodeapi)