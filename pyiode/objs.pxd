
cdef extern from "api/iode.h":
    cdef int    IodeDeleteObj(char* name, int type)

    cdef char   *IodeGetCmt(char *name)
    cdef int    IodeSetCmt(char *name, char *cmttxt)

    cdef char   *IodeGetEqsLec(char *name)
    cdef int    IodeGetEqs(char *name, char** lec, int* method, char *sample_from, char *sample_to, char** blk, char** instr, float tests[])
    cdef int    IodeSetEqs(char *name, char *eqlec)

    cdef char   *IodeGetIdt(char *name)
    cdef int    IodeSetIdt(char *name, char *idt)

    cdef char   *IodeGetLst(char *name)
    cdef int    IodeSetLst(char *name, char *lst)
    
    cdef int    IodeGetScl(char *name, double* value, double *relax, double *std)
    cdef int    IodeSetScl(char *name, double value, double relax, double std)
    
    cdef int    IodeCalcSamplePosition(char *str_la_from, char* str_la_to, int *la_pos, int *ws_pos, int *la_lg)
    cdef int    IodeSetVector(char *la_name, double *la_values, int la_pos, int ws_pos, int la_lg)