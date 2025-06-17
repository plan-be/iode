from libcpp.string cimport string
from libcpp.vector cimport vector

from pyiode.reports cimport RP_PER, RP_settime, RP_incrtime
from pyiode.reports cimport build_command_functions_list as cpp_build_command_functions_list
from pyiode.reports cimport build_report_functions_list as cpp_build_report_functions_list
from pyiode.reports cimport build_lec_functions_list as cpp_build_lec_functions_list
from pyiode.reports cimport execute_report as cpp_execute_report 
from pyiode.reports cimport execute_report_line as cpp_execute_report_line

from typing import List


def cython_set_time(period: str):
    cdef bytes b_arg = period.encode()
    cdef char* c_arg = b_arg
    RP_settime(c_arg)

def cython_increment_time(n: int):
    cdef bytes b_arg = str(n).encode()
    cdef char* c_arg = b_arg
    RP_incrtime(c_arg)

def cython_build_command_functions_list(group: int, gui: bool) -> List[str]:
    return[name.decode() for name in cpp_build_command_functions_list(group, <bint>gui)]

def cython_build_report_functions_list() -> List[str]:
    return[name.decode() for name in cpp_build_report_functions_list()]

def cython_build_lec_functions_list() -> List[str]:
    return[name.decode() for name in cpp_build_lec_functions_list()]

def cython_execute_report(filepath: str, parameters: str):
    cpp_execute_report(filepath.encode(), parameters.encode())

def cython_execute_command(command: str):    
    cpp_execute_report_line(command.encode())
