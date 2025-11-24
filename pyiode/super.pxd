from libcpp.string cimport string

cdef extern from "api/all.h":
    cdef int kmsgbox_continue
    cdef int kpause_continue

    cdef void K_end_ws(int)
