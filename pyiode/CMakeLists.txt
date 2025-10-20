# Find the required packages

message(STATUS "\n---- CYTHON ----\n")

file(INSTALL ${CMAKE_SOURCE_DIR}/README.md DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})
file(INSTALL ${CMAKE_SOURCE_DIR}/LICENSE.md DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})

# configure_file() substitutes variable values referenced as @VAR@ or ${VAR} in the input file content. 
# Each variable reference will be replaced with the current value of the variable, or the empty string 
# if the variable is not defined.
# see https://cmake.org/cmake/help/latest/command/configure_file.html#command:configure_file 
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml.in ${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/condarecipe/iode/meta.yaml.in ${CMAKE_CURRENT_SOURCE_DIR}/condarecipe/iode/meta.yaml)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/iode/__init__.py.in ${CMAKE_CURRENT_SOURCE_DIR}/iode/__init__.py)
configure_file(${CMAKE_SOURCE_DIR}/generate_stub_files.py.in ${CMAKE_SOURCE_DIR}/generate_stub_files.py)

if(Python_FOUND)
  message(CHECK_START "Search for Cython")
  find_program(CYTHON "cython")
  if(CYTHON)
    message(CHECK_PASS "found")
    message(STATUS "\nSet up target for iode Python package\n")
    cmake_print_variables(CYTHON)

    message(STATUS "\n   Copy documentation files to iode/doc\n")
    file(INSTALL ${CMAKE_SOURCE_DIR}/doc/build/iode.chm DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/iode/doc)
    file(INSTALL ${CMAKE_SOURCE_DIR}/doc/build/iode.pdf DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/iode/doc)

    set(DATA_DST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/iode/tests")
    message(STATUS "\n   Copy data files for testing to ${DATA_DST_DIR}\n")

    set(SUPPORTED_EXTENSIONS ac ae ai al as at av cmt eqs idt lst scl tbl var rep txt a2m)
    message(STATUS "Supported extensions: ${SUPPORTED_EXTENSIONS}")
    
    set(TESTS_DATA_SUBDIRS "data" "data/reports")
    foreach(subdir ${TESTS_DATA_SUBDIRS})
      # Get all files in the source directory
      set(DATA_SRC_DIR "${CMAKE_SOURCE_DIR}/tests/${subdir}")
      file(GLOB DATA_FILES LIST_DIRECTORIES false "${DATA_SRC_DIR}/*")
      message(STATUS "Get list of files from the source directory ${DATA_SRC_DIR}")

      foreach(f ${DATA_FILES})
        # only copy files with extensions "ac ae ai al as at av cmt eqs idt lst "
        # "scl tbl var rep txt a2m" and size less than 2 MB
        get_filename_component(ext "${f}" EXT)
        string(TOLOWER "${ext}" ext_lower)
        # --- remove leading dot from extension
        string(REGEX REPLACE "^\\." "" ext_lower "${ext_lower}")
        if(ext_lower IN_LIST SUPPORTED_EXTENSIONS)
          file(SIZE "${f}" fsize)
          if(fsize LESS 2097152) # 2 MB in bytes
            # get_filename_component(fname "${f}" NAME)
            # message(STATUS "Copy file '${fname}' to the destination directory ${DATA_DST_DIR}")
            file(INSTALL DESTINATION "${DATA_DST_DIR}/${subdir}" TYPE FILE FILES "${f}")
          else()
            message(STATUS "Skip file ${f} (size ${fsize} bytes exceeds 2 MB)")
          endif()
        else()
          message(STATUS "Skip file ${f} (unsupported extension .${ext_lower})")
        endif()
      endforeach()
    
    endforeach()

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
                      DEPENDS iode_cython.pyx super.h super.cpp cli.cpp a2m_print.cpp iode_database/variables_database.cpp
                      VERBATIM
                      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/super.h ${CMAKE_CURRENT_BINARY_DIR}/super.h
                      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/super.cpp ${CMAKE_CURRENT_BINARY_DIR}/super.cpp
                      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/cli.cpp ${CMAKE_CURRENT_BINARY_DIR}/cli.cpp
                      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/a2m_print.cpp ${CMAKE_CURRENT_BINARY_DIR}/a2m_print.cpp
                      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/iode_database/variables_database.cpp ${CMAKE_CURRENT_BINARY_DIR}/iode_database/variables_database.cpp
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
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:iode_cython> ${CMAKE_CURRENT_SOURCE_DIR}/iode/iode_cython.pyd
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
