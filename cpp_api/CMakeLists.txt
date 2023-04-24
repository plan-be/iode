set(CPP_UTILS
    utils/utils.h     utils/iode_exceptions.h
    utils/super.h     utils/super.cpp
    )

set(CPP_TIME
    time/period.h   time/period.cpp
    time/sample.h   time/sample.cpp
    )

set(CPP_REPORT
    report/rep_list.h   report/rep_list.cpp
)

set(CPP_OBJS
    objects/equation.h      objects/equation.cpp
    objects/identity.h      objects/identity.cpp
    objects/table.h         objects/table.cpp
    objects/scalar.h        objects/scalar.cpp
    )

set(CPP_KDB 
    KDB/kdb_global.h        KDB/kdb_global.cpp
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

set(CPP_LEGACY  
    legacy/s_iode.c legacy/b_legacy.c
   )

add_library(iodecppapi STATIC 
            iode_cpp_api.h      iode_cpp_api.cpp 
            common.h
            ${CPP_UTILS}
            ${CPP_REPORT}
            ${CPP_TIME}
            ${CPP_OBJS}
            ${CPP_KDB}
            ${CPP_COMPUTE}
            ${CPP_LEGACY}
            )

set_source_files_properties(objects/equation.cpp objects/table.cpp 
                            KDB/kdb_global.cpp 
                            report/rep_list.cpp
                            PROPERTIES COMPILE_FLAGS -Zp1)
target_include_directories(iodecppapi PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(iodecppapi PUBLIC iodeapi)
