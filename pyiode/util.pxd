from libcpp cimport bool
from libcpp.string cimport string

cdef extern from "api/all.h":
    cdef bint   IODE_IS_A_NUMBER(double)
    cdef void   IodeSuppressMsgs()
    cdef void   IodeResetMsgs()
    cdef int    RP_define(char* arg)
    cdef bool   RP_macro_exists(const string name)
    cdef string RP_get_macro(const string name)
