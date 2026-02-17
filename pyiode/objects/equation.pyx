from copy import copy
from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional

from libcpp.map cimport map
from libcpp.string cimport string
from libc.string cimport memset
from cython.operator cimport dereference

from pyiode.common cimport IodeEquationMethod, IodeEquationTest
from pyiode.objects.equation cimport CEquation
from pyiode.objects.equation cimport B_EqsStepWise
from pyiode.objects.equation cimport hash_value as hash_value_eq
from pyiode.iode_database.cpp_api_database cimport KDBEquations as KDBEquations
from pyiode.iode_database.cpp_api_database cimport global_ws_eqs as cpp_global_equations


cdef class Equation:
    cdef bint ptr_owner
    cdef CEquation* c_equation
    cdef KDBEquations* c_database
    cdef bint lock              # prevent extract_eq_from_database() to be executed

    def __cinit__(self):
        self.ptr_owner = False
        self.c_database = NULL
        self.c_equation = NULL
        self.lock = False

    def __init__(self, endogenous: str, lec: str, from_period: str, to_period: str, comment: str, 
                 instruments: str, block: str) -> Equation:
        self.ptr_owner = <bint>True
        self.c_database = NULL
        self.lock = False
        self.c_equation = new CEquation(endogenous.encode(), lec.encode(), <IodeEquationMethod>(0), from_period.encode(), 
                                        to_period.encode(), comment.encode(), instruments.encode(), block.encode(), <bint>False)

    def __dealloc__(self):
        if self.ptr_owner and self.c_equation is not NULL:
            del self.c_equation
            self.c_equation = NULL
            self.c_database = NULL
            self.lock = False

    cdef void update_owner_database(self):
        if self.c_database is not NULL:
            self.c_database.update(self.c_equation.endo, dereference(self.c_equation))

    cdef void extract_eq_from_database(self):
        cdef string endo
        # if c_equation belongs to a database, we need to (re)extract it from the database 
        # every time we need to access one of its properties since processes like estimation
        # can modify the equation
        if self.lock:
            return
        if self.c_database is not NULL:
            endo = self.c_equation.endo
            if self.ptr_owner and self.c_equation is not NULL:
                del self.c_equation
            self.c_equation = self.c_database.get(endo)
            self.ptr_owner = True

    @staticmethod
    cdef Equation _from_ptr(CEquation* ptr, bint owner=False, KDBEquations* c_database=NULL):
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
        self.extract_eq_from_database()
        return self.c_equation.get_date_as_string(format.encode()).decode()

    def get_coefficients(self) -> List[str]:
        self.extract_eq_from_database()
        return [coeff.decode() for coeff in self.c_equation.get_coefficients_list(<bint>False)]

    def _get_and_create_coefficients(self) -> List[str]:
        self.extract_eq_from_database()
        return [coeff.decode() for coeff in self.c_equation.get_coefficients_list(<bint>True)]

    def get_variables(self) -> List[str]:
        self.extract_eq_from_database()
        return [var.decode() for var in self.c_equation.get_variables_list(<bint>False)]

    def _get_and_create_variables(self) -> List[str]:
        self.extract_eq_from_database()
        return [var.decode() for var in self.c_equation.get_variables_list(<bint>True)]

    def split_equation(self) -> Tuple[str, str]:
        self.extract_eq_from_database()
        lhs, rhs = self.c_equation.split_equation()
        return lhs.decode(), rhs.decode() 

    def estimate(self, from_period: str, to_period: str, maxit: int, epsilon: float) -> bool:
        cdef CEquation* c_global_equation = NULL
        cdef string eq_name = self.c_equation.endo

        if from_period is None or to_period is None:
            c_sample = cpp_global_variables.get().get_sample()
            if from_period is None:
                from_period = c_sample.start_period.to_string().decode()
            if to_period is None:
                to_period = c_sample.end_period.to_string().decode()
        
        try:
            # NOTE: In the C API, the estimation is made on equations stored in the global equations workspace. 
            cpp_eqs_estimate(eq_name, from_period.encode(), to_period.encode(), maxit, epsilon)
            
            # copy tests values and estimation date from the global equations workspace
            c_global_equation = cpp_global_equations.get().get(eq_name)
            self.c_equation.set_sample(from_period.encode(), to_period.encode())
            self.c_equation.date = c_global_equation.date
            memcpy(&self.c_equation.tests, &c_global_equation.tests, EQS_NBTESTS * sizeof(float))
            del c_global_equation
            
            return True
        except Exception as e:
            warnings.warn(str(e), RuntimeWarning)
            return False

    def estimate_step_wise(self, from_period: Union[str, None], to_period: Union[str, None], 
                           lec_condition: str, test: str) -> bool:
        cdef bytes b_arg
        cdef char* c_arg = NULL
        cdef int res
        cdef CEquation* c_global_equation = NULL
        cdef string eq_name = self.c_equation.endo
        
        if from_period is None or to_period is None:
            c_sample = cpp_global_variables.get().get_sample()
            if from_period is None:
                from_period = c_sample.start_period.to_string().decode()
            if to_period is None:
                to_period = c_sample.end_period.to_string().decode()

        arg: str = f'{from_period} {to_period} {self.get_endogenous()} "{lec_condition}" {test}'
        b_arg = arg.encode()
        c_arg = b_arg
        
        res = B_EqsStepWise(c_arg)

        # copy tests values and estimation date from the global equations workspace
        if res == 0:
            c_global_equation = cpp_global_equations.get().get(eq_name)
            self.c_equation.set_sample(from_period.encode(), to_period.encode())
            self.c_equation.date = c_global_equation.date
            memcpy(&self.c_equation.tests, &c_global_equation.tests, EQS_NBTESTS * sizeof(float))
            del c_global_equation

        return res == 0

    def get_endogenous(self) -> str:
        return self.c_equation.endo.decode()

    def get_lec(self) -> str:
        self.extract_eq_from_database()
        return self.c_equation.lec.decode()

    def set_lec(self, value: str):
        value = value.strip()
        self.c_equation.set_lec(value.encode())
        self.update_owner_database()

    def get_method(self) -> str:
        self.extract_eq_from_database()
        return EqMethod(<int>(self.c_equation.get_method_as_int())).name

    def set_method(self, value: int):
        self.c_equation.set_method(<IodeEquationMethod>(value))
        self.update_owner_database()

    def get_sample(self) -> Sample:
        self.extract_eq_from_database()
        cdef CSample sample = self.c_equation.sample
        return Sample._from_ptr(new CSample(sample), <bint>True)

    def set_sample(self, from_period: str, to_period: str):
        self.c_equation.set_sample(from_period.encode(), to_period.encode())
        self.update_owner_database()

    def get_comment(self) -> str:
        self.extract_eq_from_database()
        return self.c_equation.get_comment().decode()

    def set_comment(self, value: str):
        self.c_equation.set_comment(value.encode())
        self.update_owner_database()

    def get_instruments(self) -> Union[str, List[str]]:
        self.extract_eq_from_database()
        _instruments = self.c_equation.instruments.decode().split(';')
        return _instruments[0] if len(_instruments) == 1 else _instruments

    def set_instruments(self, value: str):
        self.c_equation.instruments = value.encode()
        self.update_owner_database()

    def get_block(self) -> str:
        self.extract_eq_from_database()
        return self.c_equation.block.decode()

    def set_block(self, value: str):
        self.c_equation.block = value.encode()
        self.update_owner_database()

    def get_tests(self) -> Dict[str, float]:
        self.extract_eq_from_database()
        cdef map[string, float] cpp_tests = self.c_equation.get_tests_as_map()
        return {item.first.decode(): item.second for item in cpp_tests}

    def _get_list_tests(self) -> List[float]:
        self.extract_eq_from_database()
        return [self.c_equation.tests[i] for i in range(len(EqTest))]

    def _set_tests_from_list(self, tests: List[float]):
        for i, value in enumerate(tests):
            self.c_equation.set_test(<IodeEquationTest>i, value)
        self.update_owner_database()

    def reset_tests(self):
        self.c_equation.reset_tests()

    def _set_date(self, value: str, format: str='dd-mm-yyyy'):
        if not value:
            self.c_equation.reset_date()
        else:    
            self.c_equation.set_date(value.encode(), format.encode())
        self.update_owner_database()

    def equal(self, other: Equation) -> bool:
        self.extract_eq_from_database()
        return self.c_equation == other.c_equation

    def _str_(self) -> str:
        self.extract_eq_from_database()
        self.lock = True  # prevent extract_eq_from_database() to be executed

        sample = self.get_sample()
        tests = self.get_tests()
        indent = " " * len("Equation(")

        s = [f"endogenous = {self.get_endogenous()}"]
        s += [f"lec = {self.get_lec()}"]
        s += [f"method = {self.get_method()}"]
        if len(sample):
            s+= [f"sample = {sample}"]
        if self.get_comment():
            s += [f"comment = {self.get_comment()}"]
        if self.get_block():
            s += [f"block = {self.get_block()}"]
        if self.get_instruments():
            s += [f"instruments = {self.get_instruments()}"]
        if tests['corr'] > 0.0:
            indent_tests = " " * len("tests = ")
            s += ["tests = " + f",\n{indent}{indent_tests}".join(f"{key} = {value:g}" 
                                for key, value in self.get_tests().items())]
        if self.get_formated_date('dd-mm-yyyy'):
            s += [f"date = {self.get_formated_date('dd-mm-yyyy')}"]

        s = "Equation(" + f",\n{indent}".join(s) + ")"
        self.lock = False  # allow extract_eq_from_database() to be executed again
        return s

    def _repr_(self) -> str:
        self.extract_eq_from_database()
        self.lock = True  # prevent extract_eq_from_database() to be executed

        sample = self.get_sample()
        tests = self.get_tests()
        indent = " " * len("Equation(")

        s = [f"endogenous = {repr(self.get_endogenous())}"]
        s += [f"lec = {repr(self.get_lec())}"]
        s += [f"method = {repr(self.get_method())}"]
        if len(sample):
            s+= [f"from_period = '{sample.get_start()}'", f"to_period = '{sample.get_end()}'"]
        if self.get_comment():
            s += [f"comment = {repr(self.get_comment())}"]
        if self.get_block():
            s += [f"block = {repr(self.get_block())}"]
        if self.get_instruments():
            s += [f"instruments = {repr(self.get_instruments())}"]
        if tests['corr'] > 0.0:
            indent_tests = " " * len("tests = {")
            s += ["tests = {" + f",\n{indent}{indent_tests}".join(f"{key} = {value:g}" 
                                for key, value in self.get_tests().items()) + "}"]
        if self.get_formated_date('dd-mm-yyyy'):
            s += [f"date = {repr(self.get_formated_date('dd-mm-yyyy'))}"]

        s = "Equation(" + f",\n{indent}".join(s) + ")" 
        self.lock = False  # allow extract_eq_from_database() to be executed again
        return s

    def __hash__(self) -> int:
        self.extract_eq_from_database()
        return <int>hash_value_eq(dereference(self.c_equation))


