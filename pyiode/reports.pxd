# distutils: language = c++

from libcpp.string cimport string
from libcpp.vector cimport vector
from pyiode.time.period cimport PERIOD

cdef extern from "api/iode.h":
    cdef PERIOD RP_PER
    cdef int RP_settime(char* arg)
    cdef int RP_incrtime(char* arg)

# functions written in rep_list.h are intended to be only used in the GUI, not by users
cdef extern from "cpp_api/report/rep_list.h":
    cdef vector[string] build_command_functions_list(const int group, const bint gui) except +
    cdef vector[string] build_report_functions_list() except +
    cdef vector[string] build_lec_functions_list() except +

# functions to be called by the users
cdef extern from "cpp_api/report/report.h":
    cdef void execute_report(const string& filepath, const string& parameters) except +
    cdef void execute_report_line(const string& commands) except +
