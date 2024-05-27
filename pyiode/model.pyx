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
#                   init_values: int = EnumSimulationInitialization.IV_INIT_TM1, 
#                   sort_algo: int = EnumSimulationSortAlgorithm.I_SORT_BOTH, 
#                   nb_passes: int = 5, 
#                   debug: bool = False, 
#                   newton_eps: float = 1e-6, 
#                   newton_maxit: int = 50, 
#                   newton_debug: bool = False)
#  
#   model_calc_scc(nb_passes: int = 5, 
#                   pre_listname: str = "_PRE", 
#                   inter_listname: str = "_INTER", 
#                   post_listname: str = "_POST", 
#                   eqs_list = None):
#
#   model_simulate_scc( sample_from: str, sample_to: str, 
#                           pre_listname: str = "_PRE", 
#                           inter_listname: str = "_INTER", 
#                           post_listname: str = "_POST",
#                           eps: float = 0.0001, 
#                           relax: float = 1.0, 
#                           maxit: int = 100, 
#                           init_values: int = EnumSimulationInitialization.IV_INIT_TM1, 
#                           debug: bool = False, 
#                           newton_eps: float = 1e-6, 
#                           newton_maxit: int = 50, 
#                           newton_debug: bool = False)
#
#  
#   model_exchange => included in the parameters of model_simulate()
#   model_compile  => not yet implemented
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

from model cimport (IodeModelSimulate, IodeModelCalcSCC, IodeModelSimulateSCC, IodeModelSimNorm, IodeModelSimNIter, IodeModelSimCpu, 
                    IodeModelCpuSort, IodeModelCpuSCC, KSIM_MAXIT, KSIM_EPS, KSIM_RELAX, KSIM_PASSES, KSIM_SORT, KSIM_START, 
                    KSIM_CPU_SCC, KSIM_CPU_SORT)
from pyiode.common cimport EnumSimulationSortAlgorithm, EnumSimulationInitialization

# TODO: (ald) add Parameters, Returns and Examples section
def model_simulate(sample_from: str, sample_to: str, 
                    eqs_list=None, 
                    endo_exo_list=None,
                    eps: float = 0.0001, 
                    relax: float = 1.0, 
                    maxit: int = 100, 
                    init_values: int = EnumSimulationInitialization.IV_INIT_TM1, 
                    sort_algo: int = EnumSimulationSortAlgorithm.I_SORT_BOTH, 
                    nb_passes: int = 5, 
                    debug: bool = False, 
                    newton_eps: float = 1e-6, 
                    newton_maxit: int = 50, 
                    newton_debug: bool = False):
    '''
    Simulate the model defined by eqs_list on the period [sample_from, sample_to].     
    '''
    eqs_list = _arg_to_str(eqs_list, sep = ',')
    endo_exo_list = _arg_to_str(endo_exo_list, sep = ',')
    model_simulate_save_parms(eps, relax, maxit, init_values, sort_algo, nb_passes)
    
    if IodeModelSimulate(_cstr(sample_from), _cstr(sample_to), 
                         _cstr(eqs_list), _cstr(endo_exo_list),
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
    >>> from iode import SAMPLE_DATA_DIR
    >>> from iode import equations, model_calc_scc
    >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
    >>> model_calc_scc(nb_passes=1)
    '''
    eqs_list = _arg_to_str(eqs_list, sep = ',')
    if IodeModelCalcSCC(nb_passes, _cstr(pre_listname), _cstr(inter_listname), 
                        _cstr(post_listname), _cstr(eqs_list)):
        raise RuntimeError(f"Cannot create the model Connex Components")
    

# TODO: (ald) add Parameters, Returns and Examples section
def model_simulate_scc( sample_from: str, sample_to: str, 
                        pre_listname: str = "_PRE", 
                        inter_listname: str = "_INTER", 
                        post_listname: str = "_POST",
                        eps: float = 0.0001, 
                        relax: float = 1.0, 
                        maxit: int = 100, 
                        init_values: int = EnumSimulationInitialization.IV_INIT_TM1, 
                        debug: bool = False, 
                        newton_eps: float = 1e-6, 
                        newton_maxit: int = 50, 
                        newton_debug: bool = False):
    '''
    Simulate a model defined by the 3 Connex Components (stored in 3 lists).
    '''
    model_simulate_save_parms(eps, relax, maxit, init_values, -1, -1)
    if IodeModelSimulateSCC(_cstr(sample_from), _cstr(sample_to), 
                            _cstr(pre_listname), _cstr(inter_listname), _cstr(post_listname),
                            eps, relax, maxit, init_values, debug, 
                            newton_eps, newton_maxit, newton_debug):
        raise RuntimeError(f"model_simulate_scc() failed")
        

# TODO: (ald) add Parameters, Returns and Examples section
def model_simulate_save_parms(
                    eps: float = 0.0001, 
                    relax: float = 1.0, 
                    maxit: int = 100, 
                    init_values: int = EnumSimulationInitialization.IV_INIT_TM1, 
                    sort_algo: int = EnumSimulationSortAlgorithm.I_SORT_BOTH, 
                    nb_passes: int = 1):
    ''' 
        Save in the global variables KSIM_* the simulation parameters used during the last 
        call to model_simulate() and model_simulate_scc(). 
        The purpose of this function is to enable later reporting
        via the functions model_simulate_maxit(), model_simulate_eps()...
    '''

    KSIM_MAXIT = maxit
    KSIM_EPS = eps
    KSIM_RELAX = relax
    KSIM_START = init_values

    if nb_passes >= 0: 
        KSIM_PASSES = nb_passes  # not used by model_simulate_scc()
    if sort_algo >= 0: 
        KSIM_SORT = sort_algo    # id.

    
# TODO: (ald) add Parameters, Returns and Examples section
def model_simulate_maxit() -> int:
    ''' Returns the maxit parameter of the last simulation '''
    return KSIM_MAXIT

# TODO: (ald) add Parameters, Returns and Examples section
def model_simulate_eps() -> float:
    ''' Returns the eps parameter of the last simulation '''
    return KSIM_EPS

# TODO: (ald) add Parameters, Returns and Examples section  
def model_simulate_relax() -> float:
    ''' Returns the relax parameter of the last simulation '''
    return KSIM_RELAX

# TODO: (ald) add Parameters, Returns and Examples section 
def model_simulate_nb_passes() -> int:
    ''' Returns the nb_passes parameter of the last simulation '''
    return KSIM_PASSES

# TODO: (ald) add Parameters, Returns and Examples section  
def model_simulate_sort_algo() -> int:
    ''' 
    Returns the sort_algo parameter of the last simulation: 
        - SIMULATION_SORT_CONNEX (0) for SCC decomposition only
        - SIMULATION_EnumSimulationSortAlgorithm.I_SORT_BOTH (1)   for SCC decomposition and triangulation
        - SIMULATION_SORT_NONE (2)   for no reordering nor SCC decomposition
    '''
    
    return KSIM_SORT


# TODO: (ald) add Parameters, Returns and Examples section       
def model_simulate_init_values() -> int:
    ''' 
    Returns the init_values parameter of the last simulation:
        - EnumSimulationInitialization.IV_INIT_TM1      (0): Y := Y[-1], if Y null or NA 
        - EnumSimulationInitialization.IV_INIT_TM1_A    (1): Y := Y[-1], always 
        - SIMULATION_INIT_EXTRA    (2): Y := extrapolation, if Y null or NA 
        - SIMULATION_INIT_EXTRA_A  (3): Y := extrapolation, always 
        - SIMULATION_INIT_ASIS     (4): Y unchanged 
        - EnumSimulationInitialization.IV_INIT_TM1_NA   (5): Y := Y[-1], if Y is NA 
        - SIMULATION_INIT_EXTRA_NA (6): Y := extrapolation, if Y is NA 
    '''
    return KSIM_START


# TODO: (ald) add Parameters, Returns and Examples section 
def model_simulate_cpu_scc() -> int:
    ''' Returns the elapsed time in ms during the last SCC decomposition '''
    return IodeModelCpuSCC()


# TODO: (ald) add Parameters, Returns and Examples section 
def model_simulate_cpu_sort() -> int:
    ''' Returns the elapsed time in ms during the last sort algorithm '''
    return IodeModelCpuSort()


# TODO: (ald) add Parameters, Returns and Examples section  
def model_simulate_cpu(period: str) -> int:
    ''' Returns the elapsed time in ms during the last simulation of the given period'''
    return IodeModelSimCpu(_cstr(period))


# TODO: (ald) add Parameters, Returns and Examples section  
def model_simulate_niter(period: str) -> int:
    ''' Returns the number of iterations needed to reach a solution during the last simulation of the given period'''
    return IodeModelSimNIter(_cstr(period))
    

# TODO: (ald) add Parameters, Returns and Examples section
def model_simulate_norm(period: str) -> float:
    ''' Returns the convergence threshold reached during the last simulation of the given period'''
    return IodeModelSimNorm(_cstr(period))
