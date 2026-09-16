# Source files for this scr4 component.
target_sources(iode_scr4 PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/s_gzadlr.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_gzcmpr.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_gzcrc.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_gzio.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_gzucmp.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_gzdefl.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_gztree.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_gzip.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_gzutil.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_gzinfl.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_gzinbl.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_gzintr.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_gzincd.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_gzinut.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_gzinfa.c"
)
