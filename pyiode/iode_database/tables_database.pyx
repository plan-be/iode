from pathlib import Path
from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional, Any

if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

cimport cython
from cython.operator cimport dereference
from pyiode.objects.table cimport CTable
from pyiode.iode_database.cpp_api_database cimport hash_value
from pyiode.iode_database.cpp_api_database cimport KDBTables as CKDBTables
from pyiode.iode_database.cpp_api_database cimport Tables as cpp_global_tables
from pyiode.iode_database.cpp_api_database cimport KCPTR, KIPTR, KLPTR, KVPTR
from pyiode.iode_database.cpp_api_database cimport B_TBL_TITLE, B_PrintObjTblTitle

from iode.common import PrintTablesAs


cdef class Tables(CythonIodeDatabase):
    cdef bint ptr_owner
    cdef CKDBTables* database_ptr
    cdef int print_as

    def __cinit__(self, filepath: str=None) -> Tables:
        self.ptr_owner = False
        self.database_ptr = &cpp_global_tables
        self.abstract_db_ptr = &cpp_global_tables
        self.print_as = B_TBL_TITLE

    def __dealloc__(self):
        if self.ptr_owner and self.database_ptr is not NULL:
            del self.database_ptr
            self.database_ptr = NULL

    @staticmethod
    cdef Tables _from_ptr(CKDBTables* database_ptr = NULL, bint owner=False):
        # call to __new__() that bypasses the __init__() constructor.
        cdef Tables wrapper = Tables.__new__(Tables)
        if database_ptr is not NULL:
            wrapper.ptr_owner = owner
            wrapper.database_ptr = database_ptr
            wrapper.abstract_db_ptr = database_ptr
        else:
            wrapper.ptr_owner = False
            wrapper.database_ptr = &cpp_global_tables
            wrapper.abstract_db_ptr = &cpp_global_tables
        wrapper.print_as = B_TBL_TITLE
        return wrapper

    def _load(self, filepath: str):
        cdef CKDBTables* kdb = new CKDBTables(filepath.encode())
        del kdb

    def initialize_subset(self, cython_instance: Tables, pattern: str, copy: bool) -> Tables:
        cython_instance.database_ptr = cython_instance.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        return cython_instance

    def get_title(self, name: str) -> str:
        return self.database_ptr.get_title(name.encode()).decode()

    def _get_object(self, name: str, table: Table) -> Table:
        cdef CTable* c_table
        name = name.strip()
        c_table = self.database_ptr.get(name.encode())

        table.c_table_name = name.encode()
        table.c_table = c_table
        table.c_database = self.database_ptr
        table.ptr_owner = <bint>True
        return table

    def _set_object(self, name: str, table: Table):
        cdef CTable* c_table

        c_table = table.c_table
        if self.database_ptr.contains(name.encode()):
            self.database_ptr.update(name.encode(), dereference(c_table))
        else:
            self.database_ptr.add(<string>(name.encode()), dereference(c_table))

    def copy_from(self, input_files: str, names: str='*'):
        self.database_ptr.copy_from(input_files.encode(), names.encode())

    def _str_table(self, names: List[str]) -> str:
        titles = [join_lines(self.database_ptr.get_title(<string>(name.encode())).decode()) for name in names]
        columns = {"name": names, "table titles": titles}
        return table2str(columns, max_lines=10, max_width=-1, justify_funcs={"name": JUSTIFY.LEFT, "table titles": JUSTIFY.LEFT})

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


