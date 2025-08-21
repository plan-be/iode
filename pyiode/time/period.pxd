from libcpp.string cimport string
from libcpp cimport bool


# declare C++ Period class
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#declaring-a-c-class-interface 
cdef extern from "api/time/period.h":
    int get_nb_periods_per_year(char per) except +

    cdef cppclass CPeriod "Period":
        long year
        long step
        char periodicity

        CPeriod() except +
        CPeriod(const CPeriod&) except +
        CPeriod(const int, const char, const int) except +
        CPeriod(const string) except +
        int difference(const CPeriod&) except +
        CPeriod shift(const int) except +
        float to_float() except +
        string to_string()
        bool operator==(const CPeriod&)
