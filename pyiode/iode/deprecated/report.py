import warnings
from typing import Union, List 

from iode import execute_report, execute_command


def report_exec(filename_parms: str):
    warnings.warn("report_exec() is deprecated. Please use the new syntax:\n"
                  "execute_report(filename, parameters)", DeprecationWarning, stacklevel=2)
    filename, *params = filename_parms.split()
    execute_report(filename, params)

def reportline_exec(repline: Union[str, List[str]]):
    warnings.warn("reportline_exec() is deprecated. Please use the new syntax:\n"
                  "execute_command(commands)", DeprecationWarning, stacklevel=2)
    if not isinstance(repline, str):
        repline = '\n'.join(repline)
    execute_command(repline)
