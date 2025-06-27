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

from libcpp.string cimport string


# Declarations of extern C functions and vars
# -------------------------------------------
cdef extern from "api/all.h":
    # MISC functions
    #cdef char   *IODE_VERSION
    cdef int    IodeInit()
    cdef int    IodeEnd()
    
    # KDB functions
    cdef void   *K_create(int filetype, int mode)

    # SCR4 functions
    cdef int    SCR_free(void *ptr)
    cdef int    SCR_free_tbl(unsigned char **tbl)
    cdef int    SCR_tbl_size(unsigned char **tbl)

    # constants
    cdef double IODE_NAN

    # functions calling super functions internally
    cdef int     kerror(const int level, const char* fmt, ...) except -1
    cdef void    kwarning(const char* fmt, ...) noexcept
    cdef void    kmsg(const char* fmt, ...) noexcept
    cdef int     kconfirm(const char* fmt, ...) noexcept
    cdef void    kpanic()

cdef extern from "cpp_api/iode_cpp_api.h":
    cdef void   cpp_iode_init() nogil

cdef extern from "cli.cpp":
    cdef int   free_tbl(char** tbl) nogil

cdef extern from "super.cpp":
    cdef void  python_assign_super() nogil

