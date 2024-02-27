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
cdef class __CommentsDatabase(__AbstractDatabase):
    cdef CKDBComments* database_ptr

    def __cinit__(self):
        """
        """
        self.database_ptr = self.abstract_db_ptr = &cpp_global_comments

    def __dealloc__(self):
        # self.database_ptr points to the C++ global instance Comments 
        # which does not need to be manually deleted 
        pass

    # TODO: implement KDBAbstract::load() method (for global KDB only)
    def _load(self, filepath: str):
        cdef CKDBComments* kdb = new CKDBComments(filepath.encode())
        del kdb

    def subset(self, pattern: str, copy: bool = False) -> __CommentsDatabase:
        f"""{__AbstractDatabase.subset.__doc__}

        Examples
        --------
        >>> from iode import Comments
        >>> Comments.load("../data/fun.cmt")

        >>> # create a subset with all comments with name starting with 'A'
        >>> cmt_subset = Comments.subset("A*")
        >>> cmt_subset.get_names()
        ['ACAF', 'ACAG', 'ACOUG', 'AQC']
        >>> # any modification made on the subset is visible in the global database
        >>> cmt_subset['ACAF'] = "Modified Comment"
        >>> Comments['ACAF']
        'Modified Comment'

        >>> # force to return a 'deep copy' subset
        >>> cmt_subset = Comments.subset("*_", copy=True)
        >>> # any modification made on the subset let the global database unchanged
        >>> cmt_subset['BENEF_'] = "Modified Comment"
        >>> Comments['BENEF_']
        'Ondernemingen: niet-uitgekeerde winsten (vóór statistische aanpassing).'
        """
        cmt_subset = __CommentsDatabase()
        cmt_subset.database_ptr = cmt_subset.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        return cmt_subset

    def _get_object(self, name: str) -> str:
        return self.database_ptr.get(name.encode()).decode()

    def _set_object(self, name: str, value):
        if self.database_ptr.contains(name.encode()):
            self.database_ptr.update(name.encode(), value.encode())
        else:
            self.database_ptr.add(name.encode(), value.encode())
    
    def add(self, name: str, comment: str):
        """
        Add a new comment to the Comments database.
        Raise an error if the Comments database already contains a comment named 'name'.
        """
        if not isinstance(name, str):
            raise TypeError(f"'name': Expected value of type string. Got value of type {type(name).__name__}")
        self.database_ptr.add(name.encode(), comment.encode())

    def update(self, name: str, comment: str):
        """
        Update the value of an existing comment named 'name'.
        Raise an error if the Comments database does not contains any comment named 'name'.
        """
        if not isinstance(name, str):
            raise TypeError(f"'name': Expected value of type string. Got value of type {type(name).__name__}")
        self.database_ptr.update(name.encode(), comment.encode())


Comments = __CommentsDatabase()
