"""
Cython-compatible wrapper for doctest.testmod().
See https://cython.readthedocs.io/en/latest/src/userguide/faq.html#how-do-i-run-doctests-in-cython-code-pyx-files 
"""

import iode
import doctest
import inspect
import logging
import pytest
from pathlib import Path
from typing import List, Any


# https://docs.python.org/3.11/library/inspect.html#inspect.getmembers
# If the optional predicate argument - which will be called with the value object of each 
# member - is supplied, only members for which the predicate returns a true value are included.
def run_doctests(items: List[str]=None, tmp_dir: Path=None, verbose: bool=False, raise_on_error: bool=False):
    if items is not None:
        def predicate(value: Any) -> bool:
            # WARNING: Python functions defined in a Cython pyx file are of type '_cython_xxx.cython_function_or_method'.
            #          This type is not recognizable by inspect.isfunction() or inspect.isclass() but 
            #          returns True if inspect.isroutine() is used
            return (inspect.isfunction(value) or inspect.isroutine(value) or inspect.isclass(value)) \
                    and value.__name__ in items

        iode.__test__ = {}
        for name, value in inspect.getmembers(iode, predicate=predicate):
            if inspect.isclass(value):
                for name_, value_ in inspect.getmembers(value):
                    doc = inspect.getdoc(value_)
                    if doc is not None:
                        iode.__test__[f'{name}.{name_}'] = doc                 
            doc = inspect.getdoc(value)
            if doc is not None:
                iode.__test__[name] = doc

    # run doctests
    failure_count, test_count = doctest.testmod(iode, globs={"iode": iode, "tmp_dir": tmp_dir})
    assert failure_count == 0
    for name in iode.__test__:
        logging.info(f"tested function/method: '{name}'")
    logging.info(f"number of tests run: {test_count}")


def test_iode_time():
    run_doctests(items=['Period', 'Sample'])


def test_iode_objects(tmp_path):
    run_doctests(items=['Equation', 'Identity', 'Scalar', 'Table', 'split_list'], tmp_dir=tmp_path)


def test_iode_table_line_cell():
    iode.__test__ = {}
    from iode.iode_cython import TableLine
    for name_, value_ in inspect.getmembers(TableLine):
        doc = inspect.getdoc(value_)
        if doc is not None:
            iode.__test__[f'TableLine.{name_}'] = doc 
    from iode.iode_cython import TableCell
    for name_, value_ in inspect.getmembers(TableCell):
        doc = inspect.getdoc(value_)
        if doc is not None:
            iode.__test__[f'TableCell.{name_}'] = doc
    run_doctests()


def test_iode_computed_table(tmp_path):
    iode.__test__ = {}
    from iode.iode_cython import ComputedTable
    for name_, value_ in inspect.getmembers(ComputedTable):
        doc = inspect.getdoc(value_)
        if doc is not None:
            iode.__test__[f'ComputedTable.{name_}'] = doc 
    run_doctests(tmp_dir=tmp_path)


def test_iode_databases(tmp_path):
    run_doctests(items=['Comments', 'Equations', 'Identities', 'Lists', 'Scalars', 'Tables', 
                        'Variables', 'load_extra_files', 'reset_extra_files'], tmp_dir=tmp_path)


def test_iode_execute(tmp_path):
    run_doctests(items=['execute_report', 'execute_command', 'execute_lec'], tmp_dir=tmp_path)


def test_iode_writing(tmp_path):
    run_doctests(items=['w_close', 'w_dest', 'w_flush', 'w_print', 'w_print_cmd', 'w_print_enum', 
                        'w_print_par', 'w_print_pg_footer', 'w_print_pg_header', 'w_print_tit'])


def test_iode_simulation_estimation(tmp_path):
    run_doctests(items=['Simulation', 'dynamic_adjustment', 'dickey_fuller_test'], tmp_dir=tmp_path)


def test_iode_edit_and_estimate_equations(tmp_path):
    from iode.util import EditAndEstimateEquations

    iode.__test__ = {}
    iode.__test__['EditAndEstimateEquations'] = EditAndEstimateEquations.__doc__

    run_doctests(items=['EditAndEstimateEquations'], tmp_dir=tmp_path)


def test_iode_messages():
    run_doctests(items=['add_error_msg', 'clear_error_msgs', 'display_error_msgs', 
                        'reset_msgs', 'suppress_msgs'])
    iode.reset_msgs()


def test_iode_miscellaneous(tmp_path):
    from iode.reports import (build_command_functions_list, build_report_functions_list, 
                              build_lec_functions_list)
    from iode.util import (table2str, check_filepath, get_iode_file_type, iode_msg_path, 
                           print_error_msg)

    iode.__test__ = {}
    iode.__test__['build_command_functions_list'] = build_command_functions_list.__doc__ 
    iode.__test__['build_report_functions_list'] = build_report_functions_list.__doc__ 
    iode.__test__['build_lec_functions_list'] = build_lec_functions_list.__doc__ 
    iode.__test__['table2str'] = table2str.__doc__
    iode.__test__['check_filepath'] = check_filepath.__doc__
    iode.__test__['get_iode_file_type'] = get_iode_file_type.__doc__
    iode.__test__['iode_msg_path'] = iode_msg_path.__doc__
    iode.__test__['print_error_msg'] = print_error_msg.__doc__

    run_doctests(tmp_dir=tmp_path)


def test_iode_errors_warnings_messages_from_C_API():
    from iode.iode_cython import iode_error, iode_warning, iode_msg, iode_confirm, iode_panic

    # Do not call iode_error with level > 0 since it will exits the Python interpreter
    with pytest.raises(RuntimeError) as excinfo:
        iode_error(level=0, message='test error')
    assert 'test error' in str(excinfo.value)

    with pytest.warns(UserWarning, match="test warning"):
        iode_warning('test warning')

    # will simply print a message in the console
    iode_msg('test message')

    # Do not call iode_confirm since it will pause the Python interpreter
    # iode_confirm('test confirm')

    # Do not call iode_panic since it will exits the Python interpreter
    # iode_panic('test panic')
