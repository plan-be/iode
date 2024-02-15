"""
Cython-compatible wrapper for doctest.testmod().
Assume the Cython module iode.pyd is compiled.
"""

import iode
import pytest
import doctest
from pathlib import Path

# GLOBALS
IODE_DATA_DIR = Path("../data").absolute()
IODE_OUTPUT_DIR = Path("../output").absolute()
IODE_VERBOSE = 1
 

def test_cython_iode():
    # to not run doctests twice
    iode.__test__ = {}

    # run doctests
    failure_count, test_count = doctest.testmod(iode, name="iode", globs={"iode": iode})
    assert failure_count == 0
