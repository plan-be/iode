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

import sys
import numpy as np
cimport numpy as np
import cython
import pandas as pd

from typing import Union, Any, List, Tuple

try:
    import larray as la
    Array = la.Array
    Axis = la.Axis
except ImportError:
    la = None
    Array = Any
    Axis = Any

# C libraries
from libc.stdlib cimport free, malloc
from libc.stdio cimport printf

# C++ libraries
from libcpp.string cimport string
from libcpp.vector cimport vector

# Cython libraries
from cpython cimport PyObject, Py_INCREF

#from enum import Enum


# IODE defines used in python functions
# -------------------------------------
# (from #defines in iode.h)

# Object and file types
K_CMT = 0
K_EQS = 1
K_IDT = 2
K_LST = 3
K_SCL = 4
K_TBL = 5
K_VAR = 6

# Simulation parameters
SORT_CONNEX = 0
SORT_BOTH   = 1
SORT_NONE   = 2

KV_INIT_TM1      = 0
KV_INIT_TM1_A    = 1
KV_INIT_EXTRA    = 2
KV_INIT_EXTRA_A  = 3
KV_INIT_ASIS     = 4
KV_INIT_TM1_NA   = 5
KV_INIT_EXTRA_NA = 6

# Print outputs
W_GDI   = 1
W_A2M   = 7
W_MIF   = 6
W_HTML  = 5
W_RTF   = 4
W_CSV   = 8
W_DUMMY = 9

# HTOL methods
HTOL_LAST = 0
HTOL_MEAN = 1
HTOL_SUM  = 2

# LTOH defines 
LTOH_STOCK  = 0
LTOH_FLOW   = 1
LTOH_LIN    = 'L'
LTOH_CS     = 'C'
LTOH_STEP   = 'S'


# Declarations of extern C functions and vars
# -------------------------------------------
cdef extern from "iode.h":
    # MISC functions
    #cdef char   *IODE_VERSION
    cdef char   *IodeVersion()
    cdef int    IodeInit()
    cdef int    IodeEnd()
    
    # KDB functions
    cdef void   *K_create(int filetype, int mode)

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


    # SAMPLE functions
    cdef int    IodeIsSampleSet()
    cdef int    IodeGetSampleLength()
    cdef char   **IodeCreateSampleAsPeriods(char* aper_from, char* aper_to) 
    cdef char   **IodeGetSampleAsPeriods();
    cdef char   *IodeGetSampleAsString()
    cdef double *IodeGetSampleAsDoubles(int *lg)
    cdef int    IodeSetSampleStr(char* str_from, char* str_to)


    # OBJECTS functions
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
    cdef double *IodeGetVector(char *name, int *lg)
    cdef int    IodeSetVector(char *la_name, double *la_values, int la_pos, int ws_pos, int la_lg)

    # ESTIMATION functions
    cdef int    IodeEstimate(char* veqs, char* afrom, char* ato)

    # MODEL functions
    cdef int    IodeModelSimulate(char *per_from, char *per_to, char *eqs_list, char *endo_exo_list, double eps, double relax, int maxit, int init_values, int sort_algo, int nb_passes, int debug, double newton_eps, int newton_maxit, int newton_debug)

    # REPORT functions
    cdef int    B_DataUpdate(char*, int)
    cdef int    B_DataDelete(char* arg, int obj_type)

    cdef int    B_ReportExec(char *filename)
    cdef int    B_ReportLine(char *repline)
    

    # LEC functions
    cdef double *IodeExecLec(char* lec)
    cdef double IodeExecLecT(char* lec, int t)

    # WRT functions
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

    # SCR4 functions
    cdef int    SCR_free(void *ptr)

    # TO Check
    cdef int    PyIodePrint(char*name, void* values, int lg)
    cdef void   IodeSuppressMsgs()
    cdef void   IodeResetMsgs()
    cdef int    ODE_assign_super_PYIODE()

    # constants
    cdef double L_NAN

cdef extern from "s_iode.c":
    cdef int   free_tbl(char** tbl)


# IODE CONSTANTS
# --------------
nan = L_NAN
