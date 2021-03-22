try:
    from setuptools import setup
    from setuptools import Extension
except ImportError:
    from distutils.core import setup
    from distutils.extension import Extension

import numpy as np
from pathlib import Path


compiler = 'vc64'

root_dir = Path(__file__).resolve().parents[2]
api_dir = str(root_dir / 'api')
scr4_dir = str(root_dir / 'scr4')

iodemodule = Extension('iode',
    include_dirs = [np.get_include(), api_dir, scr4_dir],
    library_dirs = [f'{api_dir}/{compiler}', f'{scr4_dir}/{compiler}'],
    libraries = ['iodeapi', 's4iode', 'odbc32', 'odbccp32', 'ws2_32', 'gdi32', 'user32', 'advapi32', 'shell32', 'comdlg32', 'comctl32', 'Winspool', 'legacy_stdio_definitions'],
	extra_compile_args=["-Zi", "/Od", "/DVC", "/DSCRPROTO", "/DREALD"],
	extra_link_args=["-debug"],					
    sources = ["../iode.pyx", "../sb_gui.c", "../s_iode.c", "../b_api.c"]
    #sources = ["iode.pyx"]
    )
                    
                 

setup(
    name = 'iode',
    ext_modules = [iodemodule]
)