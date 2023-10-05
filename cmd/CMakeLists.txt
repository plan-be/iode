# CMakeList.txt : CMake project for ${IODE_CLI_TARGET}

message(STATUS "\n---- IODE COMMAND LINE ----\n")

file(INSTALL ${PROJECT_SOURCE_DIR}/iode.msg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(${IODE_CLI_TARGET} "iodecmd.c")

target_link_libraries(${IODE_CLI_TARGET} PUBLIC iode_scr4 iode_c_api)
target_include_directories(${IODE_CLI_TARGET} PUBLIC "../api")

if(TARGET ${IODE_CLI_TARGET})
	message(STATUS "New target ${IODE_CLI_TARGET} enabled\n")
else()
	message(WARNING "Could not create the target ${IODE_CLI_TARGET}\n")
endif()
