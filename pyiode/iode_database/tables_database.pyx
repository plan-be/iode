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
from pyiode.iode_database.cpp_api_database cimport KDBTables as CKDBTables
from pyiode.iode_database.cpp_api_database cimport Tables as cpp_global_tables


@cython.final
cdef class Tables(_AbstractDatabase):
    r"""
    IODE Tables database. 

    Attributes
    ----------
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
    >>> from iode import tables, SAMPLE_DATA_DIR
    >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
    >>> len(tables)
    46
    >>> tables          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Workspace: Tables
    nb tables: 46
    filename: ...\tests\data\fun.tbl
    <BLANKLINE>
      name                             table titles
    ANAKNFF     Déterminants de la croissance de K
    ANAPRIX     Analyse des prix
    C8_1        Déterminants de l'output potentiel
    C8_10       Coin salarial parafiscal
    C8_11       Propension moyenne à épargner
    ...         ...
    UCLASS      Chômage classique
    UY          Taux de chômage
    XPC         Inflation: PC
    XQBBP       Croissance
    YDH         Tableau B-3. Revenu disponible des ménages à prix constant
    <BLANKLINE>
    """
    cdef bint ptr_owner
    cdef CKDBTables* database_ptr

    def __cinit__(self, filepath: str = None) -> Tables:
        self.database_ptr = NULL
        self.abstract_db_ptr = NULL
        self.ptr_owner = False

    def __init__(self, filepath: str = None):
        # Prevent accidental instantiation from normal Python code
        # since we cannot pass a struct pointer into a Python constructor.
        raise TypeError("This class cannot be instantiated directly.")

    def __dealloc__(self):
        # if self.database_ptr points to the C++ global instance Tables 
        # which does not need to be manually deleted 
        if self.ptr_owner and self.database_ptr is not NULL:
            del self.database_ptr
            self.database_ptr = NULL

    # see https://cython.readthedocs.io/en/stable/src/userguide/extension_types.html#instantiation-from-existing-c-c-pointers 
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
        return wrapper

    # TODO: implement KDBAbstract::load() method (for global KDB only)
    def _load(self, filepath: str):
        cdef CKDBTables* kdb = new CKDBTables(filepath.encode())
        del kdb

    def _subset(self, pattern: str, copy: bool) -> Tables:
        cdef Tables subset_db = Tables.__new__(Tables)
        subset_db.database_ptr = subset_db.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        return subset_db

    def _get_object(self, key: str):
        key = key.strip()
        cdef CTable* c_table = self.database_ptr.get(key.encode())
        py_table = Table._from_ptr(c_table, <bint>True) 
        return py_table

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

    def _str_table(self, names: List[str]) -> str:
        titles = [join_lines(self.database_ptr.get_title(name.encode()).decode()) for name in names]
        columns = {"name": names, "table titles": titles}
        return table2str(columns, max_lines=10, max_width=-1, justify_funcs={"name": JUSTIFY.LEFT, "table titles": JUSTIFY.LEFT})


tables: Tables = Tables._from_ptr()
