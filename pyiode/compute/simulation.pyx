from libcpp.string cimport string
from libcpp.vector cimport vector

from pyiode.compute.simulation cimport CSimulation
from pyiode.common cimport SimuSortAlgorithm, VariablesInitialization


cdef class Simulation:
    cdef CSimulation* c_simulation

    def __cinit__(self):
        self.c_simulation = new CSimulation()

    def __dealloc__(self):
        if self.c_simulation is not NULL:
            del self.c_simulation
            self.c_simulation = NULL

    def get_convergence_threshold(self) -> float:
        return self.c_simulation.get_convergence_threshold()

    def set_convergence_threshold(self, value: float):
        self.c_simulation.set_convergence_threshold(value)

    def get_relax(self) -> float:
        return self.c_simulation.get_relax()

    def set_relax(self, value: float):
        self.c_simulation.set_relax(value)

    def get_max_nb_iterations(self) -> int:
        return self.c_simulation.get_max_nb_iterations()

    def set_max_nb_iterations(self, value: int):
        self.c_simulation.set_max_nb_iterations(value)

    def get_max_nb_iterations_newton(self) -> int:
        return self.c_simulation.get_max_nb_iterations_newton()

    def set_max_nb_iterations_newton(self, value: int):
        self.c_simulation.set_max_nb_iterations_newton(value)

    def get_sort_algorithm(self) -> str:
        return f"{SimulationSort(<int>(self.c_simulation.get_sort_algorithm())).name}"

    def get_sort_algorithm_long(self) -> str:
        return f"{SimulationSort(<int>(self.c_simulation.get_sort_algorithm())).name} " + \
               f"({self.c_simulation.get_sort_algorithm_as_string().decode()})"

    def set_sort_algorithm(self, value: int):
        self.c_simulation.set_sort(<SimuSortAlgorithm>value)

    def get_initialization_method(self) -> str:
        return f"{SimulationInitialization(<int>(self.c_simulation.get_initialization_method())).name}"

    def get_initialization_method_long(self) -> str:
        return f"{SimulationInitialization(<int>(self.c_simulation.get_initialization_method())).name} " + \
               f"({self.c_simulation.get_initialization_method_as_string().decode()})"

    def set_initialization_method(self, value: int):
        self.c_simulation.set_initialization_method(<VariablesInitialization>value)

    def get_debug(self) -> bool:
        return self.c_simulation.is_debug_active()

    def set_debug(self, value: bool):
        self.c_simulation.set_debug(value)

    def get_debug_newton(self) -> bool:
        return self.c_simulation.is_debug_newton_active()

    def set_debug_newton(self, value: bool):
        self.c_simulation.set_debug_newton(value)

    def get_nb_passes(self) -> int:
        return self.c_simulation.get_nb_passes()

    def set_nb_passes(self, value: int):
        self.c_simulation.set_nb_passes(value)

    def model_exchange(self, list_exo: str):
        self.c_simulation.model_exchange(list_exo.encode())

    def model_compile(self, list_eqs: str):
        self.c_simulation.model_compile(list_eqs.encode())

    def model_simulate(self, from_period: str, to_period: str, list_eqs: str):
        self.c_simulation.model_simulate(from_period.encode(), to_period.encode(), list_eqs.encode())

    def model_calculate_SCC(self, nb_iterations: int, pre_name: str, inter_name: str, 
                            post_name: str, list_eqs: str):
        self.c_simulation.model_calculate_SCC(nb_iterations, pre_name.encode(), inter_name.encode(), 
                                              post_name.encode(), list_eqs.encode())

    def model_simulate_SCC(self, from_period: str, to_period: str, pre_name: str, inter_name: str, post_name: str):
        self.c_simulation.model_simulate_SCC(from_period.encode(), to_period.encode(), pre_name.encode(), 
                                             inter_name.encode(), post_name.encode())
