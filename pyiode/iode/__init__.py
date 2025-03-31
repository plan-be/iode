from pathlib import Path

SAMPLE_DATA_DIR = str((Path(__file__).parent / "tests" / "data").resolve())
DOC_DIR = str((Path(__file__).parent / "doc").resolve())

# Constants
from .iode_cython import (__version__, NA, is_NA, 
                          IodeType, IodeFileType, TableLang, ImportFormats, ExportFormats, EqMethod, EqTest, 
                          TableCellType, TableCellFont, TableCellAlign, TableLineType, TableGraphType, 
                          TableGraphGrid, TableGraphAlign, TableGraphAxis, VarsMode, LowToHighType, LowToHighMethod, 
                          HighToLowType, SimulationInitialization, SimulationSort, ESTIMATION_MAXIT, ESTIMATION_EPS, 
                          AdjustmentMethod, WriteFileExt)

from .common import (IODE_DATABASE_TYPE_NAMES, IODE_FILE_TYPE_NAMES, IODE_LANGUAGES_LIST, EQUATION_METHODS_LIST, 
                     EQ_TEST_NAMES, IODE_FILE_TYPES, SIMULATION_INITIALIZATION_METHODS, SIMULATION_SORT_ALGORITHMS, 
                     PRINT_FORMATS, IMPORT_FORMATS, EXPORT_FORMATS, GRAPHS_COLORS, IODE_FONTS, PrintTablesAs, 
                     PrintEquationsAs, PrintEquationsLecAs)

# time
from .iode_cython import Period, Sample, PERIODICITY_LIST

# IODE objects
# NOTE: didn't add TableLine/TableCell/ComputedTable to avoid users to create instances of these classes.
#       TableLine/TableCell/ComputedTable instances must only be created from a Table instance.
from .iode_cython import Scalar, Table
from .objects.equation import Equation
from .objects.identity import Identity
from .util import split_list

# IODE databases
from .iode_database.extra_files import load_extra_files, reset_extra_files
from .iode_database.comments_database import Comments, comments
from .iode_database.equations_database import Equations, equations
from .iode_database.identities_database import Identities, identities
from .iode_database.lists_database import Lists, lists
from .iode_database.scalars_database import Scalars, scalars
from .iode_database.tables_database import Tables, tables
from .iode_database.variables_database import Variables, variables

# Reports
from .iode_cython import execute_report, execute_command

# Printing
from .iode_cython import (w_close, w_dest, w_flush, w_print, w_print_cmd, w_print_enum, w_print_par, 
                          w_print_pg_footer, w_print_pg_header, w_print_tit)

# Estimation - Simulation
from .iode_cython import Simulation, dynamic_adjustment, dickey_fuller_test 

# lec
from .iode_cython import execute_lec

# gui
from .gui import view_workspace

# miscellaneous
from .iode_cython import reset_msgs, suppress_msgs
from .common import FileType
from .super import *

# NOTE: didn't add TableLine/TableCell/ComputedTable to avoid users to create instances of these classes.
#       TableLine/TableCell/ComputedTable instances must only be created from a Table instance. 
__all__ = [
    # Constants
    'SAMPLE_DATA_DIR', 'DOC_DIR', 'NA', 'is_NA', 
    'IodeType', 'IodeFileType', 'TableLang', 'ImportFormats', 'ExportFormats', 'EqMethod', 'EqTest', 
    'TableCellType', 'TableCellFont', 'TableCellAlign', 'TableLineType', 'TableGraphType', 'TableGraphGrid', 
    'TableGraphAlign', 'TableGraphAxis', 'VarsMode', 'LowToHighType', 'LowToHighMethod', 'HighToLowType', 
    'SimulationInitialization', 'SimulationSort', 'ESTIMATION_MAXIT', 'ESTIMATION_EPS', 'AdjustmentMethod', 
    'WriteFileExt', 'EQ_TEST_NAMES', 'IODE_DATABASE_TYPE_NAMES', 'IODE_FILE_TYPE_NAMES', 'IODE_LANGUAGES_LIST', 
    'EQUATION_METHODS_LIST', 'FileType', 'IODE_FILE_TYPES', 'SIMULATION_INITIALIZATION_METHODS', 
    'SIMULATION_SORT_ALGORITHMS', 'PRINT_FORMATS', 'IMPORT_FORMATS', 'EXPORT_FORMATS', 'GRAPHS_COLORS', 
    'IODE_FONTS', 'PrintTablesAs', 'PrintEquationsAs', 'PrintEquationsLecAs', 

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

    # gui
    'view_workspace', 

    # miscellaneous
    'reset_msgs', 'suppress_msgs', 
]

# ==== DEPRECATED API ====

from .deprecated import *
from .deprecated import _deprecated

__all__ += _deprecated
