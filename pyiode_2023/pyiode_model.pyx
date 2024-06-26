# IODE EXTENSION FOR PYTHON
# =========================
#
#    @header4iode
# 
# IODE Model functions
# --------------------
# 
#   model_simulate(sample_from: str, sample_to: str, 
#                   eqs_list=None, endo_exo_list=None,
#                   eps: float = 0.0001, 
#                   relax: float = 1.0, 
#                   maxit: int = 100, 
#                   init_values: int = VAR_INIT_TM1, 
#                   sort_algo: int = SORT_BOTH, 
#                   nb_passes: int = 5, 
#                   debug: int = 0, 
#                   newton_eps: float = 1e-6, 
#                   newton_maxit: int = 50, 
#                   newton_debug: int = 0)          
#  
#   model_calc_scc(nb_passes: int = 5, 
#                   pre_listname: str, 
#                   inter_listname: str, 
#                   post_listname: str, 
#                   eqs_list = None)
#  
#   model_simulateparms => included int the parameters of model_simulate(), model_simulate_scc() and model_calc_scc()
#  
#   model_simulate_scc( sample_from: str, sample_to: str, 
#                       pre_listname: str = "_PRE", 
#                       inter_listname: str = "_INTER", 
#                       post_listname: str = "_POST")
                   
#   model_exchange => included int the parameters of model_simulate()
#   model_compile 
#   model_simulate_save_niters 
#   model_simulate_save_norms
#  
#  
# model_simulate_maxit() -> int                 | Returns the maxit parameter of the last simulation   
# model_simulate_eps() -> float                 | Returns the eps parameter of the last simulation
# model_simulate_relax() -> float               | Returns the relax parameter of the last simulation
# model_simulate_nb_passes() -> int             | Returns the nb_passes parameter of the last simulation
# model_simulate_sort_algo() -> int             | Returns the sort_algo parameter of the last simulation
# model_simulate_init_values() -> int           | Returns the init_values parameter of the last simulation 
# model_simulate_cpu_scc() -> int               | Returns the elapsed time during the last SCC decomposition 
# model_simulate_cpu_sort() -> int              | Returns the elapsed time during the last sort algorithm
# model_simulate_cpu(period: str) -> int        | Returns the elapsed time during the last simulation of the given period
# model_simulate_niter(period: str) -> int      | Returns the number of iterations needed to reach a solution during the last simulation of the given period
# model_simulate_norm(period: str) -> float     | Returns the convergence threshold reached during the last simulation of the given period



def model_simulate(sample_from: str, sample_to: str, 
                    eqs_list=None, 
                    endo_exo_list=None,
                    eps: float = 0.0001, 
                    relax: float = 1.0, 
                    maxit: int = 100, 
                    init_values: int = VAR_INIT_TM1, 
                    sort_algo: int = SORT_BOTH, 
                    nb_passes: int = 5, 
                    debug: int = 0, 
                    newton_eps: float = 1e-6, 
                    newton_maxit: int = 50, 
                    newton_debug: int = 0):
    '''
    Simulate the model defined by eqs_list on the period [sample_from, sample_to].     
    '''
    if isinstance(eqs_list, list):
        eqs_list = ','.join(eqs_list)
    elif eqs_list is None:
        eqs_list = ""

    if isinstance(endo_exo_list, list):
        endo_exo_list = ','.join(endo_exo_list)
    elif endo_exo_list is None:
        endo_exo_list = ""
        
    model_simulate_save_parms(eps, relax, maxit, init_values, sort_algo, nb_passes)
    
    if IodeModelSimulate(cstr(sample_from), cstr(sample_to), 
                         cstr(eqs_list), cstr(endo_exo_list),
                         eps, relax, maxit, init_values, sort_algo, nb_passes, debug, 
                         newton_eps, newton_maxit, newton_debug):
        raise RuntimeError(f"Simulation failed")
        
    
  
def model_calc_scc(nb_passes: int = 1, 
                   pre_listname: str = "_PRE", 
                   inter_listname: str = "_INTER", 
                   post_listname: str = "_POST", 
                   eqs_list = None):
    '''
    Decompose a model into its Strong Connex Components: pre-recursive, interdependent
    and post-recursive list of equations.
    
    These 3 components are saved in 3 lists whose names are given as parameters pre_listnames,
    inter_listname and post_listname. 
    
    Optionally, the interdependent list of equations may be reordered by applying an heuristic
    algorithm which tries to improve the position of the equations in the interdep list
    to accelerate the Gauss-Seidel algorithm.
    
    Parameters
    ----------
    nb_passes: int
        number of iterations of the reordering algo (aka triangulation)
    pre_listname: str
        name of the iode list where the prolog block of equations must be stored
    inter_listname: str 
        name of the iode list where the interdependent block of equations must be stored
    post_listname: str 
        name of the iode list where the epilog block of equations must be stored
    eqs_list
        list of equations defining the model or None for all equations in the current WS of equations
        
    Returns
    -------    
    No return value.  On error, raises an exception .
    
    Examples
    --------
    >>> iode.model_calc_scc(nb_passes=1)        # doctest: +SKIP
    
    '''
    if isinstance(eqs_list, list):
        eqs_list = ','.join(eqs_list)
    elif eqs_list is None:
        eqs_list = ""
    
    if IodeModelCalcSCC(nb_passes, 
                         cstr(pre_listname), cstr(inter_listname), cstr(post_listname),
                         cstr(eqs_list)):
        raise RuntimeError(f"Cannot create the model Connex Components")
    

def model_simulate_scc( sample_from: str, sample_to: str, 
                        pre_listname: str = "_PRE", 
                        inter_listname: str = "_INTER", 
                        post_listname: str = "_POST",
                        eps: float = 0.0001, 
                        relax: float = 1.0, 
                        maxit: int = 100, 
                        init_values: int = VAR_INIT_TM1, 
                        debug: int = 0, 
                        newton_eps: float = 1e-6, 
                        newton_maxit: int = 50, 
                        newton_debug: int = 0):
    '''
    Simulate a model defined by the 3 Connex Components (stored in 3 lists).
    '''

    model_simulate_save_parms(eps, relax, maxit, init_values, -1, -1)
    
    if IodeModelSimulateSCC(cstr(sample_from), cstr(sample_to), 
                         cstr(pre_listname), cstr(inter_listname), cstr(post_listname),
                         eps, relax, maxit, init_values, debug, 
                         newton_eps, newton_maxit, newton_debug):
        raise RuntimeError(f"model_simulate_scc() failed")
        

def model_simulate_save_parms(
                    eps: float = 0.0001, 
                    relax: float = 1.0, 
                    maxit: int = 100, 
                    init_values: int = VAR_INIT_TM1, 
                    sort_algo: int = SORT_BOTH, 
                    nb_passes: int = 1):
    ''' 
        Save in the global variabels KSIM_* the simulation parameters used during the last 
        call to model_simulate() and model_simulate_scc(). 
        The purpose of this function is to enable later reporting
        via the functions model_simulate_maxit(), model_simulate_eps()...
    '''
    global KSIM_MAXIT
    global KSIM_EPS
    global KSIM_RELAX
    global KSIM_PASSES
    global KSIM_SORT
    global KSIM_START

    KSIM_MAXIT = maxit
    KSIM_EPS = eps
    KSIM_RELAX = relax
    KSIM_START = init_values

    if nb_passes >= 0: KSIM_PASSES = nb_passes  # not used by model_simulate_scc()
    if sort_algo >= 0: KSIM_SORT = sort_algo    # id.
    
   
def model_simulate_maxit() -> int:
    ''' Returns the maxit parameter of the last simulation '''
    return KSIM_MAXIT

def model_simulate_eps() -> float:
    ''' Returns the eps parameter of the last simulation '''
    return KSIM_EPS

def model_simulate_relax() -> float:
    ''' Returns the relax parameter of the last simulation '''
    return KSIM_RELAX

def model_simulate_nb_passes() -> int:
    ''' Returns the nb_passes parameter of the last simulation '''
    return KSIM_PASSES

def model_simulate_sort_algo() -> int:
    ''' 
    Returns the sort_algo parameter of the last simulation: 
        - iode.SORT_CONNEX (0) for SCC decomposition only
        - iode.SORT_BOTH (1)   for SCC decomposition and triangulation
        - iode.SORT_NONE (2)   for no reordering nor SCC decomposition
    '''
    
    return KSIM_SORT

        
def model_simulate_init_values() -> int:
    ''' 
    Returns the init_values parameter of the last simulation:
        - iode.VAR_INIT_TM1      (0): Y := Y[-1], if Y null or NA 
        - iode.VAR_INIT_TM1_A    (1): Y := Y[-1], always 
        - iode.VAR_INIT_EXTRA    (2): Y := extrapolation, if Y null or NA 
        - iode.VAR_INIT_EXTRA_A  (3): Y := extrapolation, always 
        - iode.VAR_INIT_ASIS     (4): Y unchanged 
        - iode.VAR_INIT_TM1_NA   (5): Y := Y[-1], if Y is NA 
        - iode.VAR_INIT_EXTRA_NA (6): Y := extrapolation, if Y is NA 
    '''
    return KSIM_START

def model_simulate_cpu_scc() -> int:
    ''' Returns the elapsed time in ms during the last SCC decomposition '''
    return IodeModelCpuSCC()

def model_simulate_cpu_sort() -> int:
    ''' Returns the elapsed time in ms during the last sort algorithm '''
    return IodeModelCpuSort()


def model_simulate_cpu(period: str) -> int:
    ''' Returns the elapsed time in ms during the last simulation of the given period'''
    return IodeModelSimCpu(cstr(period))

def model_simulate_niter(period: str) -> int:
    ''' Returns the number of iterations needed to reach a solution during the last simulation of the given period'''
    return IodeModelSimNIter(cstr(period))

def model_simulate_norm(period: str) -> float:
    ''' Returns the convergence threshold reached during the last simulation of the given period'''
    return IodeModelSimNorm(cstr(period))
