"""
Cython-compatible wrapper for doctest.testmod().
See https://cython.readthedocs.io/en/latest/src/userguide/faq.html#how-do-i-run-doctests-in-cython-code-pyx-files 
"""

import iode
from iode import iode_cython

import doctest
import inspect
import logging
import pytest
from typing import Dict, Any


def run_doctests_for_obj(obj: Any, verbose: bool=True):
    def predicate(value: Any) -> bool:
        # WARNING: Python functions defined in a Cython pyx file are of type '_cython_xxx.cython_function_or_method'.
        #          This type is not recognizable by inspect.isfunction() or inspect.isclass() but 
        #          returns True if inspect.isroutine() is used
        return (inspect.isfunction(value) or inspect.isroutine(value) or inspect.isclass(value))

    items: Dict[str, Any] = dict()

    name = obj.__name__
    if predicate(obj):
        doc = inspect.getdoc(obj)
        if doc:
            items[name] = obj

    if inspect.isclass(obj):
        # https://docs.python.org/3.11/library/inspect.html#inspect.getmembers
        # If the optional predicate argument - which will be called with the value object of each 
        # member - is supplied, only members for which the predicate returns a true value are included.
        for _name, value in inspect.getmembers(obj, predicate=predicate):
            if _name.startswith('__'):
                continue                
            doc = inspect.getdoc(value)
            if doc:
                items[f"{name}.{_name}"] = value

    for name, item in items.items():
        doctest.run_docstring_examples(item, name=name, globs={"iode": iode}, verbose=verbose)
        if inspect.isclass(item):
            logging.info(f"tested class:           '{name}'")
        else:
            logging.info(f"tested method/function: '{name}'")


def test_iode_execute():
    run_doctests_for_obj(iode_cython.execute_report)
    run_doctests_for_obj(iode_cython.execute_command)
    run_doctests_for_obj(iode_cython.execute_lec)

def test_iode_simulation_estimation():
    run_doctests_for_obj(iode_cython.Simulation)
    run_doctests_for_obj(iode_cython.dynamic_adjustment)
    run_doctests_for_obj(iode_cython.dickey_fuller_test)

def test_iode_edit_and_estimate_equations():
    from iode.util import EditAndEstimateEquations
    run_doctests_for_obj(EditAndEstimateEquations)

def test_iode_messages():
    run_doctests_for_obj(iode_cython.reset_msgs)
    run_doctests_for_obj(iode_cython.suppress_msgs)
    iode.reset_msgs()

def test_iode_print_preferences():
    from iode.util import (set_printer_preferences, set_A2M_preferences, set_MIF_preferences, 
                           set_RTF_preferences, set_HTML_preferences)
    set_printer_preferences()
    set_A2M_preferences()
    set_MIF_preferences()
    set_RTF_preferences()
    set_HTML_preferences()

def test_iode_miscellaneous():
    from iode.reports import (build_command_functions_list, build_report_functions_list, 
                              build_lec_functions_list)
    from iode.util import table2str, check_filepath, get_iode_file_type

    iode_cython.__test__ = {}
    run_doctests_for_obj(build_command_functions_list)
    run_doctests_for_obj(build_report_functions_list)
    run_doctests_for_obj(build_lec_functions_list)
    run_doctests_for_obj(table2str)
    run_doctests_for_obj(check_filepath)
    run_doctests_for_obj(get_iode_file_type)

def test_iode_errors_warnings_messages_from_C_API(capsys):
    from iode.iode_cython import (iode_error, iode_warning, iode_msg, iode_confirm, 
                                  iode_pause, iode_msgbox)

    with pytest.raises(RuntimeError) as excinfo:
        iode_error(message='test error')
    assert 'test error' in str(excinfo.value)

    with pytest.warns(RuntimeWarning, match="test warning"):
        iode_warning('test warning')

    # will simply print a message in the console
    iode_msg('test message')
    captured = capsys.readouterr()
    assert captured.out == "test message\n"

    # Do not call iode_confirm since it will pause the Python interpreter
    # iode_confirm('test confirm')

    # Do not call iode_kpause since it will pause the Python interpreter
    # iode_pause()

    iode_msgbox('MSG BOX', 'test msgbox')
    captured = capsys.readouterr()
    assert captured.out == "MSG BOX: test msgbox\n"
