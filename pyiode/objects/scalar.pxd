from libcpp.string cimport string
from libcpp cimport bool


# declare C++ Scalar class
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#declaring-a-c-class-interface 
cdef extern from "objects/scalar.h":
    cdef cppclass Scalar:
        double val
        double relax
        double std
        Scalar() except +
        Scalar(const double, const double, const double) except +
        string to_string()
        bool operator==(const Scalar& other)

    size_t hash_value(Scalar&)
