# distutils: language = c++

from libcpp.string cimport string
from libcpp.vector cimport vector

cdef extern from "cpp_api/lec/lec.h":
    double execute_lec(string& lec, const int t)
    double execute_lec(const string& lec, const string& period)
    vector[double] execute_lec(const string& lec)
