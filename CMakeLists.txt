# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.24)

# List of CMAKE variables: https://cmake.org/cmake/help/latest/manual/cmake-variables.7.html

# ---- dependencies ----

# -- Google Test --
# lines below declares a dependency on GoogleTest which is downloaded from GitHub and installed
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ---- main project ----
project ("iode" VERSION 7.0 HOMEPAGE_URL "https://iode.plan.be/doku.php" LANGUAGES C CXX)

# -- global variables --
set(IODE_COMPANY "Federal Planning Bureau (Belgium)")
set(IODE_EMAIL "ald@plan.be; gb@plan.be; jmarc.paul@gmail.com")
set(IODE_BINARY_DIR ${CMAKE_BINARY_DIR})

# -- resources directory --
set(IODE_RESOUCES_DIR "${PROJECT_SOURCE_DIR}/resources")

# -- compiler options --
# SET(CMAKE_CXX_FLAGS_DEBUG "")
# SET(CMAKE_CXX_FLAGS_RELEASE "/O2 /Ob2 /Oi /Ot")

# -- print global info --
message(STATUS "======== global info ========")
message(STATUS "IODE version:       ${PROJECT_VERSION}")
message(STATUS "HOMEPAGE URL:       ${PROJECT_HOMEPAGE_URL}")
message(STATUS "OS:                 ${CMAKE_SYSTEM_NAME}")
message(STATUS "Architecture:       ${CMAKE_C_COMPILER_ARCHITECTURE_ID}")
message(STATUS "Build Config:       ${CMAKE_BUILD_TYPE}")
message(STATUS "C compiler:         ${CMAKE_C_COMPILER_ID}")
message(STATUS "C compiler flags:   ${CMAKE_C_FLAGS}")
message(STATUS "C++ compiler:       ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ compiler flags: ${CMAKE_CXX_FLAGS}")
if(MSVC)
  message(STATUS "MSVC version:       ${MSVC_VERSION}")
endif()
if(IODE_ENABLE_SANITIZER)
  message(STATUS "Sanitize:           Yes")
else()
  message(STATUS "Sanitize:           No")
endif()
message(STATUS "Generator:          ${CMAKE_GENERATOR}")

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
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND IODE_ENABLE_SANITIZER)
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
# Debug info flag
if(MSVC)
  add_compile_options(-D_CRT_SECURE_NO_WARNINGS)
endif()

# /Zi | g:	Generates complete debugging information
if(MSVC)
  set(IODE_DEBUG_INFO_FLAG "-Zi")
else()
  set(IODE_DEBUG_INFO_FLAG "-g")
endif()

# C++20 (required for the module std format) makes the compiler MSVC 
# much more strict by default when passing a string literal 
# "string" value for a char* argument of a C function.
# The option /Zc:strictStrings with the minus at the end deactivates 
# this behavior of the MSVC compiler
if(MSVC)
  add_compile_options("/Zc:strictStrings-")
endif()

# global flags
if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
  add_compile_options("-Wno-implicit-int")
  add_compile_options("-Wno-implicit-function-declaration")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(${IODE_DEBUG_INFO_FLAG})
endif()

# some code in the C++ API requires C++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- preprocessor ----
if(MSVC)
  add_compile_definitions(WINDOWS_IGNORE_PACKING_MISMATCH DOS VC WINDOWS DOSW32 REALD SCRPROTO IODEWIN NOEMS NOEMF)
else()
  add_compile_definitions(LINUX -DUNIX -DSCOU -DREALD -DSCRPROTO -DNOEMS -DNOEMF)
endif()
add_compile_definitions("DEBUG=$<CONFIG:Debug>")

# ---- include ----
# CMAKE_SOURCE_DIR is the full path to the top level 
# of the current CMake source tree
include_directories(${CMAKE_SOURCE_DIR})
message(STATUS "Full path to the top level directory: ${CMAKE_SOURCE_DIR}")
message(STATUS "=============================")

# ---- search for librairies and packages ----
message(STATUS "SEARCH FOR PACKAGES:\n")

# -- find Boost --
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
message(STATUS " ")

# -- find Qt --
message(CHECK_START "Search for Qt6 package")
if (CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    list(APPEND CMAKE_PREFIX_PATH "C:\\Qt\\6.4.0\\msvc2019_64;C:\\Qt\\6.4.2\\msvc2019_64")
endif()
find_package(Qt6 COMPONENTS Core Widgets Gui Charts PrintSupport)

if(Qt6_FOUND)
  message(CHECK_PASS "found")
else()
  message(CHECK_FAIL "not found")
endif()
message(STATUS " ")

# -- find Python --
message(CHECK_START "Search for Python")
find_package(Python 3.9 COMPONENTS Interpreter Development REQUIRED)
if(Python_FOUND)
  message(CHECK_PASS "found")
  message(STATUS "Python EXECUTABLE: ${Python_EXECUTABLE}")
  set(Python_VERSION_ "${Python_VERSION_MAJOR}${Python_VERSION_MINOR}")
else()
  message(CHECK_FAIL "not found")
endif()

message(STATUS "=============================")

# ---- copy files to output build directory ----
message(STATUS "COPY FILES TO ${CMAKE_BINARY_DIR} ...\n")

# -- copy files for tests and gui --
message(STATUS "\n-> test files <-")
file(INSTALL tests/data DESTINATION ${CMAKE_BINARY_DIR}/tests)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tests/output)
# -- copy files for user doc --
message(STATUS "\n-> user doc files <-")
file(INSTALL doc/build/iode.chm DESTINATION ${CMAKE_BINARY_DIR}/doc)
file(INSTALL doc/build/iode.pdf DESTINATION ${CMAKE_BINARY_DIR}/doc)
file(INSTALL doc/build/readme.htm DESTINATION ${CMAKE_BINARY_DIR}/doc)
file(INSTALL doc/build/keyboard_shortcuts.pdf DESTINATION ${CMAKE_BINARY_DIR}/doc)
# -- copy syntax files --
message(STATUS "\n-> files to highlight IODE report commands in Texpad and Notepad++ <-")
file(INSTALL resources/syntax/ioderep.syn DESTINATION ${CMAKE_BINARY_DIR}/syntax)
file(INSTALL resources/syntax/npp_syntax.xml DESTINATION ${CMAKE_BINARY_DIR}/syntax)
# -- copy misc files --
message(STATUS "\n-> error messages file <-")
file(INSTALL iode.msg DESTINATION ${CMAKE_BINARY_DIR})
message(STATUS "DONE\n")

message(STATUS "=============================")
message(STATUS "CREATE TARGETS:\n")

# ---- libraries ----
add_subdirectory ("scr4")
add_subdirectory ("api")
add_subdirectory ("cpp_api")

# ---- executables ----
set(IODE_CLI_TARGET iodecmd)
add_subdirectory ("cmd")
set(IODE_GUI_TARGET iode_gui)
add_subdirectory ("gui")
add_subdirectory ("pyiode")

# ---- tests ----
add_subdirectory ("tests")

# ---- user doc ----
add_subdirectory ("doc")

# -- nsis installer --
if(TARGET ${IODE_GUI_TARGET})
  add_subdirectory("nsis")
endif()

message(STATUS "=============================\n")

# ---- print info ----
message(STATUS "======== target iode_scr4 ========")
get_target_property(_COMPILE_OPTIONS iode_scr4 COMPILE_OPTIONS)
get_target_property(_COMPILE_DEFINITIONS iode_scr4 COMPILE_DEFINITIONS)
get_target_property(_INCLUDE_DIRECTORIES iode_scr4 INCLUDE_DIRECTORIES)
get_target_property(_DEPENDENCIES iode_scr4 LINK_LIBRARIES)
message(STATUS "Compile options:       ${_COMPILE_OPTIONS}")
message(STATUS "Compile definitions:   ${_COMPILE_DEFINITIONS}")
message(STATUS "Include directories:   ${_INCLUDE_DIRECTORIES}")
message(STATUS "Dependencies:          ${_DEPENDENCIES}")
message(STATUS "======== target iode_c_api ========")
get_target_property(_COMPILE_OPTIONS iode_c_api COMPILE_OPTIONS)
get_target_property(_COMPILE_DEFINITIONS iode_c_api COMPILE_DEFINITIONS)
get_target_property(_INCLUDE_DIRECTORIES iode_c_api INCLUDE_DIRECTORIES)
get_target_property(_DEPENDENCIES iode_c_api LINK_LIBRARIES)
message(STATUS "Compile options:       ${_COMPILE_OPTIONS}")
message(STATUS "Compile definitions:   ${_COMPILE_DEFINITIONS}")
message(STATUS "Include directories:   ${_INCLUDE_DIRECTORIES}")
message(STATUS "Dependencies:          ${_DEPENDENCIES}")
message(STATUS "======== target iode_cpp_api ========")
get_target_property(_COMPILE_OPTIONS iode_cpp_api COMPILE_OPTIONS)
get_target_property(_COMPILE_DEFINITIONS iode_cpp_api COMPILE_DEFINITIONS)
get_target_property(_INCLUDE_DIRECTORIES iode_cpp_api INCLUDE_DIRECTORIES)
get_target_property(_DEPENDENCIES iode_cpp_api LINK_LIBRARIES)
message(STATUS "Compile options:       ${_COMPILE_OPTIONS}")
message(STATUS "Compile definitions:   ${_COMPILE_DEFINITIONS}")
message(STATUS "Include directories:   ${_INCLUDE_DIRECTORIES}")
message(STATUS "Dependencies:          ${_DEPENDENCIES}")
message(STATUS "===================================")
