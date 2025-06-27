
cdef extern from "api/all.h":
    cdef int    W_dest(char *filename,int type)
    cdef int    W_close()
    cdef int    W_flush()
    cdef int    W_printf(char *fmt, char *txt)
    cdef void   W_print_enum(int )
    cdef void   W_print_cmd(int )
    cdef void   W_print_par(int )
    cdef void   W_print_tit(int )
    cdef void   W_print_pg_header(char* arg)
    cdef void   W_print_pg_footer(char* arg)
