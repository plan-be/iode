from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool

from pyiode.time.period cimport CPeriod


# declare C++ Sample class
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#declaring-a-c-class-interface 
cdef extern from "api/time/sample.h":
    cdef cppclass CSample "Sample":
        CPeriod  start_period
        CPeriod  end_period  
        short    nb_periods  

        CSample(const CSample& sample) except +
        CSample(const string&, const string&) except +
        int get_period_position(const CPeriod&) except +
        int get_period_position(const string&) except +
        vector[string] get_list_periods() except +
        vector[float] get_list_periods_as_float() except +
        CSample intersection(const CSample&) except +
        string to_string()
        bool operator==(const CSample&)
