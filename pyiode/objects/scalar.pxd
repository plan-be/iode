# distutils: language = c++

from libcpp.string cimport string
from libcpp cimport bool


# declare C++ Scalar class
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#declaring-a-c-class-interface 
cdef extern from "cpp_api/objects/scalar.h":
    cdef cppclass CScalar "Scalar":
        double val
        double relax
        double std
        CScalar() except +
        CScalar(const double, const double, const double) except +
        string to_string()
        bool operator==(const CScalar& other)

    size_t hash_value(CScalar&)
