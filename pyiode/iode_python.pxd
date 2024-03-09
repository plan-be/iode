# IODE EXTENSION FOR PYTHON
# =========================
#
#    @header4iode
# 

# Info by GdM:
# ------------
# Il faut utiliser la « compiler directive » binding=True ou embedsignature=True (juste la signature pas d’autocompletion, etc mais plus performant)
# Tu peux soit rajouter # cython: binding=True au début de ton fichier .pyx
# Ou rajouter @cython.binding(True) avant chaque fonction qui fait partie de ton “API Python”.
# Ou rajouter compiler_directives={'binding':  True} dans l’appel à cythonize dans ton setup.py


# Declarations of extern C functions and vars
# -------------------------------------------
cdef extern from "iode.h":
    # MISC functions
    #cdef char   *IODE_VERSION
    cdef int    IodeInit()
    cdef int    IodeEnd()
    
    # KDB functions
    cdef void   *K_create(int filetype, int mode)

    # SCR4 functions
    cdef int    SCR_free(void *ptr)
    cdef int    SCR_free_tbl(char **tbl)

    # MODEL functions
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

    # IDENTITY functions
    cdef int    IodeExecuteIdts(char *smpl, char *idt_list, char *var_files, char *scl_files, int trace)
 

    # TO Check
    cdef int    PyIodePrint(char*name, void* values, int lg)
    cdef int    ODE_assign_super_PYIODE()

    # constants
    cdef double L_NAN

cdef extern from "s_pyiode.c":
    cdef int   free_tbl(char** tbl)
