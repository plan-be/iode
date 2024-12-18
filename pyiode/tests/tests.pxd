cdef extern from "api/iode.h":
    cdef void    kerror(const int level, const char* fmt) except *
    cdef void    kwarning(const char* fmt) noexcept
    cdef void    kpause() noexcept
    cdef void    kmsg(const char* fmt) noexcept
    cdef void    kpanic() except *
    cdef int     kconfirm(const char* fmt) noexcept
