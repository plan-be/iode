cdef extern from "api/iode.h":
    cdef void    kwarning(const char* fmt) noexcept
    cdef void    kpause() noexcept
    cdef void    kmsg(const char* fmt) noexcept
    cdef void    kpanic() except *
    cdef int     kconfirm(const char* fmt) noexcept
    cdef void    kpause() noexcept
    cdef int     kmsgbox(const unsigned char* title, const unsigned char* msg, 
                         const unsigned char** buts) noexcept

cdef extern from "cpp_api/utils/super.h":
    cdef void cpp_kerror(const char* msg) except +
