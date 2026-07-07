from libcpp.string cimport string

cdef extern from "api/all.h":
    cdef void K_end_ws(int)
