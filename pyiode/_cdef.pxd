
cdef extern from "api/iode.h":
    cdef double *IodeGetVector(char *name, int *lg)
