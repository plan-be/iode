# CMakeList.txt : CMake project for api, include source and define
# project specific logic here.
#

# Add source to this project's executable.
set (UTILS buf.c pack.c per.c yy.c)
set (CCS k_ccall.c k_cccmt.c k_cceqs.c  k_ccidt.c k_cclst.c k_ccscl.c k_cctbl.c k_ccvar.c)
set (OBJS k_objs.c k_objsv.c k_pack.c k_objfile.C k_objvers.c k_kdb.c k_ws.c)

set (SOURCES 
		${UTILS} 
		${OBJS}
		${CCS}
		k_vers.c
		k_lec.c 
		b_dir.c 
)

set (HEADERS 
		iode.h iodebase.h iodevarg.h
)

add_library (iodeapi STATIC ${SOURCES} ${HEADERS})

# Compiler options
add_compile_options(-Zi -Zp1)
add_compile_definitions(DOS VC WINDOWS_IGNORE_PACKING_MISMATCH WINDOWS DOSW32 REALD SCRPROTO IODEWIN NOEMS NOEMF)

target_link_libraries(iodeapi PUBLIC "scr4iode")
target_include_directories(iodeapi PUBLIC "../scr4")
