from pathlib import Path

SAMPLE_DATA_DIR = str((Path(__file__).parent / "tests" / "data").resolve())

# Constants
from .iode_python import (__version__, NA, is_NA, 
                          IodeTypes, IodeFileType, IodeLanguage, EqMethod, EqTest, TableCellType, TableCellFont, TableCellAlign, 
                          TableLineType, TableLineGraph, TableGraphGrid, TableGraphAlign, TableGraphAxis, 
                          VarsMode, LowToHighType, LowToHighMethod, HighToLowType, SimulationInitialization, SimulationSort, 
                          ESTIMATION_MAXIT, ESTIMATION_EPS, AdjustmentMethod, WriteFileExt, EQ_TEST_NAMES)

# time
from .iode_python import Period, Sample, periodicities

# IODE objects
# NOTE: didn't add TableLine/TableCell/ComputedTable to avoid users to create instances of these classes.
#       TableLine/TableCell/ComputedTable instances must only be created from a Table instance.
from .iode_python import Equation, List, Scalar, Table
from .util import split_list

# IODE databases
from .iode_python import comments, equations, identities, lists, scalars, tables, variables
from .iode_python import Comments, Equations, Identities, Lists, Scalars, Tables, Variables

# Reports
from .iode_python import execute_report, execute_command

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
    'IodeTypes', 'IodeFileType', 'IodeLanguage', 'EqMethod', 'EqTest', 'TableCellType', 'TableCellFont', 'TableCellAlign', 
    'TableLineType', 'TableLineGraph', 'TableGraphGrid', 'TableGraphAlign', 'TableGraphAxis', 
    'VarsMode', 'LowToHighType', 'LowToHighMethod', 'HighToLowType', 'SimulationInitialization', 'SimulationSort', 
    'ESTIMATION_MAXIT', 'ESTIMATION_EPS', 'AdjustmentMethod', 'WriteFileExt', 'EQ_TEST_NAMES', 

    # time
    'Period', 'Sample', 'periodicities',

    #IODE objects
    'Equation', 'List', 'Scalar', 'Table', 'split_list', 
    
    # IODE databases
    'comments', 'equations', 'identities', 'lists', 'scalars', 'tables', 'variables', 
    'Comments', 'Equations', 'Identities', 'Lists', 'Scalars', 'Tables', 'Variables', 

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
