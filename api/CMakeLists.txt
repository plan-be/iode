# CMakeList.txt : CMake project for api, include source and define
# project specific logic here.
#

message(STATUS "\n---- C API ----\n")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/version.h)


add_library(iode_c_api STATIC "")

# Add source to this project's library.
add_subdirectory(utils)
add_subdirectory(ascii)
add_subdirectory(objs)
add_subdirectory(lec)
add_subdirectory(gsample)
add_subdirectory(print)
add_subdirectory(write)
add_subdirectory(simulation)
add_subdirectory(estimation)
add_subdirectory(conversion)
add_subdirectory(report)

target_sources(iode_c_api PRIVATE b_args.c b_errors.c) # MISC
target_sources(iode_c_api PRIVATE k_exec.c) # IDT
target_sources(iode_c_api PRIVATE k_super.c) # KSUPER
target_sources(iode_c_api PRIVATE k_lang.c) # KLANG
target_sources(iode_c_api PRIVATE b_iodeini.c) # INI
target_sources(iode_c_api PRIVATE b_a2mini.c) # A2M

target_sources(iode_c_api PUBLIC iode.h iodebase.h iodeapi.h)

# From Wikipedia (https://en.wikipedia.org/wiki/C_mathematical_functions#libm): 
# Under POSIX systems like Linux and BSD, the mathematical functions 
# (as declared in <math.h>) are bundled separately in the mathematical library libm.
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(iode_c_api PUBLIC m)
  	target_compile_options(iode_c_api PRIVATE "-Wno-implicit-int")
  	target_compile_options(iode_c_api PRIVATE "-Wno-implicit-function-declaration")
endif()
target_link_libraries(iode_c_api PUBLIC "iode_scr4")
target_include_directories(iode_c_api PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(TARGET iode_c_api)
	message(STATUS "New target iode_c_api enabled\n")
else()
	message(WARNING "Could not create the target iode_c_api\n")
endif()
