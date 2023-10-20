
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
        
        nanobind_add_module(iode py_sample.h py_ws.h py_numpy.h pyiode.cpp)

        target_link_libraries(iode PRIVATE iode_cpp_api)

        message(STATUS "New target iode enabled\n")
    else()
        message(CHECK_FAIL "not found")
        message(WARNING "Could not create the target iode\n")
    endif()
else()
    message(WARNING "Python not found")
endif()
