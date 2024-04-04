message(STATUS "\n---- TESTS PYTHON ----\n")

if(Python_FOUND)
  message(STATUS "Python EXECUTABLE: ${Python_EXECUTABLE}")

  message(STATUS "\n-> copy python test files <-")
  file(INSTALL ${CMAKE_SOURCE_DIR}/tests/test_python DESTINATION ${CMAKE_BINARY_DIR}/tests
       FILES_MATCHING PATTERN "*.py")

  add_custom_target(test_python COMMAND pytest
                    DEPENDS test_iode.py test_cython_iode.py
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Running tests for Python IODE"
                    )
  else()
  message(STATUS "Python not found")
endif()
