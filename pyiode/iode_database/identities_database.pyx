from typing import Union, Tuple, List, Optional, Any

cimport cython
from cython.operator cimport dereference
from libcpp.memory cimport shared_ptr

from pyiode.iode_database.cpp_api_database cimport hash_value
from pyiode.iode_database.cpp_api_database cimport KDBIdentities
from pyiode.iode_database.cpp_api_database cimport global_ws_idt as cpp_global_identities
from pyiode.iode_database.cpp_api_database cimport global_ws_var as cpp_global_variables

import pandas as pd


cdef class Identities(CythonIodeDatabase):
    cdef bint ptr_owner
    cdef KDBIdentities* database

    def __cinit__(self, filepath: str=None) -> Identities:
        self.ptr_owner = False
        self.database = cpp_global_identities.get()
        self.abstract_database = cpp_global_identities.get()

    def __dealloc__(self):
        if self.ptr_owner and self.database is not NULL:
            del self.database
            self.database = NULL

    @staticmethod
    cdef Identities _from_ptr(KDBIdentities* database = NULL, bint owner=False):
        # call to __new__() that bypasses the __init__() constructor.
        cdef Identities wrapper = Identities.__new__(Identities)
        if database is not NULL:
            wrapper.ptr_owner = owner
            wrapper.database = database
            wrapper.abstract_database = database
        else:
            wrapper.ptr_owner = False
            wrapper.database = cpp_global_identities.get()
            wrapper.abstract_database = cpp_global_identities.get()
        return wrapper

    def _load(self, filepath: str):
        if self.database is not NULL:
            self.database.load(filepath.encode())

    def initialize_subset(self, pattern: str, copy: bool) -> Identities:
        cdef KDBIdentities* subset_db_ptr = new KDBIdentities(self.database, pattern.encode(), <bint>copy)
        subset = Identities._from_ptr(subset_db_ptr, <bint>True)
        return subset

    def _get_object(self, name: str) -> Identity:
        cdef shared_ptr[CIdentity] idt_ptr
        name  = name.strip()
        idt_ptr = self.database.get_obj_ptr(name.encode())
        identity = Identity._from_ptr(idt_ptr)
        return identity

    def _set_object(self, name: str, value: str) -> Identity:
        cdef shared_ptr[CIdentity] idt_ptr
        name = name.strip()
        value = value.strip()
        idt_ptr = make_shared[CIdentity](value.encode())
        # NOTE: the C++ set_obj_ptr() method creates a copy of the object stored 
        #       in the passed shared pointer to avoid that two entries (shared_ptr<T>) 
        #       in the IODE database point to the same object in memory
        idt_ptr = self.database.set_obj_ptr(name.encode(), idt_ptr)
        identity = Identity._from_ptr(idt_ptr)
        return identity

    def copy_from(self, input_files: str, names: str='*'):
        self.database.copy_from(input_files.encode(), names.encode())

    def merge(self, other: Identities, overwrite: bool=True):        
        cdef KDBIdentities* other_db_ptr = other.database
        self.database.merge(dereference(other_db_ptr), <bint>overwrite, <bint>False)

    def execute(self, identities: str, from_period: str, to_period: str, var_files: str, scalar_files: str, trace: bool=False) -> bool:
        return self.database.execute_identities(from_period.encode(), to_period.encode(), identities.encode(), 
                                                    var_files.encode(), scalar_files.encode(), <bint>trace)

    def __hash__(self) -> int:
        return hash_value(dereference(self.database))


