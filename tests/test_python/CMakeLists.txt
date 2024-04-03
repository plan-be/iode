message(STATUS "\n---- TESTS PYTHON ----\n")

if(Python_FOUND AND TARGET iode_python)
  message(STATUS "Python EXECUTABLE: ${Python_EXECUTABLE}")

  add_custom_target(test_python
                    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/pyiode/iode.pyd iode.pyd
                    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/pyiode/iode.msg iode.msg
                    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/pyiode/tests/data ./tests/data
                    COMMAND pytest
                    DEPENDS test_iode.py test_cython_iode.py
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Running tests for Python IODE")

  add_dependencies(test_python iode_python)

  # add_custom_target(test_nanobind
  #                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:iode_python_api> to ${CMAKE_BINARY_DIR}/tests/test_nanobind
  #                   COMMAND pytest
  #                   DEPENDS test_iode.py
  #                   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  #                   COMMENT "Running tests for Python IODE")
  # 
  # add_dependencies(test_nanobind iode_python_api)

  message(STATUS "\n-> copy python test files <-")
  file(INSTALL ${CMAKE_SOURCE_DIR}/tests/test_python DESTINATION ${CMAKE_BINARY_DIR}/tests
       FILES_MATCHING PATTERN "*.py")
  file(INSTALL ${CMAKE_SOURCE_DIR}/tests/test_python/ DESTINATION ${CMAKE_BINARY_DIR}/tests/test_nanobind
       FILES_MATCHING PATTERN "*.py")

  else()
  message(STATUS "Python not found")
endif()