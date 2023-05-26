# CMakeList.txt : CMake project for api, include source and define
# project specific logic here.
#

add_library(iodeapi STATIC "")

# Add source to this project's library.
add_subdirectory(utils)
add_subdirectory(ascii)
add_subdirectory(objs)
add_subdirectory(lec)
add_subdirectory(simulation)
add_subdirectory(estimation)

target_sources(iodeapi PRIVATE b_args.c b_errors.c) # BRICKS
target_sources(iodeapi PRIVATE k_exec.c) # IDT
target_sources(iodeapi PRIVATE k_super.c) # KSUPER
target_sources(iodeapi PRIVATE k_lang.c) # KLANG
target_sources(iodeapi PRIVATE c_cc.c c_calc.c) # GSAMPLE
target_sources(iodeapi PRIVATE k_print.c k_graph.c) # KPRINT
target_sources(iodeapi PRIVATE w_wrt.c w_wrt1.c) # WRT
target_sources(iodeapi PRIVATE b_iodeini.c) # INI
target_sources(iodeapi PRIVATE b_a2mini.c) # A2M
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
