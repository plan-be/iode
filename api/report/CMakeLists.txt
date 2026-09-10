
add_subdirectory(commands)
add_subdirectory(engine)
add_subdirectory(undoc)

target_sources(iode_c_api PRIVATE report.cpp rep_list.cpp)
