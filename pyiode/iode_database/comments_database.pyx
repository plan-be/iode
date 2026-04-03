from collections.abc import Iterable
from typing import Union, Tuple, List, Optional, Any

cimport cython
from cython.operator cimport dereference
from libcpp.memory cimport shared_ptr

from pyiode.iode_database.cpp_api_database cimport hash_value
from pyiode.iode_database.cpp_api_database cimport KDBComments
from pyiode.iode_database.cpp_api_database cimport global_ws_cmt as cpp_global_comments
from pyiode.iode_database.cpp_api_database cimport B_FileImportCmt

import pandas as pd


cdef class Comments(CythonIodeDatabase):
    cdef shared_ptr[KDBComments] database_ptr
    cdef KDBComments* database

    def __cinit__(self, filepath: str=None) -> Comments:
        self.database_ptr = cpp_global_comments
        self.database = self.database_ptr.get()
        self.abstract_database = self.database

    def __dealloc__(self):
        self.database_ptr.reset()
        self.database = NULL

    @staticmethod
    cdef Comments _from_ptr(shared_ptr[KDBComments] database_ptr):
        # call to __new__() that bypasses the __init__() constructor.
        cdef Comments wrapper = Comments.__new__(Comments)
        if database_ptr.get() is not NULL:
            wrapper.database_ptr = database_ptr
        else:
            wrapper.database_ptr = cpp_global_comments
        wrapper.database = wrapper.database_ptr.get()
        wrapper.abstract_database = wrapper.database
        return wrapper

    def _load(self, filepath: str):
        if self.database is not NULL:
            self.database.load(filepath.encode())

    def initialize_subset(self, pattern: str, copy: bool) -> Comments:
        cdef shared_ptr[KDBComments] subset_db_ptr = self.database.get_subset(pattern.encode(), <bint>copy)
        subset = Comments._from_ptr(subset_db_ptr)
        return subset

    def _get_object(self, name: str) -> str:
        name = name.strip()
        return self.database.get(name.encode()).decode()

    def _set_object(self, name: str, value: str):
        name = name.strip()
        self.database.set(name.encode(), <string>(value.encode()))

    def copy_from(self, input_files: str, names: str='*'):
        self.database.copy_from(input_files.encode(), names.encode())

    def merge(self, other: Comments, overwrite: bool=True):        
        cdef shared_ptr[KDBComments] other_database = other.database_ptr
        self.database.merge(dereference(other_database), <bint>overwrite, <bint>False)

    @classmethod
    def convert_file(cls, args: str) -> int:
        return B_FileImportCmt(args.encode('utf-8'))

    def __hash__(self) -> int:
        return hash_value(dereference(self.database))


