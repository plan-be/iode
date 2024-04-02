
cdef extern from "api/iode.h":
    cdef int    IodeEstimate(char* veqs, char* afrom, char* ato)
