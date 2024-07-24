add_executable(sandbox_exe main.c)
add_executable(sandbox_cpp_exe main.cpp)

# NOTE: use PRIVATE to NOT propagate compile options to other lib/executable depending on iode_c_api.
#       Option ${IODE_ALIGNEMENT_FLAG} makes compilation of the Google Test lib to fail
target_link_libraries(sandbox_exe PUBLIC iode_c_api)
target_link_libraries(sandbox_cpp_exe PUBLIC iode_cpp_api)

target_include_directories(sandbox_exe PRIVATE "${CMAKE_SOURCE_DIR}\\scr4" ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(sandbox_cpp_exe PRIVATE "${CMAKE_SOURCE_DIR}\\scr4" ${CMAKE_CURRENT_SOURCE_DIR})

file(INSTALL ${PROJECT_SOURCE_DIR}/iode.msg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
