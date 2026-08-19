from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.memory cimport shared_ptr

from pyiode.compute.simulation cimport CSimulation, RPF_SimNIterInt, RPF_SimNormReal
from pyiode.compute.simulation cimport B_ModelSimulateSaveNIters, B_ModelSimulateSaveNorms
from pyiode.compute.simulation cimport global_simu as cpp_global_simu
from pyiode.common cimport SimuSortAlgorithm, VariablesInitialization


cdef class CythonSimulation:
    cdef shared_ptr[CSimulation] simu_ptr
    cdef CSimulation* c_simulation

    def __cinit__(self):
        self.simu_ptr = cpp_global_simu
        self.c_simulation = self.simu_ptr.get()

    def __dealloc__(self):
        self.simu_ptr.reset()
        self.c_simulation = NULL

    def reset(self):
        self.c_simulation.reset()

    def get_convergence_threshold(self) -> float:
        return self.c_simulation.epsilon

    def set_convergence_threshold(self, value: float):
        self.c_simulation.epsilon = value

    def get_relax(self) -> float:
        return self.c_simulation.relax

    def set_relax(self, value: float):
        self.c_simulation.relax = value

    def get_max_nb_iterations(self) -> int:
        return self.c_simulation.max_iter

    def set_max_nb_iterations(self, value: int):
        self.c_simulation.max_iter = value

    def get_max_nb_iterations_newton(self) -> int:
        return self.c_simulation.newton_max_iter

    def set_max_nb_iterations_newton(self, value: int):
        self.c_simulation.newton_max_iter = value

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
        return self.c_simulation.nb_passes

    def set_nb_passes(self, value: int):
        self.c_simulation.nb_passes = value

    def get_nb_iterations(self, period: str) -> int:
        cdef bytes b_period = period.encode('utf-8')
        cdef unsigned char* c_period = b_period
        nb_iter: int = RPF_SimNIterInt(&c_period)
        return nb_iter

    def get_norm(self, period: str) -> float:
        cdef bytes b_period = period.encode('utf-8')
        cdef unsigned char* c_period = b_period
        norm: float = RPF_SimNormReal(&c_period)
        return norm

    def save_nb_iterations(self, var_name: str) -> bool:
        cdef bytes b_var_name = var_name.encode('utf-8')
        cdef char* c_var_name = b_var_name
        res: int = B_ModelSimulateSaveNIters(c_var_name)
        return res == 0

    def save_norms(self, var_name: str) -> bool:
        cdef bytes b_var_name = var_name.encode('utf-8')
        cdef char* c_var_name = b_var_name
        res: int = B_ModelSimulateSaveNorms(c_var_name)
        return res == 0

    def model_exchange(self, list_exo: str) -> bool:
        return self.c_simulation.exchange(list_exo.encode())

    def model_compile(self, list_eqs: str) -> bool:
        return self.c_simulation.compile(list_eqs.encode())

    def model_simulate(self, from_period: str, to_period: str, list_eqs: str) -> bool:
        return self.c_simulation.simulate(from_period.encode(), to_period.encode(), list_eqs.encode())

    def model_calculate_SCC(self, nb_iterations: int, pre_name: str, inter_name: str, 
                            post_name: str, list_eqs: str) -> bool:
        return self.c_simulation.calculate_SCC(nb_iterations, pre_name.encode(), inter_name.encode(), 
                                                     post_name.encode(), list_eqs.encode())

    def model_simulate_SCC(self, from_period: str, to_period: str, pre_name: str, 
                           inter_name: str, post_name: str) -> bool:
        return self.c_simulation.simulate_SCC(from_period.encode(), to_period.encode(), pre_name.encode(), 
                                                    inter_name.encode(), post_name.encode())
