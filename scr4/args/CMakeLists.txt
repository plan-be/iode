# Source files for this scr4 component.
target_sources(iode_scr4 PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/s_ainit.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_aerr.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_aexec.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_aexp.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_aiexp.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_acerr.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_aexps.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_aexpsd.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_acheck.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_achkin.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_achkac.c"
)
