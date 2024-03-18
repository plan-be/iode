
message(STATUS "\n---- CYTHON JMP 12/2023 ----\n")

# ---- GENERATE FILE FROM TEMPLATE ----
# configure_file() substitutes variable values referenced as @VAR@ or ${VAR} in the input file content. 
# Each variable reference will be replaced with the current value of the variable, or the empty string 
# if the variable is not defined.
# see https://cmake.org/cmake/help/latest/command/configure_file.html#command:configure_file 
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cythonize_iode.py.in
  ${CMAKE_CURRENT_SOURCE_DIR}/cythonize_iode.py
)
