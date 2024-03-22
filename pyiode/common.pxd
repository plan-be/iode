from libcpp.string cimport string
from libcpp.vector cimport vector


cdef extern from "iode.h":
    cdef double L_NAN
    cdef int I_NB_TYPES


cdef extern from "common.h":
    cdef enum EnumIodeType:
        I_COMMENTS,
        I_EQUATIONS,
        I_IDENTITIES,
        I_LISTS,
        I_SCALARS,
        I_TABLES,
        I_VARIABLES

    cdef vector[string] vIodeTypes

    cdef enum EnumSimulationInitialization:
        IV_INIT_TM1,
        IV_INIT_TM1_A,
        IV_INIT_EXTRA,
        IV_INIT_EXTRA_A,
        IV_INIT_ASIS,
        IV_INIT_TM1_NA,
        IV_INIT_EXTRA_NA

    cdef vector[string] v_simulation_initialization

cdef extern from "KDB/kdb_variables.h":
    cdef enum EnumIodeVarMode:
        I_VAR_MODE_LEVEL,
        I_VAR_MODE_DIFF,
        I_VAR_MODE_GROWTH_RATE,
        I_VAR_MODE_Y0Y_DIFF,
        I_VAR_MODE_Y0Y_GROWTH_RATE

    cdef vector[string] v_var_modes
    
