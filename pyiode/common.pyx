# distutils: language = c++

from typing import List, Dict, Any
import sys
if sys.version_info.minor >= 11:
    from enum import IntEnum, StrEnum
else:
    from enum import Enum, IntEnum
    StrEnum = Enum

from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.map cimport map

from pyiode.common cimport IODE_NAN, IODE_NB_TYPES, EQS_NBTESTS, I_DEFAULT_FILENAME
from pyiode.common cimport ESTIMATION_MAXIT as C_ESTIMATION_MAXIT 
from pyiode.common cimport ESTIMATION_EPS as C_ESTIMATION_EPS
from pyiode.common cimport (IodeEquationMethod, IodeEquationTest, 
                            IodeLowToHigh, IodeHighToLow, 
                            IodeVarMode, VariablesInitialization, 
                            SimuSortAlgorithm, IodeAdjustmentMethod)
from pyiode.common cimport IodeType as CIodeType
from pyiode.common cimport IodeImportFormat as CIodeImportFormat 
from pyiode.common cimport IodeExportFormat as CIodeExportFormat 
from pyiode.common cimport IodeFileType as CIodeFileType
from pyiode.common cimport TableLang as CTableLang
from pyiode.common cimport TableCellType as CTableCellType 
from pyiode.common cimport TableCellFont as CTableCellFont
from pyiode.common cimport TableCellAlign as CTableCellAlign 
from pyiode.common cimport TableLineType as CTableLineType
from pyiode.common cimport TableGraphAlign as CTableGraphAlign 
from pyiode.common cimport TableGraphAxis as CTableGraphAxis 
from pyiode.common cimport TableGraphGrid as CTableGraphGrid 
from pyiode.common cimport TableGraphType as CTableGraphType
from pyiode.common cimport CFileType
from pyiode.common cimport (v_iode_types, v_file_types, v_table_langs, v_eq_methods, v_var_modes, 
                            v_simulation_initialization, v_simulation_sort_algorithm, 
                            v_adjustment_method, mLowToHigh)

try:
    import larray as la
    Array = la.Array
    Axis = la.Axis
except ImportError:
    la = None
    Array = Any
    Axis = Any


# The function below exists because Cython seems to refuse to iterate over 
# a const std container except in a function or method
def _cpp_string_vector_to_py(vector[string] vec) -> List[str]:
    return [value.decode() for value in vec]

# The function below exists because Cython seems to refuse to iterate over 
# a const std container except in a function or method
def _cpp_char_string_map_to_py(map[char, string] c_map) -> Dict[str, str]:
    return {chr(item.first): item.second.decode() for item in c_map}


# IODE CONSTANTS
# --------------
NA: float = IODE_NAN
NAN_REP: str = "--"

IODE_DEFAULT_DATABASE_FILENAME: str = I_DEFAULT_FILENAME.decode('utf-8')


# IODE ENUMS
# ----------

class IodeType(IntEnum):
    COMMENTS = CIodeType.COMMENTS
    EQUATIONS = CIodeType.EQUATIONS
    IDENTITIES = CIodeType.IDENTITIES
    LISTS = CIodeType.LISTS
    SCALARS = CIodeType.SCALARS
    TABLES = CIodeType.TABLES
    VARIABLES = CIodeType.VARIABLES

class IodeFileType(IntEnum):
    FILE_COMMENTS = CIodeFileType.FILE_COMMENTS
    FILE_EQUATIONS = CIodeFileType.FILE_EQUATIONS
    FILE_IDENTITIES = CIodeFileType.FILE_IDENTITIES
    FILE_LISTS = CIodeFileType.FILE_LISTS
    FILE_SCALARS = CIodeFileType.FILE_SCALARS
    FILE_TABLES = CIodeFileType.FILE_TABLES
    FILE_VARIABLES = CIodeFileType.FILE_VARIABLES
    FILE_REP = CIodeFileType.FILE_REP
    FILE_A2M = CIodeFileType.FILE_A2M
    FILE_AGL = CIodeFileType.FILE_AGL
    FILE_PRF = CIodeFileType.FILE_PRF
    FILE_DIF = CIodeFileType.FILE_DIF
    FILE_MIF = CIodeFileType.FILE_MIF
    FILE_RTF = CIodeFileType.FILE_RTF
    FILE_PS = CIodeFileType.FILE_PS,
    FILE_AAS = CIodeFileType.FILE_AAS
    FILE_TXT = CIodeFileType.FILE_TXT
    FILE_CSV = CIodeFileType.FILE_CSV
    FILE_HTML = CIodeFileType.FILE_HTML
    FILE_REF = CIodeFileType.FILE_REF
    FILE_LOG = CIodeFileType.FILE_LOG
    FILE_SETTINGS = CIodeFileType.FILE_SETTINGS
    FILE_ANY = CIodeFileType.FILE_ANY
    DIRECTORY = CIodeFileType.DIRECTORY

class TableLang(IntEnum):
    ENGLISH = CTableLang.TABLE_ENGLISH
    DUTCH = CTableLang.TABLE_DUTCH
    FRENCH = CTableLang.TABLE_FRENCH

class ImportFormats(IntEnum):
    ASCII = CIodeImportFormat.IMPORT_ASCII
    ROT_ASCII = CIodeImportFormat.IMPORT_ROT_ASCII
    DIF = CIodeImportFormat.IMPORT_DIF
    BISTEL = CIodeImportFormat.IMPORT_BISTEL
    NIS = CIodeImportFormat.IMPORT_NIS
    GEM = CIodeImportFormat.IMPORT_GEM
    PRN = CIodeImportFormat.IMPORT_PRN
    TXT = CIodeImportFormat.IMPORT_TXT

class ExportFormats(IntEnum):
    CSV = CIodeExportFormat.EXPORT_CSV
    DIF = CIodeExportFormat.EXPORT_DIF
    WKS = CIodeExportFormat.EXPORT_WKS
    TSP = CIodeExportFormat.EXPORT_TSP
    RCSV = CIodeExportFormat.EXPORT_RCSV

class EqMethod(IntEnum):
    LSQ = 0
    ZELLNER = 1
    INSTRUMENTAL = 2
    GLS = 3
    MAX_LIKELIHOOD = 4

class EqTest(IntEnum):
    CORR = IodeEquationTest.EQ_CORR
    STDEV = IodeEquationTest.EQ_STDEV
    MEANY = IodeEquationTest.EQ_MEANY
    SSRES = IodeEquationTest.EQ_SSRES
    STDERR = IodeEquationTest.EQ_STDERR
    STDERRP = IodeEquationTest.EQ_STDERRP
    FSTAT = IodeEquationTest.EQ_FSTAT
    R2 = IodeEquationTest.EQ_R2
    R2ADJ = IodeEquationTest.EQ_R2ADJ
    DW = IodeEquationTest.EQ_DW
    LOGLIK = IodeEquationTest.EQ_LOGLIK

class TableCellType(IntEnum):
    LEC = CTableCellType.TABLE_CELL_LEC
    STRING = CTableCellType.TABLE_CELL_STRING

class TableCellFont(IntEnum):
    NORMAL = CTableCellFont.TABLE_CELL_NORMAL
    BOLD = CTableCellFont.TABLE_CELL_BOLD
    ITALIC = CTableCellFont.TABLE_CELL_ITALIC
    UNDERLINE = CTableCellFont.TABLE_CELL_UNDERLINE

class TableCellAlign(IntEnum):
    CENTER = CTableCellAlign.TABLE_CELL_CENTER
    LEFT = CTableCellAlign.TABLE_CELL_LEFT
    RIGHT = CTableCellAlign.TABLE_CELL_RIGHT
    DECIMAL = CTableCellAlign.TABLE_CELL_DECIMAL

class TableLineType(IntEnum):
    FILES = CTableLineType.TABLE_LINE_FILES
    MODE = CTableLineType.TABLE_LINE_MODE
    CELL = CTableLineType.TABLE_LINE_CELL
    SEP = CTableLineType.TABLE_LINE_SEP
    TITLE = CTableLineType.TABLE_LINE_TITLE
    DATE = CTableLineType.TABLE_LINE_DATE

class TableGraphType(IntEnum):
    LINE = CTableGraphType.TABLE_GRAPH_LINE
    SCATTER = CTableGraphType.TABLE_GRAPH_SCATTER
    BAR = CTableGraphType.TABLE_GRAPH_BAR

class TableGraphGrid(IntEnum):
    MAJOR = CTableGraphGrid.TABLE_GRAPH_MAJOR
    NONE = CTableGraphGrid.TABLE_GRAPH_NONE
    MINOR = CTableGraphGrid.TABLE_GRAPH_MINOR

class TableGraphAlign(IntEnum):
    LEFT = CTableGraphAlign.TABLE_GRAPH_LEFT
    CENTER = CTableGraphAlign.TABLE_GRAPH_CENTER
    RIGHT = CTableGraphAlign.TABLE_GRAPH_RIGHT

class TableGraphAxis(IntEnum):
    VALUES = CTableGraphAxis.TABLE_GRAPH_VALUES
    LOG = CTableGraphAxis.TABLE_GRAPH_LOG
    SEMILOG = CTableGraphAxis.TABLE_GRAPH_SEMILOG
    PERCENT = CTableGraphAxis.TABLE_GRAPH_PERCENT

class VarsMode(IntEnum):
    LEVEL = IodeVarMode.VAR_MODE_LEVEL
    DIFF = IodeVarMode.VAR_MODE_DIFF
    GROWTH_RATE = IodeVarMode.VAR_MODE_GROWTH_RATE
    Y0Y_DIFF = IodeVarMode.VAR_MODE_Y0Y_DIFF
    Y0Y_GROWTH_RATE = IodeVarMode.VAR_MODE_Y0Y_GROWTH_RATE

class LowToHighType(IntEnum):
    STOCK = IodeLowToHigh.LTOH_STOCK
    FLOW = IodeLowToHigh.LTOH_FLOW

class LowToHighMethod(StrEnum):
    LINEAR = 'L'
    CUBIC_SPLINES = 'C'
    STEP = 'S'

class HighToLowType(IntEnum):
    LAST = IodeHighToLow.HTOL_LAST
    MEAN = IodeHighToLow.HTOL_MEAN
    SUM = IodeHighToLow.HTOL_SUM

class SimulationSort(IntEnum):
    CONNEX = SimuSortAlgorithm.SORT_CONNEX
    BOTH = SimuSortAlgorithm.SORT_BOTH
    NONE = SimuSortAlgorithm.SORT_NONE

class SimulationInitialization(IntEnum):
    TM1 = VariablesInitialization.VAR_INIT_TM1
    TM1_A = VariablesInitialization.VAR_INIT_TM1_A
    EXTRA = VariablesInitialization.VAR_INIT_EXTRA
    EXTRA_A = VariablesInitialization.VAR_INIT_EXTRA_A
    ASIS = VariablesInitialization.VAR_INIT_ASIS
    TM1_NA = VariablesInitialization.VAR_INIT_TM1_NA
    EXTRA_NA = VariablesInitialization.VAR_INIT_EXTRA_NA

ESTIMATION_MAXIT = C_ESTIMATION_MAXIT 
ESTIMATION_EPS: float = C_ESTIMATION_EPS

class AdjustmentMethod(IntEnum):
    PARTIAL = IodeAdjustmentMethod.AM_PARTIAL_ADJUSTMENT
    ERROR_CORRECTION = IodeAdjustmentMethod.AM_ERROR_CORRECTION_METHOD

class WriteFileExt(IntEnum):
    GDI = 1
    A2M = 7
    MIF = 6
    HTML = 5
    RTF = 4
    CSV = 8
    DUMMY = 9

# Miscellaneous
# -------------

def _get_file_types() -> List[str, List[str]]:
    """Returns a list of tuples containing the name and extensions of each file type."""
    cdef size_t i
    cdef CFileType file_type
    py_file_types_list: List[str, List[str]] = []
    for i in range(v_file_types.size()):
        file_type = v_file_types[i]
        name: str = file_type.name.decode('utf-8')
        extensions: List[str] = [ext.decode('utf-8') for ext in file_type.v_ext]
        py_file_types_list.append((name, extensions))
    return py_file_types_list
