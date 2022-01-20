# CMakeList.txt : CMake project for api, include source and define
# project specific logic here.
#

# Add source to this project's executable.
set (DOCS DESCRIPTION.md LEC.md)
set (UTILS buf.c pack.c per.c yy.c)
set (CCS k_ccall.c k_cccmt.c k_cceqs.c  k_ccidt.c k_cclst.c k_ccscl.c k_cctbl.c k_ccvar.c)
set (OBJS k_vers.c k_objs.c k_objsv.c k_pack.c k_objfile.C k_xdr.c k_objvers.c k_kdb.c k_ws.c  k_wsvar.c k_cmp.c k_val.c k_lec.c k_grep.c k_tbl.c k_eqs.c) 
set (BRICKS b_args.c b_errors.c)
set (LEC l_alloc.c l_err.c l_token.c l_cc1.c l_cc2.c l_eqs.c l_link.c l_exec.c l_exec_fns.c l_exec_ops.c l_exec_tfn.c l_exec_mtfn.c l_exec_val.c l_hodrick.c k_lec.c) 
set (KSUPER k_super.c)
#set (WRT w_wrt.c w_wrt1.c w_wrtv.c)
#set (A2M b_a2mini.c)
 
set (SOURCES 
		${UTILS} 
		${OBJS}
		${CCS}
		${BRICKS}
		${LEC}
		${KSUPER}
		#${WRT}
		#${A2M}
)

set (HEADERS 
		iode.h iodebase.h iodeapi.h iodevarg.h
)

add_library (iodeapi STATIC ${SOURCES} ${HEADERS} ${DOCS})

# Compiler options
add_compile_options(-Zi -Zp1)
add_compile_definitions(WINDOWS_IGNORE_PACKING_MISMATCH DOS VC WINDOWS_IGNORE_PACKING_MISMATCH WINDOWS DOSW32 REALD SCRPROTO IODEWIN NOEMS NOEMF)

target_link_libraries(iodeapi PUBLIC "scr4iode")
target_include_directories(iodeapi PUBLIC "../scr4")
