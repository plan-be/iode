from collections.abc import Iterable
from typing import Union, Tuple, List, Optional, Any

cimport cython
from cython.operator cimport dereference
from pyiode.iode_database.cpp_api_database cimport hash_value
from pyiode.iode_database.cpp_api_database cimport KDBComments as CppKDBComments
from pyiode.iode_database.cpp_api_database cimport global_ws_cmt as cpp_global_comments
from pyiode.iode_database.cpp_api_database cimport B_FileImportCmt

import pandas as pd


cdef class Comments(CythonIodeDatabase):
    cdef bint ptr_owner
    cdef CppKDBComments* database_ptr

    def __cinit__(self, filepath: str=None) -> Comments:
        self.ptr_owner = False
        self.database_ptr = cpp_global_comments.get()
        self.abstract_db_ptr = cpp_global_comments.get()

    def __dealloc__(self):
        if self.ptr_owner and self.database_ptr is not NULL:
            del self.database_ptr
            self.database_ptr = NULL

    @staticmethod
    cdef Comments _from_ptr(CppKDBComments* database_ptr = NULL, bint owner=False):
        # call to __new__() that bypasses the __init__() constructor.
        cdef Comments wrapper = Comments.__new__(Comments)
        if database_ptr is not NULL:
            wrapper.ptr_owner = owner
            wrapper.database_ptr = database_ptr
            wrapper.abstract_db_ptr = database_ptr
        else:
            wrapper.ptr_owner = False
            wrapper.database_ptr = cpp_global_comments.get()
            wrapper.abstract_db_ptr = cpp_global_comments.get()
        return wrapper

    def _load(self, filepath: str):
        if self.database_ptr is not NULL:
            self.database_ptr.load(filepath.encode())

    def initialize_subset(self, subset: Comments, pattern: str, copy: bool) -> Comments:
        subset.database_ptr = new CppKDBComments(self.database_ptr, pattern.encode(), <bint>copy)
        subset.abstract_db_ptr = subset.database_ptr
        subset.ptr_owner = True
        return subset

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

    @classmethod
    def convert_file(cls, args: str) -> int:
        return B_FileImportCmt(args.encode('utf-8'))

    def __hash__(self) -> int:
        return hash_value(dereference(self.database_ptr))


