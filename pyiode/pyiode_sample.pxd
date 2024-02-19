from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool

from pyiode_period cimport Period


# declare C++ Sample class
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#declaring-a-c-class-interface 
cdef extern from "time/sample.h":
    cdef cppclass Sample:
        Sample() except +
        Sample(const Sample&) except +
        Sample(const Period&, const Period&) except +
        Sample(const string, const string) except +
        Period start_period()
        Period end_period()
        int nb_periods()
        int get_period_position(const Period&) except +
        int get_period_position(const string&) except +
        vector[string] get_list_periods() except +
        vector[float] get_list_periods_as_float() except +
        Sample intersection(const Sample&) except +
        string to_string()
        bool operator==(const Sample&)
