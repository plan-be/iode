from pathlib import Path

SAMPLE_DATA_DIR = str((Path(__file__).parent / "tests" / "data").resolve())

# Constants
from .iode_python import (nan, __version__, 
                          COMMENTS, EQUATIONS, IDENTITIES, LISTS, SCALARS, TABLES, VARIABLES, IODE_TYPES_LIST, NB_TYPES, 
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
                          SIMU_INIT_TM1, SIMU_INIT_TM1_A, SIMU_INIT_EXTRA, SIMU_INIT_EXTRA_A, SIMU_INIT_ASIS, SIMU_INIT_TM1_NA, 
                          SIMU_INIT_EXTRA_NA, SIMULATION_INITIALIZATION_LIST, 
                          SORT_BOTH, SORT_CONNEX, SORT_NONE, 
                          HTOL_LAST, HTOL_MEAN, HTOL_SUM, LTOH_CS, LTOH_FLOW, LTOH_LIN, LTOH_STEP, LTOH_STOCK, 
                          W_A2M, W_CSV, W_DUMMY, W_GDI, W_HTML, W_MIF, W_RTF)

# time
from .iode_python import Period, Sample, periodicities

#IODE objects
from .iode_python import Equation, List, Scalar

# IODE databases
from .iode_python import Comments, Equations, Identities, Lists, Scalars, Variables 

# Reports

# Printing
from .iode_python import (w_close, w_dest, w_flush, w_print, w_print_cmd, w_print_enum, w_print_par, 
                          w_print_pg_footer, w_print_pg_header, w_print_tit)

# Estimation - Simulation
from .iode_python import (model_calc_scc, model_simulate, model_simulate_cpu, model_simulate_cpu_scc, 
                          model_simulate_cpu_sort, model_simulate_eps, model_simulate_init_values, 
                          model_simulate_maxit, model_simulate_nb_passes, model_simulate_niter, model_simulate_norm, 
                          model_simulate_relax, model_simulate_save_parms, model_simulate_scc, model_simulate_sort_algo)

# miscellaneous
from .iode_python import exec_lec, add_error_msg, clear_error_msgs, display_error_msgs, reset_msgs, suppress_msgs


__all__ = [
    # Constants
    'SAMPLE_DATA_DIR', 'nan',
    'COMMENTS', 'EQUATIONS', 'IDENTITIES', 'LISTS', 'SCALARS', 'TABLES', 'VARIABLES', 'IODE_TYPES_LIST', 'NB_TYPES', 
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
    'SIMU_INIT_TM1', 'SIMU_INIT_TM1_A', 'SIMU_INIT_EXTRA', 'SIMU_INIT_EXTRA_A', 'SIMU_INIT_ASIS', 'SIMU_INIT_TM1_NA', 
    'SIMU_INIT_EXTRA_NA', 'SIMULATION_INITIALIZATION_LIST', 
    'SORT_BOTH', 'SORT_CONNEX', 'SORT_NONE', 
    'HTOL_LAST', 'HTOL_MEAN', 'HTOL_SUM', 'LTOH_CS', 'LTOH_FLOW', 'LTOH_LIN', 'LTOH_STEP', 'LTOH_STOCK', 
    'W_A2M', 'W_CSV', 'W_DUMMY', 'W_GDI', 'W_HTML', 'W_MIF', 'W_RTF',

    # time
    'Period', 'Sample', 'periodicities',

    #IODE objects
    'Equation', 'List', 'Scalar',
    
    # IODE databases
    'Comments', 'Equations', 'Identities', 'Lists', 'Scalars', 'Variables', 

    # Reports

    # Printing
    'w_close', 'w_dest', 'w_flush', 'w_print', 'w_print_cmd', 'w_print_enum', 'w_print_par', 'w_print_pg_footer', 
    'w_print_pg_header', 'w_print_tit', 

    # Estimation - Simulation
    'model_calc_scc', 'model_simulate', 'model_simulate_cpu', 'model_simulate_cpu_scc', 'model_simulate_cpu_sort', 
    'model_simulate_eps', 'model_simulate_init_values', 'model_simulate_maxit', 'model_simulate_nb_passes', 
    'model_simulate_niter', 'model_simulate_norm', 'model_simulate_relax', 'model_simulate_save_parms', 
    'model_simulate_scc', 'model_simulate_sort_algo', 

    # miscellaneous
    'exec_lec', 'add_error_msg', 'clear_error_msgs', 'display_error_msgs', 'reset_msgs', 'suppress_msgs', 
]

# ==== DEPRECATED API ====

# time
from .iode_python import (ws_sample_get, ws_sample_nb_periods, ws_sample_set, ws_sample_to_larray_axis, 
                          ws_sample_to_list, ws_sample_to_string) 

# IODE databases
from .iode_python import (ws_clear, ws_clear_all, ws_clear_cmt, ws_clear_eqs, ws_clear_idt, ws_clear_lst, ws_clear_scl, 
                          ws_clear_tbl, ws_clear_var, ws_content, ws_content_cmt, ws_content_eqs, ws_content_idt, 
                          ws_content_lst, ws_content_scl, ws_content_tbl, ws_content_var, ws_load, ws_load_cmt, ws_load_eqs, 
                          ws_load_idt, ws_load_lst, ws_load_scl, ws_load_tbl, ws_load_var, ws_load_var_to_larray, ws_save, 
                          ws_save_cmt, ws_save_eqs, ws_save_idt, ws_save_lst, ws_save_scl, ws_save_tbl, ws_save_var)

#IODE objects
from .iode_python import (get_cmt, get_eqs, get_eqs_lec, get_idt, get_lst, get_scl, get_var, get_var_as_ndarray, 
                          set_cmt, set_eqs, set_idt, set_lst, set_scl, set_var, data_update, data_update_cmt, data_update_eqs, 
                          data_update_idt, data_update_lst, data_update_scl, data_update_var, delete_obj, delete_objects, 
                          delete_cmt, delete_eqs, delete_idt, delete_lst, delete_scl, delete_tbl, delete_var) 

from .iode_python import (idt_execute, eqs_estimate, ws_htol, ws_htol_last, ws_htol_mean, ws_htol_sum, ws_ltoh, ws_ltoh_flow, 
                          ws_ltoh_stock) 

# Reports
from .iode_python import report_exec, reportline_exec

# compatibility with other libraries
from .iode_python import df_to_ws, larray_to_ws, ws_to_df, ws_to_larray


_deprecated = [
    # time
    'ws_sample_get', 'ws_sample_nb_periods', 'ws_sample_set', 'ws_sample_to_larray_axis', 'ws_sample_to_list', 'ws_sample_to_string', 
    
    # IODE databases
    'ws_clear', 'ws_clear_all', 'ws_clear_cmt', 'ws_clear_eqs', 'ws_clear_idt', 'ws_clear_lst', 'ws_clear_scl', 
    'ws_clear_tbl', 'ws_clear_var', 'ws_content', 'ws_content_cmt', 'ws_content_eqs', 'ws_content_idt', 'ws_content_lst', 
    'ws_content_scl', 'ws_content_tbl', 'ws_content_var', 'ws_load', 'ws_load_cmt', 'ws_load_eqs', 'ws_load_idt', 
    'ws_load_lst', 'ws_load_scl', 'ws_load_tbl', 'ws_load_var', 'ws_load_var_to_larray', 'ws_save', 'ws_save_cmt', 
    'ws_save_eqs', 'ws_save_idt', 'ws_save_lst', 'ws_save_scl', 'ws_save_tbl', 'ws_save_var', 

    #IODE objects
    'get_cmt', 'get_eqs', 'get_eqs_lec', 'get_idt', 'get_lst', 'get_scl', 'get_var', 'get_var_as_ndarray', 
    'set_cmt', 'set_eqs', 'set_idt', 'set_lst', 'set_scl', 'set_var', 
    'data_update', 'data_update_cmt', 'data_update_eqs', 'data_update_idt', 'data_update_lst', 'data_update_scl', 'data_update_var', 
    'delete_obj', 'delete_objects', 'delete_cmt', 'delete_eqs', 'delete_idt', 'delete_lst', 'delete_scl', 'delete_tbl', 'delete_var', 
    
    'idt_execute', 'eqs_estimate', 
    'ws_htol', 'ws_htol_last', 'ws_htol_mean', 'ws_htol_sum','ws_ltoh', 'ws_ltoh_flow', 'ws_ltoh_stock', 

    # Reports
    'report_exec', 'reportline_exec',

    # compatibility with other libraries
    'df_to_ws', 'larray_to_ws', 'ws_to_df', 'ws_to_larray'
]

__all__ += _deprecated
