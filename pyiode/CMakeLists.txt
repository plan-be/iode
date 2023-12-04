# Find the required packages

message(STATUS "\n---- CYTHON ----\n")

if(Python_FOUND)
  message(CHECK_START "Search for Cython")
  find_program(CYTHON "cython")
  if(CYTHON)
    message(CHECK_PASS "found")
    message(STATUS "\nSet up target for iode Python package\n")
    cmake_print_variables(CYTHON)

    if(MSVC)
      # set __version__ for the iode package
      configure_file(${CMAKE_CURRENT_SOURCE_DIR}/_version.py.in ${CMAKE_CURRENT_SOURCE_DIR}/_version.py)

      file(INSTALL ${CMAKE_SOURCE_DIR}/tests/data/fun.cmt DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/tests/data)
      file(INSTALL ${CMAKE_SOURCE_DIR}/tests/data/fun.eqs DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/tests/data)
      file(INSTALL ${CMAKE_SOURCE_DIR}/tests/data/fun.idt DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/tests/data)
      file(INSTALL ${CMAKE_SOURCE_DIR}/tests/data/fun.lst DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/tests/data)
      file(INSTALL ${CMAKE_SOURCE_DIR}/tests/data/fun.scl DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/tests/data)
      file(INSTALL ${CMAKE_SOURCE_DIR}/tests/data/fun.tbl DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/tests/data)
      file(INSTALL ${CMAKE_SOURCE_DIR}/tests/data/fun.var DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/tests/data)

      file(INSTALL ${CMAKE_CURRENT_SOURCE_DIR}/s_iode.c DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
      file(INSTALL ${CMAKE_CURRENT_SOURCE_DIR}/iode DESTINATION ${CMAKE_CURRENT_BINARY_DIR} FILES_MATCHING PATTERN "*.py")

      # See https://scikit-build-core.readthedocs.io/en/latest/getting_started.html#cmake-file 
      # WARNING: Apparently, the name of the compiled *.pyx must be the same as the target
      add_custom_command(OUTPUT iode_python.c
                        DEPENDS iode_python.pyx s_iode.c iode_python.pxi pyiode_cdef.pyx pyiode_data.pyx pyiode_estim.pyx
                                 pyiode_exec.pyx pyiode_larray.pyx pyiode_lec.pyx pyiode_model.pyx pyiode_objs.pyx 
                                 pyiode_print.pyx pyiode_reports.pyx pyiode_sample.pyx pyiode_util.pyx pyiode_ws.pyx
                        VERBATIM
                        COMMAND "${CYTHON}" "${CMAKE_CURRENT_SOURCE_DIR}/iode_python.pyx" --output-file "${CMAKE_CURRENT_BINARY_DIR}/iode_python.c")

      python_add_library(iode_python MODULE "${CMAKE_CURRENT_BINARY_DIR}/iode_python.c" 
                                            "${CMAKE_CURRENT_BINARY_DIR}/s_iode.c" WITH_SOABI)

      target_link_libraries(iode_python PRIVATE iode_c_api Python::NumPy)
      target_include_directories(iode_python PRIVATE "${CMAKE_SOURCE_DIR}/scr4" "${CMAKE_SOURCE_DIR}/api")
      target_compile_definitions(iode_python PUBLIC NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION)

      add_custom_command(TARGET iode_python PRE_BUILD COMMAND "${CYTHON}" "--version")

      install(TARGETS iode_python LIBRARY DESTINATION iode)
    endif()
  else()
    message(CHECK_FAIL "Cython not found")
  endif()

  if(TARGET iode_python)
    message(STATUS "\nNew target iode_python enabled\n")
  else()
    message(WARNING "Could not create the target iode_python\n")
  endif()
else()
  message(WARNING "Python not found")
endif()
