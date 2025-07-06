message(STATUS "======== start archives targets ========")

# set(IODE_API_TARGET archives_api)
# add_subdirectory("api_06_07_2025")

set(IODE_CLI_TARGET archives_cpp_cli)
add_subdirectory("cli")

set(IODE_CPP_GUI_TARGET archives_cpp_gui)
add_subdirectory("cpp_gui")

# -- nsis installer --
if(TARGET ${IODE_CPP_GUI_TARGET})
  add_subdirectory("nsis")
endif()

# add_subdirectory("pyiode_nanobind")

message(STATUS "======== end archives targets ========")
