
cdef extern from "iode.h":
    cdef char   *IodeVersion()
    cdef void   IodeSuppressMsgs()
    cdef void   IodeResetMsgs()
