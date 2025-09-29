from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.map cimport map


cdef extern from "api/all.h":
    cdef double IODE_NAN
    cdef int IODE_NB_TYPES
    cdef char* I_DEFAULT_FILENAME

    cdef enum IodeType:
        COMMENTS,
        EQUATIONS,
        IDENTITIES,
        LISTS,
        SCALARS,
        TABLES,
        VARIABLES

    cdef enum IodeExportFormat:
        EXPORT_CSV,
        EXPORT_DIF,
        EXPORT_WKS,
        EXPORT_TSP,
        EXPORT_RCSV

    cdef enum IodeImportFormat:
        IMPORT_ASCII,
        IMPORT_ROT_ASCII,
        IMPORT_DIF,
        IMPORT_BISTEL,
        IMPORT_NIS,
        IMPORT_GEM,
        IMPORT_PRN,
        IMPORT_TXT

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

    cdef enum TableLang:
        TABLE_ENGLISH,
        TABLE_DUTCH,
        TABLE_FRENCH

    cdef vector[string] v_table_langs

cdef extern from "api/all.h":
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

cdef extern from "api/all.h":
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

cdef extern from "api/objs/equations.h":
    cdef vector[string] v_eq_methods

cdef extern from "api/objs/tables.h":
    cdef enum TableCellType:
        TABLE_CELL_LEC,
        TABLE_CELL_STRING

    cdef enum TableCellFont:
        TABLE_CELL_NORMAL,
        TABLE_CELL_BOLD,
        TABLE_CELL_ITALIC,
        TABLE_CELL_UNDERLINE

    cdef enum TableCellAlign:
        TABLE_CELL_CENTER,
        TABLE_CELL_LEFT,
        TABLE_CELL_RIGHT,
        TABLE_CELL_DECIMAL

    cdef enum TableLineType:
        TABLE_LINE_FILES,
        TABLE_LINE_MODE,
        TABLE_LINE_CELL,
        TABLE_LINE_SEP,
        TABLE_LINE_TITLE,
        TABLE_LINE_DATE

    cdef enum TableGraphType:
        TABLE_GRAPH_LINE,
        TABLE_GRAPH_SCATTER,
        TABLE_GRAPH_BAR

    cdef enum TableGraphGrid:
        TABLE_GRAPH_MAJOR,
        TABLE_GRAPH_NONE,
        TABLE_GRAPH_MINOR

    cdef enum TableTextAlign:
        TABLE_TEXT_LEFT,
        TABLE_TEXT_CENTER,
        TABLE_TEXT_RIGHT

    cdef enum TableGraphAxis:
        TABLE_GRAPH_VALUES,
        TABLE_GRAPH_LOG,
        TABLE_GRAPH_SEMILOG,
        TABLE_GRAPH_PERCENT

cdef extern from "api/all.h":
    cdef enum IodeLowToHigh:
        LTOH_STOCK,
        LTOH_FLOW

    cdef enum IodeHighToLow:
        HTOL_LAST,
        HTOL_MEAN,
        HTOL_SUM 

cdef extern from "cpp_api/KDB/kdb_global.h":
    cdef map[char, string] mLowToHigh

cdef extern from "api/all.h":
    cdef enum IodeVarMode:
        VAR_MODE_LEVEL,
        VAR_MODE_DIFF,
        VAR_MODE_GROWTH_RATE,
        VAR_MODE_Y0Y_DIFF,
        VAR_MODE_Y0Y_GROWTH_RATE

cdef extern from "cpp_api/KDB/kdb_variables.h":
    cdef vector[string] v_var_modes
    
cdef extern from "api/all.h":
    cdef enum SimuSortAlgorithm:
        SORT_CONNEX,
        SORT_BOTH,
        SORT_NONE

    cdef enum IodeAdjustmentMethod:
        AM_PARTIAL_ADJUSTMENT,
        AM_ERROR_CORRECTION_METHOD

cdef extern from "cpp_api/compute/simulation.h":
    cdef vector[string] v_simulation_sort_algorithm

cdef extern from "cpp_api/compute/estimation.h":
    cdef vector[string] v_adjustment_method
    cdef int    ESTIMATION_MAXIT
    cdef double ESTIMATION_EPS
