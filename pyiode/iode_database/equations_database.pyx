from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional, Any

cimport cython
from cython.operator cimport dereference
from libcpp.memory cimport shared_ptr

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
    cdef shared_ptr[KDBEquations] database_ptr
    cdef KDBEquations* database

    def __cinit__(self, filepath: str=None) -> Equations:
        self.database_ptr = cpp_global_equations
        self.database = self.database_ptr.get()
        self.abstract_database = self.database

    def __dealloc__(self):
        self.database_ptr.reset()
        self.database = NULL

    @staticmethod
    cdef Equations _from_ptr(shared_ptr[KDBEquations] database_ptr):
        # call to __new__() that bypasses the __init__() constructor.
        cdef Equations wrapper = Equations.__new__(Equations)
        if database_ptr.get() is not NULL:
            wrapper.database_ptr = database_ptr
        else:
            wrapper.database_ptr = cpp_global_equations
        wrapper.database = wrapper.database_ptr.get()
        wrapper.abstract_database = wrapper.database
        return wrapper

    def _load(self, filepath: str):
        if self.database is not NULL:
            self.database.load(filepath.encode())

    def initialize_subset(self, pattern: str, copy: bool) -> Equations:
        cdef shared_ptr[KDBEquations] subset_db_ptr = self.database.get_subset(pattern.encode(), <bint>copy)
        subset = Equations._from_ptr(subset_db_ptr)
        return subset

    def get_lec(self, name: str) -> str:
        return self.database.get_lec(name.encode()).decode()

    def _get_object(self, name: str) -> Equation:
        cdef shared_ptr[CEquation] eq_ptr
        name = name.strip()
        eq_ptr = self.database.get_obj_ptr(name.encode())
        eq = Equation._from_ptr(eq_ptr)
        return eq

    def _set_object(self, name: str, eq: Equation) -> Equation:
        cdef shared_ptr[CEquation] eq_ptr
        # NOTE: the C++ set_obj_ptr() method creates a copy of the object stored 
        #       in the passed shared pointer to avoid that two entries (shared_ptr<T>) 
        #       in the IODE database point to the same object in memory
        eq_ptr = self.database.set_obj_ptr(name.encode(), eq.eq_ptr)
        eq.update_ptr(eq_ptr)
        return eq

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
        self.database.copy_from(input_files.encode(), names.encode())

    def merge(self, other: Equations, overwrite: bool=True):        
        cdef KDBEquations* other_database = other.database
        self.database.merge(dereference(other_database), <bint>overwrite, <bint>False)

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
        return hash_value(dereference(self.database))


