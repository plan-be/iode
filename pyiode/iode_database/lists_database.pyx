# distutils: language = c++

from collections.abc import Iterable
from typing import Union, Tuple, List, Optional, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

cimport cython
from pyiode.iode_database.database cimport KDBLists as CKDBLists
from pyiode.iode_database.database cimport Lists as cpp_global_lists


@cython.final
cdef class Lists(_AbstractDatabase):
    """
    IODE Lists database. 

    Attributes
    ----------
    filename: str
    description: str

    Parameters
    ----------
    filepath: str, optional
        file containing the IODE lists to load.

    Returns
    -------
    Lists

    Examples
    --------
    >>> from iode import lists, SAMPLE_DATA_DIR
    >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
    >>> len(lists)
    17
    """

    cdef CKDBLists* database_ptr

    def __cinit__(self, filepath: str = None) -> Lists:
        self.database_ptr = NULL
        self.abstract_db_ptr = NULL

    def __init__(self, filepath: str = None):
        # Prevent accidental instantiation from normal Python code
        # since we cannot pass a struct pointer into a Python constructor.
        raise TypeError("This class cannot be instantiated directly.")

    def __dealloc__(self):
        # self.database_ptr points to the C++ global instance Lists 
        # which does not need to be manually deleted 
        pass

    # see https://cython.readthedocs.io/en/stable/src/userguide/extension_types.html#instantiation-from-existing-c-c-pointers 
    @staticmethod
    def _get_instance() -> Lists:
        # call to __new__() that bypasses the __init__() constructor.
        cdef Lists wrapper = Lists.__new__(Lists)
        wrapper.database_ptr = &cpp_global_lists
        wrapper.abstract_db_ptr = &cpp_global_lists
        return wrapper

    # TODO: implement KDBAbstract::load() method (for global KDB only)
    def _load(self, filepath: str):
        cdef CKDBLists* kdb = new CKDBLists(filepath.encode())
        del kdb

    def _subset(self, pattern: str, copy: bool) -> Lists:
        cdef Lists subset_db = Lists.__new__(Lists)
        subset_db.database_ptr = subset_db.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        return subset_db

    def _get_object(self, key):
        if not isinstance(key, str):
            raise TypeError(f"Cannot get list '{key}'.\nExpected a string value for the name " + 
                            f"but got value of type {type(key).__name__}")
        key = key.strip()
        return self.database_ptr.get(key.encode()).decode()

    def _set_object(self, key, value):
        if not isinstance(key, str):
            raise TypeError(f"Cannot set list '{key}'.\nExpected a string value for the name " + 
                            f"but got name value of type {type(key).__name__}")
        key = key.strip()
        if not isinstance(value, str):
            raise TypeError(f"Cannot set list '{key}'.\nExpected a string value for {key} " + 
                            f"but got value of type {type(value).__name__}")
        value = value.strip()
        if self.database_ptr.contains(key.encode()):
            self.database_ptr.update(key.encode(), value.encode())
        else:
            self.database_ptr.add(key.encode(), value.encode())


lists: Lists = Lists._get_instance()
