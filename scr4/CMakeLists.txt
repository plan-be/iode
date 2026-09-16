# CMakeList.txt : CMake project for scr4, include source and define
# project specific logic here.
#

message(STATUS "\n---- SCR4 ----\n")

# Add source to this project's executable.
add_library(iode_scr4 STATIC)

# Source files
add_subdirectory(swap)
add_subdirectory(lst)
add_subdirectory(base)
add_subdirectory(ini)
add_subdirectory(strs)
add_subdirectory(dde)
add_subdirectory(yy)
add_subdirectory(mat)
add_subdirectory(a2m)
add_subdirectory(args)
add_subdirectory(date)
add_subdirectory(cpp)
add_subdirectory(odbc)
add_subdirectory(gzip)
add_subdirectory(wsys)
add_subdirectory(isc)
# target_include_directories(iode_scr4 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
	target_compile_options(iode_scr4 PRIVATE "-Wno-implicit-int")
	target_compile_options(iode_scr4 PRIVATE "-Wno-implicit-function-declaration")
endif()

if (MSVC)
	target_compile_options(iode_scr4 PRIVATE
		/W4
		# /WX	# Treats all compiler warnings as errors.
		/we4013 # function undefined; assuming extern returning int
		/we4020 # too many actual parameters
		/we4024 # different types for formal and actual parameter
		/we4047 # different levels of indirection
		/we4113 # function parameter lists differ
		/we4133 # incompatible pointer types
		/we4431 # missing type specifier; int assumed
	)
endif()

if(TARGET iode_scr4)
	message(STATUS "New target iode_scr4 enabled\n")
else()
	message(WARNING "Could not create the target iode_scr4\n")
endif()
