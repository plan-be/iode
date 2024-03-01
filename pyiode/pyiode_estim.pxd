
cdef extern from "iode.h":
    cdef int    IodeEstimate(char* veqs, char* afrom, char* ato)
