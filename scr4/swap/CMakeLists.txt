# Source files for this scr4 component.
target_sources(iode_scr4 PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/s_swallc.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_swfile.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_swab.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_swsegs.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_swems.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_swmisc.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_swvars.c"
)
