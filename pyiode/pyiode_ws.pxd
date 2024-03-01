
cdef extern from "iode.h":
    # WS functions
    cdef int    IodeLoad(char *filename, int filetype)
    cdef int    IodeSave(char *filename, int filetype) 
    cdef int    IodeClearWs(int filetype)
    cdef int    IodeClearAll()
    cdef char   **IodeContents(char *pattern, int objtype)
    cdef int    B_WsHtoLLast(char *)
    cdef int    B_WsHtoLMean(char *)
    cdef int    B_WsHtoLSum(char *)
    cdef int    B_WsLtoHStock(char *)
    cdef int    B_WsLtoHFlow(char *)