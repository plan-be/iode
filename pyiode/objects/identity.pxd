# distutils: language = c++

from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.pair cimport pair
from libcpp cimport bool


cdef extern from "api/objs/identities.h":

    # declare C++ Identity class
    # see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#declaring-a-c-class-interface 
    cdef cppclass CIdentity "Identity":
        CIdentity(string&) except +
        CIdentity(const CIdentity& other) except +
        
        string get_lec() except +
        void set_lec(string&) except +
        vector[string] get_coefficients_list(const bool create_if_not_exit) except +
        vector[string] get_variables_list(const bool create_if_not_exit) except +

        bool operator==(const CIdentity&) except +
