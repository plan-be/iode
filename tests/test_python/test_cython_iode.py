"""
Cython-compatible wrapper for doctest.testmod().
See https://cython.readthedocs.io/en/latest/src/userguide/faq.html#how-do-i-run-doctests-in-cython-code-pyx-files 
"""

import iode
import doctest
import inspect
from pathlib import Path

# GLOBALS
IODE_DATA_DIR = Path("../data").absolute()
IODE_OUTPUT_DIR = Path("../output").absolute()
IODE_VERBOSE = 1


def test_cython_iode():
    iode.__test__ = {}
    for name, value in inspect.getmembers(iode):
       doc = inspect.getdoc(value)
       module = inspect.getmodule(value)
       module = module.__name__ if module is not None else 'None'
       if 'iode' in module and doc is not None:
           print(f"add test for function/method '{name}'")
           iode.__test__[name] = value.__doc__

    # run doctests
    failure_count, test_count = doctest.testmod(iode, globs={"iode": iode})
    assert failure_count == 0
    assert test_count > 0
