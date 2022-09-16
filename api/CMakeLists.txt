# CMakeList.txt : CMake project for api, include source and define
# project specific logic here.
#

# Add source to this project's executable.
set (DOCS DESCRIPTION.md LEC.md SIMUL.md TBL_CALC.md PRINTING.md CONVERSIONS.md ESTIM.md IDENTITIES.md)
set (UTILS buf.c pack.c per.c yy.c)
set (CCS k_ccall.c k_cccmt.c k_cceqs.c  k_ccidt.c k_cclst.c k_ccscl.c k_cctbl.c k_ccvar.c)
set (OBJS k_vers.c k_objs.c k_objsv.c k_pack.c k_objfile.C k_xdr.c k_objvers.c k_kdb.c k_ws.c k_wsvar.c k_cmp.c k_val.c k_lec.c k_grep.c k_tbl.c k_eqs.c k_lst.c) 
set (BRICKS b_args.c b_errors.c)
set (LEC l_alloc.c l_err.c l_token.c l_cc1.c l_cc2.c l_eqs.c l_link.c l_exec.c l_exec_fns.c l_exec_ops.c l_exec_tfn.c l_exec_mtfn.c l_exec_val.c l_hodrick.c l_debug.c l_newton.c l_secant.c) 
set (SIMUL k_sim_main.c k_sim_order.c k_sim_exo2endo.c k_sim_scc.c)
set (IDT k_exec.c)
set (KSUPER k_super.c)
set (KLANG k_lang.c)
set (GSAMPLE c_cc.c c_calc.c)
set (KPRINT k_print.c k_graph.c)
set (WRT w_wrt.c w_wrt1.c)
set (INI b_iodeini.c)
set (A2M b_a2mini.c)
set (ESTIM k_est.c e_error.c e_est.c e_prep.c e_print.c e_step.c e_tests.c e_dftest.c)
set (REPORTS b_data.c b_ras.c b_est.c b_fsys.c b_file.c b_idt.c b_xode.c b_ltoh.c b_htol.c b_model.c) 
set (CONVERSIONS k_rules.c k_emain.c k_ecsv.c k_edif.c k_emain.c k_etsp.c k_ewks.c k_wks.c k_imain.c k_iasc.c k_ibst.c k_idif.c k_igem.c k_imain.c k_inis.c k_iprn.c k_irasc.c k_itxt.c)  
set (SOURCES 
		${UTILS} 
		${OBJS}
		${CCS}
		${BRICKS}
		${LEC}
		${SIMUL}
		${IDT}
		${KSUPER} 
		${KLANG}
		${GSAMPLE}
		${KPRINT}
		${WRT}
		${INI}
		${A2M}
		${ESTIM}
		${REPORTS}
		${CONVERSIONS}
)

set (HEADERS 
		iode.h iodebase.h iodeapi.h iodevarg.h
)

add_library (iodeapi STATIC ${SOURCES} ${HEADERS} ${DOCS})

# NOTE: use PRIVATE to NOT propagate compile options to other lib/executable depending on iodeapi.
#       Option -Zp1 makes compilation of the Google Test lib to fail
target_compile_options(iodeapi PRIVATE -Zp1)
target_link_libraries(iodeapi PUBLIC "scr4iode")
target_include_directories(iodeapi PUBLIC "../scr4")
