
message(STATUS "\n---- GUI (QT) ----\n")

file(INSTALL ${CMAKE_SOURCE_DIR}/LICENSE.md DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})
file(INSTALL ${CMAKE_SOURCE_DIR}/doc/build/keyboard_shortcuts.pdf DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/iode_gui/doc)

set(RESOURCE_FILENAME "iode_resource")
set(RC_RESOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/iode_gui/resources/${RESOURCE_FILENAME}.rc")
set(QRC_RESOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/iode_gui/resources/${RESOURCE_FILENAME}.qrc")
set(PY_RESOURCE_FILENAME "${RESOURCE_FILENAME}_rc")
set(PY_RESOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/iode_gui/${PY_RESOURCE_FILENAME}.py")

# configure_file() substitutes variable values referenced as @VAR@ or ${VAR} in the input file content. 
# Each variable reference will be replaced with the current value of the variable, or the empty string 
# if the variable is not defined.
# see https://cmake.org/cmake/help/latest/command/configure_file.html#command:configure_file 
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml.in ${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/iode_gui/__init__.py.in ${CMAKE_CURRENT_SOURCE_DIR}/iode_gui/__init__.py)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/iode_gui/utils.py.in ${CMAKE_CURRENT_SOURCE_DIR}/iode_gui/utils.py)
