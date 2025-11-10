from pathlib import Path
from typing import Union, List

from iode.iode_cython import cython_set_time, cython_increment_time
from iode.iode_cython import cython_build_command_functions_list 
from iode.iode_cython import cython_build_lec_functions_list
from iode.iode_cython import cython_build_report_functions_list
from iode.iode_cython import cython_execute_command
from iode.iode_cython import cython_execute_report

from iode.time.period import Period


def set_t(time: Union[str, Period]):
    """
    Set the value of *t* in `{report expressions}`.

    Parameters
    ----------
    time: str or Period
        Period to set. If str, it must be a valid IODE period (e.g. "2000Y1", "2005Y1", etc.)

    Examples
    --------
    >>> from iode.reports import set_t, execute_command
    >>> set_t("2000Y1")
    >>> execute_command("value of t: {t@T}")
    iode> value of t: 2000Y1
    """
    if isinstance(time, Period):
        time = str(time)
    elif not isinstance(time, str):
        raise TypeError("'time' must be a string or a Period object.")
    cython_set_time(time)

def increment_t(increment: int):
    """
    Increment the value of *t* in `{report expressions}` by a given number of periods.

    Parameters
    ----------
    increment: int
        Number of periods to increment *t* by. 
        For example, if *t* is "2000Y1" and increment is 1, then *t* will become "2001Y1".

    Examples
    --------
    >>> from iode.reports import set_t, increment_t, execute_command
    >>> set_t("2000Y1")
    >>> increment_t(1)
    >>> execute_command("value of t: {t@T}")
    iode> value of t: 2001Y1
    """
    if not isinstance(increment, int):
        raise TypeError("'increment' must be an integer.")
    cython_increment_time(increment)

def build_command_functions_list(group: int, gui: bool=False) -> List[str]:
    r"""
    Setup the IODE $/#-functions (i.e. IODE commands).

    Interactive commands (which open something in the GUI) starts with the character '#'.
    Non interactive commands starts with the character '$'.

    Parameters
    ----------
    group: int
        'group' is used to defined groups for the color highlighting.
        Possible values are either 0 or 1.
    gui: bool, optional
        False: for the console, True: for the GUI

    Notes
    -----
    See b_rep_syntax.c (C API) for the list

    Warnings
    --------
    Intended to be used only for building the GUI, not called by users

    Examples
    --------
    >>> from iode.reports import build_command_functions_list
    >>> build_command_functions_list(0, False)     # doctest: +ELLIPSIS
    ['$abort', '$ask', '$beep', ..., '$undef', '$undefine', '$vseps']
    >>> build_command_functions_list(0, True)      # doctest: +ELLIPSIS
    ['#abort', '#ask', '#beep', ..., '#undef', '#undefine', '#vseps']
    >>> build_command_functions_list(1, False)     # doctest: +ELLIPSIS
    ['$A2mToCsv', '$A2mToHtml', '$A2mToMif', ..., '$WsSeasonAdj', '$WsTrend', '$WsTrendStd']
    >>> build_command_functions_list(1, True)      # doctest: +ELLIPSIS
    ['#DataCalcLst', '#DataCompareCmt', '#DataCompareEqs', ..., '#WsSeasonAdj', '#WsTrend', '#WsTrendStd']
    """
    return cython_build_command_functions_list(group, gui)


def build_report_functions_list() -> List[str]:
    r"""
    Setup the @-functions (i.e. the IODE functions)

    Notes
    -----
    See b_rep_syntax.c (C API) for the list

    Warnings
    --------
    Intended to be used only for building the GUI, not called by users

    Examples
    --------
    >>> from iode.reports import build_report_functions_list
    >>> build_report_functions_list()          # doctest: +ELLIPSIS
    ['@ansi', '@cexpand', '@chdir', '@chronoget', ..., '@vliste', '@void', '@vtake', '@vvalue']
    """
    return cython_build_report_functions_list()


def build_lec_functions_list() -> List[str]:
    r"""
    Set the list of functions available in LEC expression.

    Notes
    -----
    See l_token.c (C API) for the list

    Warnings
    --------
    Intended to be used only for building the GUI, not called by users
    Examples
    --------
    >>> from iode.reports import build_lec_functions_list
    >>> build_lec_functions_list()         # doctest: +ELLIPSIS
    ['abs', 'acf', 'acos', 'and', ..., 'urandom', 'var', 'vmax', 'vmin']
    """
    return cython_build_lec_functions_list()


def execute_report(filepath: Union[str, Path], parameters: Union[str, List[str]]=None):
    r"""
    Execute an IODE report.

    Parameters
    ----------
    filepath: str, Path
        Filepath of the IODE report to execute. 
        The extension of the file must be *.rep*.
    parameters: str or list(str), optional
        Parameter(s) passed to the report.
        If multiple parameters are passed in one string, they must be separated by a whitespace.
        Default to None.

    Notes
    -----
    Equivalent to the IODE command `$ReportExec`

    Examples
    --------
    >>> from iode import execute_report, variables
    >>> from pathlib import Path
    >>> output_dir = getfixture('tmp_path')
    >>> create_var_rep = str(output_dir / "create_var.rep")
    >>> result_var_file = str(output_dir / "test_var.av")

    >>> # create an dump report
    >>> with open(create_var_rep, "w") as f:                # doctest: +ELLIPSIS
    ...     f.write("$WsClearVar\n")
    ...     f.write("$WsSample 2000Y1 2005Y1\n")
    ...     f.write("$DataCalcVar %1% t+1 \n")
    ...     f.write("$DataCalcVar %2% t-1 \n")
    ...     f.write("$DataCalcVar %3% %1%/%2%\n")
    ...     f.write("$DataCalcVar %4% grt %1% \n")
    ...     f.write(f"$WsSaveVar {result_var_file}\n")
    12
    24
    22
    22
    25
    26
    ...

    >>> # execute report
    >>> execute_report(create_var_rep, ["A", "B", "C", "D"])    # doctest: +ELLIPSIS
    iode> $WsClearVar
    iode> $WsSample 2000Y1 2005Y1
    iode> $DataCalcVar A t+1
    iode> $DataCalcVar B t-1
    iode> $DataCalcVar C A/B
    iode> $DataCalcVar D grt A
    iode> $WsSaveVar ...test_var.av
    Saving ...test_var.av

    >>> # check content of file test_var.av
    >>> with open(result_var_file, "r") as f:                   # doctest: +NORMALIZE_WHITESPACE
    ...     print(f.read())
    sample 2000Y1 2005Y1
    A 1 2 3 4 5 6
    B -1 0 1 2 3 4
    C -1 na 3 2 1.66666666666667 1.5
    D na 100 50 33.3333333333333 25 20
    <BLANKLINE>
    """
    if isinstance(filepath, str):
        filepath = Path(filepath)
    if not filepath.exists():
        raise ValueError(f"file '{str(filepath)}' not found.")
    filepath = str(filepath)

    if parameters is None:
        parameters = ''
    if not isinstance(parameters, str):
        parameters = ' '.join(parameters)
    
    cython_execute_report(filepath, parameters)


def execute_command(command: Union[str, List[str]]):
    r"""
    Execute one or several IODE commands. 

    Parameters
    ----------
    command: str or list(str)
        IODE command(s) to execute. 
        If multiple commands are passed as one string, each command must end 
        with the special character *\n*.

    Examples
    --------
    >>> from iode import execute_command
    >>> from pathlib import Path
    >>> output_dir = getfixture('tmp_path')
    >>> result_var_file = str(output_dir / "test_var.av")

    >>> # execute IODE command one by one
    >>> execute_command("$WsClearVar")
    iode> $WsClearVar
    >>> execute_command("$WsSample 2000Y1 2005Y1")
    iode> $WsSample 2000Y1 2005Y1
    >>> execute_command("$DataCalcVar A t+1")
    iode> $DataCalcVar A t+1
    >>> execute_command("$DataCalcVar B t-1")
    iode> $DataCalcVar B t-1
    >>> execute_command("$DataCalcVar C A/B")
    iode> $DataCalcVar C A/B
    >>> execute_command("$DataCalcVar D grt A")
    iode> $DataCalcVar D grt A
    >>> execute_command(f"$WsSaveVar {result_var_file}")    # doctest: +ELLIPSIS
    iode> $WsSaveVar ...test_var.av
    Saving ...test_var.av

    >>> # check content of file test_var.av
    >>> with open(result_var_file, "r") as f:               # doctest: +NORMALIZE_WHITESPACE
    ...     print(f.read())
    sample 2000Y1 2005Y1
    A 1 2 3 4 5 6
    B -1 0 1 2 3 4
    C -1 na 3 2 1.66666666666667 1.5
    D na 100 50 33.3333333333333 25 20
    <BLANKLINE>
    """
    if not isinstance(command, str):
        command = '\n'.join(command)
    
    cython_execute_command(command)

