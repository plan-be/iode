
message(STATUS "\n---- C++ API ----\n")

add_library(iode_cpp_api STATIC 
            iode_cpp_api.h      iode_cpp_api.cpp 
            common.h
            legacy/s_iode.c
            )

target_include_directories(iode_cpp_api PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(iode_cpp_api PUBLIC iode_c_api)

if(TARGET iode_cpp_api)
	message(STATUS "New target iode_cpp_api enabled\n")
else()
	message(WARNING "Could not create the target iode_cpp_api\n")
endif()
