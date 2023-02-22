# IODE EXTENSION FOR PYTHON
# =========================
#
#    @header4iode
# 
# IODE Model functions
# --------------------
# 
#   model_simulate(sample, ...)
#   model_calc_scc 
#   model_simulateparms 
#   model_simulate_scc 
#   model_exchange 
#   model_compile 
#   model_simulate_save_niters 
#   model_simulate_save_norms

def model_simulate(sample_from:str, sample_to:str, eqs_list=None, endo_exo_list=None,
                    eps:float=0.0001, relax:float=1.0, maxit:int=100, 
                    init_values:int=KV_INIT_TM1, sort_algo:int=SORT_BOTH, nb_passes:int=5, 
                    debug:int=0, newton_eps:float=1e-6, newton_maxit:int=50, newton_debug:int=0
                   )->int:
    '''
    Simulate the model defined by eqs_list on the period [sample_from, sample_to].     
    '''
    if type(eqs_list) == list:
        eqs_list = ','.join(eqs_list)
    elif eqs_list is None:
        eqs_list = ""

    if type(endo_exo_list) == list:
        endo_exo_list = ','.join(endo_exo_list)
    elif endo_exo_list is None:
        endo_exo_list = ""
        
    rc = IodeModelSimulate(cstr(sample_from), cstr(sample_to), cstr(eqs_list), cstr(endo_exo_list),
                eps, relax, maxit, init_values, sort_algo, nb_passes, debug, 
                newton_eps, newton_maxit, newton_debug)

    return rc
    
    
