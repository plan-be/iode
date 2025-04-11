from collections.abc import Iterable
from typing import Union, Tuple, List, Optional, Any

cimport cython
from cython.operator cimport dereference
from pyiode.iode_database.cpp_api_database cimport hash_value
from pyiode.iode_database.cpp_api_database cimport KDBLists as CKDBLists
from pyiode.iode_database.cpp_api_database cimport Lists as cpp_global_lists
from pyiode.iode_database.cpp_api_database cimport KCPTR, KIPTR, KLPTR, KVPTR

import pandas as pd


cdef class Lists(CythonIodeDatabase):
    cdef bint ptr_owner
    cdef CKDBLists* database_ptr

    def __cinit__(self, filepath: str=None) -> Lists:
        self.ptr_owner = False
        self.database_ptr = &cpp_global_lists
        self.abstract_db_ptr = &cpp_global_lists

    def __dealloc__(self):
        if self.ptr_owner and self.database_ptr is not NULL:
            del self.database_ptr
            self.database_ptr = NULL

    @staticmethod
    cdef Lists _from_ptr(CKDBLists* database_ptr = NULL, bint owner=False):
        # call to __new__() that bypasses the __init__() constructor.
        cdef Lists wrapper = Lists.__new__(Lists)
        if database_ptr is not NULL:
            wrapper.ptr_owner = owner
            wrapper.database_ptr = database_ptr
            wrapper.abstract_db_ptr = database_ptr
        else:
            wrapper.ptr_owner = False
            wrapper.database_ptr = &cpp_global_lists
            wrapper.abstract_db_ptr = &cpp_global_lists
        return wrapper

    def _load(self, filepath: str):
        cdef CKDBLists* kdb = new CKDBLists(filepath.encode())
        del kdb

    def initialize_subset(self, cython_instance: Lists, pattern: str, copy: bool) -> Lists:
        cython_instance.database_ptr = cython_instance.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        return cython_instance

    def _get_object(self, name: str) -> str:
        name = name.strip()
        return self.database_ptr.get(name.encode()).decode()

    def _set_object(self, name: str, value: str):
        name = name.strip()
        if self.database_ptr.contains(name.encode()):
            self.database_ptr.update(name.encode(), <string>(value.encode()))
        else:
            self.database_ptr.add(name.encode(), value.encode())

    def copy_from(self, input_files: str, names: str='*'):     
        self.database_ptr.copy_from(input_files.encode(), names.encode())

    def __hash__(self) -> int:
        return hash_value(dereference(self.database_ptr))


