from libcpp.string cimport string
from libcpp.vector cimport vector

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


def cython_dickey_fuller_test(scalars_db: Scalars, lec: str, drift: bool, trend: bool, order: int) -> Scalars:
    # NOTE: cpp_dickey_fuller_test allocates a new KDBScalars* pointer
    cdef KDBScalars* cpp_scalars
    cpp_scalars = cpp_dickey_fuller_test(lec.encode(), <bint>drift, <bint>trend, order)
    if cpp_scalars is NULL:
        return None

    scalars_db.ptr_owner = <bint>True
    scalars_db.database_ptr = cpp_scalars
    scalars_db.abstract_db_ptr = cpp_scalars
    return scalars_db


cdef class CythonCorrelationMatrix:
    cdef bint ptr_owner
    cdef CCorrelationMatrix* c_correlation_matrix

    def __cinit__(self):
        self.ptr_owner = False
        self.c_correlation_matrix = NULL

    def __init__(self):
        raise TypeError("This class cannot be instantiated directly")

    def __dealloc__(self):
        if self.ptr_owner and self.c_correlation_matrix is not NULL:
            del self.c_correlation_matrix
            self.c_correlation_matrix = NULL

    @staticmethod
    cdef CythonCorrelationMatrix from_ptr(CCorrelationMatrix* ptr, bint owner=False):
        """
        Factory function to create CythonCorrelationMatrix objects from a given CCorrelationMatrix pointer.
        """
        # Fast call to __new__() that bypasses the __init__() constructor.
        cdef CythonCorrelationMatrix wrapper = CythonCorrelationMatrix.__new__(CythonCorrelationMatrix)
        wrapper.c_correlation_matrix = ptr
        wrapper.ptr_owner = owner
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

    def get_scalars_db(self, scalars_db: Scalars) -> Scalars:
        cdef KDBScalars* c_scalars_ptr = self.c_estimation_ptr.get_scalars()
        if c_scalars_ptr is NULL:
            return None
        scalars_db.ptr_owner = <bint>False
        scalars_db.database_ptr = c_scalars_ptr
        scalars_db.abstract_db_ptr = c_scalars_ptr
        return scalars_db

    def get_equations_list(self) -> List[str]:
        return [eq_name.decode() for eq_name in self.c_estimation_ptr.get_list_equations()]

    def get_equations_db(self, equations_db: Equations) -> Equations:
        cdef KDBEquations* c_equations_ptr = self.c_estimation_ptr.get_equations()
        if c_equations_ptr is NULL:
            return None
        equations_db.ptr_owner = <bint>False
        equations_db.database_ptr = c_equations_ptr
        equations_db.abstract_db_ptr = c_equations_ptr
        return equations_db

    def update_current_equation(self, lec: str, comment: str):
        self.c_estimation_ptr.update_current_equation(lec.encode(), comment.encode())

    def get_current_equation(self, eq: Equation) -> Equation:
        cdef CEquation* c_current_eq = self.c_estimation_ptr.current_equation()
        if c_current_eq is NULL:
            return None
        eq.ptr_owner = <bint>False
        eq.c_equation = c_current_eq
        return eq

    def get_next_equation(self, eq: Equation) -> Equation:
        cdef CEquation* c_next_eq = self.c_estimation_ptr.next_equation()
        if c_next_eq is NULL:
            return None
        eq.ptr_owner = <bint>False
        eq.c_equation = c_next_eq
        return eq

    def get_correlation_matrix(self, corr_matrix: CythonCorrelationMatrix) -> CythonCorrelationMatrix:
        # WARNING: The pointer to the CCorrelationMatrix object is deleted by the 
        #          CEditAndEstimateEquations instance
        cdef CCorrelationMatrix* c_corr_matrix = self.c_estimation_ptr.get_correlation_matrix()
        if c_corr_matrix is NULL:
            return None
        corr_matrix.ptr_owner = <bint>False
        corr_matrix.c_correlation_matrix = c_corr_matrix
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
