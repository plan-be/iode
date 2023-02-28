# SETUPIODE.PY
# ------------
# Called by makepy.bat to create iode.pyd for the running python version 

# Import needed modules
try:
    from setuptools import setup
    from setuptools import Extension
except ImportError:
    from distutils.core import setup
    from distutils.extension import Extension

import numpy as np
import sys
import os

# Read the env var ISVS to select the right directory and lib names
ISVS = os.getenv("ISVS")
if ISVS == '-vs':
    # Visual Studio dir structure
    build_dir = '../out/build/windows-x64-debug'
    iode_incl_dir = '../api'
    scr4_incl_dir = '../scr4'
    iode_lib_dir = f'{build_dir}/api'
    scr4_lib_dir = f'{build_dir}/scr4'
    scr4iode = 'scr4iode'
else:
    # Command line directory structure
    iode_dir = '../api'
    scr4_dir = '../../scr4_src'
    iode_incl_dir = f'{iode_dir}'
    scr4_incl_dir = f'{scr4_dir}'
    iode_lib_dir = f'{iode_dir}/vc64'
    scr4_lib_dir = f'{scr4_dir}/vc64'
    scr4iode = 's4iode'

# Cythonize iode.pyx, compile generated file (iode.c) and link with appropriated libs
iodemodule = Extension('iode',
    include_dirs = [np.get_include(), iode_incl_dir, scr4_incl_dir],
    library_dirs = [iode_lib_dir, scr4_lib_dir],
    libraries = ['iodeapi', scr4iode, 'odbc32', 'odbccp32', 'ws2_32', 'gdi32', 'user32', 'advapi32', 'shell32', 'comdlg32', 'comctl32', 'Winspool', 'legacy_stdio_definitions'],
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

# Create the .pyd file (?)
setup(
    name = 'iode',
    ext_modules = [iodemodule]
)
