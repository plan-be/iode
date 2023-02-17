try:
    from setuptools import setup
    from setuptools import Extension
except ImportError:
    from distutils.core import setup
    from distutils.extension import Extension

import numpy as np

build_dir = '../out/build/windows-x64-debug'
iodeapi_dir = f'{build_dir}/api'
scr4iode_dir = f'{build_dir}/scr4'

# VERSION PRE Visual Studio
iodemodule = Extension('iode',
    include_dirs = [np.get_include(), '../api', '../scr4'],
    library_dirs = [iodeapi_dir, scr4iode_dir],
    libraries = ['iodeapi', 'scr4iode', 'odbc32', 'odbccp32', 'ws2_32', 'gdi32', 'user32', 'advapi32', 'shell32', 'comdlg32', 'comctl32', 'Winspool', 'legacy_stdio_definitions'],
	extra_compile_args=["-Zi", "/Od", "/DVC", "/DSCRPROTO", "/DREALD"],
	extra_link_args=["-debug"],					
    sources = ["iode.pyx", "s_iode.c"],
    depends = [ "iode.pxy", "s_iode.c", "iode.pxi", 
                "pyiode_cdef.pyx", "pyiode_data.pyx", "pyiode_estim.pyx", 
                "pyiode_exec.pyx",  "pyiode_larray.pyx", "pyiode_lec.pyx",
                "pyiode_model.pyx", "pyiode_objs.pyx", "pyiode_print.pyx", 
                "pyiode_reports.pyx", "pyiode_sample.pyx", "pyiode_util.pyx",  
                "pyiode_ws.pyx"]
)

# VERSION Visual Studio
##iodemodule = Extension('iode',
##    include_dirs = [np.get_include(), 'C:/Users/jmp/source/repos/iode/api', 'C:/Users/jmp/source/repos/iode/scr4'],
##    library_dirs = ['C:/Users/jmp/source/repos/iode/out/build/windows-x64-debug/api', 'C:/Users/jmp/source/repos/iode/out/build/windows-x64-debug/scr4'],
##    libraries = ['iodeapi', 'scr4iode', 'odbc32', 'odbccp32', 'ws2_32', 'gdi32', 'user32', 'advapi32', 'shell32', 'comdlg32', 'comctl32', 'Winspool', 'legacy_stdio_definitions'],
##	extra_compile_args=["-Zi", "/Od", "/DVC", "/DSCRPROTO", "/DREALD"],
##	extra_link_args=["-debug"],					
##    sources = ["..\\iode.pyx", "..\\sb_gui.c", "..\\s_iode.c"]
##    #sources = ["..\\iode.pyx", "..\\sb_gui.c", "..\\s_iode.c", "..\\b_api.c"]
##    #sources = ["iode.pyx"]
##    )
                 

setup(
    name = 'iode',
    ext_modules = [iodemodule]
)