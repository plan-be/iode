# Source files for this scr4 component.
target_sources(iode_scr4 PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/s_vars.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_dirstd.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_dirspd.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_dirsta.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_dirfp.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_dircwd.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_dirj.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_dirfmt.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_dirspn.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/s_wsockc.c"
)
