
cdef extern from "api/iode.h":
    cdef int    IodeDeleteObj(char* name, int type)
