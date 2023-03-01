
cmake_minimum_required (VERSION 3.16)

# Add source to this project's executable.
set (PYX_SOURCES
	iode.pyx iode.pxi 
	pyiode_cdef.pyx pyiode_util.pyx pyiode_lec.pyx pyiode_sample.pyx pyiode_ws.pyx pyiode_data.pyx pyiode_estim.pyx
	pyiode_model.pyx pyiode_exec.pyx pyiode_print.pyx pyiode_reports.pyx pyiode_objs.pyx pyiode_larray.pyx
)
set (C_SOURCES s_iode.c)
set (SCRIPTS cythonize_iode.py makepy.bat)
set (TESTS test_iode.py) 
set (DOCU PYIODE.md)

# add_executable(pyiode ${PYX_SOURCES} ${C_SOURCES} ${SCRIPTS} ${DOCU} ${TESTS})
# target_include_directories(pyiode PUBLIC "../scr4" "../api")
# target_link_libraries(pyiode iodeapi scr4iode)


add_custom_target(pyiode SOURCES ${PYX_SOURCES} ${C_SOURCES} ${SCRIPTS} ${DOCU} ${TESTS})