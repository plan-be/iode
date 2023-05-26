# CMakeList.txt : CMake project for api, include source and define
# project specific logic here.
#

add_library(iodeapi STATIC "")

# Add source to this project's library.
add_subdirectory(utils)

target_sources(iodeapi PRIVATE k_ccall.c k_cccmt.c k_cceqs.c  k_ccidt.c k_cclst.c k_ccscl.c k_cctbl.c k_ccvar.c) # CCS
target_sources(iodeapi PRIVATE k_vers.c k_objs.c k_objsv.c k_pack.c k_objfile.c k_xdr.c k_objvers.c k_kdb.c k_ws.c k_wsvar.c k_cmp.c k_val.c k_lec.c k_grep.c k_tbl.c k_eqs.c k_lst.c)  # OBJS
target_sources(iodeapi PRIVATE b_args.c b_errors.c) # BRICKS
target_sources(iodeapi PRIVATE l_alloc.c l_err.c l_token.c l_cc1.c l_cc2.c l_eqs.c l_link.c l_exec.c l_exec_fns.c l_exec_ops.c l_exec_tfn.c l_exec_mtfn.c l_exec_val.c l_hodrick.c l_debug.c l_newton.c l_secant.c)  # LEC
target_sources(iodeapi PRIVATE k_sim_main.c k_sim_order.c k_sim_exo2endo.c k_sim_scc.c) # SIMUL
target_sources(iodeapi PRIVATE k_exec.c) # IDT
target_sources(iodeapi PRIVATE k_super.c) # KSUPER
target_sources(iodeapi PRIVATE k_lang.c) # KLANG
target_sources(iodeapi PRIVATE c_cc.c c_calc.c) # GSAMPLE
target_sources(iodeapi PRIVATE k_print.c k_graph.c) # KPRINT
target_sources(iodeapi PRIVATE w_wrt.c w_wrt1.c) # WRT
target_sources(iodeapi PRIVATE b_iodeini.c) # INI
target_sources(iodeapi PRIVATE b_a2mini.c) # A2M
target_sources(iodeapi PRIVATE k_est.c e_error.c e_est.c e_prep.c e_print.c e_step.c e_tests.c e_dftest.c) # ESTIM
target_sources(iodeapi PRIVATE b_data.c b_ras.c b_est.c b_fsys.c b_file.c b_idt.c b_xode.c b_ltoh.c b_htol.c b_model.c b_ws.c) # REPORT_COMMANDS
target_sources(iodeapi PRIVATE b_rep_cmds.c b_rep_debug.c b_rep_defs.c b_rep_engine.c b_rep_fns.c b_rep_foreach.c b_rep_proc.c b_rep_super.c b_rep_syntax.c b_rep_utils.c) # REPORT_ENGINE
target_sources(iodeapi PRIVATE b_dde.c b_pdest.c b_print.c b_view.c b_step.c b_season.c b_trend.c b_eviews.c b_ds.c b_sql.c b_api.c) # REPORT_UNDOC
target_sources(iodeapi PRIVATE k_rules.c k_emain.c k_ecsv.c k_edif.c k_emain.c k_etsp.c k_ewks.c k_wks.c k_imain.c k_iasc.c k_ibst.c k_idif.c k_igem.c k_imain.c k_inis.c k_iprn.c k_irasc.c k_itxt.c)   # CONVERSIONS

target_sources(iodeapi PUBLIC iode.h iodebase.h iodeapi.h)

#add_library (iodeapi STATIC ${SOURCES} ${HEADERS})

# NOTE: use PRIVATE to NOT propagate compile options to other lib/executable depending on iodeapi.
#       Option -Zp1 makes compilation of the Google Test lib to fail
target_compile_options(iodeapi PRIVATE -Zp1)
target_link_libraries(iodeapi PUBLIC "scr4iode")
target_include_directories(iodeapi PRIVATE "../scr4" ${CMAKE_CURRENT_SOURCE_DIR})
