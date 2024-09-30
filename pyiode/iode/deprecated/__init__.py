# ==== DEPRECATED API ====

# time
from .time import ws_sample_get, ws_sample_nb_periods, ws_sample_set, ws_sample_to_list, ws_sample_to_string 

# IODE databases
from .workspace import (ws_clear, ws_clear_all, ws_clear_cmt, ws_clear_eqs, ws_clear_idt, ws_clear_lst, ws_clear_scl, 
                        ws_clear_tbl, ws_clear_var, ws_content, ws_content_cmt, ws_content_eqs, ws_content_idt, 
                        ws_content_lst, ws_content_scl, ws_content_tbl, ws_content_var, ws_load, ws_load_cmt, ws_load_eqs, 
                        ws_load_idt, ws_load_lst, ws_load_scl, ws_load_tbl, ws_load_var, ws_save, 
                        ws_save_cmt, ws_save_eqs, ws_save_idt, ws_save_lst, ws_save_scl, ws_save_tbl, ws_save_var, 
                        ws_htol, ws_htol_last, ws_htol_mean, ws_htol_sum, ws_ltoh, ws_ltoh_flow, ws_ltoh_stock)

#IODE objects
from .iode_objects import (get_cmt, get_eqs, get_eqs_lec, get_idt, get_lst, get_scl, get_var, get_var_as_ndarray, 
                           set_cmt, set_eqs, set_idt, set_lst, set_scl, set_var, data_update, data_update_cmt, data_update_eqs, 
                           data_update_idt, data_update_lst, data_update_scl, data_update_var, delete_obj, delete_objects, 
                           delete_cmt, delete_eqs, delete_idt, delete_lst, delete_scl, delete_tbl, delete_var, idt_execute) 

# lec
from .lec import exec_lec

# estimation
from .estimation import eqs_estimate

# model
from .model import (model_simulate, model_calc_scc, model_simulate_scc, model_simulate_save_parms, 
                    model_simulate_maxit, model_simulate_eps, model_simulate_relax, model_simulate_nb_passes, 
                    model_simulate_sort_algo, model_simulate_init_values, model_simulate_niter, model_simulate_norm)

# Reports
from .report import report_exec, reportline_exec

# compatibility with other libraries
from .compat_pandas import df_to_ws, ws_to_df
from .compat_larray import larray_to_ws, ws_to_larray, ws_load_var_to_larray, ws_sample_to_larray_axis


_deprecated = [
    # time
    'ws_sample_get', 'ws_sample_nb_periods', 'ws_sample_set', 'ws_sample_to_list', 'ws_sample_to_string', 
    
    # IODE databases
    'ws_clear', 'ws_clear_all', 'ws_clear_cmt', 'ws_clear_eqs', 'ws_clear_idt', 'ws_clear_lst', 'ws_clear_scl', 
    'ws_clear_tbl', 'ws_clear_var', 'ws_content', 'ws_content_cmt', 'ws_content_eqs', 'ws_content_idt', 
    'ws_content_lst', 'ws_content_scl', 'ws_content_tbl', 'ws_content_var', 'ws_load', 'ws_load_cmt', 'ws_load_eqs', 
    'ws_load_idt', 'ws_load_lst', 'ws_load_scl', 'ws_load_tbl', 'ws_load_var', 'ws_save', 'ws_save_cmt', 
    'ws_save_eqs', 'ws_save_idt', 'ws_save_lst', 'ws_save_scl', 'ws_save_tbl', 'ws_save_var', 
    'ws_htol', 'ws_htol_last', 'ws_htol_mean', 'ws_htol_sum', 'ws_ltoh', 'ws_ltoh_flow', 'ws_ltoh_stock', 

    #IODE objects
    'get_cmt', 'get_eqs', 'get_eqs_lec', 'get_idt', 'get_lst', 'get_scl', 'get_var', 'get_var_as_ndarray', 
    'set_cmt', 'set_eqs', 'set_idt', 'set_lst', 'set_scl', 'set_var', 
    'data_update', 'data_update_cmt', 'data_update_eqs', 'data_update_idt', 'data_update_lst', 'data_update_scl', 
    'data_update_var', 'delete_obj', 'delete_objects', 'delete_cmt', 'delete_eqs', 'delete_idt', 'delete_lst', 
    'delete_scl', 'delete_tbl', 'delete_var', 'idt_execute',  
    
    # lec
    'exec_lec', 

    # estimation
    'eqs_estimate', 
    
    # model
    'model_simulate', 'model_calc_scc', 'model_simulate_scc', 'model_simulate_save_parms', 
    'model_simulate_maxit', 'model_simulate_eps', 'model_simulate_relax', 'model_simulate_nb_passes', 
    'model_simulate_sort_algo', 'model_simulate_init_values', 'model_simulate_niter', 'model_simulate_norm',

    # Reports
    'report_exec', 'reportline_exec', 

    # compatibility with other libraries
    'df_to_ws', 'ws_to_df', 'larray_to_ws', 'ws_to_larray', 'ws_load_var_to_larray', 'ws_sample_to_larray_axis'
]
