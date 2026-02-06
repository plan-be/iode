from typing import Union, Tuple, List, Optional, Any

cimport cython
from cython.operator cimport dereference
from pyiode.iode_database.cpp_api_database cimport hash_value
from pyiode.iode_database.cpp_api_database cimport KDBIdentities
from pyiode.iode_database.cpp_api_database cimport global_ws_idt as cpp_global_identities
from pyiode.iode_database.cpp_api_database cimport global_ws_var as cpp_global_variables

import pandas as pd


cdef class Identities(CythonIodeDatabase):
    cdef bint ptr_owner
    cdef KDBIdentities* database_ptr

    def __cinit__(self, filepath: str=None) -> Identities:
        self.ptr_owner = False
        self.database_ptr = cpp_global_identities.get()
        self.abstract_db_ptr = cpp_global_identities.get()

    def __dealloc__(self):
        if self.ptr_owner and self.database_ptr is not NULL:
            del self.database_ptr
            self.database_ptr = NULL

    @staticmethod
    cdef Identities _from_ptr(KDBIdentities* database_ptr = NULL, bint owner=False):
        # call to __new__() that bypasses the __init__() constructor.
        cdef Identities wrapper = Identities.__new__(Identities)
        if database_ptr is not NULL:
            wrapper.ptr_owner = owner
            wrapper.database_ptr = database_ptr
            wrapper.abstract_db_ptr = database_ptr
        else:
            wrapper.ptr_owner = False
            wrapper.database_ptr = cpp_global_identities.get()
            wrapper.abstract_db_ptr = cpp_global_identities.get()
        return wrapper

    def _load(self, filepath: str):
        if self.database_ptr is not NULL:
            self.database_ptr.load(filepath.encode())

    def initialize_subset(self, subset: Identities, pattern: str, copy: bool) -> Identities:
        subset.database_ptr = new KDBIdentities(self.database_ptr, pattern.encode(), <bint>copy)
        subset.abstract_db_ptr = subset.database_ptr
        subset.ptr_owner = True
        return subset

    def _get_object(self, name: str, identity: Identity) -> Identity:
        cdef CIdentity c_identity
        name  = name.strip()
        c_identity = self.database_ptr.get(name.encode())
        
        identity.c_identity = new CIdentity(c_identity)
        identity.ptr_owner = <bint>True
        return identity

    def _set_object(self, name: str, value: str):
        cdef CIdentity* c_idt = NULL
        name = name.strip()
        value = value.strip()
        c_idt = new CIdentity(value.encode())
        self.database_ptr.set_obj_ptr(name.encode(), c_idt)

    def copy_from(self, input_files: str, names: str='*'):
        self.database_ptr.copy_from(input_files.encode(), names.encode())

    def merge(self, other: Identities, overwrite: bool=True):        
        cdef KDBIdentities* other_db_ptr = other.database_ptr
        self.database_ptr.merge(dereference(other_db_ptr), <bint>overwrite, <bint>False)

    def execute(self, identities: str, from_period: str, to_period: str, var_files: str, scalar_files: str, trace: bool=False) -> bool:
        return self.database_ptr.execute_identities(from_period.encode(), to_period.encode(), identities.encode(), 
                                                    var_files.encode(), scalar_files.encode(), <bint>trace)

    def __hash__(self) -> int:
        return hash_value(dereference(self.database_ptr))


