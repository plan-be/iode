
add_executable(sanbox_Z1 main.cpp)
add_executable(sanbox_exe main.cpp)

add_executable(testalign_Z1 testalign.c)
add_executable(testalign testalign.c)

# Alignement flag
if(MSVC)
  set(IODE_ALIGNEMENT_FLAG "-Zp1")
else()
  set(IODE_ALIGNEMENT_FLAG "-fpack-struct=1")
endif()

# NOTE: use PRIVATE to NOT propagate compile options to other lib/executable depending on iode_c_api.
#       Option ${IODE_ALIGNEMENT_FLAG} makes compilation of the Google Test lib to fail
target_compile_options(sanbox_Z1 PRIVATE ${IODE_ALIGNEMENT_FLAG})
target_compile_definitions(sanbox_Z1 PRIVATE ALIGN1)

target_compile_options(testalign_Z1 PRIVATE ${IODE_ALIGNEMENT_FLAG})

# target_link_libraries(sandbox_Z1 PUBLIC iode_c_api)
# target_link_libraries(sandbox_exe PUBLIC iode_c_api)

target_include_directories(sanbox_Z1 PRIVATE "${CMAKE_SOURCE_DIR}\\scr4" ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(sanbox_exe PRIVATE "${CMAKE_SOURCE_DIR}\\scr4" ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(testalign_Z1 PRIVATE "${CMAKE_SOURCE_DIR}\\scr4" "${CMAKE_SOURCE_DIR}\\api")
target_include_directories(testalign PRIVATE "${CMAKE_SOURCE_DIR}\\scr4" "${CMAKE_SOURCE_DIR}\\api")

file(INSTALL ${PROJECT_SOURCE_DIR}/iode.msg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
