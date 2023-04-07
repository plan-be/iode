message(STATUS "\n---- NSIS ----\n")

message(STATUS "Generating the NSIS definitions file ${CMAKE_CURRENT_BINARY_DIR}/iode_definitions.nsh")
message(STATUS "from the template file ${PROJECT_SOURCE_DIR}/nsis/iode_definitions_template.nsh.in")
# ---- GENERATE NSH SCRIPT FROM TEMPLATE ----
# Copies an "iode_definitions_template.nsh.in" file to an "iode_definitions_template.nsh" file 
# and substitutes variable values referenced as @VAR@ or ${VAR} in the input file content. 
# Each variable reference will be replaced with the current value of the variable, or the empty string if the variable is not defined.
# see https://cmake.org/cmake/help/latest/command/configure_file.html#command:configure_file 
configure_file(
  ${PROJECT_SOURCE_DIR}/nsis/iode_definitions_template.nsh.in
  ${CMAKE_CURRENT_BINARY_DIR}/iode_definitions.nsh
)
message(STATUS "DONE\n")

set(IODE_NSIS_SCRIPT "iode.nsi")

message(STATUS "Generating the NSIS script ${CMAKE_CURRENT_BINARY_DIR}/${IODE_NSIS_SCRIPT}")
message(STATUS "from template file ${PROJECT_SOURCE_DIR}/nsis/iode_template.nsi")

file(READ "${PROJECT_SOURCE_DIR}/nsis/iode_template.nsi" FILE_CONTENTS)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${IODE_NSIS_SCRIPT}" "${FILE_CONTENTS}")

# see https://cmake.org/cmake/help/latest/command/cmake_path.html#relative-path 
set(RELATIVE_DIR ${CMAKE_BINARY_DIR})
cmake_path(RELATIVE_PATH RELATIVE_DIR BASE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# GUI executable file
message(STATUS "GUI executable file:")
message(STATUS "\t${RELATIVE_DIR}\\gui\\${IODE_GUI_TARGET}.exe")

set(QT_DIR "${RELATIVE_DIR}\\gui")
file(READ "${CMAKE_CURRENT_BINARY_DIR}/${IODE_NSIS_SCRIPT}" FILE_CONTENTS)
string(REPLACE "<QT DIR>" ${QT_DIR} FILE_CONTENTS "${FILE_CONTENTS}")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${IODE_NSIS_SCRIPT}" "${FILE_CONTENTS}")

# user documentation
message(STATUS "User doc:")
file(GLOB DOC_FILES RELATIVE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR}/doc/*.*)
foreach(DOC_FILE ${DOC_FILES})
  message(STATUS "\t${DOC_FILE}")
endforeach()

set(DOC_DIR "${RELATIVE_DIR}\\doc")
file(READ "${CMAKE_CURRENT_BINARY_DIR}/${IODE_NSIS_SCRIPT}" FILE_CONTENTS)
string(REPLACE "<DOC DIR>" ${DOC_DIR} FILE_CONTENTS "${FILE_CONTENTS}")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${IODE_NSIS_SCRIPT}" "${FILE_CONTENTS}")

# misc
message(STATUS "error messages:")
message(STATUS "\t${RELATIVE_DIR}\\iode.msg")
set(FILES_TO_INSTALL "File ${RELATIVE_DIR}\\iode.msg")
set(FILES_TO_DELETE "Delete $INSTDIR\\iode.msg")

file(READ "${CMAKE_CURRENT_BINARY_DIR}/${IODE_NSIS_SCRIPT}" FILE_CONTENTS)
string(REPLACE "<MISC INSTALL>" ${FILES_TO_INSTALL} FILE_CONTENTS "${FILE_CONTENTS}")
string(REPLACE "<MISC DELETE>" ${FILES_TO_DELETE} FILE_CONTENTS "${FILE_CONTENTS}")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${IODE_NSIS_SCRIPT}" "${FILE_CONTENTS}")

# example files
message(STATUS "Example files:")
file(GLOB EXAMPLE_FILES RELATIVE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR}/tests/data/fun.*)
foreach(EXAMPLE_FILE ${EXAMPLE_FILES})
  message(STATUS "\t${EXAMPLE_FILE}")
endforeach()

set(EXAMPLE_PATH "${RELATIVE_DIR}\\tests\\data\\fun.*")
file(READ "${CMAKE_CURRENT_BINARY_DIR}/${IODE_NSIS_SCRIPT}" FILE_CONTENTS)
string(REPLACE "<EXAMPLE PATH>" ${EXAMPLE_PATH} FILE_CONTENTS "${FILE_CONTENTS}")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${IODE_NSIS_SCRIPT}" "${FILE_CONTENTS}")

# syntax files
message(STATUS "Syntax files:")
file(GLOB SYNTAX_FILES RELATIVE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR}/syntax/*.*)
foreach(SYNTAX_FILE ${SYNTAX_FILES})
  message(STATUS "\t${SYNTAX_FILE}")
endforeach()

set(SYNTAX_PATH "${RELATIVE_DIR}\\syntax")
file(READ "${CMAKE_CURRENT_BINARY_DIR}/${IODE_NSIS_SCRIPT}" FILE_CONTENTS)
string(REPLACE "<SYNTAX PATH>" ${SYNTAX_PATH} FILE_CONTENTS "${FILE_CONTENTS}")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${IODE_NSIS_SCRIPT}" "${FILE_CONTENTS}")

message(STATUS "DONE\n")

# ---- add nsis target ----
ADD_CUSTOM_TARGET(nsis_windows_installer
  COMMAND makensis.exe ${IODE_NSIS_SCRIPT}
  COMMENT "NSIS: Creating Windows installer"
  DEPENDS ${IODE_NSIS_SCRIPT} "${CMAKE_BINARY_DIR}/gui/${IODE_GUI_TARGET}.exe")

ADD_CUSTOM_TARGET(nsis ALL) # Entry point of execution.
ADD_DEPENDENCIES(nsis ${IODE_GUI_TARGET})
ADD_DEPENDENCIES(nsis nsis_windows_installer)
