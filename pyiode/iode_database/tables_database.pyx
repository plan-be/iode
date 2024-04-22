# distutils: language = c++

from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

cimport cython
from cython.operator cimport dereference
from pyiode.objects.table cimport CTable
from pyiode.iode_database.database cimport KDBTables as CKDBTables
from pyiode.iode_database.database cimport Tables as cpp_global_tables


@cython.final
cdef class Tables(_AbstractDatabase):
    """
    IODE Tables database. 

    Attributes
    ----------
    iode_type: str
    filename: str
    description: str

    Parameters
    ----------
    filepath: str, optional
        file containing the IODE tables to load.

    Returns
    -------
    Tables

    Examples
    --------
    >>> from iode import Tables, SAMPLE_DATA_DIR
    >>> tbl_db = Tables(f"{SAMPLE_DATA_DIR}/fun.tbl")
    >>> len(tbl_db)
    46
    """

    cdef CKDBTables* database_ptr

    def __cinit__(self, filepath: str = None) -> Tables:
        self.database_ptr = self.abstract_db_ptr = &cpp_global_tables
        if filepath is not None:
            self.load(filepath)

    def __dealloc__(self):
        # self.database_ptr points to the C++ global instance Tables 
        # which does not need to be manually deleted 
        pass

    # TODO: implement KDBAbstract::load() method (for global KDB only)
    def _load(self, filepath: str):
        cdef CKDBTables* kdb = new CKDBTables(filepath.encode())
        del kdb

    def subset(self, pattern: str, copy: bool = False) -> Tables:
        subset_ = Tables()
        subset_.database_ptr = subset_.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        return subset_

    def _get_object(self, key):
        if not isinstance(key, str):
            raise TypeError(f"Cannot get object {key}.\nExpected a string value for {key} " + 
                            f"but got value of type {type(key).__name__}")
        key = key.strip()

        c_tbl = self.database_ptr.get(key.encode())
        table = Table()
        del table.c_table 
        table.c_table = new CTable(c_tbl)
        return table

    def _set_object(self, key, value):
        cdef CTable* c_table
        if not isinstance(key, str):
            raise TypeError(f"Cannot set object {key}.\nExpected a string value for {key} " + 
                            f"but got value of type {type(key).__name__}")
        key = key.strip()

        if self.database_ptr.contains(key.encode()):
            # update existing table
            if not isinstance(value, Table):
                raise TypeError(f"Update table '{key}': Expected input of type 'Table'. "
                                f"Got value of type {type(value).__name__} instead")
            c_table = (<Table>value).c_table
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
