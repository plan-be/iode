# distutils: language = c++

from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

cimport cython
from cython.operator cimport dereference
from pyiode.objects.equation cimport CEquation
from pyiode.iode_database.database cimport KDBEquations as CKDBEquations
from pyiode.iode_database.database cimport Equations as cpp_global_equations

EquationInput = Union[str, Dict[str, Any], Equation]


@cython.final
cdef class Equations(_AbstractDatabase):
    """
    IODE Equations database. 

    Attributes
    ----------
    iode_type: str
    filename: str
    description: str

    Parameters
    ----------
    filepath: str, optional
        file containing the IODE equations to load.

    Returns
    -------
    Equations

    Examples
    --------
    >>> from iode import equations, SAMPLE_DATA_DIR
    >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
    >>> len(equations)
    274
    """

    cdef CKDBEquations* database_ptr

    def __cinit__(self, filepath: str = None) -> Equations:
        self.database_ptr = NULL
        self.abstract_db_ptr = NULL

    def __init__(self, filepath: str = None):
        # Prevent accidental instantiation from normal Python code
        # since we cannot pass a struct pointer into a Python constructor.
        raise TypeError("This class cannot be instantiated directly.")

    def __dealloc__(self):
        # self.database_ptr points to the C++ global instance Equations 
        # which does not need to be manually deleted 
        pass

    # see https://cython.readthedocs.io/en/stable/src/userguide/extension_types.html#instantiation-from-existing-c-c-pointers 
    @staticmethod
    def _get_instance() -> Equations:
        # call to __new__() that bypasses the __init__() constructor.
        cdef Equations wrapper = Equations.__new__(Equations)
        wrapper.database_ptr = &cpp_global_equations
        wrapper.abstract_db_ptr = &cpp_global_equations
        return wrapper

    # TODO: implement KDBAbstract::load() method (for global KDB only)
    def _load(self, filepath: str):
        cdef CKDBEquations* kdb = new CKDBEquations(filepath.encode())
        del kdb

    def _subset(self, pattern: str, copy: bool) -> Equations:
        # call to __new__() that bypasses the __init__() constructor.
        cdef Equations subset_db = Equations.__new__(Equations)
        subset_db.database_ptr = subset_db.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        return subset_db

    def _get_object(self, key):
        if not isinstance(key, str):
            raise TypeError(f"Cannot get object {key}.\nExpected a string value for {key} " + 
                            f"but got value of type {type(key).__name__}")
        key = key.strip()

        c_eq = self.database_ptr.get(key.encode())
        str_sample = c_eq.get_sample().to_string().decode()
        from_period, to_period = str_sample.split(':')
        eq = Equation(key, c_eq.get_lec().decode(), c_eq.get_method().decode(), 
                      from_period, to_period, c_eq.get_comment().decode(), 
                      c_eq.get_instruments().decode(), c_eq.get_block().decode())
        eq.c_equation.date = c_eq.date
        for i, test_val in enumerate(c_eq.tests): 
            eq.c_equation.set_test(<EnumIodeEquationTest>i, test_val)
        return eq

    def _set_object(self, key, value):
        cdef CEquation* c_equation
        if not isinstance(key, str):
            raise TypeError(f"Cannot set object {key}.\nExpected a string value for {key} " + 
                            f"but got value of type {type(key).__name__}")
        key = key.strip()

        if self.database_ptr.contains(key.encode()):
            # update existing equation
            if isinstance(value, str):
                equation = self._get_object(key)
                equation.set_lec(value.strip(), key)
            elif isinstance(value, Equation):
                equation = value
            elif isinstance(value, dict):
                equation = self._get_object(key)
                if 'lec' in value:
                    equation.set_lec(value.pop('lec').strip(), key)
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
                    warnings.warn(f"Update equation '{key}': 'tests' cannot be updated manually. Skipped new values.")
                    del value['tests']
                if 'date' in value:
                    warnings.warn(f"Update equation '{key}': 'date' cannot be updated manually. Skipped new value.")
                    del value['date']
                if len(value):
                    raise ValueError(f"Update equation '{key}': only 'lec', 'method', 'sample', 'comment', "
                                     f"'instruments' and 'block' keys are accepted. "
                                     f"Got unknown key(s): {';'.join(value.keys())}")
            else:
                raise TypeError(f"Update equation '{key}': Expected input to be of type str or dict or Equation. "
                                f"Got value of type {type(value).__name__}")
            c_equation = (<Equation>equation).c_equation
            self.database_ptr.update(key.encode(), dereference(c_equation))
        else:
            # add a new equation
            if isinstance(value, str):
                equation = Equation(name=key, lec=value.strip())
            elif isinstance(value, Equation):
                equation = value
            elif isinstance(value, (tuple, list)):
                value = [key] + list(value)
                equation = Equation(*value)
            elif isinstance(value, dict):
                value['name'] = key
                equation = Equation(**value)
            else:
                raise TypeError(f"New equation '{key}': Expected input to be of type str or tuple or list or "
                                f"dict or Equation. Got value of type {type(value).__name__}")
            c_equation = (<Equation>equation).c_equation
            self.database_ptr.add(key.encode(), dereference(c_equation))


equations: Equations = Equations._get_instance()
