cdef extern from "api/all.h":
    cdef bint   IODE_IS_A_NUMBER(double)
    cdef void   IodeSuppressMsgs()
    cdef void   IodeResetMsgs()
