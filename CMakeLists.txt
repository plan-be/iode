# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.16)

# ---- dependencies ----

# -- Google Test --
# lines below declares a dependency on GoogleTest which is downloaded from GitHub and installed
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ---- main project ----
project ("iode")

# some code in the C++ API requires C++ 17
set(CMAKE_CXX_STANDARD 17)

# ---- compile options ----
# /Zi	Generates complete debugging information
add_compile_options(-Zi)

# ---- preprocessor ----
add_compile_definitions(WINDOWS_IGNORE_PACKING_MISMATCH DOS VC WINDOWS DOSW32 REALD SCRPROTO IODEWIN NOEMS NOEMF)

# ---- libraries ----
add_subdirectory ("scr4")
add_subdirectory ("api")
add_subdirectory ("cpp_api")

# ---- executables ----
file(INSTALL tests/data DESTINATION ${CMAKE_BINARY_DIR}/tests)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tests/output)

add_subdirectory ("tests")

# ---- print info ----
message(STATUS "======== target scr4iode ========")
get_target_property(_COMPILE_OPTIONS scr4iode COMPILE_OPTIONS)
get_target_property(_COMPILE_DEFINITIONS scr4iode COMPILE_DEFINITIONS)
get_target_property(_INCLUDE_DIRECTORIES scr4iode INCLUDE_DIRECTORIES)
get_target_property(_DEPENDENCIES scr4iode LINK_LIBRARIES)
message(STATUS "Compile options:       ${_COMPILE_OPTIONS}")
message(STATUS "Compile definitions:   ${_COMPILE_DEFINITIONS}")
message(STATUS "Include directories:   ${_INCLUDE_DIRECTORIES}")
message(STATUS "Dependencies:          ${_DEPENDENCIES}")
message(STATUS "======== target iodeapi ========")
get_target_property(_COMPILE_OPTIONS iodeapi COMPILE_OPTIONS)
get_target_property(_COMPILE_DEFINITIONS iodeapi COMPILE_DEFINITIONS)
get_target_property(_INCLUDE_DIRECTORIES iodeapi INCLUDE_DIRECTORIES)
get_target_property(_DEPENDENCIES iodeapi LINK_LIBRARIES)
message(STATUS "Compile options:       ${_COMPILE_OPTIONS}")
message(STATUS "Compile definitions:   ${_COMPILE_DEFINITIONS}")
message(STATUS "Include directories:   ${_INCLUDE_DIRECTORIES}")
message(STATUS "Dependencies:          ${_DEPENDENCIES}")
message(STATUS "======== target iodecppapi ========")
get_target_property(_COMPILE_OPTIONS iodecppapi COMPILE_OPTIONS)
get_target_property(_COMPILE_DEFINITIONS iodecppapi COMPILE_DEFINITIONS)
get_target_property(_INCLUDE_DIRECTORIES iodecppapi INCLUDE_DIRECTORIES)
get_target_property(_DEPENDENCIES iodecppapi LINK_LIBRARIES)
message(STATUS "Compile options:       ${_COMPILE_OPTIONS}")
message(STATUS "Compile definitions:   ${_COMPILE_DEFINITIONS}")
message(STATUS "Include directories:   ${_INCLUDE_DIRECTORIES}")
message(STATUS "Dependencies:          ${_DEPENDENCIES}")
message(STATUS "===================================")
