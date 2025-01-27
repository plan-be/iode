message(STATUS "======== start archives targets ========")

set(IODE_CLI_TARGET archives_cpp_cli)
add_subdirectory("cli")

set(IODE_CPP_GUI_TARGET archives_cpp_gui)
add_subdirectory("cpp_gui")

add_subdirectory("pyiode_nanobind")

message(STATUS "======== end archives targets ========")
