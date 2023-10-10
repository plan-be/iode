message(STATUS "\n---- TESTS PYTHON (NANOBIND) ----\n")

message(CHECK_START "Search for Python")
find_package(Python COMPONENTS Interpreter Development)

if(Python_FOUND AND TARGET iode)
  message(CHECK_PASS "found")
  message(STATUS "Python EXECUTABLE: ${Python_EXECUTABLE}")

  set(Python_VERSION_ "${Python_VERSION_MAJOR}${Python_VERSION_MINOR}")

  # See https://cmake.org/cmake/help/latest/manual/cmake.1.html#cmdoption-cmake-E-arg-copy
  #     https://cmake.org/cmake/help/latest/manual/cmake.1.html#cmdoption-cmake-E-arg-copy_directory
  add_custom_target(test_nanobind
                    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_BINARY_DIR}/python_api" "."
                    COMMAND pytest --durations=0
                    DEPENDS test_iode.py
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Running tests for Python IODE")

  add_dependencies(test_nanobind iode)

  message(STATUS "\n-> copy python test files <-")
  file(INSTALL ${CMAKE_SOURCE_DIR}/tests/test_nanobind DESTINATION ${CMAKE_BINARY_DIR}/tests
       FILES_MATCHING PATTERN "*.py")

  else()
  message(CHECK_FAIL "not found")
endif()