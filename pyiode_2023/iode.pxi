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
import larray as la
import pandas as pd
 
from libc.stdlib cimport free, malloc
from libc.stdio cimport printf
from cpython cimport PyObject, Py_INCREF
from typing import Union, List, Tuple, Optional

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
    cdef void   ODE_assign_super_PYIODE()
    
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


    # IDENTITY functions
    cdef int    IodeExecuteIdts(char *smpl, char *idt_list, char *var_files, char *scl_files, int trace)
 

    # MODEL functions
    cdef int    IodeModelSimulate(char *per_from, char *per_to, char *eqs_list, char *endo_exo_list, double eps, double relax, int maxit, int init_values, int sort_algo, int nb_passes, int debug, double newton_eps, int newton_maxit, int newton_debug)
    cdef int    IodeModelCalcSCC(int nbtris, char* pre_listname, char* inter_listname, char* post_listname, char *eqs_list)
    cdef int    IodeModelSimulateSCC(char *per_from, char *per_to, 
                                     char *pre_eqlist, char *inter_eqlist, char* post_eqlist,
                                     double eps, double relax, int maxit, 
                                     int init_values, int debug, 
                                     double newton_eps, int newton_maxit, int newton_debug)
    cdef int   KSIM_MAXIT 
    cdef float KSIM_EPS   
    cdef float KSIM_RELAX 
    cdef int   KSIM_PASSES
    cdef int   KSIM_SORT
    cdef int   KSIM_START 
    cdef int   KSIM_CPU_SCC
    cdef int   KSIM_CPU_SORT

    cdef float  IodeModelSimNorm(char* period)
    cdef int    IodeModelSimNIter(char* period)
    cdef int    IodeModelSimCpu(char* period)
    cdef int    IodeModelCpuSort()
    cdef int    IodeModelCpuSCC()
    
        
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

    # SCR4 functions & Vars
    cdef int    SCR_free_tbl(char **tbl)
    cdef int    SCR_free(void *ptr)
    cdef int    DebugActif

    # Message
    cdef void   IodeSuppressMsgs()
    cdef void   IodeResetMsgs()
    cdef void   IodeAddErrorMsg(char* msg)
    cdef void   IodeDisplayErrorMsgs()
    cdef void   IodeClearErrorMsgs()

    # TO Check
    cdef int    PyIodePrint(char*name, void* values, int lg)
    
    # Virtual (aka super) functions
    cdef void   ODE_assign_super_PYIODE()



# IODE OBJECT CLASS DECLARATIONS 
# ------------------------------
# Note : to be improved / replaced bu C++ classes when Cython 3 becomes stable

class Scalar:
    def __init__(self, name:str, val:float=0.9, relax:float=1.0, std:float=float("nan")):
        self.name = name
        #iodescl = iodescl_to_ndarray(cstr(name))
        self.value = val
        self.relax = relax
        self.std = std # or float("NaN") ?


    def __str__(self):
        return f'{self.name}=({self.value}, {self.relax}, {self.std})'

    def __repr__(self):
        return f'iode.scl("{self.name}", {self.value}, {self.relax}, {self.std})'


class Equation:
    def __init__(self, 
                 name:str,       
                 lec:str,        
                 method:int=0, 
                 sample_from:str="",
                 sample_to:str="",  
                 blk:str="",        
                 instr:str="",      
                 e_stdev :float=0.0, 
                 e_meany :float=0.0, 
                 e_ssres :float=0.0, 
                 e_stderr:float=0.0, 
                 e_fstat :float=0.0, 
                 e_r2    :float=0.0, 
                 e_r2adj :float=0.0, 
                 e_dw    :float=0.0, 
                 e_loglik:float=0.0  
                 ):
                 
        self.name = name
        self.lec = lec
        self.method = method
        self.sample_from = sample_from
        self.sample_to = sample_to
        self.blk      = blk
        self.instr    = instr
        self.e_stdev  = e_stdev
        self.e_meany  = e_meany 
        self.e_ssres  = e_ssres 
        self.e_stderr = e_stderr
        self.e_fstat  = e_fstat 
        self.e_r2     = e_r2    
        self.e_r2adj  = e_r2adj 
        self.e_dw     = e_dw    
        self.e_loglik = e_loglik

    def __str__(self):
        methods = ["LSQ", "ZELLNER", "INF", "GLS", "MAXLIK"];
        res =  f'''{self.name} {{
    "{self.lec}"'''
        
#       if self.sample_from != "":
#             res += f'   {methods[self.method]}\n'
#             res += f'   SAMPLE {self.sample_from}{self.sample_to}\n'
#             res += f'   BLOCK "{self.blk}"\n'
#             res += f'   INSTRUMENTS "{self.instr}"\n'
#             res += f'   STDEV  {self.e_stdev }\n'
#             res += f'   MEANY  {self.e_meany }\n'
#             res += f'   SSRES  {self.e_ssres }\n'
#             res += f'   STDERR {self.e_stderr}\n'
#             res += f'   FSTAT  {self.e_fstat }\n'
#             res += f'   R2     {self.e_r2    }\n'
#             res += f'   R2ADJ  {self.e_r2adj }\n'
#             res += f'   DW     {self.e_dw    }\n'
#             res += f'   LOGLIK {self.e_loglik}\n'
#  
#       res += f'}}\n'
#       return res

        if self.sample_from != "":
        
            res += f'''
    {methods[self.method]}
    SAMPLE {self.sample_from}{self.sample_to}
    BLOCK "{self.blk}"
    INSTRUMENTS "{self.instr}"
    STDEV  {self.e_stdev }
    MEANY  {self.e_meany }
    SSRES  {self.e_ssres }
    STDERR {self.e_stderr}
    FSTAT  {self.e_fstat }
    R2     {self.e_r2    }
    R2ADJ  {self.e_r2adj }
    DW     {self.e_dw    }
    LOGLIK {self.e_loglik}'''
    
        res += f'}}'
        return res
    

    def __repr__(self):
#        res =  f'iode.eqs("{self.name}", "{self.lec}", {self.method}, '
#        res += f'"{self.sample_from}", "{self.sample_to}", "{self.blk}", "{self.instr}", '
#        res += f'{self.e_stdev }, '
#        res += f'{self.e_meany }, '
#        res += f'{self.e_ssres }, '
#        res += f'{self.e_stderr}, '
#        res += f'{self.e_fstat }, '
#        res += f'{self.e_r2    }, '
#        res += f'{self.e_r2adj }, '
#        res += f'{self.e_dw    }, '
#        res += f'{self.e_loglik})'
#        return res
        res =  f'''iode.eqs("{self.name}", "{self.lec}", {self.method}, 
    "{self.sample_from}", "{self.sample_to}", "{self.blk}", "{self.instr}", '
    {self.e_stdev }, 
    {self.e_meany }, 
    {self.e_ssres }, 
    {self.e_stderr}, 
    {self.e_fstat }, 
    {self.e_r2    }, 
    {self.e_r2adj }, 
    {self.e_dw    }, 
    {self.e_loglik})'''
    
        return res


#class period:
#    def __init__(self, y, s, p):
#        self.p_y = y
#        self.p_s = s
#        self.p_p = p
#
#    def __str__(self):
#        return f"{self.p_y}{self.p_p}{self.p_s}"
#
#class sample:
#    def __init__(self, p1, p2):
#        self.s_p1 = p1
#        self.s_p2 = p2
#
#    def __str__(self):
#        return f"{self.s_p1}:{self.s_p2}"
# 

