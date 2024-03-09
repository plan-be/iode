
cdef extern from "iode.h":
    cdef char   *IodeVersion()
    cdef void   IodeSuppressMsgs()
    cdef void   IodeResetMsgs()
    cdef void   IodeAddErrorMsg(char* msg)
    cdef void   IodeDisplayErrorMsgs()
    cdef void   IodeClearErrorMsgs()
