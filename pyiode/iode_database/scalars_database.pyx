from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional, Any

cimport cython
from cython.operator cimport dereference
from libcpp.memory cimport shared_ptr

from pyiode.iode_database.cpp_api_database cimport hash_value
from pyiode.iode_database.cpp_api_database cimport KDBScalars
from pyiode.iode_database.cpp_api_database cimport global_ws_scl as cpp_global_scalars

import pandas as pd


cdef class Scalars(CythonIodeDatabase):
    cdef shared_ptr[KDBScalars] database_ptr
    cdef KDBScalars* database

    def __cinit__(self, filepath: str=None) -> Scalars:
        self.database_ptr = cpp_global_scalars
        self.database = self.database_ptr.get()
        self.abstract_database = self.database

    def __dealloc__(self):
        self.database_ptr.reset()
        self.database = NULL

    @staticmethod
    cdef Scalars _from_ptr(shared_ptr[KDBScalars] database_ptr):
        # call to __new__() that bypasses the __init__() constructor.
        cdef Scalars wrapper = Scalars.__new__(Scalars)
        if database_ptr.get() is not NULL:
            wrapper.database_ptr = database_ptr
        else:
            wrapper.database_ptr = cpp_global_scalars
        wrapper.database = wrapper.database_ptr.get()
        wrapper.abstract_database = wrapper.database
        return wrapper

    def _load(self, filepath: str):
        if self.database is not NULL:
            self.database.load(filepath.encode())

    def initialize_subset(self, pattern: str, copy: bool) -> Scalars:
        cdef shared_ptr[KDBScalars] subset_db_ptr = self.database.get_subset(pattern.encode(), <bint>copy)
        subset = Scalars._from_ptr(subset_db_ptr)
        return subset

    def _get_object(self, name: str) -> Scalar:
        cdef shared_ptr[CScalar] scl_ptr
        name = name.strip()
        scl_ptr = self.database.get_obj_ptr(name.encode())
        scalar = Scalar._from_ptr(scl_ptr) 
        return scalar

    def _set_object(self, name: str, scalar: Scalar) -> Scalar:
        cdef shared_ptr[CScalar] scl_ptr
        # NOTE: the C++ set_obj_ptr() method creates a copy of the object stored 
        #       in the passed shared pointer to avoid that two entries (shared_ptr<T>) 
        #       in the IODE database point to the same object in memory
        scl_ptr = self.database.set_obj_ptr(name.encode(), scalar.scl_ptr)
        scalar.update_ptr(scl_ptr)
        return scalar

    def copy_from(self, input_files: str, names: str='*'):
        self.database.copy_from(input_files.encode(), names.encode())

    def merge(self, other: Scalars, overwrite: bool=True):        
        cdef shared_ptr[KDBScalars] other_database = other.database_ptr
        self.database.merge(other_database, <bint>overwrite, <bint>False)

    def __hash__(self) -> int:
        return hash_value(dereference(self.database))


