
cdef extern from "api/iode.h":
    cdef int    B_DataUpdate(char*, int)
    cdef int    B_DataDelete(char* arg, int obj_type)
