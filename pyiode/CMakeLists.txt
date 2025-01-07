# Find the required packages

message(STATUS "\n---- CYTHON ----\n")

file(INSTALL ${CMAKE_SOURCE_DIR}/README.md DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})
file(INSTALL ${CMAKE_SOURCE_DIR}/LICENSE.md DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})

# configure_file() substitutes variable values referenced as @VAR@ or ${VAR} in the input file content. 
# Each variable reference will be replaced with the current value of the variable, or the empty string 
# if the variable is not defined.
# see https://cmake.org/cmake/help/latest/command/configure_file.html#command:configure_file 
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml.in ${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/common.pyx.in ${CMAKE_CURRENT_SOURCE_DIR}/common.pyx)
configure_file(${CMAKE_SOURCE_DIR}/generate_stub_files.py.in ${CMAKE_SOURCE_DIR}/generate_stub_files.py)

if(Python_FOUND)
  message(CHECK_START "Search for Cython")
  find_program(CYTHON "cython")
  if(CYTHON)
    message(CHECK_PASS "found")
    message(STATUS "\nSet up target for iode Python package\n")
    cmake_print_variables(CYTHON)

    file(INSTALL ${CMAKE_SOURCE_DIR}/iode.msg DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/iode)
    file(INSTALL ${CMAKE_SOURCE_DIR}/tests/data DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/iode/tests)
    # copy all Python files to binary directory to then be able to generate stub files *.pyi
    file(INSTALL ${CMAKE_CURRENT_SOURCE_DIR}/iode DESTINATION ${CMAKE_CURRENT_BINARY_DIR} 
         FILES_MATCHING PATTERN "*.py")

    if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/iode_cython.cpp")
      file(REMOVE "${CMAKE_CURRENT_BINARY_DIR}/iode_cython.cpp")
    endif()

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
    #        - https://github.com/cython/cython/pull/4548 
    #          In Python, when running 'import xxx', Python will look for a function named PyInit_xxx
    #          in the xxx.pyd file. By default, Cython will use the name of the compiled .pyx file to generate 
    #          the PyInit_xxx function. This behavior is override by the --module-name option. 
    add_custom_command(OUTPUT iode_cython.cpp
                      DEPENDS iode_cython.pyx super.h super.cpp cli.cpp 
                      VERBATIM
                      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/super.h ${CMAKE_CURRENT_BINARY_DIR}/super.h
                      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/super.cpp ${CMAKE_CURRENT_BINARY_DIR}/super.cpp
                      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/cli.cpp ${CMAKE_CURRENT_BINARY_DIR}/cli.cpp
                      COMMAND cython ${CMAKE_CURRENT_SOURCE_DIR}/iode_cython.pyx -3 --verbose --cplus 
                      --line-directives --include-dir ${CMAKE_CURRENT_SOURCE_DIR} --module-name iode.iode_cython
                      --directive cpp_locals=True,binding=True,embedsignature=True,embedsignature.format=python
                      --output-file ${CMAKE_CURRENT_BINARY_DIR}/iode_cython.cpp)

    python_add_library(iode_cython MODULE ${CMAKE_CURRENT_BINARY_DIR}/iode_cython.cpp WITH_SOABI)

    target_link_libraries(iode_cython PRIVATE iode_cpp_api Python::NumPy)
    target_include_directories(iode_cython PRIVATE ${CMAKE_SOURCE_DIR}/scr4 ${CMAKE_SOURCE_DIR}/api ${CMAKE_SOURCE_DIR}/cpp_api)
    target_compile_definitions(iode_cython PUBLIC NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION)

    # Add a custom command that runs the Python script to generate stub files 
    # after the iode_cython target is built
    add_custom_command(TARGET iode_cython POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:iode_cython> ${CMAKE_CURRENT_BINARY_DIR}/iode
        COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/generate_stub_files.py)
    
    install(TARGETS iode_cython LIBRARY DESTINATION iode)
  else()
    message(CHECK_FAIL "Cython not found")
  endif()

  if(TARGET iode_cython)
    message(STATUS "\nNew target iode_cython enabled\n")
  else()
    message(WARNING "Could not create the target iode_cython\n")
  endif()
else()
  message(WARNING "Python not found")
endif()
