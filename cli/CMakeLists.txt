# CMakeList.txt : CMake project for ${IODE_CLI_TARGET}

message(STATUS "\n---- IODE COMMAND LINE ----\n")

if(SKBUILD)
	message(STATUS "Skip target ${IODE_CLI_TARGET}\n")
else()
	file(INSTALL ${PROJECT_SOURCE_DIR}/iode.msg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

	add_executable(${IODE_CLI_TARGET} "iodecmd.cpp")

	target_link_libraries(${IODE_CLI_TARGET} PUBLIC iode_cpp_api)
	target_include_directories(${IODE_CLI_TARGET} PUBLIC "${CMAKE_SOURCE_DIR}\\api")

	add_custom_command(TARGET ${IODE_CLI_TARGET} POST_BUILD
    	COMMAND ${CMAKE_COMMAND} -E echo "Copy $<TARGET_FILE:${IODE_CLI_TARGET}> into test/cli"
    	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${IODE_CLI_TARGET}> ${CMAKE_SOURCE_DIR}/tests/cli/$<TARGET_FILE_NAME:${IODE_CLI_TARGET}>
	)

	if(TARGET ${IODE_CLI_TARGET})
		message(STATUS "New target ${IODE_CLI_TARGET} enabled\n")
	else()
		message(WARNING "Could not create the target ${IODE_CLI_TARGET}\n")
	endif()
endif()
