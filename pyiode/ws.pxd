
cdef extern from "api/iode.h":
    cdef int    B_WsHtoLLast(char *)
    cdef int    B_WsHtoLMean(char *)
    cdef int    B_WsHtoLSum(char *)
    cdef int    B_WsLtoHStock(char *)
    cdef int    B_WsLtoHFlow(char *)
