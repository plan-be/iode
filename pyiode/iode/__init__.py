from pathlib import Path

SAMPLE_DATA_DIR = str((Path(__file__).parent / "tests" / "data").resolve())

# Constants
from .iode_python import (nan, __version__, 
                          COMMENTS, EQUATIONS, IDENTITIES, LISTS, SCALARS, TABLES, VARIABLES, IODE_TYPES_LIST, NB_TYPES, 
                          FILE_COMMENTS, FILE_EQUATIONS, FILE_IDENTITIES, FILE_LISTS, FILE_SCALARS, FILE_TABLES, FILE_VARIABLES, 
                          FILE_REPORTS, FILE_TEXT, FILE_ASCII, FILE_A2M, FILE_RTF, FILE_HTML, FILE_MIF, FILE_CSV, FILE_REF, 
                          FILE_AGL, FILE_DIF, FILE_LOGS, FILE_SETTINGS, FILE_ANY, FILE_DIRECTORY, 
                          LANG_ENGLISH, LANG_DUTCH, LANG_FRENCH, 
                          EQ_METHOD_LSQ, EQ_METHOD_ZELLNER, EQ_METHOD_INSTRUMENTAL, EQ_METHOD_GLS, EQ_METHOD_MAX_LIKELIHOOD, EQUATION_METHODS, 
                          EQ_TEST_CORR, EQ_TEST_STDEV, EQ_TEST_MEANY, EQ_TEST_SSRES, EQ_TEST_STDERR, EQ_TEST_STDERRP, EQ_TEST_FSTAT, 
                          EQ_TEST_R2, EQ_TEST_R2ADJ, EQ_TEST_DW, EQ_TEST_LOGLIK, EQ_NB_TESTS, 
                          CELL_TYPE_LEC, CELL_TYPE_STRING, CELL_FONT_NORMAL, CELL_FONT_BOLD, CELL_FONT_ITALIC, CELL_FONT_UNDERLINE, 
                          CELL_ALIGN_CENTER, CELL_ALIGN_LEFT, CELL_ALIGN_RIGHT, CELL_ALIGN_DECIMAL, 
                          LINE_TYPE_FILES, LINE_TYPE_MODE, LINE_TYPE_CELL, LINE_TYPE_LINE, LINE_TYPE_TITLE, LINE_TYPE_DATE, 
                          GRAPH_TYPE_LINE, GRAPH_TYPE_SCATTER, GRAPH_TYPE_BAR, GRAPH_GRID_MAJOR, GRAPH_GRID_NONE, GRAPH_GRID_MINOR, 
                          GRAPH_ALIGN_LEFT, GRAPH_ALIGN_CENTER, GRAPH_ALIGN_RIGHT, GRAPH_AXIS_VALUES, GRAPH_AXIS_LOG, 
                          GRAPH_AXIS_SEMILOG, GRAPH_AXIS_PERCENT, 
                          VAR_MODE_LEVEL, VAR_MODE_DIFF, VAR_MODE_GROWTH_RATE, VAR_MODE_Y0Y_DIFF, VAR_MODE_Y0Y_GROWTH_RATE, VARIABLES_MODES, 
                          SIMULATION_INIT_TM1, SIMULATION_INIT_TM1_A, SIMULATION_INIT_EXTRA, SIMULATION_INIT_EXTRA_A, SIMULATION_INIT_ASIS, 
                          SIMULATION_INIT_TM1_NA, SIMULATION_INIT_EXTRA_NA, SIMULATION_INITIALIZATION_LIST, 
                          SIMULATION_SORT_BOTH, SIMULATION_SORT_CONNEX, SIMULATION_SORT_NONE, 
                          ESTIMATION_MAXIT, ESTIMATION_EPS, ADJUSTMENT_PARTIAL, ADJUSTMENT_ERROR_CORRECTION_METHOD, ADJUSTMENT_METHODS_LIST, 
                          LTOH_STOCK, LTOH_FLOW, LTOH_CUBIC_SPLINESS, LTOH_LINEAR, LTOH_STEP, HTOL_LAST, HTOL_MEAN, HTOL_SUM, 
                          W_A2M, W_CSV, W_DUMMY, W_GDI, W_HTML, W_MIF, W_RTF)

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
    'SAMPLE_DATA_DIR', 'nan',
    'COMMENTS', 'EQUATIONS', 'IDENTITIES', 'LISTS', 'SCALARS', 'TABLES', 'VARIABLES', 'IODE_TYPES_LIST', 'NB_TYPES', 
    'FILE_COMMENTS', 'FILE_EQUATIONS', 'FILE_IDENTITIES', 'FILE_LISTS', 'FILE_SCALARS', 'FILE_TABLES', 'FILE_VARIABLES', 
    'FILE_REPORTS', 'FILE_TEXT', 'FILE_ASCII', 'FILE_A2M', 'FILE_RTF', 'FILE_HTML', 'FILE_MIF', 'FILE_CSV', 'FILE_REF', 
    'FILE_AGL', 'FILE_DIF', 'FILE_LOGS', 'FILE_SETTINGS', 'FILE_ANY', 'FILE_DIRECTORY', 
    'LANG_ENGLISH', 'LANG_DUTCH', 'LANG_FRENCH', 
    'EQ_METHOD_LSQ', 'EQ_METHOD_ZELLNER', 'EQ_METHOD_INSTRUMENTAL', 'EQ_METHOD_GLS', 'EQ_METHOD_MAX_LIKELIHOOD', 'EQUATION_METHODS', 
    'EQ_TEST_CORR', 'EQ_TEST_STDEV', 'EQ_TEST_MEANY', 'EQ_TEST_SSRES', 'EQ_TEST_STDERR', 'EQ_TEST_STDERRP', 'EQ_TEST_FSTAT', 
    'EQ_TEST_R2', 'EQ_TEST_R2ADJ', 'EQ_TEST_DW', 'EQ_TEST_LOGLIK', 'EQ_NB_TESTS', 
    'CELL_TYPE_LEC', 'CELL_TYPE_STRING', 'CELL_FONT_NORMAL', 'CELL_FONT_BOLD', 'CELL_FONT_ITALIC', 'CELL_FONT_UNDERLINE', 
    'CELL_ALIGN_CENTER', 'CELL_ALIGN_LEFT', 'CELL_ALIGN_RIGHT', 'CELL_ALIGN_DECIMAL', 
    'LINE_TYPE_FILES', 'LINE_TYPE_MODE', 'LINE_TYPE_CELL', 'LINE_TYPE_LINE', 'LINE_TYPE_TITLE', 'LINE_TYPE_DATE', 
    'GRAPH_TYPE_LINE', 'GRAPH_TYPE_SCATTER', 'GRAPH_TYPE_BAR', 'GRAPH_GRID_MAJOR', 'GRAPH_GRID_NONE', 'GRAPH_GRID_MINOR', 
    'GRAPH_ALIGN_LEFT', 'GRAPH_ALIGN_CENTER', 'GRAPH_ALIGN_RIGHT', 'GRAPH_AXIS_VALUES', 'GRAPH_AXIS_LOG', 
    'GRAPH_AXIS_SEMILOG', 'GRAPH_AXIS_PERCENT',
    'VAR_MODE_LEVEL', 'VAR_MODE_DIFF', 'VAR_MODE_GROWTH_RATE', 'VAR_MODE_Y0Y_DIFF', 'VAR_MODE_Y0Y_GROWTH_RATE', 'VARIABLES_MODES', 
    'SIMULATION_INIT_TM1', 'SIMULATION_INIT_TM1_A', 'SIMULATION_INIT_EXTRA', 'SIMULATION_INIT_EXTRA_A', 'SIMULATION_INIT_ASIS', 
    'SIMULATION_INIT_TM1_NA', 'SIMULATION_INIT_EXTRA_NA', 'SIMULATION_INITIALIZATION_LIST',
    'SIMULATION_SORT_BOTH', 'SIMULATION_SORT_CONNEX', 'SIMULATION_SORT_NONE', 
    'ESTIMATION_MAXIT', 'ESTIMATION_EPS', 'ADJUSTMENT_PARTIAL', 'ADJUSTMENT_ERROR_CORRECTION_METHOD', 'ADJUSTMENT_METHODS_LIST', 
    'LTOH_STOCK', 'LTOH_FLOW', 'LTOH_CUBIC_SPLINESS', 'LTOH_LINEAR', 'LTOH_STEP', 'HTOL_LAST', 'HTOL_MEAN', 'HTOL_SUM', 
    'W_A2M', 'W_CSV', 'W_DUMMY', 'W_GDI', 'W_HTML', 'W_MIF', 'W_RTF',

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
