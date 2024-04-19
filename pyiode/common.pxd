from libcpp.string cimport string
from libcpp.vector cimport vector


cdef extern from "api/iode.h":
    cdef double L_NAN
    cdef int I_NB_TYPES


cdef extern from "cpp_api/common.h":
    cdef enum EnumIodeType:
        I_COMMENTS,
        I_EQUATIONS,
        I_IDENTITIES,
        I_LISTS,
        I_SCALARS,
        I_TABLES,
        I_VARIABLES

    cdef vector[string] vIodeTypes

    cdef enum EnumLang:
        IT_ENGLISH,
        IT_DUTCH,
        IT_FRENCH

    cdef vector[string] vLangs

    cdef enum EnumSimulationInitialization:
        IV_INIT_TM1,
        IV_INIT_TM1_A,
        IV_INIT_EXTRA,
        IV_INIT_EXTRA_A,
        IV_INIT_ASIS,
        IV_INIT_TM1_NA,
        IV_INIT_EXTRA_NA

    cdef vector[string] v_simulation_initialization

cdef extern from "cpp_api/objects/equation.h":
    cdef enum EnumIodeEquationMethod:
        IE_LSQ,
        IE_ZELLNER,
        IE_INSTRUMENTAL,
        IE_GLS,
        IE_MAX_LIKELIHOOD

    cdef vector[string] v_eq_methods

    cdef enum EnumIodeEquationTest:
        IE_CORR,
        IE_STDEV,
        IE_MEANY, 
        IE_SSRES, 
        IE_STDERR,
        IE_STDERRP,
        IE_FSTAT, 
        IE_R2,
        IE_R2ADJ, 
        IE_DW,
        IE_LOGLIK
        
    cdef int EQS_NBTESTS

cdef extern from "cpp_api/objects/table.h":
    cdef enum EnumCellType:
        IT_LEC,
        IT_STRING

    cdef enum EnumCellFont:
        IT_NORMAL,
        IT_BOLD,
        IT_ITALIC,
        IT_UNDERLINE

    cdef enum EnumCellAlign:
        IT_CENTER,
        IT_LEFT,
        IT_RIGHT,
        IT_DECIMAL

    cdef enum EnumLineType:
        IT_FILES,
        IT_MODE,
        IT_CELL,
        IT_LINE,
        IT_TITLE,
        IT_DATE

    cdef enum EnumGraphType:
        IG_LINE,
        IG_SCATTER,
        IG_BAR

    cdef enum EnumGraphGrid:
        IG_MAJOR,
        IG_NONE,
        IG_MINOR

    cdef enum EnumGraphAlign:
        IG_LEFT,
        IG_CENTER,
        IG_RIGHT

    cdef enum EnumGraphAxis:
        IG_VALUES,
        IG_LOG,
        IG_SEMILOG,
        IG_PERCENT

cdef extern from "cpp_api/KDB/kdb_variables.h":
    cdef enum EnumIodeVarMode:
        I_VAR_MODE_LEVEL,
        I_VAR_MODE_DIFF,
        I_VAR_MODE_GROWTH_RATE,
        I_VAR_MODE_Y0Y_DIFF,
        I_VAR_MODE_Y0Y_GROWTH_RATE

    cdef vector[string] v_var_modes
    
