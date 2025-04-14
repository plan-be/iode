cdef extern from "api/iode.h":
    cdef bint   IODE_IS_A_NUMBER(double)
    cdef void   IodeSuppressMsgs()
    cdef void   IodeResetMsgs()
