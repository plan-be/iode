
cdef extern from "api/iode.h":
    cdef double *IodeExecLec(char* lec)
    cdef double IodeExecLecT(char* lec, int t)
