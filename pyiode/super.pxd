cdef extern from "api/iode.h":
    ctypedef struct KDB
    ctypedef struct TBL
    cdef KDB* KT_WS

    cdef void K_end_ws(int)
    cdef int K_find(KDB* kdb, char* name)
    cdef int K_add(KDB* kdb, char* name, ...)
    cdef int K_del(KDB* kdb, int pos)
