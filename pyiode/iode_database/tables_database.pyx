from pathlib import Path
from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional, Any

cimport cython
from cython.operator cimport dereference
from pyiode.objects.table cimport CTable
from pyiode.iode_database.cpp_api_database cimport hash_value
from pyiode.iode_database.cpp_api_database cimport KDBTables
from pyiode.iode_database.cpp_api_database cimport global_ws_tbl as cpp_global_tables
from pyiode.iode_database.cpp_api_database cimport B_TABLE_TITLE, B_PrintObjTblTitle


cdef class Tables(CythonIodeDatabase):
    cdef bint ptr_owner
    cdef KDBTables* database_ptr
    cdef int print_as

    def __cinit__(self, filepath: str=None) -> Tables:
        self.ptr_owner = False
        self.database_ptr = cpp_global_tables.get()
        self.abstract_db_ptr = cpp_global_tables.get()
        self.print_as = B_TABLE_TITLE

    def __dealloc__(self):
        if self.ptr_owner and self.database_ptr is not NULL:
            del self.database_ptr
            self.database_ptr = NULL

    @staticmethod
    cdef Tables _from_ptr(KDBTables* database_ptr = NULL, bint owner=False):
        # call to __new__() that bypasses the __init__() constructor.
        cdef Tables wrapper = Tables.__new__(Tables)
        if database_ptr is not NULL:
            wrapper.ptr_owner = owner
            wrapper.database_ptr = database_ptr
            wrapper.abstract_db_ptr = database_ptr
        else:
            wrapper.ptr_owner = False
            wrapper.database_ptr = cpp_global_tables.get()
            wrapper.abstract_db_ptr = cpp_global_tables.get()
        wrapper.print_as = B_TABLE_TITLE
        return wrapper

    def _load(self, filepath: str):
        if self.database_ptr is not NULL:
            self.database_ptr.load(filepath.encode())

    def initialize_subset(self, subset: Tables, pattern: str, copy: bool) -> Tables:
        subset.database_ptr = new KDBTables(self.database_ptr, pattern.encode(), <bint>copy)
        subset.abstract_db_ptr = subset.database_ptr
        subset.ptr_owner = True
        return subset

    def get_title(self, name: str) -> str:
        return self.database_ptr.get_title(name.encode()).decode()

    def _get_object(self, name: str, table: Table) -> Table:
        cdef CTable c_table
        name = name.strip()
        c_table = self.database_ptr.get(name.encode())
        table.c_table_name = name.encode()
        table.c_table = new CTable(c_table)
        table.ptr_owner = <bint>True
        return table

    def _set_object(self, name: str, table: Table):
        cdef CTable* table_ptr = table.c_table
        self.database_ptr.set_obj_ptr(name.encode(), table_ptr)

    def copy_from(self, input_files: str, names: str='*'):
        self.database_ptr.copy_from(input_files.encode(), names.encode())

    def merge(self, other: Tables, overwrite: bool=True):        
        cdef KDBTables* other_db_ptr = other.database_ptr
        self.database_ptr.merge(dereference(other_db_ptr), <bint>overwrite, <bint>False)

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
        self.database_ptr.print_to_file(filepath.encode(), generalized_sample.encode(), 
                                        names.encode(), nb_decimals, c_format)

    def __hash__(self) -> int:
        return hash_value(dereference(self.database_ptr))


