# Find the required packages

message(STATUS "\n---- CYTHON ----\n")

if(MSVC)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(IODE_CYTHON_FLAGS "\"${IODE_DEBUG_INFO_FLAG}\", \"/Od\"")
  else()
    set(IODE_CYTHON_FLAGS "\"/O2\"")
  endif()
  set(IODE_CYTHON_FLAGS "${IODE_CYTHON_FLAGS}, \"/DVC\", \"/DSCRPROTO\", \"/DREALD\"")
else()
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(IODE_CYTHON_FLAGS "\"${IODE_DEBUG_INFO_FLAG}\", \"-Og\"")
  else()
  set(IODE_CYTHON_FLAGS "\"-Ofast\"")
  endif()
endif()
set(IODE_CYTHON_FLAGS "[${IODE_CYTHON_FLAGS}]")

message(STATUS "Compile options for Cython: ${IODE_CYTHON_FLAGS}\n")

message(STATUS "Generating the Python script ${CMAKE_CURRENT_SOURCE_DIR}/cythonize_iode.py")
message(STATUS "from the template file ${CMAKE_CURRENT_SOURCE_DIR}/cythonize_iode.py.in")

# ---- GENERATE FILE FROM TEMPLATE ----
# configure_file() substitutes variable values referenced as @VAR@ or ${VAR} in the input file content. 
# Each variable reference will be replaced with the current value of the variable, or the empty string 
# if the variable is not defined.
# see https://cmake.org/cmake/help/latest/command/configure_file.html#command:configure_file 
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cythonize_iode.py.in
  ${CMAKE_CURRENT_SOURCE_DIR}/cythonize_iode.py
)

message(STATUS "DONE\n")

# NOTES: I tried to activate and deactivate conda environments through the present CMakeLists.txt 
#        but failed miserably. After some research on Google, it appears I was doing the wrong way.
#        quotes:
#        1. "Generally, this CMake script does not seem like a good approach. 
#        Tight-coupling the compilation of code to the existence of a particularly-named Conda 
#        environment at the user level seems to go against the spirit of CMake, which aims to automate 
#        the discovery of software dependencies so that users don't need commonly hardcoded locations.
#        Perhaps it might be worth reassessing what is trying to be accomplished and the strategy to 
#        get there."
#        See https://stackoverflow.com/a/72479869
#        2. "conda activate won’t work in CMake because it modifies the caller’s environment through 
#        either being sourced (if conda is a shell function) or by launching a new command 
#        (which also can’t affect CMake).
#        Environments like that need to be set up outside of CMake whether it be HomeBrew, Nix, 
#        vcpkg, spack, or conda.
#        For example, on Conda Forge, lots of packages compile with CMake, but there CMake is executed 
#        in the context of an already activated environment and itself knows nothing about Conda. 
#        This makes it so the CMake code is completely agnostic to how its dependencies are provided, 
#        which I regard as cleaner engineering."
#        See https://discourse.cmake.org/t/create-and-activate-python-conda-environment-with-cmake/5777/2 


if(Python_FOUND)
  if(MSVC)
    set(COMPILER msvc)

    set(CYTHON_OUTPUT "iode.cp${Python_VERSION_}-win_amd64.pyd")

    add_custom_target(iode_python
                      COMMAND ${Python_EXECUTABLE} cythonize_iode.py build_ext --inplace --compiler=${COMPILER}
                      DEPENDS cythonize_iode.py
                      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                      COMMENT "Building iode.pyd for Python version ${Python_VERSION}"
                      )
    add_dependencies(iode_python iode_c_api)

    add_custom_command(TARGET iode_python POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy ${CYTHON_OUTPUT} ${CMAKE_CURRENT_BINARY_DIR}/py${Python_VERSION_}/iode.pyd 
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        COMMENT "Moving iode.pyd file to ${CMAKE_CURRENT_BINARY_DIR}/py${Python_VERSION_} directory"
                        )
  endif()

  if(TARGET iode_python)
    message(STATUS "New target iode_python enabled\n")
  else()
    message(WARNING "Could not create the target iode_python\n")
  endif()
else()
  message(WARNING "Python not found")
endif()
