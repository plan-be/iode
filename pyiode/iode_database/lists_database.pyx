from collections.abc import Iterable
from typing import Union, Tuple, List, Optional, Any

cimport cython
from cython.operator cimport dereference
from libcpp.memory cimport shared_ptr

from pyiode.iode_database.cpp_api_database cimport hash_value
from pyiode.iode_database.cpp_api_database cimport KDBLists
from pyiode.iode_database.cpp_api_database cimport global_ws_lst as cpp_global_lists

import pandas as pd


cdef class Lists(CythonIodeDatabase):
    cdef shared_ptr[KDBLists] database_ptr
    cdef KDBLists* database

    def __cinit__(self, filepath: str=None) -> Lists:
        self.database_ptr = cpp_global_lists
        self.database = self.database_ptr.get()
        self.abstract_database = self.database

    def __dealloc__(self):
        self.database_ptr.reset()
        self.database = NULL

    @staticmethod
    cdef Lists _from_ptr(shared_ptr[KDBLists] database_ptr):
        # call to __new__() that bypasses the __init__() constructor.
        cdef Lists wrapper = Lists.__new__(Lists)
        if database_ptr.get() is not NULL:
            wrapper.database_ptr = database_ptr
        else:
            wrapper.database_ptr = cpp_global_lists
        wrapper.database = wrapper.database_ptr.get()
        wrapper.abstract_database = wrapper.database
        return wrapper

    def _load(self, filepath: str):
        if self.database is not NULL:
            self.database.load(filepath.encode())

    def initialize_subset(self, pattern: str, copy: bool) -> Lists:
        cdef shared_ptr[KDBLists] subset_db_ptr = self.database.get_subset(pattern.encode(), <bint>copy)
        subset = Lists._from_ptr(subset_db_ptr)
        return subset

    def _get_object(self, name: str) -> str:
        name = name.strip()
        return self.database.get(name.encode()).decode()

    def _set_object(self, name: str, value: str):
        name = name.strip()
        self.database.set(name.encode(), <string>(value.encode()))

    def copy_from(self, input_files: str, names: str='*'):     
        self.database.copy_from(input_files.encode(), names.encode())

    def merge(self, other: Lists, overwrite: bool=True):        
        cdef KDBLists* other_database = other.database
        self.database.merge(dereference(other_database), <bint>overwrite, <bint>False)

    def __hash__(self) -> int:
        return hash_value(dereference(self.database))


