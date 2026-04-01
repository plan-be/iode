from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.memory cimport shared_ptr, make_shared

from pyiode.common cimport IodeAdjustmentMethod
from pyiode.time.sample cimport CSample
from pyiode.objects.equation cimport CEquation
from pyiode.iode_database.cpp_api_database cimport KDBScalars
from pyiode.iode_database.cpp_api_database cimport KDBEquations
from pyiode.iode_database.cpp_api_database cimport global_ws_var as cpp_global_variables
from pyiode.compute.estimation cimport dynamic_adjustment as cpp_dynamic_adjustment
from pyiode.compute.estimation cimport dickey_fuller_test as cpp_dickey_fuller_test
from pyiode.compute.estimation cimport CCorrelationMatrix
from pyiode.compute.estimation cimport CEditAndEstimateEquations

import warnings
from typing import Union, Tuple, List, Optional


def cython_dynamic_adjustment(method: int, eqs: str, c1: str, c2: str) -> str:
    return cpp_dynamic_adjustment(<IodeAdjustmentMethod>method, eqs.encode(), c1.encode(), c2.encode()).decode()


def cython_dickey_fuller_test(lec: str, drift: bool, trend: bool, order: int) -> Scalars:
    # NOTE: cpp_dickey_fuller_test allocates a new KDBScalars* pointer
    cdef KDBScalars* cpp_scalars
    cpp_scalars = cpp_dickey_fuller_test(lec.encode(), <bint>drift, <bint>trend, order)
    if cpp_scalars is NULL:
        return None
    cdef shared_ptr[KDBScalars] scalars_ptr = shared_ptr[KDBScalars](cpp_scalars)
    scalars_db = Scalars._from_ptr(scalars_ptr)
    return scalars_db


cdef class CythonCorrelationMatrix:
    cdef shared_ptr[CCorrelationMatrix] correlation_matrix_ptr 
    cdef CCorrelationMatrix* c_correlation_matrix

    def __cinit__(self):
        self.c_correlation_matrix = NULL

    def __init__(self):
        raise TypeError("This class cannot be instantiated directly")

    def __dealloc__(self):
        self.correlation_matrix_ptr.reset()
        self.c_correlation_matrix = NULL
    
    cdef update_ptr(self, shared_ptr[CCorrelationMatrix] correlation_matrix_ptr):
        self.correlation_matrix_ptr = correlation_matrix_ptr
        self.c_correlation_matrix = correlation_matrix_ptr.get()

    @staticmethod
    cdef CythonCorrelationMatrix _from_ptr(shared_ptr[CCorrelationMatrix] corr_ptr):
        """
        Factory function to create CythonCorrelationMatrix objects from a given CCorrelationMatrix pointer.
        """
        # Fast call to __new__() that bypasses the __init__() constructor.
        cdef CythonCorrelationMatrix wrapper = CythonCorrelationMatrix.__new__(CythonCorrelationMatrix)
        if corr_ptr.get() is NULL:
            return None
        wrapper.update_ptr(corr_ptr)
        return wrapper

    def is_undefined(self) -> bool:
        return self.c_correlation_matrix is NULL

    def get_shape(self) -> Tuple[int, int]:
        if self.is_undefined():
            return 0, 0
        else:
            return self.c_correlation_matrix.nb_coeffs, self.c_correlation_matrix.nb_coeffs

    def get_names(self) -> List[str]:
        if self.is_undefined():
            return None
        else:
            return [self.c_correlation_matrix.get_name(i).decode() for i in range(len(self))]

    def name(self, index: int) -> str:
        if self.is_undefined():
            return None
        else:
            return self.c_correlation_matrix.get_name(index).decode()

    def __len__(self) -> int:
        if self.is_undefined():
            return 0
        else:
            return self.c_correlation_matrix.nb_coeffs

    def __getitem__(self, key: Tuple[int, int]) -> float:
        if self.is_undefined():
            return None
        else:
            row, column = key
            return self.c_correlation_matrix.get_value(row, column)


cdef class CythonEditAndEstimateEquations:
    cdef CEditAndEstimateEquations* c_estimation_ptr

    def __cinit__(self):
        self.c_estimation_ptr = NULL

    def __init__(self, from_period: str, to_period: str):
        self.c_estimation_ptr = new CEditAndEstimateEquations(from_period.encode(), to_period.encode())

    def __dealloc__(self):
        if self.c_estimation_ptr is not NULL:    
            del self.c_estimation_ptr
            self.c_estimation_ptr  = NULL

    def get_sample(self) -> Sample:
        cdef CSample* c_sample = self.c_estimation_ptr.get_sample()
        return Sample._from_ptr(c_sample, <bint>False)

    def set_sample(self, from_period: str, to_period: str):
        self.c_estimation_ptr.set_sample(from_period.encode(), to_period.encode())

    def get_block(self) -> str:
        return self.c_estimation_ptr.get_block().decode()

    def set_block(self, block: str, current_eq_name: str):
        self.c_estimation_ptr.set_block(block.encode(), current_eq_name.encode())

    def get_method(self) -> str:
        return self.c_estimation_ptr.get_method().decode()

    def set_method(self, value: int):
        self.c_estimation_ptr.set_method(<IodeEquationMethod>value)

    def get_instruments(self) -> Union[str, List[str]]:
        _instruments = self.c_estimation_ptr.get_instruments().decode().split(';')
        return _instruments[0] if len(_instruments) == 1 else _instruments

    def set_instruments(self, value: str):
        self.c_estimation_ptr.set_instruments(value.encode())

    def update_scalars(self):
        self.c_estimation_ptr.update_scalars()

    def get_scalars_db(self) -> Scalars:
        cdef KDBScalars* c_scalars_ptr = self.c_estimation_ptr.get_scalars()
        if c_scalars_ptr is NULL:
            return None
        # TODO : remove lines below when self.c_estimation_ptr.get_scalars() will return 
        #        a shared_ptr<KDBScalars> instead of a raw pointer KDBScalars*
        cdef shared_ptr[KDBScalars] scalars_ptr = make_shared[KDBScalars](<bint>False)
        scalars_db = Scalars._from_ptr(scalars_ptr)
        scalars_db.database = c_scalars_ptr
        scalars_db.abstract_database = c_scalars_ptr
        # ---- END TODO ----
        # scalars_db = Scalars._from_ptr(scalars_ptr)
        return scalars_db

    def get_equations_list(self) -> List[str]:
        return [eq_name.decode() for eq_name in self.c_estimation_ptr.get_list_equations()]

    def get_equations_db(self) -> Equations:
        cdef KDBEquations* c_equations_ptr = self.c_estimation_ptr.get_equations()
        if c_equations_ptr is NULL:
            return None
        # TODO : remove lines below when self.c_estimation_ptr.get_equations() will return 
        #        a shared_ptr<KDBEquations> instead of a raw pointer KDBEquations*
        cdef shared_ptr[KDBEquations] equations_ptr = make_shared[KDBEquations](<bint>False) 
        equations_db = Equations._from_ptr(equations_ptr)
        equations_db.database = c_equations_ptr
        equations_db.abstract_database = c_equations_ptr
        # ---- END TODO ----
        # equations_db = Equations._from_ptr(equations_ptr)
        return equations_db

    def update_current_equation(self, lec: str, comment: str):
        self.c_estimation_ptr.update_current_equation(lec.encode(), comment.encode())

    def get_current_equation(self) -> Equation:
        cdef shared_ptr[CEquation] c_current_eq = self.c_estimation_ptr.current_equation()
        eq = Equation._from_ptr(c_current_eq)
        return eq

    def get_next_equation(self) -> Equation:
        cdef shared_ptr[CEquation] c_next_eq = self.c_estimation_ptr.next_equation()
        eq = Equation._from_ptr(c_next_eq)
        return eq

    def get_correlation_matrix(self) -> CythonCorrelationMatrix:
        # WARNING: The pointer to the CCorrelationMatrix object is deleted by the 
        #          CEditAndEstimateEquations instance
        cdef shared_ptr[CCorrelationMatrix] corr_matrix_ptr = self.c_estimation_ptr.get_correlation_matrix()
        if corr_matrix_ptr.get() is NULL:
            return None
        corr_matrix = CythonCorrelationMatrix._from_ptr(corr_matrix_ptr)
        return corr_matrix

    def get_observed_values(self, name: str) -> List[float]:
        cdef string s_name = name.encode('utf-8')
        return [value for value in self.c_estimation_ptr.get_observed_values(s_name)]

    def get_fitted_values(self, name: str) -> List[float]:
        cdef string s_name = name.encode('utf-8')
        return [value for value in self.c_estimation_ptr.get_fitted_values(s_name)]

    def get_residual_values(self, name: str) -> List[float]:
        cdef string s_name = name.encode('utf-8')
        return [value for value in self.c_estimation_ptr.get_residual_values(s_name)]

    def estimate(self, maxit: int, epsilon: float):
        self.c_estimation_ptr.estimate(maxit, epsilon)

    def get_is_done(self) -> bool:
        return self.c_estimation_ptr.is_estimation_done()

    def save(self, from_period: str, to_period: str) -> List[str]:
        cdef vector[string] v_new_eqs = self.c_estimation_ptr.save(from_period.encode(), to_period.encode())
        return [name.decode() for name in v_new_eqs]
