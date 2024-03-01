
cdef extern from "iode.h":
    cdef double *IodeGetVector(char *name, int *lg)
