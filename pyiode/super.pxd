from libcpp.string cimport string

cdef extern from "api/all.h":
    ctypedef struct KDB:
        bint contains(string& name)
        bint add(string& name, char* value) except +
        bint remove(string& name) except +

    cdef KDB* KT_WS

    cdef int kmsgbox_continue
    cdef int kpause_continue

    cdef void K_end_ws(int)
