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
                     PRINT_FORMATS, IMPORT_FORMATS, EXPORT_FORMATS, GRAPHS_COLORS, IODE_FONTS, PERIODICITY_LIST, 
                     WRITE_FILE_EXT, PrintTablesAs, PrintEquationsAs, PrintEquationsLecAs)

# time
from .time.period import Period 
from .time.sample import Sample

# IODE objects
# NOTE: didn't add TableLine/TableCell/ComputedTable to avoid users to create instances of these classes.
#       TableLine/TableCell/ComputedTable instances must only be created from a Table instance.
from .objects.equation import Equation
from .objects.identity import Identity
from .objects.scalar import Scalar
from .objects.table import Table
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
from .reports import execute_report, execute_command

# Printing
from .write import (write_close, write_destination, write_flush, write, write_code_block, write_enum, 
                    write_paragraph, write_title, write_page_footer, write_page_header)

# Simulation
from .compute.simulation import Simulation

# Estimation
from .compute.estimation import dynamic_adjustment, dickey_fuller_test 

# lec
from .lec import execute_lec

# gui
from .gui import view_workspace

# miscellaneous
from .iode_cython import enable_msgs, suppress_msgs
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
    'WriteFileExt', 'WRITE_FILE_EXT', 'EQ_TEST_NAMES', 'IODE_DATABASE_TYPE_NAMES', 'IODE_FILE_TYPE_NAMES', 
    'IODE_LANGUAGES_LIST', 'EQUATION_METHODS_LIST', 'FileType', 'IODE_FILE_TYPES', 
    'SIMULATION_INITIALIZATION_METHODS', 'SIMULATION_SORT_ALGORITHMS', 'PRINT_FORMATS', 'IMPORT_FORMATS', 
    'EXPORT_FORMATS', 'GRAPHS_COLORS', 'IODE_FONTS', 'PrintTablesAs', 'PrintEquationsAs', 'PrintEquationsLecAs', 

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
    'write_close', 'write_destination', 'write_flush', 'write', 'write_code_block', 'write_enum', 
    'write_paragraph', 'write_title', 'write_page_footer', 'write_page_header', 

    # Simulation
    'Simulation', 

    # Estimation
    'dynamic_adjustment', 'dickey_fuller_test', 

    # lec
    'execute_lec', 

    # gui
    'view_workspace', 

    # miscellaneous
    'enable_msgs', 'suppress_msgs', 
]

# ==== DEPRECATED API ====

from .deprecated import *
from .deprecated import _deprecated

__all__ += _deprecated
