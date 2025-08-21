# distutils: language = c++

from libcpp.string cimport string
from libcpp cimport bool


# declare C++ Scalar class
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#declaring-a-c-class-interface 
cdef extern from "api/objs/scalars.h":
    cdef cppclass CScalar "Scalar":
        double value
        double relax
        double std
        
        CScalar() except +
        CScalar(const CScalar& other) except +
        CScalar(const double, const double) except +
        string to_string()
        bool operator==(const CScalar& other)

    size_t hash_value(CScalar&)
