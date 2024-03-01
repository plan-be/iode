
cdef extern from "iode.h":
    cdef int    B_ReportExec(char *filename)
    cdef int    B_ReportLine(char *repline)
