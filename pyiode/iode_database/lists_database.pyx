# distutils: language = c++

from collections.abc import Iterable
from typing import Union, Tuple, List, Optional, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

cimport cython
from pyiode.iode_database.cpp_api_database cimport KDBLists as CKDBLists
from pyiode.iode_database.cpp_api_database cimport Lists as cpp_global_lists


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
    cdef bint ptr_owner
    cdef CKDBLists* database_ptr

    def __cinit__(self, filepath: str = None) -> Lists:
        self.database_ptr = NULL
        self.abstract_db_ptr = NULL
        self.ptr_owner = False

    def __init__(self, filepath: str = None):
        # Prevent accidental instantiation from normal Python code
        # since we cannot pass a struct pointer into a Python constructor.
        raise TypeError("This class cannot be instantiated directly.")

    def __dealloc__(self):
        # if self.database_ptr points to the C++ global instance Lists 
        # which does not need to be manually deleted 
        if self.ptr_owner and self.database_ptr is not NULL:
            del self.database_ptr
            self.database_ptr = NULL

    # see https://cython.readthedocs.io/en/stable/src/userguide/extension_types.html#instantiation-from-existing-c-c-pointers 
    @staticmethod
    cdef Lists _from_ptr(CKDBLists* database_ptr = NULL, bint owner=False):
        # call to __new__() that bypasses the __init__() constructor.
        cdef Lists wrapper = Lists.__new__(Lists)
        if database_ptr is not NULL:
            wrapper.ptr_owner = owner
            wrapper.database_ptr = database_ptr
            wrapper.abstract_db_ptr = database_ptr
        else:
            wrapper.ptr_owner = False
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

    def from_series(self, s: Series):
        r"""
        Copy the pandas Series `s` into the IODE Lists database.
        The IODE list names to copy are deduced from the index of the Series.

        Parameters
        ----------
        s: Series
            pandas Series containing the IODE lists to copy into the IODE Lists database.

        Notes
        -----
        The index of the passed Series is sorted in alphabetical order before 
        copying to IODE Lists database.

        See Also
        --------
        Lists.to_series

        Examples
        --------
        >>> from iode import lists, split_list
        >>> import pandas as pd
        >>> lists.clear()
        >>> len(lists)
        0

        >>> # create the pandas Series
        >>> names = ["SEMICOLON_LST", "COMMA_LST", "WHITESPACE_LST", "TAB_LST", "MIX_LST"]
        >>> semicolon_lst = "A;B;C;D;E;F"
        >>> comma_lst = "A,B,C,D,E,F"
        >>> whitespace_lst = "A B C D E F" 
        >>> tab_lst = "A    B    C    D    E    F"
        >>> mix_lst = "  A;B,C D    E;F  "
        >>> data = [semicolon_lst, comma_lst, whitespace_lst, tab_lst, mix_lst]
        >>> s = pd.Series(data=data, index=names, dtype=str, name="Lists")
        >>> # display the pandas series
        >>> s          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        SEMICOLON_LST                        A;B;C;D;E;F
        COMMA_LST                            A,B,C,D,E,F
        WHITESPACE_LST                       A B C D E F
        TAB_LST               A    B    C    D    E    F
        MIX_LST                         A;B,C D    E;F
        Name: Lists, dtype: object

        >>> # load into the IODE Lists database
        >>> lists.from_series(s)
        >>> len(lists)
        5
        
        >>> lists.get_names()
        ['COMMA_LST', 'MIX_LST', 'SEMICOLON_LST', 'TAB_LST', 'WHITESPACE_LST']
        >>> lists["SEMICOLON_LST"]
        'A;B;C;D;E;F'
        >>> split_list(lists["SEMICOLON_LST"])
        ['A', 'B', 'C', 'D', 'E', 'F']
        >>> # note: leading and trailing whitespaces are automatically stripped 
        >>> #       when copied into the Lists database
        >>> mix_lst
        '  A;B,C D    E;F  '
        >>> lists["MIX_LST"]
        'A;B,C D    E;F'
        >>> split_list(lists["MIX_LST"])
        ['A', 'B', 'C', 'D', 'E', 'F']
        """
        if pd is None:
            raise RuntimeError("pandas library not found")

        for index, value in s.items():
            self._set_object(index, value)

    def to_series(self) -> Series:
        r"""
        Create a pandas Series from the current Lists database.
        The index of the returned Series is build from the Lists names.

        See Also
        --------
        Lists.from_series

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, lists
        >>> import pandas as pd
        >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
        >>> len(lists)
        17

        >>> # Export the IODE Lists database as a pandas Series
        >>> s = lists.to_series()
        >>> len(s)
        17

        >>> s.index.to_list()               # doctest: +ELLIPSIS
        ['COPY', 'COPY0', 'COPY1', ..., 'XSCENARIO', '_SCAL', '_SEARCH']
        >>> lists["ENVI"]                   # doctest: +NORMALIZE_WHITESPACE
        'EX;PWMAB;PWMS;PWXAB;PWXS;QWXAB;QWXS;POIL;NATY;TFPFHP_' 
        >>> s["ENVI"]                       # doctest: +NORMALIZE_WHITESPACE
        'EX;PWMAB;PWMS;PWXAB;PWXS;QWXAB;QWXS;POIL;NATY;TFPFHP_' 
        >>> lists["MAINEQ"]                 # doctest: +NORMALIZE_WHITESPACE
        'W;NFYH;KNFF;PC;PXAB;PMAB;QXAB;QMAB;QC_'
        >>> s["MAINEQ"]                     # doctest: +NORMALIZE_WHITESPACE
        'W;NFYH;KNFF;PC;PXAB;PMAB;QXAB;QMAB;QC_'

        >>> # Export a subset of the IODE Lists database as a pandas Series
        >>> s = lists["E*;MA*"].to_series()
        >>> len(s)
        5

        >>> s.index.to_list()
        ['ENDO', 'ENDO0', 'ENDO1', 'ENVI', 'MAINEQ']
        >>> lists["ENVI"]                   # doctest: +NORMALIZE_WHITESPACE
        'EX;PWMAB;PWMS;PWXAB;PWXS;QWXAB;QWXS;POIL;NATY;TFPFHP_' 
        >>> s["ENVI"]                       # doctest: +NORMALIZE_WHITESPACE
        'EX;PWMAB;PWMS;PWXAB;PWXS;QWXAB;QWXS;POIL;NATY;TFPFHP_' 
        >>> lists["MAINEQ"]                 # doctest: +NORMALIZE_WHITESPACE
        'W;NFYH;KNFF;PC;PXAB;PMAB;QXAB;QMAB;QC_'
        >>> s["MAINEQ"]                     # doctest: +NORMALIZE_WHITESPACE
        'W;NFYH;KNFF;PC;PXAB;PMAB;QXAB;QMAB;QC_'
        """
        if pd is None:
            raise RuntimeError("pandas library not found")
        
        names = self.get_names()
        data = [self._get_object(name) for name in names]
        return pd.Series(data=data, index=names, dtype=str, name=self.__class__.__name__)


lists: Lists = Lists._from_ptr()
