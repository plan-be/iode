from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool

from time.period cimport CPeriod


# declare C++ Sample class
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#declaring-a-c-class-interface 
cdef extern from "cpp_api/time/sample.h":
    cdef cppclass CSample "Sample":
        CSample() except +
        CSample(const CSample&) except +
        CSample(const CPeriod&, const CPeriod&) except +
        CSample(const string, const string) except +
        CPeriod start_period()
        CPeriod end_period()
        int nb_periods()
        int get_period_position(const CPeriod&) except +
        int get_period_position(const string&) except +
        vector[string] get_list_periods() except +
        vector[float] get_list_periods_as_float() except +
        CSample intersection(const CSample&) except +
        string to_string()
        bool operator==(const CSample&)
