from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional, Any

cimport cython
from cython.operator cimport dereference
from pyiode.iode_database.cpp_api_database cimport hash_value
from pyiode.iode_database.cpp_api_database cimport KDBScalars as CKDBScalars
from pyiode.iode_database.cpp_api_database cimport Scalars as cpp_global_scalars
from pyiode.iode_database.cpp_api_database cimport KCPTR, KIPTR, KLPTR, KVPTR

import pandas as pd


cdef class Scalars(CythonIodeDatabase):
    cdef bint ptr_owner
    cdef CKDBScalars* database_ptr

    def __cinit__(self, filepath: str=None) -> Scalars:
        self.ptr_owner = False
        self.database_ptr = &cpp_global_scalars
        self.abstract_db_ptr = &cpp_global_scalars

    def __dealloc__(self):
        if self.ptr_owner and self.database_ptr is not NULL:
            del self.database_ptr
            self.database_ptr = NULL

    @staticmethod
    cdef Scalars _from_ptr(CKDBScalars* database_ptr = NULL, bint owner=False):
        # call to __new__() that bypasses the __init__() constructor.
        cdef Scalars wrapper = Scalars.__new__(Scalars)
        if database_ptr is not NULL:
            wrapper.ptr_owner = owner
            wrapper.database_ptr = database_ptr
            wrapper.abstract_db_ptr = database_ptr
        else:
            wrapper.ptr_owner = False
            wrapper.database_ptr = &cpp_global_scalars
            wrapper.abstract_db_ptr = &cpp_global_scalars
        return wrapper

    def _load(self, filepath: str):
        cdef CKDBScalars* kdb = new CKDBScalars(filepath.encode())
        del kdb

    def initialize_subset(self, cython_instance: Scalars, pattern: str, copy: bool) -> Scalars:
        cython_instance.database_ptr = cython_instance.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        return cython_instance

    def _get_object(self, name: str, scalar: Scalar) -> Scalar:
        cdef CScalar* c_scalar
        name = name.strip()
        c_scalar = self.database_ptr.get(name.encode())
        
        scalar.c_scalar = c_scalar 
        # self.database_ptr.get() does not allocate a new C++ Scalar instance
        scalar.ptr_owner = <bint>False
        return scalar

    def _set_object(self, name: str, scalar: Scalar):
        c_scalar = scalar.c_scalar
        
        # update a scalar
        if self.database_ptr.contains(name.encode()):
            self.database_ptr.update(name.encode(), c_scalar.val, c_scalar.relax, c_scalar.std)
        # add a new scalar
        else:
            self.database_ptr.add(name.encode(), c_scalar.val, c_scalar.relax, c_scalar.std)

    def copy_from(self, input_files: str, names: str='*'):
        self.database_ptr.copy_from(input_files.encode(), names.encode())

    def __hash__(self) -> int:
        return hash_value(dereference(self.database_ptr))


