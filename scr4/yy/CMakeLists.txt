# Source files for this scr4 component.
target_sources(iode_scr4 PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/s_ybase.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_ydef.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_yincl.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_ylex.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_yread.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_ydebug.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_yerror.c"
)
