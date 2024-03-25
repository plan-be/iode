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
        module = inspect.getmodule(value)
        module = module.__name__ if module is not None else ''
        if 'iode' in module:
            if inspect.isclass(value):
                for name_, value_ in inspect.getmembers(value):
                    doc = inspect.getdoc(value_)
                    if doc is not None:
                        print(f"add test for method '{name}.{name_}'")
                        iode.__test__[f'{name}.{name_}'] = doc                 
            doc = inspect.getdoc(value)
            if doc is not None:
                print(f"add test for function/method '{name}'")
                iode.__test__[name] = doc

    # run doctests
    failure_count, test_count = doctest.testmod(iode, globs={"iode": iode})
    assert failure_count == 0
    assert test_count > 0
