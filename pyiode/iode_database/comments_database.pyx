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
    cdef CKDBComments* database_ptr

    def __cinit__(self, filepath: str = None) -> Comments:
        """
        Get an instance of the IODE Comments database. 
        Load the IODE comments from 'filepath' if given.

        Parameters
        ----------
        filepath: str, optional
            file containing the IODE comments to load.

        Returns
        -------
        Comments

        Examples
        --------
        >>> from iode import Comments, SAMPLE_DATA_DIR
        >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> len(cmt_db)
        317
        """
        self.database_ptr = self.abstract_db_ptr = &cpp_global_comments
        if filepath is not None:
            self.load(filepath)

    def __dealloc__(self):
        # self.database_ptr points to the C++ global instance Comments 
        # which does not need to be manually deleted 
        pass

    # TODO: implement KDBAbstract::load() method (for global KDB only)
    def _load(self, filepath: str):
        cdef CKDBComments* kdb = new CKDBComments(filepath.encode())
        del kdb

    def subset(self, pattern: str, copy: bool = False) -> Comments:
        subset_ = Comments()
        subset_.database_ptr = subset_.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        return subset_

    def _get_object(self, key):
        if not isinstance(key, str):
            raise TypeError(f"Cannot get object {key}.\nExpected a string value for {key} " + 
                "but got value of type {type(filepath).__name__}")
        return self.database_ptr.get(key.encode()).decode()

    def _set_object(self, key, value):
        if not isinstance(key, str):
            raise TypeError(f"Cannot set object {key}.\nExpected a string value for {key} " + 
                "but got value of type {type(filepath).__name__}")
        if self.database_ptr.contains(key.encode()):
            self.database_ptr.update(key.encode(), value.encode())
        else:
            self.database_ptr.add(key.encode(), value.encode())
    
    def add(self, name: str, comment: str):
        if not isinstance(name, str):
            raise TypeError(f"'name': Expected value of type string. Got value of type {type(name).__name__}")
        self.database_ptr.add(name.encode(), comment.encode())

    def update(self, name: str, comment: str):
        if not isinstance(name, str):
            raise TypeError(f"'name': Expected value of type string. Got value of type {type(name).__name__}")
        self.database_ptr.update(name.encode(), comment.encode())
