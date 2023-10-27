message(STATUS "\n---- TESTS PYTHON ----\n")

message(CHECK_START "Search for Python")
find_package(Python COMPONENTS Interpreter Development)

if(Python_FOUND AND TARGET iode_python)
  message(CHECK_PASS "found")
  message(STATUS "Python EXECUTABLE: ${Python_EXECUTABLE}")

  set(Python_VERSION_ "${Python_VERSION_MAJOR}${Python_VERSION_MINOR}")

  add_custom_target(test_python
                    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/pyiode/py${Python_VERSION_}/iode.pyd" "iode.pyd" 
                    COMMAND pytest --durations=0 -vv
                    DEPENDS test_iode.py
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Running tests for Python IODE")

  add_dependencies(test_python iode_python)

  message(STATUS "\n-> copy python test files <-")
  file(INSTALL ${CMAKE_SOURCE_DIR}/tests/test_python DESTINATION ${CMAKE_BINARY_DIR}/tests
       FILES_MATCHING PATTERN "*.py")

  else()
  message(CHECK_FAIL "not found")
endif()