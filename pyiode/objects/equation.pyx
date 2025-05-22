from copy import copy
from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional

from libcpp.map cimport map
from libcpp.string cimport string
from libc.string cimport memset
from cython.operator cimport dereference

from pyiode.common cimport IodeEquationMethod, IodeEquationTest
from pyiode.objects.equation cimport EQ
from pyiode.objects.equation cimport CEquation
from pyiode.objects.equation cimport B_EqsStepWise
from pyiode.objects.equation cimport hash_value as hash_value_eq
from pyiode.iode_database.cpp_api_database cimport KDBEquations as CKDBEquations


cdef class Equation:
    cdef bint ptr_owner
    cdef CEquation* c_equation
    cdef CKDBEquations* c_database

    def __cinit__(self):
        self.ptr_owner = False
        self.c_database = NULL
        self.c_equation = NULL

    def __init__(self, endogenous: str, lec: str, from_period: str, to_period: str, comment: str, 
                 instruments: str, block: str) -> Equation:
        self.ptr_owner = <bint>True
        self.c_database = NULL
        self.c_equation = new CEquation(endogenous.encode(), lec.encode(), <IodeEquationMethod>(0), from_period.encode(), 
                                        to_period.encode(), comment.encode(), instruments.encode(), block.encode(), <bint>False)

    def __dealloc__(self):
        if self.ptr_owner and self.c_equation is not NULL:
            del self.c_equation
            self.c_equation = NULL
            self.c_database = NULL

    @staticmethod
    cdef Equation _from_ptr(CEquation* ptr, bint owner=False, CKDBEquations* c_database=NULL):
        """
        Factory function to create Equation objects from a given CEquation pointer.
        """
        # Fast call to __new__() that bypasses the __init__() constructor.
        cdef Equation wrapper = Equation.__new__(Equation)
        wrapper.c_equation = ptr
        wrapper.c_database = c_database
        wrapper.ptr_owner = owner
        return wrapper

    def get_formated_date(self, format: str='dd-mm-yyyy') -> str:
        return self.c_equation.get_date_as_string(format.encode()).decode()

    def get_coefficients(self) -> List[str]:
        return [coeff.decode() for coeff in self.c_equation.get_coefficients_list(<bint>False)]

    def _get_and_create_coefficients(self) -> List[str]:
        return [coeff.decode() for coeff in self.c_equation.get_coefficients_list(<bint>True)]

    def get_variables(self) -> List[str]:
        return [var.decode() for var in self.c_equation.get_variables_list(<bint>False)]

    def _get_and_create_variables(self) -> List[str]:
        return [var.decode() for var in self.c_equation.get_variables_list(<bint>True)]

    def split_equation(self) -> Tuple[str, str]:
        lhs, rhs = self.c_equation.split_equation()
        return lhs.decode(), rhs.decode() 

    def estimate(self, from_period: str=None, to_period: str=None) -> bool:
        if from_period is None or to_period is None:
            c_sample = cpp_global_variables.get_sample()
            if from_period is None:
                from_period = c_sample.start_period().to_string().decode()
            if to_period is None:
                to_period = c_sample.end_period().to_string().decode()
        
        try:
            cpp_eqs_estimate(self.c_equation.get_endo(), from_period.encode(), to_period.encode())
            return True
        except Exception as e:
            warnings.warn(str(e), RuntimeWarning)
            return False

    def estimate_step_wise(self, from_period: Union[str, None], to_period: Union[str, None], 
                           lec_condition: str, test: str) -> bool:
        cdef bytes b_arg
        cdef char* c_arg = NULL
        cdef int res
        
        if from_period is None or to_period is None:
            c_sample = cpp_global_variables.get_sample()
            if from_period is None:
                from_period = c_sample.start_period().to_string().decode()
            if to_period is None:
                to_period = c_sample.end_period().to_string().decode()

        arg: str = f'{from_period} {to_period} {self.get_endogenous()} "{lec_condition}" {test}'
        b_arg = arg.encode()
        c_arg = b_arg
        
        res = B_EqsStepWise(c_arg)
        return res == 0

    cdef void update_owner_database(self):
        if self.c_database is not NULL:
            self.c_database.update(self.c_equation.get_endo(), dereference(self.c_equation))

    cdef void reset_date_and_tests(self):
        self.c_equation.date = 0L
        memset(&self.c_equation.tests, 0, EQS_NBTESTS * sizeof(float))

    def get_endogenous(self) -> str:
        return self.c_equation.get_endo().decode()

    def get_lec(self) -> str:
        return self.c_equation.get_lec().decode()

    def set_lec(self, value: str):
        value = value.strip()
        self.c_equation.set_lec(value.encode())
        self.reset_date_and_tests()
        self.update_owner_database()

    def get_method(self) -> str:
        return EqMethod(<int>(self.c_equation.get_method_as_int())).name

    def set_method(self, value: int):
        self.c_equation.set_method(<IodeEquationMethod>(value))
        self.reset_date_and_tests()
        self.update_owner_database()

    def get_sample(self) -> Sample:
        cdef CSample sample = self.c_equation.get_sample()
        return Sample._from_ptr(new CSample(sample), <bint>True)

    def set_sample(self, from_period: str, to_period: str):
        self.c_equation.set_sample(from_period.encode(), to_period.encode())
        self.reset_date_and_tests()
        self.update_owner_database()

    def get_comment(self) -> str:
        return self.c_equation.get_comment().decode()

    def set_comment(self, value: str):
        self.c_equation.set_comment(value.encode())
        self.update_owner_database()

    def get_instruments(self) -> Union[str, List[str]]:
        _instruments = self.c_equation.get_instruments().decode().split(';')
        return _instruments[0] if len(_instruments) == 1 else _instruments

    def set_instruments(self, value: str):
        self.c_equation.set_instruments(value.encode())
        self.reset_date_and_tests()
        self.update_owner_database()

    def get_block(self) -> str:
        return self.c_equation.get_block().decode()

    def set_block(self, value: str):
        self.c_equation.set_block(value.encode())
        self.reset_date_and_tests()
        self.update_owner_database()

    def get_tests(self) -> Dict[str, float]:
        cdef map[string, float] cpp_tests = self.c_equation.get_tests_as_map()
        return {item.first.decode(): item.second for item in cpp_tests}

    def _get_list_tests(self) -> List[float]:
        return [self.c_equation.tests[i] for i in range(len(EqTest))]

    def _set_tests_from_list(self, tests: List[float]):
        for i, value in enumerate(tests):
            self.c_equation.set_test(<IodeEquationTest>i, value)
        self.update_owner_database()

    def _set_date(self, value: str, format: str='dd-mm-yyyy'):
        if not value:
            self.c_equation.reset_date()
        else:    
            self.c_equation.set_date(value.encode(), format.encode())
        self.update_owner_database()

    def equal(self, other: Equation) -> bool:
        return self.c_equation == other.c_equation

    def __hash__(self) -> int:
        return <int>hash_value_eq(dereference(self.c_equation))


