from libcpp.string cimport string
from libcpp cimport bool

cdef extern from "api/iode.h":
    ctypedef struct PERIOD:
        long p_y
        long p_s
        char p_p
        char p_pad[3]

    PERIOD* PER_atoper(char*)

# declare C++ Period class
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#declaring-a-c-class-interface 
cdef extern from "time/period.h":
    cdef cppclass CPeriod "Period":
        long p_y
        char p_p
        long p_s
        CPeriod() except +
        CPeriod(const CPeriod&) except +
        CPeriod(const int, const char, const int) except +
        CPeriod(const string) except +
        int nb_periods_per_year() except +
        int difference(const CPeriod&) except +
        CPeriod shift(const int) except +
        float to_float() except +
        string to_string()
        bool operator==(const CPeriod&)
