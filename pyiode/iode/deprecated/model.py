import warnings
from typing import List 

from iode import Simulation, SimulationSort, SimulationInitialization


def model_simulate(sample_from: str, sample_to: str, 
                   eqs_list=None, 
                   endo_exo_list=None,
                   eps: float = 0.0001, 
                   relax: float = 1.0, 
                   maxit: int = 100, 
                   init_values: int = SimulationInitialization.TM1.value,
                   sort_algo: int = SimulationSort.BOTH.value, 
                   nb_passes: int = 5, 
                   debug: bool = False, 
                   newton_eps: float = 1e-6, 
                   newton_maxit: int = 50, 
                   newton_debug: bool = False):
    warnings.warn("model_simulate() is deprecated. Please use the new syntax:\n" + 
                  "simu = Simulation(convergence_threshold, relax, max_nb_iterations, sort_algorithm, " +
                  "initialization_method, debug, nb_passes, debug_newton)\n" + 
                  "simu.model_exchange(list_exo)\n" + 
                  "simu.model_simulate(from_period, to_period, list_eqs)", DeprecationWarning, stacklevel=2)
    simu = Simulation(eps, relax, maxit, sort_algo, init_values, debug, nb_passes, newton_debug)
    simu.max_nb_iterations_newton = newton_maxit
    if endo_exo_list is not None:
        simu.model_exchange(endo_exo_list)
    simu.model_simulate(sample_from, sample_to, eqs_list)

def model_calc_scc(nb_passes: int = 1, 
                   pre_listname: str = "_PRE", 
                   inter_listname: str = "_INTER", 
                   post_listname: str = "_POST", 
                   eqs_list = None):
    warnings.warn("model_calc_scc() is deprecated. Please use the new syntax:\n" + 
                  "simu = Simulation()\n" + 
                  "simu.model_calc_scc(nb_iterations, pre_name, inter_name, post_name, list_eqs)", 
                  DeprecationWarning, stacklevel=2)
    simu = Simulation()
    simu.model_calc_scc(nb_passes, pre_listname, inter_listname, post_listname, eqs_list)

def model_simulate_scc( sample_from: str, sample_to: str, 
                        pre_listname: str = "_PRE", 
                        inter_listname: str = "_INTER", 
                        post_listname: str = "_POST",
                        eps: float = 0.0001, 
                        relax: float = 1.0, 
                        maxit: int = 100, 
                        init_values: int = SimulationInitialization.TM1.value,
                        debug: bool = False, 
                        newton_eps: float = 1e-6, 
                        newton_maxit: int = 50, 
                        newton_debug: bool = False):
    warnings.warn("model_simulate_scc() is deprecated. Please use the new syntax:\n" + 
                  "simu = Simulation(convergence_threshold, relax, max_nb_iterations, sort_algorithm, " +
                  "initialization_method, debug, nb_passes, debug_newton)\n" + 
                  "simu.model_simulate_scc(from_period, to_period, pre_name, inter_name, post_name)", 
                  DeprecationWarning, stacklevel=2)
    simu = Simulation(eps, relax, maxit, init_values, debug, newton_debug)
    simu.max_nb_iterations_newton = newton_maxit
    simu.model_simulate_scc(sample_from, sample_to, pre_listname, inter_listname, post_listname)

def model_simulate_save_parms(
                    eps: float = 0.0001, 
                    relax: float = 1.0, 
                    maxit: int = 100, 
                    init_values: int = SimulationInitialization.TM1.value,
                    sort_algo: int = SimulationSort.BOTH.value, 
                    nb_passes: int = 1):
    warnings.warn("model_simulate_save_parms() is deprecated. Please use the new syntax:\n" + 
                  "simu = Simulation(convergence_threshold, relax, max_nb_iterations, sort_algorithm, " +
                  "initialization_method, debug, nb_passes, debug_newton)", DeprecationWarning, stacklevel=2)
    Simulation(eps, relax, maxit, sort_algo, init_values, nb_passes)
    
def model_simulate_maxit() -> int:
    warnings.warn("model_simulate_maxit() is deprecated. Please use the new syntax:\n" + 
                  "simu = Simulation()\nsimu.max_nb_iterations",
                  DeprecationWarning, stacklevel=2)
    simu = Simulation()
    return simu.max_nb_iterations

def model_simulate_eps() -> float:
    warnings.warn("model_simulate_eps() is deprecated. Please use the new syntax:\n" + 
                  "simu = Simulation()\nsimu.convergence_threshold",
                  DeprecationWarning, stacklevel=2)
    simu = Simulation()
    return simu.convergence_threshold
  
def model_simulate_relax() -> float:
    warnings.warn("model_simulate_relax() is deprecated. Please use the new syntax:\n" + 
                  "simu = Simulation()\nsimu.relax",
                  DeprecationWarning, stacklevel=2)
    simu = Simulation()
    return simu.relax
 
def model_simulate_nb_passes() -> int:
    warnings.warn("model_simulate_nb_passes() is deprecated. Please use the new syntax:\n" + 
                  "simu = Simulation()\nsimu.nb_passes",
                  DeprecationWarning, stacklevel=2)
    simu = Simulation()
    return simu.nb_passes
   
def model_simulate_sort_algo() -> int:
    warnings.warn("model_simulate_sort_algo() is deprecated. Please use the new syntax:\n" + 
                  "simu = Simulation()\nsimu.sort_algorithm",
                  DeprecationWarning, stacklevel=2)
    simu = Simulation()
    return simu.sort_algorithm
      
def model_simulate_init_values() -> int:
    warnings.warn("model_simulate_init_values() is deprecated. Please use the new syntax:\n" + 
                  "simu = Simulation()\nsimu.initialization_method",
                  DeprecationWarning, stacklevel=2)
    simu = Simulation()
    return simu.initialization_method

def model_simulate_niter(period: str) -> int:
    warnings.warn("model_simulate_niter() is deprecated. Please use the new syntax:\n" + 
                  "simu = Simulation()\n???",
                  DeprecationWarning, stacklevel=2)

def model_simulate_norm(period: str) -> float:
    warnings.warn("model_simulate_norm() is deprecated. Please use the new syntax:\n" + 
                  "simu = Simulation()\n???",
                  DeprecationWarning, stacklevel=2)
