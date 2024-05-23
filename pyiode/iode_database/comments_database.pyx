# distutils: language = c++

from collections.abc import Iterable
from typing import Union, Tuple, List, Optional, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

cimport cython
from pyiode.iode_database.database cimport KDBComments as CKDBComments
from pyiode.iode_database.database cimport Comments as cpp_global_comments


@cython.final
cdef class Comments(_AbstractDatabase):
    """
    IODE Comments database. 

    Attributes
    ----------
    filename: str
    description: str

    Parameters
    ----------
    filepath: str, optional
        file containing the IODE comments to load.

    Returns
    -------
    Comments

    Examples
    --------
    >>> from iode import comments, SAMPLE_DATA_DIR
    >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
    >>> len(comments)
    317
    """

    cdef CKDBComments* database_ptr

    def __cinit__(self, filepath: str = None) -> Comments:
        self.database_ptr = NULL
        self.abstract_db_ptr = NULL

    def __init__(self, filepath: str = None):
        # Prevent accidental instantiation from normal Python code
        # since we cannot pass a struct pointer into a Python constructor.
        raise TypeError("This class cannot be instantiated directly.")

    def __dealloc__(self):
        # self.database_ptr points to the C++ global instance Comments 
        # which does not need to be manually deleted 
        pass

    # see https://cython.readthedocs.io/en/stable/src/userguide/extension_types.html#instantiation-from-existing-c-c-pointers 
    @staticmethod
    def _get_instance() -> Comments:
        # call to __new__() that bypasses the __init__() constructor.
        cdef Comments wrapper = Comments.__new__(Comments)
        wrapper.database_ptr = &cpp_global_comments
        wrapper.abstract_db_ptr = &cpp_global_comments
        return wrapper

    # TODO: implement KDBAbstract::load() method (for global KDB only)
    def _load(self, filepath: str):
        cdef CKDBComments* kdb = new CKDBComments(filepath.encode())
        del kdb

    def _subset(self, pattern: str, copy: bool) -> Comments:
        # call to __new__() that bypasses the __init__() constructor.
        cdef Comments subset_ = Comments.__new__(Comments)
        subset_.database_ptr = subset_.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        return subset_

    def _get_object(self, key):
        if not isinstance(key, str):
            raise TypeError(f"Cannot get comment '{key}'.\nExpected a string value for the name " + 
                            f"but got value of type {type(key).__name__}")
        key = key.strip()
        return self.database_ptr.get(key.encode()).decode()

    def _set_object(self, key, value):
        if not isinstance(key, str):
            raise TypeError(f"Cannot set comment '{key}'.\nExpected a string value for the name " + 
                            f"but got name value of type {type(key).__name__}")
        key = key.strip()
        if not isinstance(value, str):
            raise TypeError(f"Cannot set comment '{key}'.\nExpected a string value for {key} " + 
                            f"but got value of type {type(value).__name__}")
        value = value.strip()
        if self.database_ptr.contains(key.encode()):
            self.database_ptr.update(key.encode(), value.encode())
        else:
            self.database_ptr.add(key.encode(), value.encode())


comments: Comments = Comments._get_instance()
