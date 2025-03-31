from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

cimport cython
from cython.operator cimport dereference
from pyiode.objects.equation cimport CEquation
from pyiode.iode_database.cpp_api_database cimport hash_value
from pyiode.iode_database.cpp_api_database cimport KDBEquations as CKDBEquations
from pyiode.iode_database.cpp_api_database cimport Equations as cpp_global_equations
from pyiode.iode_database.cpp_api_database cimport Variables as cpp_global_variables
from pyiode.iode_database.cpp_api_database cimport eqs_estimate as cpp_eqs_estimate
from pyiode.iode_database.cpp_api_database cimport KCPTR, KIPTR, KLPTR, KVPTR
from pyiode.iode_database.cpp_api_database cimport B_EQS_INFOS, B_PrintObjEqsInfos
from pyiode.iode_database.cpp_api_database cimport B_EQS_LEC, B_PrintObjLec

import pandas as pd
from iode.common import PrintEquationsAs, PrintEquationsLecAs


cdef class Equations(IodeDatabase):
    cdef bint ptr_owner
    cdef CKDBEquations* database_ptr

    def __cinit__(self, filepath: str=None) -> Equations:
        self.database_ptr = NULL
        self.abstract_db_ptr = NULL
        self.ptr_owner = False

    def __dealloc__(self):
        if self.ptr_owner and self.database_ptr is not NULL:
            del self.database_ptr
            self.database_ptr = NULL

    @staticmethod
    cdef Equations _from_ptr(CKDBEquations* database_ptr = NULL, bint owner=False):
        # call to __new__() that bypasses the __init__() constructor.
        cdef Equations wrapper = Equations.__new__(Equations)
        if database_ptr is not NULL:
            wrapper.ptr_owner = owner
            wrapper.database_ptr = database_ptr
            wrapper.abstract_db_ptr = database_ptr
        else:
            wrapper.ptr_owner = False
            wrapper.database_ptr = &cpp_global_equations
            wrapper.abstract_db_ptr = &cpp_global_equations
        return wrapper

    @staticmethod
    def __init_instance(instance: Equations) -> Self:
        instance.ptr_owner = False
        instance.database_ptr = &cpp_global_equations
        instance.abstract_db_ptr = &cpp_global_equations
        return instance

    def _load(self, filepath: str):
        cdef CKDBEquations* kdb = new CKDBEquations(filepath.encode())
        del kdb

    def _subset(self, pattern: str, copy: bool) -> Equations:
        subset_db: Equations = self._new_instance()
        subset_db.database_ptr = subset_db.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        return subset_db

    def get_lec(self, key: Union[str, int]) -> str:
        if isinstance(key, int):
            return self.database_ptr.get_lec(<int>key).decode()
        else:
            return self.database_ptr.get_lec(<string>(key.encode())).decode()

    def _get_object(self, key: Union[str, int], eq: Equation) -> Equation:
        cdef CEquation* c_eq
        if isinstance(key, int):
            c_eq = self.database_ptr.get(<int>key)
        else:
            key = key.strip()
            c_eq = self.database_ptr.get(<string>(key.encode()))
        
        eq.c_equation = c_eq
        eq.c_database = self.database_ptr
        eq.ptr_owner = <bint>True
        return eq

    def _set_object(self, key: Union[str, int], value):
        cdef CEquation* c_equation

        if isinstance(key, str):
            key = key.strip()

        # update existing equation
        if isinstance(key, int) or self.database_ptr.contains(key.encode()):
            endogenous = key

        if isinstance(value, pd.Series):
            value = value.to_dict()

        # update existing equation
        if isinstance(key, int) or self.database_ptr.contains(key.encode()):
            if isinstance(value, str):
                equation = self._get_object(key)
                equation.lec = value
            elif isinstance(value, Equation):
                equation = value
            elif isinstance(value, dict):
                equation = self._get_object(key)
                if 'lec' in value:
                    equation.lec = value.pop('lec')
                if 'method' in value:
                    equation.method = value.pop('method')
                if 'sample' in value:
                    equation.sample = value.pop('sample')
                if 'comment' in value:
                    equation.comment = value.pop('comment')
                if 'instruments' in value:
                    equation.instruments = value.pop('instruments')
                if 'block' in value:
                    equation.block = value.pop('block')
                if 'tests' in value:
                    warnings.warn(f"Cannot update equation '{key}': 'tests' cannot be updated manually. Skipped new values.")
                    del value['tests']
                if 'date' in value:
                    warnings.warn(f"Cannot update equation '{key}': 'date' cannot be updated manually. Skipped new value.")
                    del value['date']
                if len(value):
                    raise ValueError(f"Cannot update equation '{key}': only 'lec', 'method', 'sample', 'comment', "
                                     f"'instruments' and 'block' keys are accepted. "
                                     f"Got unknown key(s): {';'.join(value.keys())}")
            else:
                raise TypeError(f"Cannot update equation '{key}': Expected input to be of type str or dict or Equation. "
                                f"Got value of type {type(value).__name__}")

            c_equation = (<Equation>equation).c_equation
            if isinstance(key, int):
                self.database_ptr.update(<int>key, dereference(c_equation))
            else:
                self.database_ptr.update(<string>(key.encode()), dereference(c_equation))
        # add a new equation
        else:
            if isinstance(value, str):
                equation = Equation(endogenous=key, lec=value.strip())
            elif isinstance(value, Equation):
                equation = value
            elif isinstance(value, (tuple, list)):
                value = [key] + list(value)
                equation = Equation(*value)
            elif isinstance(value, dict):
                value['endogenous'] = key
                equation = Equation(**value)
            else:
                raise TypeError(f"Cannot add equation '{key}': Expected input to be of type str or tuple or list or "
                                f"dict or Equation. Got value of type {type(value).__name__}")
            
            c_equation = (<Equation>equation).c_equation
            self.database_ptr.add(key.encode(), dereference(c_equation))

    def estimate(self, from_period: Union[str, Period]=None, to_period: Union[str, Period]=None, list_eqs: Union[str, List[str]]=None) -> bool:
        if from_period is None or to_period is None:
            c_sample = cpp_global_variables.get_sample()
            if from_period is None:
                from_period = c_sample.start_period().to_string().decode()
            if to_period is None:
                to_period = c_sample.end_period().to_string().decode()
        
        if isinstance(from_period, Period):
            from_period = str(from_period)

        if isinstance(to_period, Period):
            to_period = str(to_period)

        if list_eqs is None:
            list_eqs = ';'.join(self.names)

        if not isinstance(list_eqs, str) and isinstance(list_eqs, Iterable) and \
            all(isinstance(item, str) for item in list_eqs):
            list_eqs = ';'.join(list_eqs)
        
        try:
            cpp_eqs_estimate(list_eqs.encode(), from_period.encode(), to_period.encode())
            return True
        except RuntimeError as e:
            warnings.warn(str(e), RuntimeWarning)
            return False

    def copy_from(self, input_files: Union[str, List[str]], names: Union[str, List[str]]='*'):
        if not (self.is_global_workspace or self.is_detached):
            raise RuntimeError("Cannot call 'copy_from' method on a subset of a workspace")
        input_files, names = self._copy_from(input_files, names)
        self.database_ptr.copy_from(input_files.encode(), names.encode())

    def get_print_equations_as(self) -> PrintEquationsAs:
        return PrintEquationsAs(B_EQS_INFOS)

    def set_print_equations_as(self, value: Union[PrintEquationsAs, str]):
        if isinstance(value, str):
            upper_str = value.upper()
            if upper_str not in PrintEquationsAs.__members__:
                raise ValueError(f"Invalid value '{value}'. "
                                 f"Expected one of {', '.join(PrintEquationsAs.__members__.keys())}. ")
            value = PrintEquationsAs[upper_str]
        value = str(int(value))
        B_PrintObjEqsInfos(value.encode())

    def get_print_equations_lec_as(self) -> PrintEquationsLecAs:
        return PrintEquationsLecAs(B_EQS_LEC)

    def set_print_equations_lec_as(self, value: Union[PrintEquationsLecAs, str]):
        if isinstance(value, str):
            upper_str = value.upper()
            if upper_str not in PrintEquationsLecAs.__members__:
                raise ValueError(f"Invalid value '{value}'. "
                                 f"Expected one of {', '.join(PrintEquationsLecAs.__members__.keys())}. ")
            value = PrintEquationsLecAs[upper_str]
        value = str(int(value))
        B_PrintObjLec(value.encode())

    def get_print_equations_lec_as(self) -> PrintEquationsLecAs:
        return PrintEquationsLecAs(B_EQS_LEC)

    def set_print_equations_lec_as(self, value: Union[PrintEquationsLecAs, str]):
        if isinstance(value, str):
            upper_str = value.upper()
            if upper_str not in PrintEquationsLecAs.__members__:
                raise ValueError(f"Invalid value '{value}'. "
                                 f"Expected one of {', '.join(PrintEquationsLecAs.__members__.keys())}. ")
            value = PrintEquationsLecAs[upper_str]
        value = str(int(value))
        B_PrintObjLec(value.encode())

    def __hash__(self) -> int:
        return hash_value(dereference(self.database_ptr))


