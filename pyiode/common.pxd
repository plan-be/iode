from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.map cimport map


cdef extern from "api/iode.h":
    cdef double IODE_NAN
    cdef int IODE_NB_TYPES

    cdef enum IodeDatabaseType:
        COMMENTS,
        EQUATIONS,
        IDENTITIES,
        LISTS,
        SCALARS,
        TABLES,
        VARIABLES

cdef extern from "cpp_api/common.h":
    cdef vector[string] v_iode_types

    cdef enum IodeFileType:
        FILE_ASCII,
        FILE_COMMENTS,
        FILE_EQUATIONS,
        FILE_IDENTITIES,
        FILE_LISTS,
        FILE_SCALARS,
        FILE_TABLES,
        FILE_VARIABLES,
        ASCII_COMMENTS,
        ASCII_EQUATIONS,
        ASCII_IDENTITIES,
        ASCII_LISTS,
        ASCII_SCALARS,
        ASCII_TABLES,
        ASCII_VARIABLES,
        FILE_REP,
        FILE_A2M,
        FILE_AGL,
        FILE_PRF,
        FILE_DIF,
        FILE_MIF,
        FILE_RTF,
        FILE_PS ,
        FILE_AAS,
        FILE_TXT,
        FILE_CSV,
        FILE_HTML,
        FILE_REF,
        FILE_LOG,
        FILE_SETTINGS,
        FILE_ANY,
        DIRECTORY

    cdef struct CFileType "FileType":
        string name
        vector[string] v_ext
        CFileType(string& name, vector[string]& v_vec)

    cdef vector[CFileType] v_file_types

    cdef enum EnumLang:
        IT_ENGLISH,
        IT_DUTCH,
        IT_FRENCH

    cdef vector[string] vLangs

cdef extern from "api/iode.h":
    cdef enum VariablesInitialization:
        VAR_INIT_TM1,
        VAR_INIT_TM1_A,
        VAR_INIT_EXTRA,
        VAR_INIT_EXTRA_A,
        VAR_INIT_ASIS,
        VAR_INIT_TM1_NA,
        VAR_INIT_EXTRA_NA

cdef extern from "cpp_api/common.h":
    cdef vector[string] v_simulation_initialization

cdef extern from "api/iode.h":
    cdef enum IodeEquationMethod:
        EQ_LSQ,
        EQ_ZELLNER,
        EQ_INSTRUMENTAL,
        EQ_GLS,
        EQ_MAX_LIKELIHOOD

    cdef enum IodeEquationTest:
        EQ_CORR,
        EQ_STDEV,
        EQ_MEANY, 
        EQ_SSRES, 
        EQ_STDERR,
        EQ_STDERRP,
        EQ_FSTAT,
        EQ_R2,
        EQ_R2ADJ, 
        EQ_DW,
        EQ_LOGLIK

    cdef int EQS_NBTESTS

cdef extern from "cpp_api/objects/equation.h":
    cdef vector[string] v_eq_methods

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

cdef extern from "api/iode.h":
    cdef enum IodeLowToHigh:
        LTOH_STOCK,
        LTOH_FLOW

    cdef enum IodeHighToLow:
        HTOL_LAST,
        HTOL_MEAN,
        HTOL_SUM 

cdef extern from "cpp_api/KDB/kdb_global.h":
    cdef map[char, string] mLowToHigh

cdef extern from "api/iode.h":
    cdef enum IodeVarMode:
        VAR_MODE_LEVEL,
        VAR_MODE_DIFF,
        VAR_MODE_GROWTH_RATE,
        VAR_MODE_Y0Y_DIFF,
        VAR_MODE_Y0Y_GROWTH_RATE

cdef extern from "cpp_api/KDB/kdb_variables.h":
    cdef vector[string] v_var_modes
    
cdef extern from "api/iode.h":
    cdef enum SimuSortAlgorithm:
        SORT_CONNEX,
        SORT_BOTH,
        SORT_NONE

cdef extern from "cpp_api/compute/simulation.h":
    cdef vector[string] v_simulation_sort_algorithm

cdef extern from "cpp_api/compute/estimation.h":
    cdef enum EnumIodeAdjustmentMethod:
        AM_PARTIAL_ADJUSTMENT,
        AM_ERROR_CORRECTION_METHOD

    cdef vector[string] v_adjustment_method
    cdef int    ESTIMATION_MAXIT
    cdef double ESTIMATION_EPS
