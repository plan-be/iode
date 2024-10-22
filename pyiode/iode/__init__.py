from pathlib import Path

SAMPLE_DATA_DIR = str((Path(__file__).parent / "tests" / "data").resolve())

# Constants
from .iode_python import (__version__, NA, is_NA, _IODE_DEFAULT_DATABASE_FILENAME, 
                          IodeTypes, IodeFileType, TableLang, EqMethod, EqTest, TableCellType, TableCellFont, TableCellAlign, 
                          TableLineType, TableGraphType, TableGraphGrid, TableGraphAlign, TableGraphAxis, 
                          VarsMode, LowToHighType, LowToHighMethod, HighToLowType, SimulationInitialization, SimulationSort, 
                          ESTIMATION_MAXIT, ESTIMATION_EPS, AdjustmentMethod, WriteFileExt, EQ_TEST_NAMES, IODE_DATABASE_TYPE_NAMES, 
                          IODE_FILE_TYPE_NAMES, IODE_LANGUAGES_LIST, EQUATION_METHODS_LIST, FileType, IODE_FILE_TYPES)

# time
from .iode_python import Period, Sample, periodicities

# IODE objects
# NOTE: didn't add TableLine/TableCell/ComputedTable to avoid users to create instances of these classes.
#       TableLine/TableCell/ComputedTable instances must only be created from a Table instance.
from .iode_python import Equation, Identity, Scalar, Table
from .util import split_list

# IODE databases
from .iode_python import comments, equations, identities, lists, scalars, tables, variables
from .iode_python import Comments, Equations, Identities, Lists, Scalars, Tables, Variables
from .iode_python import load_extra_files

# Reports
from .iode_python import execute_report, execute_command
from .iode_python import _build_command_functions_list, _build_report_functions_list, _build_lec_functions_list

# Printing
from .iode_python import (w_close, w_dest, w_flush, w_print, w_print_cmd, w_print_enum, w_print_par, 
                          w_print_pg_footer, w_print_pg_header, w_print_tit)

# Estimation - Simulation
from .iode_python import Simulation, dynamic_adjustment, dickey_fuller_test 

# lec
from .iode_python import execute_lec

# miscellaneous
from .iode_python import add_error_msg, clear_error_msgs, display_error_msgs, reset_msgs, suppress_msgs


# NOTE: didn't add TableLine/TableCell/ComputedTable to avoid users to create instances of these classes.
#       TableLine/TableCell/ComputedTable instances must only be created from a Table instance. 
__all__ = [
    # Constants
    'SAMPLE_DATA_DIR', 'NA', 'is_NA', 
    'IodeTypes', 'IodeFileType', 'TableLang', 'EqMethod', 'EqTest', 'TableCellType', 'TableCellFont', 'TableCellAlign', 
    'TableLineType', 'TableGraphType', 'TableGraphGrid', 'TableGraphAlign', 'TableGraphAxis', 
    'VarsMode', 'LowToHighType', 'LowToHighMethod', 'HighToLowType', 'SimulationInitialization', 'SimulationSort', 
    'ESTIMATION_MAXIT', 'ESTIMATION_EPS', 'AdjustmentMethod', 'WriteFileExt', 'EQ_TEST_NAMES', 'IODE_DATABASE_TYPE_NAMES', 
    'IODE_FILE_TYPE_NAMES', 'IODE_LANGUAGES_LIST', 'EQUATION_METHODS_LIST', 'FileType', 'IODE_FILE_TYPES', 

    # time
    'Period', 'Sample', 'periodicities',

    #IODE objects
    'Equation', 'Identity', 'Scalar', 'Table', 'split_list', 
    
    # IODE databases
    'comments', 'equations', 'identities', 'lists', 'scalars', 'tables', 'variables', 
    'Comments', 'Equations', 'Identities', 'Lists', 'Scalars', 'Tables', 'Variables', 
    'load_extra_files', 

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

# ==== ONLY FOR GUI ====
from .util import table2str, _check_filepath, _get_iode_file_type
from .iode_python import (_iode_msg_path, _print_error_msg, _build_command_functions_list, 
                          _build_report_functions_list, _build_lec_functions_list)
from .iode_python import EditAndEstimateEquations

# ==== DEPRECATED API ====

from .deprecated import *
from .deprecated import _deprecated

__all__ += _deprecated
