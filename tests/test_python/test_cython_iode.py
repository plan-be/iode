"""
Cython-compatible wrapper for doctest.testmod().
See https://cython.readthedocs.io/en/latest/src/userguide/faq.html#how-do-i-run-doctests-in-cython-code-pyx-files 
"""

import iode
import doctest
import inspect
import logging


def test_cython_iode():
    iode.__test__ = {}
    for name, value in inspect.getmembers(iode):
        if "Database" in type(value).__name__:
            value = type(value)
        if name != "Path" and inspect.isclass(value):
            for name_, value_ in inspect.getmembers(value):
                doc = inspect.getdoc(value_)
                if doc is not None:
                    iode.__test__[f'{name}.{name_}'] = doc                 
        doc = inspect.getdoc(value)
        if doc is not None:
            iode.__test__[name] = doc

    from iode.iode_python import TableLine
    for name_, value_ in inspect.getmembers(TableLine):
        doc = inspect.getdoc(value_)
        if doc is not None:
            iode.__test__[f'TableLine.{name_}'] = doc 

    from iode.iode_python import TableCell
    for name_, value_ in inspect.getmembers(TableCell):
        doc = inspect.getdoc(value_)
        if doc is not None:
            iode.__test__[f'TableCell.{name_}'] = doc 

    from iode.iode_python import ComputedTable
    for name_, value_ in inspect.getmembers(ComputedTable):
        doc = inspect.getdoc(value_)
        if doc is not None:
            iode.__test__[f'ComputedTable.{name_}'] = doc 

    from iode.iode_python import EditAndEstimateEquations
    for name_, value_ in inspect.getmembers(EditAndEstimateEquations):
        doc = inspect.getdoc(value_)
        if doc is not None:
            iode.__test__[f'EditAndEstimateEquations.{name_}'] = doc 

    from iode.util import table2str
    iode.__test__['table2str'] = table2str.__doc__

    from iode.iode_python import _iode_msg_path
    iode.__test__['_iode_msg_path'] = _iode_msg_path.__doc__

    iode.suppress_msgs()

    # run doctests
    failure_count, test_count = doctest.testmod(iode, globs={"iode": iode})
    assert failure_count == 0
    assert test_count > 0
    for name in iode.__test__:
        logging.info(f"tested function/method: '{name}'")
