from libcpp cimport bool
from libcpp.string cimport string

cdef extern from "api/all.h":
    cdef void   skip_message(const bool value)
    cdef void   skip_pause(const bool value)
    cdef void   skip_msg_box(const bool value)
    cdef bint   IODE_IS_A_NUMBER(double)
    cdef int    RP_define(char* arg)
    cdef bool   RP_macro_exists(const string name)
    cdef string RP_get_macro(const string name)
