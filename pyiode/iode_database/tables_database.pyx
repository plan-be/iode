from pathlib import Path
from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional, Any

cimport cython
from cython.operator cimport dereference
from libcpp.memory cimport shared_ptr

from pyiode.objects.table cimport CTable
from pyiode.iode_database.cpp_api_database cimport hash_value
from pyiode.iode_database.cpp_api_database cimport KDBTables
from pyiode.iode_database.cpp_api_database cimport global_ws_tbl as cpp_global_tables
from pyiode.iode_database.cpp_api_database cimport B_TABLE_TITLE, B_PrintObjTblTitle


cdef class Tables(CythonIodeDatabase):
    cdef shared_ptr[KDBTables] database_ptr
    cdef KDBTables* database
    cdef int print_as

    def __cinit__(self, filepath: str=None) -> Tables:
        self.database_ptr = cpp_global_tables
        self.database = self.database_ptr.get()
        self.abstract_database = self.database
        self.print_as = B_TABLE_TITLE

    def __dealloc__(self):
        self.database_ptr.reset()
        self.database = NULL

    @staticmethod
    cdef Tables _from_ptr(shared_ptr[KDBTables] database_ptr):
        # call to __new__() that bypasses the __init__() constructor.
        cdef Tables wrapper = Tables.__new__(Tables)
        if database_ptr.get() is not NULL:
            wrapper.database_ptr = database_ptr
        else:
            wrapper.database_ptr = cpp_global_tables
        wrapper.database = wrapper.database_ptr.get()
        wrapper.abstract_database = wrapper.database
        wrapper.print_as = B_TABLE_TITLE
        return wrapper

    def _load(self, filepath: str):
        if self.database is not NULL:
            self.database.load(filepath.encode())

    def initialize_subset(self, pattern: str, copy: bool) -> Tables:
        cdef shared_ptr[KDBTables] subset_db_ptr = self.database.get_subset(pattern.encode(), <bint>copy)
        subset = Tables._from_ptr(subset_db_ptr)
        return subset

    def get_title(self, name: str) -> str:
        return self.database.get_title(name.encode()).decode()

    def _get_object(self, name: str) -> Table:
        cdef shared_ptr[CTable] tbl_ptr
        name = name.strip()
        tbl_ptr = self.database.get_obj_ptr(name.encode())
        table = Table._from_ptr(tbl_ptr, name.encode())
        return table

    def _set_object(self, name: str, table: Table) -> Table:
        cdef shared_ptr[CTable] tbl_ptr
        # NOTE: the C++ set_obj_ptr() method creates a copy of the object stored 
        #       in the passed shared pointer to avoid that two entries (shared_ptr<T>) 
        #       in the IODE database point to the same object in memory
        tbl_ptr = self.database.set_obj_ptr(name.encode(), table.tbl_ptr)
        table.update_ptr(tbl_ptr)
        return table

    def copy_from(self, input_files: str, names: str='*'):
        self.database.copy_from(input_files.encode(), names.encode())

    def merge(self, other: Tables, overwrite: bool=True):        
        cdef KDBTables* other_database = other.database
        self.database.merge(dereference(other_database), <bint>overwrite, <bint>False)

    # Specify how to print a TABLE 
    #      0 : print table full definitions
    #      1 : print only table titles
    #      2 : print computed version of tables
    def get_print_tables_as(self) -> int:
        return self.print_as

    def set_print_tables_as(self, value: int):
        self.print_as = value
        if value <= 1:
            B_PrintObjTblTitle(str(value).encode())

    def cpp_tables_print_to_file(self, filepath: str, names: List[str], format: str, generalized_sample: str, nb_decimals: int):
        cdef char c_format = format.encode('utf-8')[0]
        self.database.print_to_file(filepath.encode(), generalized_sample.encode(), 
                                        names.encode(), nb_decimals, c_format)

    def __hash__(self) -> int:
        return hash_value(dereference(self.database))


