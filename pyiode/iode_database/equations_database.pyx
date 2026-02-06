from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional, Any

cimport cython
from cython.operator cimport dereference
from pyiode.objects.equation cimport CEquation
from pyiode.iode_database.cpp_api_database cimport hash_value
from pyiode.iode_database.cpp_api_database cimport KDBEquations
from pyiode.iode_database.cpp_api_database cimport global_ws_eqs as cpp_global_equations
from pyiode.iode_database.cpp_api_database cimport global_ws_var as cpp_global_variables
from pyiode.iode_database.cpp_api_database cimport eqs_estimate as cpp_eqs_estimate
from pyiode.iode_database.cpp_api_database cimport B_EQS_INFOS, B_PrintObjEqsInfos
from pyiode.iode_database.cpp_api_database cimport B_EQS_LEC, B_PrintObjLec

import pandas as pd


cdef class Equations(CythonIodeDatabase):
    cdef bint ptr_owner
    cdef KDBEquations* database_ptr

    def __cinit__(self, filepath: str=None) -> Equations:
        self.ptr_owner = False
        self.database_ptr = cpp_global_equations.get()
        self.abstract_db_ptr = cpp_global_equations.get()

    def __dealloc__(self):
        if self.ptr_owner and self.database_ptr is not NULL:
            del self.database_ptr
            self.database_ptr = NULL

    @staticmethod
    cdef Equations _from_ptr(KDBEquations* database_ptr = NULL, bint owner=False):
        # call to __new__() that bypasses the __init__() constructor.
        cdef Equations wrapper = Equations.__new__(Equations)
        if database_ptr is not NULL:
            wrapper.ptr_owner = owner
            wrapper.database_ptr = database_ptr
            wrapper.abstract_db_ptr = database_ptr
        else:
            wrapper.ptr_owner = False
            wrapper.database_ptr = cpp_global_equations.get()
            wrapper.abstract_db_ptr = cpp_global_equations.get()
        return wrapper

    def _load(self, filepath: str):
        if self.database_ptr is not NULL:
            self.database_ptr.load(filepath.encode())

    def initialize_subset(self, subset: Equations, pattern: str, copy: bool) -> Equations:
        subset.database_ptr = new KDBEquations(self.database_ptr, pattern.encode(), <bint>copy)
        subset.abstract_db_ptr = subset.database_ptr
        subset.ptr_owner = True
        return subset

    def get_lec(self, name: str) -> str:
        return self.database_ptr.get_lec(name.encode()).decode()

    def _get_object(self, name: str, eq: Equation) -> Equation:
        cdef CEquation c_eq
        name = name.strip()
        c_eq = self.database_ptr.get(name.encode())
        eq.c_equation = new CEquation(c_eq)
        eq.ptr_owner = <bint>True
        return eq

    def _set_object(self, name: str, eq: Equation):
        cdef CEquation* equation_ptr = eq.c_equation
        self.database_ptr.set_obj_ptr(name.encode(), equation_ptr)

    def estimate(self, from_period: Union[str, Period], to_period: Union[str, Period], 
                 list_eqs: Union[str, List[str]], maxit: int, epsilon: float) -> bool:
        if from_period is None or to_period is None:
            c_sample = cpp_global_variables.get().get_sample()
            if from_period is None:
                from_period = c_sample.start_period.to_string().decode()
            if to_period is None:
                to_period = c_sample.end_period.to_string().decode()
        
        if isinstance(from_period, Period):
            from_period = str(from_period)

        if isinstance(to_period, Period):
            to_period = str(to_period)

        if list_eqs is None:
            list_eqs = ';'.join(self.property_names())

        if not isinstance(list_eqs, str) and isinstance(list_eqs, Iterable) and \
            all(isinstance(item, str) for item in list_eqs):
            list_eqs = ';'.join(list_eqs)
        
        try:
            cpp_eqs_estimate(list_eqs.encode(), from_period.encode(), to_period.encode(), maxit, epsilon)
            return True
        except RuntimeError as e:
            warnings.warn(str(e), RuntimeWarning)
            return False

    def copy_from(self, input_files: str, names: str='*'):
        self.database_ptr.copy_from(input_files.encode(), names.encode())

    def merge(self, other: Equations, overwrite: bool=True):        
        cdef KDBEquations* other_db_ptr = other.database_ptr
        self.database_ptr.merge(dereference(other_db_ptr), <bint>overwrite, <bint>False)

    # Information detail to print (for equations)
    #    0: equation only 
    #    1: equation + comment
    #    2: equation + comment + estimation results
    def get_print_equations_as(self) -> int:
        return B_EQS_INFOS

    def set_print_equations_as(self, value: str):
        B_PrintObjEqsInfos(value.encode())

    # Specify how to print a LEC expression 
    #    0 : print the LEC form as is
    #    1 : replace all scalars by their values
    #    2 : replaced all scalars by their values + t-tests
    def get_print_equations_lec_as(self) -> int:
        return B_EQS_LEC

    def set_print_equations_lec_as(self, value: str):
        B_PrintObjLec(value.encode())

    def __hash__(self) -> int:
        return hash_value(dereference(self.database_ptr))


