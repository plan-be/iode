cdef extern from "api/iode.h":
    cdef void K_end_ws(int)
    cdef void(*kerror_super)(const int level, const char* fmt) except +
    cdef void(*kwarning_super)(const char* msg) except +
    cdef void(*kmsg_super)(const char* fmt) except +
    cdef int(*kconfirm_super)(const char* msg) except +
    cdef void(*kpanic_super)() except +
