# CMakeList.txt : CMake project for api, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.16)

project ("iodeapi" C)

# Add source to this project's executable.
set (SOURCES buf.c k_pack.c)
set (HEADERS iode.h iodebase.h iodevarg.h)
add_library (iodeapi STATIC ${SOURCES} ${HEADERS})

# Compiler options
add_compile_options(-Zi -Zp1)
add_compile_definitions(DOS VC WINDOWS_IGNORE_PACKING_MISMATCH WINDOWS DOSW32 REALD SCRPROTO IODEWIN NOEMS NOEMF)

target_link_libraries(iodeapi PUBLIC "scr4iode")
target_include_directories(iodeapi PUBLIC "../scr4")

# TODO: Add tests and install targets if needed.
