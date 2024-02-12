from libcpp.string cimport string
from libcpp cimport bool

# declare C++ Period class
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#declaring-a-c-class-interface 
cdef extern from "time/period.h":
    cdef cppclass Period:
        long p_y
        char p_p
        long p_s
        Period() except +
        Period(const Period&) except +
        Period(const int, const char, const int) except +
        Period(const string) except +
        int nb_periods_per_year() except +
        int difference(const Period&) except +
        Period shift(const int) except +
        float to_float() except +
        string to_string()
        bool operator==(const Period&)
