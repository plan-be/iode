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


cdef class Tables(IodeDatabase):
    cdef bint ptr_owner
    cdef CKDBTables* database_ptr
    cdef int print_as

    def __cinit__(self, filepath: str=None) -> Tables:
        self.database_ptr = NULL
        self.abstract_db_ptr = NULL
        self.ptr_owner = False
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

    @staticmethod
    def __init_instance(instance: Tables) -> Self:
        instance.ptr_owner = False
        instance.database_ptr = &cpp_global_tables
        instance.abstract_db_ptr = &cpp_global_tables
        return instance

    def _load(self, filepath: str):
        cdef CKDBTables* kdb = new CKDBTables(filepath.encode())
        del kdb

    def _subset(self, pattern: str, copy: bool) -> Tables:
        subset_db: Tables = self._new_instance()
        subset_db.database_ptr = subset_db.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        return subset_db

    def get_title(self, key: Union[str, int]) -> str:
        if isinstance(key, int):
            return self.database_ptr.get_title(<int>key).decode()
        else:
            return self.database_ptr.get_title(<string>(key.encode())).decode()

    def _get_object(self, key: Union[str, int]) -> Table:
        cdef CTable* c_table
        if isinstance(key, int):
            c_table = self.database_ptr.get(<int>key)
            name = self.get_name(<int>key)
        else:
            key = key.strip()
            c_table = self.database_ptr.get(<string>(key.encode()))
            name = key

        py_table = Table._from_ptr(c_table, <bint>True, name.encode(), self.database_ptr) 
        return py_table

    def _set_object(self, key: Union[str, int], value):
        cdef CTable* c_table

        if isinstance(key, str):
            key = key.strip()

        if isinstance(key, int) or self.database_ptr.contains(key.encode()):
            # update existing table
            if not isinstance(value, Table):
                raise TypeError(f"Update table '{key}': Expected input of type 'Table'. "
                                f"Got value of type {type(value).__name__} instead")
            c_table = (<Table>value).c_table
            if isinstance(key, int):
                self.database_ptr.update(<int>key, dereference(c_table))
            else:
                self.database_ptr.update(<string>(key.encode()), dereference(c_table))
        else:
            # add a new table
            if isinstance(value, int):
                table = Table(nb_columns=value)
            elif isinstance(value, Table):
                table = value
            elif isinstance(value, (tuple, list)):
                table = Table(*value)
            elif isinstance(value, dict):
                table = Table(**value)
            else:
                raise TypeError(f"New table '{key}': Expected input to be of type int or tuple or list or "
                                f"dict or Table. Got value of type {type(value).__name__} instead")
            c_table = (<Table>table).c_table
            self.database_ptr.add(<string>(key.encode()), dereference(c_table))

    def copy_from(self, input_files: Union[str, List[str]], names: Union[str, List[str]]='*'):
        if not (self.is_global_workspace or self.is_detached):
            raise RuntimeError("Cannot call 'copy_from' method on a subset of a workspace")
        input_files, names = self._copy_from(input_files, names)
        self.database_ptr.copy_from(input_files.encode(), names.encode())

    def _str_table(self, names: List[str]) -> str:
        titles = [join_lines(self.database_ptr.get_title(<string>(name.encode())).decode()) for name in names]
        columns = {"name": names, "table titles": titles}
        return table2str(columns, max_lines=10, max_width=-1, justify_funcs={"name": JUSTIFY.LEFT, "table titles": JUSTIFY.LEFT})

    def get_print_tables_as(self) -> PrintTablesAs:
        return PrintTablesAs(self.print_as)

    def set_print_tables_as(self, value: Union[PrintTablesAs, str]):
        if isinstance(value, str):
            upper_str = value.upper()
            if upper_str not in PrintTablesAs.__members__:
                raise ValueError(f"Invalid value '{value}'. "
                                 f"Expected one of {', '.join(PrintTablesAs.__members__.keys())}. ")
            value = PrintTablesAs[upper_str]
        value = int(value)
        self.print_as = value
        if value <= 1:
            B_PrintObjTblTitle(str(value).encode())

    def print_to_file(self, filepath: Union[str, Path], names: Union[str, List[str]]=None, format: str=None, generalized_sample: str=None, nb_decimals: int=4) -> None:
        cdef char c_format = b'\0'
        if self.print_tables_as != PrintTablesAs.COMPUTED:
            self._print_to_file(filepath, names, format)
        else:
            if format is not None:
                if not len(format):
                    raise ValueError("format must be a non-empty char")
                c_format = format.encode('utf-8')[0]

            if isinstance(filepath, str):
                if not len(filepath):
                    raise ValueError("'filepath' must be a non-empty string or a Path object.")
                filepath = Path(filepath)
            if filepath.suffix:
                c_format = filepath.suffix.encode('utf-8')[1]
            filepath = str(filepath.resolve())

            names = ';'.join(self.get_names(names))
            
            if generalized_sample is None or len(generalized_sample) == 0:
                raise ValueError("'generalized_sample' must be a non-empty string.")

            self.database_ptr.print_to_file(filepath.encode(), generalized_sample.encode(), 
                                            names.encode(), nb_decimals, c_format)

    def __hash__(self) -> int:
        return hash_value(dereference(self.database_ptr))


