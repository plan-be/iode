from collections.abc import Iterable
from typing import Union, Tuple, List, Optional, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

cimport cython
from cython.operator cimport dereference
from pyiode.iode_database.cpp_api_database cimport hash_value
from pyiode.iode_database.cpp_api_database cimport KDBIdentities as CKDBIdentities
from pyiode.iode_database.cpp_api_database cimport Identities as cpp_global_identities
from pyiode.iode_database.cpp_api_database cimport Variables as cpp_global_variables
from pyiode.iode_database.cpp_api_database cimport KCPTR, KIPTR, KLPTR, KVPTR

import pandas as pd


cdef class Identities(CythonIodeDatabase):
    cdef bint ptr_owner
    cdef CKDBIdentities* database_ptr

    def __cinit__(self, filepath: str=None) -> Identities:
        self.ptr_owner = False
        self.database_ptr = &cpp_global_identities
        self.abstract_db_ptr = &cpp_global_identities

    def __dealloc__(self):
        if self.ptr_owner and self.database_ptr is not NULL:
            del self.database_ptr
            self.database_ptr = NULL

    @staticmethod
    cdef Identities _from_ptr(CKDBIdentities* database_ptr = NULL, bint owner=False):
        # call to __new__() that bypasses the __init__() constructor.
        cdef Identities wrapper = Identities.__new__(Identities)
        if database_ptr is not NULL:
            wrapper.ptr_owner = owner
            wrapper.database_ptr = database_ptr
            wrapper.abstract_db_ptr = database_ptr
        else:
            wrapper.ptr_owner = False
            wrapper.database_ptr = &cpp_global_identities
            wrapper.abstract_db_ptr = &cpp_global_identities
        return wrapper

    def _load(self, filepath: str):
        cdef CKDBIdentities* kdb = new CKDBIdentities(filepath.encode())
        del kdb

    def initialize_subset(self, cython_instance: Identities, pattern: str, copy: bool) -> Identities:
        cython_instance.database_ptr = cython_instance.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        return cython_instance

    def _get_object(self, name: str, identity: Identity) -> Identity:
        cdef CIdentity* c_identity
        name  = name.strip()
        c_identity = self.database_ptr.get(name.encode())
        
        identity.c_identity = c_identity
        # self.database_ptr.get() does not allocate a new C++ Identity instance
        identity.ptr_owner = <bint>False
        return identity

    def _set_object(self, name: str, value: Union[str, Identity]):
        if isinstance(value, Identity):
            value = str(value)
        value = value.strip()

        name = name.strip()
        if self.database_ptr.contains(name.encode()):
            self.database_ptr.update(name.encode(), <string>(value.encode()))
        else:
            self.database_ptr.add(name.encode(), value.encode())

    def copy_from(self, input_files: str, names: str='*'):
        self.database_ptr.copy_from(input_files.encode(), names.encode())

    def execute(self, identities: Union[str, List[str]]=None, from_period: Union[str, Period]=None, to_period: Union[str, Period]=None, var_files: Union[str, List[str]]=None, scalar_files: Union[str, List[str]]=None, trace: bool=False):
        if identities is None:
            identities = ""
        if isinstance(identities, str):
            pass
        elif isinstance(identities, Iterable) and all(isinstance(item, str) for item in identities):
            identities = ';'.join(identities)
        else:
            raise TypeError("'identities': Expected value of type str or a list of str. "
                            f"Got value of type {type(identities).__name__}")
        
        if from_period is None or to_period is None:
            c_sample = cpp_global_variables.get_sample()
            if from_period is None:
                from_period = c_sample.start_period().to_string().decode()
            if to_period is None:
                to_period = c_sample.end_period().to_string().decode()
        
        if isinstance(from_period, Period):
            from_period = str(from_period)
        if not isinstance(from_period, str):
            raise TypeError("'from_period': Expected value of type str or Period. "
                            f"Got value of type {type(from_period).__name__}")

        if isinstance(to_period, Period):
            to_period = str(to_period)
        if not isinstance(to_period, str):
            raise TypeError("'to_period': Expected value of type str or Period. "
                            f"Got value of type {type(to_period).__name__}")

        if var_files is None:
            var_files = ""
        if isinstance(var_files, str):
            pass
        elif isinstance(var_files, Iterable) and all(isinstance(item, str) for item in var_files):
            var_files = ';'.join(var_files)
        else:
            raise TypeError("'var_files': Expected value of type str or a list of str. "
                            f"Got value of type {type(var_files).__name__}")

        if scalar_files is None:
            scalar_files = ""
        if isinstance(scalar_files, str):
            pass
        elif isinstance(scalar_files, Iterable) and all(isinstance(item, str) for item in scalar_files):
            scalar_files = ';'.join(scalar_files)
        else:
            raise TypeError("'scalar_files': Expected value of type str or a list of str. "
                            f"Got value of type {type(scalar_files).__name__}")

        self.database_ptr.execute_identities(from_period.encode(), to_period.encode(), identities.encode(), 
                                            var_files.encode(), scalar_files.encode(), <bint>trace)

    def __hash__(self) -> int:
        return hash_value(dereference(self.database_ptr))


