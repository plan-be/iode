# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.16)

# List of CMAKE variables: https://cmake.org/cmake/help/latest/manual/cmake-variables.7.html

# ---- user defined variables ----
set(IS_DEBUG_BUILD (CMAKE_BUILD_TYPE STREQUAL  "Debug"))
set(IS_GNU (CMAKE_CXX_COMPILER_ID MATCHES GNU))
set(IS_CLANG (CMAKE_CXX_COMPILER_ID MATCHES Clang))

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
project ("iode" VERSION 7.01)

message(STATUS "======== global info ========")
message(STATUS "IODE version: ${PROJECT_VERSION}")
message(STATUS "OS:           ${CMAKE_SYSTEM_NAME}")
message(STATUS "Architecture: ${CMAKE_C_COMPILER_ARCHITECTURE_ID}")
message(STATUS "Build Config: ${CMAKE_BUILD_TYPE}")
if(IODE_ENABLE_SANITIZER)
  message(STATUS "Sanitize:     Yes")
else()
  message(STATUS "Sanitize:     No")
endif()
message(STATUS "C compiler:   ${CMAKE_C_COMPILER_ID}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER_ID}")
if(MSVC)
  message(STATUS "MSVC version: ${MSVC_VERSION}")
endif()
message(STATUS "Generator:    ${CMAKE_GENERATOR}")

# ---- sanitizers ----
# add sanitizer flags if compiled and linked with DEBUG configuration 
# * AddressSanitizer (aka ASan) is a memory error detector for C/C++. 
#   It finds:
#      o Use after free (dangling pointer dereference)
#      o Heap buffer overflow
#      o Stack buffer overflow
#      o Global buffer overflow
#      o Use after return
#      o Use after scope
#      o Initialization order bugs
#      o Memory leaks
#   In order to use AddressSanitizer you will need to compile and link your program the -fsanitize=address flag. 
#   See https://github.com/google/sanitizers/wiki/AddressSanitizer for more details
# Note: sanitizer options are availalbe for cl, clang and gcc
if(IS_DEBUG_BUILD AND IODE_ENABLE_SANITIZER)
if(MSVC)
  # Warning: many options are currently (Visual Studio 2019) NOT avaible for MSVC
  #set(IODE_SANITIZER_FLAGS "-fsanitize=address")
  set(IODE_SANITIZER_FLAGS "-fsanitize=address")
else()
  # Only for GCC and Clang:
  # o To get a reasonable performance add -O1 or higher. 
  # o To get nicer stack traces in error messages add -fno-omit-frame-pointer
  # o To ignore functions, use -fsanitize-blacklist=<filename>.txt.
  #   See https://github.com/google/sanitizers/wiki/AddressSanitizer#turning-off-instrumentation
  # o To enable continue-after-error, compile with -fsanitize-recover=address and 
  #   then run your code with ASAN_OPTIONS=halt_on_error=0
  set(IODE_SANITIZER_FLAGS "-fsanitize-recover=address -O1 -fno-omit-frame-pointer")
endif()

message(STATUS "========= sanitizer =========")
message(STATUS "Enable Sanitizer Flags: ${IODE_SANITIZER_FLAGS}")

set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${IODE_SANITIZER_FLAGS}")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${IODE_SANITIZER_FLAGS}")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} ${IODE_SANITIZER_FLAGS}")

# Note: incompatible options when using Visual Studio:
#         o The /RTC options are incompatible with AddressSanitizer and should be disabled.
#         See https://docs.microsoft.com/en-us/cpp/sanitizers/asan-known-issues?view=msvc-170#incompatible-options
if(MSVC)
  string(REPLACE "/RTC1" "" CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
  string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
endif()

else()
  set(IODE_SANITIZER_FLAGS "")
endif()

message(STATUS "=============================")

# ---- compile options ----
add_compile_options(-D_CRT_SECURE_NO_WARNINGS)

# /Zi	Generates complete debugging information
if (CMAKE_BUILD_TYPE STREQUAL  "Debug")
   add_compile_options(-Zi)
endif()

# some code in the C++ API requires C++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- preprocessor ----
add_compile_definitions(WINDOWS_IGNORE_PACKING_MISMATCH DOS VC WINDOWS DOSW32 REALD SCRPROTO IODEWIN NOEMS NOEMF)

# ---- include ----
# CMAKE_SOURCE_DIR is the full path to the top level 
# of the current CMake source tree
include_directories(${CMAKE_SOURCE_DIR})
message(STATUS "Full path to the top level directory: ${CMAKE_SOURCE_DIR}")

# -- Boost --
list(APPEND CMAKE_PREFIX_PATH "C:\\boost")
set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS        OFF)  # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost 1.74)

message(CHECK_START "Search for Boost package")
if(Boost_FOUND)
  message(CHECK_PASS "version ${Boost_VERSION}")
  include_directories(${Boost_INCLUDE_DIRS})
else()
  message(CHECK_FAIL "not found")
endif()

# ---- libraries ----
add_subdirectory ("scr4")
add_subdirectory ("api")
add_subdirectory ("cpp_api")

# ---- copy files for tests and gui ----
file(INSTALL tests/data DESTINATION ${CMAKE_BINARY_DIR}/tests)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tests/output)

# ---- executables ----
add_subdirectory ("gui")
add_subdirectory ("tests")
add_subdirectory ("pyiode")

# ---- user doc ----
add_subdirectory ("doc")

# ---- copy files for user doc ----
file(INSTALL doc/build/iode.chm DESTINATION ${CMAKE_BINARY_DIR}/doc)
file(INSTALL doc/build/iode.pdf DESTINATION ${CMAKE_BINARY_DIR}/doc)
file(INSTALL doc/build/readme.htm DESTINATION ${CMAKE_BINARY_DIR}/doc)
file(INSTALL doc/build/keyboard_shortcuts.pdf DESTINATION ${CMAKE_BINARY_DIR}/doc)

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
