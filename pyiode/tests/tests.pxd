cdef extern from "api/iode.h":
    cdef void    kerror(const int level, const char* fmt, ...)
    cdef void    kwarning(const char* fmt, ...)
    cdef void    kpause()
    cdef void    kmsg(const char* fmt, ...)
    cdef void    kpanic()
    cdef int     kconfirm(const char* fmt, ...)
