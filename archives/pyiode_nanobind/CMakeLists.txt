
message(STATUS "\n---- IODE PYTHON ----\n")

if(Python_FOUND)
    # The lines below configure CMake to perform an optimized release build by default unless another 
    # build type is specified. Without this addition, binding code may run slowly and produce large binaries.
    # See https://nanobind.readthedocs.io/en/latest/building.html 
    if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
    endif()

    # Detect the installed nanobind package and import it into CMake
    execute_process(
        COMMAND "${Python_EXECUTABLE}" -m nanobind --cmake_dir
        OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE NB_DIR)
    list(APPEND CMAKE_PREFIX_PATH "${NB_DIR}")
    message(STATUS "CMake directory for nanobind: ${NB_DIR} \n")

    message(CHECK_START "Search for python package nanobind")
    find_package(nanobind CONFIG)
    if(nanobind_FOUND)
        message(CHECK_PASS "found")

        file(INSTALL ${CMAKE_SOURCE_DIR}/tests/data/fun.cmt DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/iode/tests/data)
        file(INSTALL ${CMAKE_SOURCE_DIR}/tests/data/fun.eqs DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/iode/tests/data)
        file(INSTALL ${CMAKE_SOURCE_DIR}/tests/data/fun.idt DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/iode/tests/data)
        file(INSTALL ${CMAKE_SOURCE_DIR}/tests/data/fun.lst DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/iode/tests/data)
        file(INSTALL ${CMAKE_SOURCE_DIR}/tests/data/fun.scl DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/iode/tests/data)
        file(INSTALL ${CMAKE_SOURCE_DIR}/tests/data/fun.tbl DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/iode/tests/data)
        file(INSTALL ${CMAKE_SOURCE_DIR}/tests/data/fun.var DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/iode/tests/data)
        
        # set __version__ for the iode package
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/iode/_version.py.in ${CMAKE_CURRENT_SOURCE_DIR}/iode/_version.py)

        nanobind_add_module(iode_python_api 
        
                            # Target the stable ABI for Python 3.12+, which reduces
                            # the number of binary wheels that must be built. This
                            # does nothing on older Python versions
                            STABLE_ABI

                            # Build libnanobind statically and merge it into the
                            # extension (which itself remains a shared library)
                            #
                            # If your project builds multiple extensions, you can
                            # replace this flag by NB_SHARED to conserve space by
                            # reusing a shared libnanobind across libraries
                            NB_STATIC
        
                            py_numpy.h 
                            py_constants.h     py_constants.cpp 
                            py_sample.h        py_sample.cpp 
                            py_ws.h            py_ws.cpp
                            py_objs.h          py_objs.cpp 
                                               pyiode.cpp)

        target_link_libraries(iode_python_api PRIVATE iode_cpp_api)

        # Install directive for scikit-build-core
        install(TARGETS iode_python_api LIBRARY DESTINATION iode)

        message(STATUS "New target iode_python_api enabled\n")
    else()
        message(CHECK_FAIL "not found")
        message(WARNING "Could not create the target iode_python_api\n")
    endif()
else()
    message(WARNING "Python not found")
endif()
