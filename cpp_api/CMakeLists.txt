
message(STATUS "\n---- C++ API ----\n")

set(CPP_UTILS
    utils/super.h     utils/super.cpp
    )

set(CPP_REPORT
    report/report.h     report/report.cpp
    report/rep_list.h   report/rep_list.cpp
)

set(CPP_LEC 
    lec/lec.h               lec/lec.cpp
    )

set(CPP_LEGACY  legacy/s_iode.c)

add_library(iode_cpp_api STATIC 
            iode_cpp_api.h      iode_cpp_api.cpp 
            common.h
            ${CPP_UTILS}
            ${CPP_REPORT}
            ${CPP_LEC}
            ${CPP_LEGACY}
            )

target_include_directories(iode_cpp_api PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(iode_cpp_api PUBLIC iode_c_api)

if(TARGET iode_cpp_api)
	message(STATUS "New target iode_cpp_api enabled\n")
else()
	message(WARNING "Could not create the target iode_cpp_api\n")
endif()
