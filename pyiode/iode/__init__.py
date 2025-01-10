from pathlib import Path

SAMPLE_DATA_DIR = str((Path(__file__).parent / "tests" / "data").resolve())

# Constants
from .iode_cython import (__version__, NA, is_NA, 
                          IodeType, IodeFileType, TableLang, ImportFormats, ExportFormats, EqMethod, EqTest, 
                          TableCellType, TableCellFont, TableCellAlign, TableLineType, TableGraphType, 
                          TableGraphGrid, TableGraphAlign, TableGraphAxis, VarsMode, LowToHighType, LowToHighMethod, 
                          HighToLowType, SimulationInitialization, SimulationSort, ESTIMATION_MAXIT, ESTIMATION_EPS, 
                          AdjustmentMethod, WriteFileExt)

from .common import (IODE_DATABASE_TYPE_NAMES, IODE_FILE_TYPE_NAMES, IODE_LANGUAGES_LIST, EQUATION_METHODS_LIST, 
                     EQ_TEST_NAMES, IODE_FILE_TYPES, SIMULATION_INITIALIZATION_METHODS, SIMULATION_SORT_ALGORITHMS, 
                     PRINT_FORMATS, IMPORT_FORMATS, EXPORT_FORMATS)

# time
from .iode_cython import Period, Sample, PERIODICITY_LIST

# IODE objects
# NOTE: didn't add TableLine/TableCell/ComputedTable to avoid users to create instances of these classes.
#       TableLine/TableCell/ComputedTable instances must only be created from a Table instance.
from .iode_cython import Equation, Identity, Scalar, Table
from .util import split_list

# IODE databases
from .iode_cython import comments, equations, identities, lists, scalars, tables, variables
from .iode_cython import Comments, Equations, Identities, Lists, Scalars, Tables, Variables
from .iode_cython import load_extra_files, reset_extra_files

# Reports
from .iode_cython import execute_report, execute_command

# Printing
from .iode_cython import (w_close, w_dest, w_flush, w_print, w_print_cmd, w_print_enum, w_print_par, 
                          w_print_pg_footer, w_print_pg_header, w_print_tit)

# Estimation - Simulation
from .iode_cython import Simulation, dynamic_adjustment, dickey_fuller_test 

# lec
from .iode_cython import execute_lec

# miscellaneous
from .iode_cython import add_error_msg, clear_error_msgs, display_error_msgs, reset_msgs, suppress_msgs
from .common import FileType
from .super import *

# NOTE: didn't add TableLine/TableCell/ComputedTable to avoid users to create instances of these classes.
#       TableLine/TableCell/ComputedTable instances must only be created from a Table instance. 
__all__ = [
    # Constants
    'SAMPLE_DATA_DIR', 'NA', 'is_NA', 
    'IodeType', 'IodeFileType', 'TableLang', 'ImportFormats', 'ExportFormats', 'EqMethod', 'EqTest', 
    'TableCellType', 'TableCellFont', 'TableCellAlign', 'TableLineType', 'TableGraphType', 'TableGraphGrid', 
    'TableGraphAlign', 'TableGraphAxis', 'VarsMode', 'LowToHighType', 'LowToHighMethod', 'HighToLowType', 
    'SimulationInitialization', 'SimulationSort', 'ESTIMATION_MAXIT', 'ESTIMATION_EPS', 'AdjustmentMethod', 
    'WriteFileExt', 'EQ_TEST_NAMES', 'IODE_DATABASE_TYPE_NAMES', 'IODE_FILE_TYPE_NAMES', 'IODE_LANGUAGES_LIST', 
    'EQUATION_METHODS_LIST', 'FileType', 'IODE_FILE_TYPES', 'SIMULATION_INITIALIZATION_METHODS', 
    'SIMULATION_SORT_ALGORITHMS', 'PRINT_FORMATS', 'IMPORT_FORMATS', 'EXPORT_FORMATS', 

    # time
    'Period', 'Sample', 'PERIODICITY_LIST',

    #IODE objects
    'Equation', 'Identity', 'Scalar', 'Table', 'split_list', 
    
    # IODE databases
    'comments', 'equations', 'identities', 'lists', 'scalars', 'tables', 'variables', 
    'Comments', 'Equations', 'Identities', 'Lists', 'Scalars', 'Tables', 'Variables', 
    'load_extra_files', 'reset_extra_files', 

    # Reports
    'execute_report', 'execute_command', 

    # Printing
    'w_close', 'w_dest', 'w_flush', 'w_print', 'w_print_cmd', 'w_print_enum', 'w_print_par', 'w_print_pg_footer', 
    'w_print_pg_header', 'w_print_tit', 

    # Estimation - Simulation
    'Simulation', 'dynamic_adjustment', 'dickey_fuller_test', 

    # lec
    'execute_lec', 

    # miscellaneous
    'add_error_msg', 'clear_error_msgs', 'display_error_msgs', 'reset_msgs', 'suppress_msgs', 
]

# ==== DEPRECATED API ====

from .deprecated import *
from .deprecated import _deprecated

__all__ += _deprecated
