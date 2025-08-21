
message(STATUS "\n---- C++ API ----\n")

set(CPP_UTILS
    utils/super.h     utils/super.cpp
    )

set(CPP_REPORT
    report/report.h     report/report.cpp
    report/rep_list.h   report/rep_list.cpp
)

set(CPP_OBJS
    objects/equation.h      objects/equation.cpp
    objects/table.h         objects/table.cpp
    )

set(CPP_KDB 
    KDB/kdb_global.h        KDB/kdb_global.cpp
    KDB/kdb_reference.h     KDB/kdb_reference.cpp
    KDB/kdb_abstract.h      KDB/kdb_abstract.cpp
    KDB/kdb_template.h
    KDB/kdb_comments.h      KDB/kdb_comments.cpp
    KDB/kdb_equations.h     KDB/kdb_equations.cpp
    KDB/kdb_identities.h    KDB/kdb_identities.cpp
    KDB/kdb_lists.h         KDB/kdb_lists.cpp
    KDB/kdb_scalars.h       KDB/kdb_scalars.cpp
    KDB/kdb_tables.h        KDB/kdb_tables.cpp
    KDB/kdb_variables.h     KDB/kdb_variables.cpp
    )

set(CPP_COMPUTE
    compute/simulation.h    compute/simulation.cpp
    compute/estimation.h    compute/estimation.cpp
    )

set(CPP_LEC 
    lec/lec.h               lec/lec.cpp
    )

set(CPP_COMPUTED_TABLE
    computed_table/computed_table.h             computed_table/computed_table.cpp
    computed_table/computed_table_graph.h       computed_table/computed_table_graph.cpp
    )

set(CPP_LEGACY  legacy/s_iode.c)

add_library(iode_cpp_api STATIC 
            iode_cpp_api.h      iode_cpp_api.cpp 
            common.h
            ${CPP_UTILS}
            ${CPP_REPORT}
            ${CPP_OBJS}
            ${CPP_KDB}
            ${CPP_COMPUTE}
            ${CPP_LEC}
            ${CPP_COMPUTED_TABLE}
            ${CPP_LEGACY}
            )

target_include_directories(iode_cpp_api PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(iode_cpp_api PUBLIC iode_c_api)

if(TARGET iode_cpp_api)
	message(STATUS "New target iode_cpp_api enabled\n")
else()
	message(WARNING "Could not create the target iode_cpp_api\n")
endif()
