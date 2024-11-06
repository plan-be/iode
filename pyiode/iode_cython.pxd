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
cdef extern from "api/iode.h":
    # MISC functions
    #cdef char   *IODE_VERSION
    cdef int    IodeInit(char*)
    cdef int    IodeEnd()
    
    # KDB functions
    cdef void   *K_create(int filetype, int mode)

    # SCR4 functions
    cdef int    SCR_free(void *ptr)
    cdef int    SCR_free_tbl(unsigned char **tbl)

    # constants
    cdef double IODE_NAN

cdef extern from "cpp_api/iode_cpp_api.h":
    cdef void   cpp_iode_init(string)

cdef extern from "s_pyiode.cpp":
    cdef int   ODE_assign_super_PYIODE()
    cdef int   free_tbl(char** tbl)
