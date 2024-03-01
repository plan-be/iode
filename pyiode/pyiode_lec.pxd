
cdef extern from "iode.h":
    cdef double *IodeExecLec(char* lec)
    cdef double IodeExecLecT(char* lec, int t)
