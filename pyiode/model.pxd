
cdef extern from "api/iode.h":
    cdef int    IodeModelSimulate(char *per_from, char *per_to, char *eqs_list, char *endo_exo_list, double eps, double relax, int maxit, int init_values, int sort_algo, int nb_passes, int debug, double newton_eps, int newton_maxit, int newton_debug)
    cdef int    IodeModelCalcSCC(int nbtris, char* pre_listname, char* inter_listname, char* post_listname, char *eqs_list)
    cdef int    IodeModelSimulateSCC(char *per_from, char *per_to, 
                                     char *pre_eqlist, char *inter_eqlist, char* post_eqlist,
                                     double eps, double relax, int maxit, 
                                     int init_values, int debug, 
                                     double newton_eps, int newton_maxit, int newton_debug)
    cdef float  IodeModelSimNorm(char* period)
    cdef int    IodeModelSimNIter(char* period)
    cdef int    IodeModelSimCpu(char* period)
    cdef int    IodeModelCpuSort()
    cdef int    IodeModelCpuSCC()
    cdef int    KSIM_MAXIT 
    cdef float  KSIM_EPS   
    cdef float  KSIM_RELAX 
    cdef int    KSIM_PASSES
    cdef int    KSIM_SORT
    cdef int    KSIM_START 
    cdef int    KSIM_CPU_SCC
    cdef int    KSIM_CPU_SORT