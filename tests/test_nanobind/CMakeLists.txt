message(STATUS "\n---- TESTS PYTHON (NANOBIND) ----\n")

if(Python_FOUND AND TARGET iode_python_api)
  message(STATUS "Python EXECUTABLE: ${Python_EXECUTABLE}")

  set(Python_VERSION_ "${Python_VERSION_MAJOR}${Python_VERSION_MINOR}")

  # See https://cmake.org/cmake/help/latest/manual/cmake.1.html#cmdoption-cmake-E-arg-copy
  #     https://cmake.org/cmake/help/latest/manual/cmake.1.html#cmdoption-cmake-E-arg-copy_directory
  add_custom_target(test_nanobind
                    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_BINARY_DIR}/python_api" "."
                    COMMAND pytest --durations=0 -vv
                    DEPENDS test_iode.py
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Running tests for Python IODE")

  add_dependencies(test_nanobind iode_python_api)

  message(STATUS "\n-> copy python test files <-")
  file(INSTALL ${CMAKE_SOURCE_DIR}/tests/test_nanobind DESTINATION ${CMAKE_BINARY_DIR}/tests
       FILES_MATCHING PATTERN "*.py")

  else()
  message(STATUS "Python not found")
endif()