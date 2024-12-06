cdef extern from "api/iode.h":
    cdef void K_end_ws(int)
    cdef void(*kerror_super)(const int level, const char* fmt);
    cdef void(*kwarning_super)(const char* msg)
    cdef void(*kmsg_super)(const char* fmt)
    cdef int(*kconfirm_super)(const char* msg)
    cdef void(*kpanic_super)()
