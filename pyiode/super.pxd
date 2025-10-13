from pyiode.iode_database.cpp_api_database cimport KDB

cdef extern from "api/all.h":
    cdef KDB* KT_WS

    cdef int   kmsgbox_continue
    cdef int   kpause_continue

    cdef void K_end_ws(int)
    cdef bint K_add(KDB* kdb, char* name, ...)
