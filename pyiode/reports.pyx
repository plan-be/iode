# distutils: language = c++

from libcpp.string cimport string
from libcpp.vector cimport vector

from pyiode.reports cimport build_command_functions_list as cpp_build_command_functions_list
from pyiode.reports cimport build_report_functions_list as cpp_build_report_functions_list
from pyiode.reports cimport build_lec_functions_list as cpp_build_lec_functions_list
from pyiode.reports cimport execute_report as cpp_execute_report 
from pyiode.reports cimport execute_report_line as cpp_execute_report_line

from pathlib import Path
from typing import Union, List


def build_command_functions_list(group: int, gui: bool=False) -> List[str]:
    """
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
    ['$label', '$goto', '$ask', ..., '$next', '$procdef', '$procexec']
    >>> build_command_functions_list(0, True)      # doctest: +ELLIPSIS
    ['#label', '#goto', '#ask', ..., '#next', '#procdef', '#procexec']
    >>> build_command_functions_list(1, False)     # doctest: +ELLIPSIS
    ['$FileImportVar', '$FileImportCmt', '$FileDelete', ..., '$CsvDec', '$CsvNaN', '$CsvAxes']
    >>> build_command_functions_list(1, True)      # doctest: +ELLIPSIS
    ['#FileImportVar', '#FileImportCmt', '#WsSample', ..., '#ReportExec', '#ReportEdit', '#ReportPrompt']
    """
    return[name.decode() for name in cpp_build_command_functions_list(group, <bint>gui)]

def build_report_functions_list() -> List[str]:
    """
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
    ['@upper', '@date', '@time', '@lower', ..., '@mkdir', '@rmdir', '@void', '@version']
    """
    return[name.decode() for name in cpp_build_report_functions_list()]

def build_lec_functions_list() -> List[str]:
    """
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
    return[name.decode() for name in cpp_build_lec_functions_list()]


# functions to be called by the users
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

    cpp_execute_report(filepath.encode(), parameters.encode())


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
    >>> result_var_file = str(output_dir / "test_var.av")

    >>> # execute IODE command one by one
    >>> execute_command("$WsClearVar")
    >>> execute_command("$WsSample 2000Y1 2005Y1")
    >>> execute_command("$DataCalcVar A t+1")
    >>> execute_command("$DataCalcVar B t-1")
    >>> execute_command("$DataCalcVar C A/B")
    >>> execute_command("$DataCalcVar D grt A")
    >>> execute_command(f"$WsSaveVar {result_var_file}")    # doctest: +ELLIPSIS
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
    
    cpp_execute_report_line(command.encode())
