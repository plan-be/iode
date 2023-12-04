# Find the required packages

message(STATUS "\n---- CYTHON ----\n")

if(Python_FOUND)
  message(CHECK_START "Search for Cython")
  find_program(CYTHON "cython")
  if(CYTHON)
    message(CHECK_PASS "found")
    message(STATUS "\nSet up target for iode Python package\n")
    cmake_print_variables(CYTHON)

    # ---- GENERATE FILE FROM TEMPLATE ----
    # configure_file() substitutes variable values referenced as @VAR@ or ${VAR} in the input file content. 
    # Each variable reference will be replaced with the current value of the variable, or the empty string 
    # if the variable is not defined.
    # see https://cmake.org/cmake/help/latest/command/configure_file.html#command:configure_file 
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/common.pyx.in ${CMAKE_CURRENT_SOURCE_DIR}/common.pyx)

    file(INSTALL ${CMAKE_CURRENT_SOURCE_DIR}/s_pyiode.cpp DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(INSTALL ${CMAKE_CURRENT_SOURCE_DIR}/iode DESTINATION ${CMAKE_CURRENT_BINARY_DIR} FILES_MATCHING PATTERN "*.py")

    # WARNING: 
    # 1. Files hierarchy: 
    #    - https://stackoverflow.com/a/10535470
    #    - https://github.com/cython/cython/wiki/PackageHierarchy
    # 2. Link Extension module with C/C++ libraries:
    #    - If several pyx files working on the same C/C++ memory space -> must link to SHARED C/C++ libraries.
    #      See https://groups.google.com/g/cython-users/c/GAAPYb2X304 
    #      -> Let two pyx files being compiled separately and links to the same C/C++ library. 
    #         If statically linked, each extension module will have a independent pool itself. 
    #         The problem exists for cdef way, too.
    #         See https://groups.google.com/g/cython-users/c/GAAPYb2X304/m/rF8T9LCJAwAJ 

    # Notes: - See https://scikit-build-core.readthedocs.io/en/latest/getting_started.html#cmake-file 
    #        - add_custom_command() defines a command to generate specified OUTPUT file(s). A target created in the 
    #          same directory (CMakeLists.txt file) that specifies any output of the custom command as a source file 
    #          is given a rule to generate the file using the command at build time.
    #        - In Python, when running 'import iode', Python will look for a function named PyInit_iode 
    #          in the .pyd file. By default, Cython will use the name of the compiled .pyx file to generate 
    #          the PyInit_(...) function. This behavior is override by the --module-name option as below. 
    add_custom_command(OUTPUT iode_python.cpp
                      DEPENDS iode_python.pyx s_pyiode.cpp
                      VERBATIM
                      COMMAND ${CYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/iode_python.pyx -3 --verbose --cplus 
                      --line-directives --include-dir ${CMAKE_CURRENT_SOURCE_DIR} 
                      --directive binding=True,embedsignature=True,embedsignature.format=python
                      --module-name iode
                      --output-file ${CMAKE_CURRENT_BINARY_DIR}/iode_python.cpp)

    python_add_library(iode_python MODULE ${CMAKE_CURRENT_BINARY_DIR}/iode_python.cpp WITH_SOABI)

    target_link_libraries(iode_python PRIVATE iode_cpp_api Python::NumPy)
    target_include_directories(iode_python PRIVATE ${CMAKE_SOURCE_DIR}/scr4 ${CMAKE_SOURCE_DIR}/api ${CMAKE_SOURCE_DIR}/cpp_api)
    target_compile_definitions(iode_python PUBLIC NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION)

    add_custom_command(TARGET iode_python PRE_BUILD COMMAND ${CYTHON} --version)
    add_custom_command(TARGET iode_python POST_BUILD
                      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:iode_python> ${CMAKE_CURRENT_BINARY_DIR}/iode.pyd 
                      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/iode.msg ${CMAKE_CURRENT_BINARY_DIR}/iode.msg 
                      COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/tests/data ${CMAKE_CURRENT_BINARY_DIR}/tests/data
                      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                      )

    install(TARGETS iode_python LIBRARY DESTINATION iode)
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
