# distutils: language = c++

from collections.abc import Iterable
from typing import Union, Tuple, List, Optional, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

cimport cython
from pyiode.iode_database.cpp_api_database cimport KDBIdentities as CKDBIdentities
from pyiode.iode_database.cpp_api_database cimport Identities as cpp_global_identities
from pyiode.iode_database.cpp_api_database cimport Variables as cpp_global_variables


@cython.final
cdef class Identities(_AbstractDatabase):
    r"""
    IODE Identities database. 

    Attributes
    ----------
    filename: str
    description: str

    Parameters
    ----------
    filepath: str, optional
        file containing the IODE identities to load.

    Returns
    -------
    Identities

    Examples
    --------
    >>> from iode import identities, SAMPLE_DATA_DIR
    >>> identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")
    >>> len(identities)
    48
    >>> identities              # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Workspace: Identities
    nb identities: 48
    filename: ...\tests\data\fun.idt
    <BLANKLINE>
     name                                                   identities
    AOUC        ((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VM+VAFF))[-1]
    AOUC_       exp(ln(((WCF/NFYH)/QL)+PKF/(QAFF/KNFFY))*(QAFF/(QX+QAFF)+.05)[-1]+ln PM*(QM/ (QAFF+QM)-0.05)[-1])
    FLGR        FLG/VBBP
    GAP2        100*(QAFF_/(Q_F+Q_I))
    GAP_        100*((QAF_/Q_F)-1)
    ...         ...
    XTFP        grt TFPFHP_
    XW          0
    Y           QBBP_P
    YSEFPR      YSEFP/WBGP
    YSFICR      YSFIC/(TWGP*ZJ)
    <BLANKLINE>
    """
    cdef bint ptr_owner
    cdef CKDBIdentities* database_ptr

    def __cinit__(self, filepath: str = None) -> Identities:
        self.database_ptr = NULL
        self.abstract_db_ptr = NULL
        self.ptr_owner = False

    def __init__(self, filepath: str = None):
        # Prevent accidental instantiation from normal Python code
        # since we cannot pass a struct pointer into a Python constructor.
        raise TypeError("This class cannot be instantiated directly.")

    def __dealloc__(self):
        # if self.database_ptr points to the C++ global instance Identities 
        # which does not need to be manually deleted 
        if self.ptr_owner and self.database_ptr is not NULL:
            del self.database_ptr
            self.database_ptr = NULL

    # see https://cython.readthedocs.io/en/stable/src/userguide/extension_types.html#instantiation-from-existing-c-c-pointers 
    @staticmethod
    cdef Identities _from_ptr(CKDBIdentities* database_ptr = NULL, bint owner=False):
        # call to __new__() that bypasses the __init__() constructor.
        cdef Identities wrapper = Identities.__new__(Identities)
        if database_ptr is not NULL:
            wrapper.ptr_owner = owner
            wrapper.database_ptr = database_ptr
            wrapper.abstract_db_ptr = database_ptr
        else:
            wrapper.ptr_owner = False
            wrapper.database_ptr = &cpp_global_identities
            wrapper.abstract_db_ptr = &cpp_global_identities
        return wrapper

    # TODO: implement KDBAbstract::load() method (for global KDB only)
    def _load(self, filepath: str):
        cdef CKDBIdentities* kdb = new CKDBIdentities(filepath.encode())
        del kdb

    def _subset(self, pattern: str, copy: bool) -> Identities:
        cdef Identities subset_db = Identities.__new__(Identities)
        subset_db.database_ptr = subset_db.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        return subset_db

    def _get_object(self, key: str):
        key = key.strip()
        return self.database_ptr.get_lec(key.encode()).decode()

    def _set_object(self, key, value):
        if not isinstance(key, str):
            raise TypeError(f"Cannot set identity '{key}'.\nExpected a string value for the name " + 
                            f"but got name value of type {type(key).__name__}")
        key = key.strip()
        if not isinstance(value, str):
            raise TypeError(f"Cannot set identity '{key}'.\nExpected a string value for {key} " + 
                            f"but got value of type {type(value).__name__}")
        value = value.strip()
        if self.database_ptr.contains(key.encode()):
            self.database_ptr.update(key.encode(), value.encode())
        else:
            self.database_ptr.add(key.encode(), value.encode())

    def execute(self, identities: Union[str, List[str]] = None, from_period: Union[str, Period] = None, 
        to_period: Union[str, Period] = None, var_files: Union[str, List[str]] = None, 
        scalar_files: Union[str, List[str]] = None, trace: bool = False):
        """
        Execute the specified identity(ies).

        Parameters
        ----------
        identities : str or List[str], optional
            The identities to execute. If not provided, all identities are executed.
        from_period : str or Period, optional
            The starting period of the execution range. 
            Defaults to the starting period of the current Variables sample.
        to_period : str or Period, optional
            The ending period of the execution range.
            Defaults to the ending period of the current Variables sample.
        var_files : str or List[str], optional
            The variable files from which the required Variables are temporarily loaded.
            The Variables database is left unchanged after the execution of the identity(ies).
            Defaults to None (current content of the Variables database is used). 
        scalar_files : str or List[str], optional
            The scalar files from which the required Scalars are temporarily loaded.
            The Variables database is left unchanged after the execution of the identity(ies).
            Defaults to None (current content of the Scalars database is used).
        trace : bool, optional
            If True, prints the source of the variables and scalars.

        Returns
        -------
        None

        Examples
        --------
        >>> from iode import identities, variables, SAMPLE_DATA_DIR
        >>> identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> sample = variables.sample
        >>> sample
        '1960Y1:2015Y1'
        >>> identities["GAP_"]
        '100*((QAF_/Q_F)-1)'
        >>> identities["GAP2"]
        '100*(QAFF_/(Q_F+Q_I))'
        >>> # reset variables GAP_ and GAP2
        >>> variables["GAP_"] = 0.
        >>> variables["GAP_"]                   # doctest: +ELLIPSIS 
        [0.0, 0.0, 0.0, ..., 0.0, 0.0, 0.0]
        >>> variables["GAP2"] = 0.
        >>> variables["GAP2"]                   # doctest: +ELLIPSIS 
        [0.0, 0.0, 0.0, ..., 0.0, 0.0, 0.0]

        >>> # compute GAP_ and GAP2 (assuming Scalars and Variables are already loaded)
        >>> identities.execute("GAP_;GAP2")
        >>> variables["GAP_"]                   # doctest: +ELLIPSIS 
        [-3.20493949860704, -3.981808446333557, ..., 3.7800671441993616, 3.2396415884531793]
        >>> variables["GAP2"]                   # doctest: +ELLIPSIS 
        [96.92655844699298, 97.39603007168847, ..., 102.14581982070376, 101.58578527761608]

        >>> # compute GAP_ and GAP2 over a subset of the sample
        >>> variables["GAP_"] = 0.
        >>> variables["GAP2"] = 0.
        >>> identities.execute("GAP_;GAP2", "2000Y1", "2005Y1")
        >>> variables["GAP_", "2000Y1:2005Y1"]
        [4.510095736743436, 3.312304975734315, 2.6151793579969107, 3.464117181974924, 5.478645527985804, 5.578699398837528]
        >>> variables["GAP2", "2000Y1:2005Y1"]
        [104.60957761618035, 103.05782573291968, 102.17336700422976, 102.82322081548728, 104.4719275849864, 104.3586710898436]

        >>> # compute GAP_ and GAP2 assuming Variables are not already loaded
        >>> variables.clear()
        >>> variables.sample = '1960Y1:2015Y1'
        >>> variables.names
        []
        >>> # setting the var_files argument will fetch the required values of 
        >>> # 'QAF_', 'QAFF_', 'Q_F' and 'Q_I' from the passed Variables file
        >>> identities.execute("GAP_;GAP2", var_files=f"{SAMPLE_DATA_DIR}/fun.var")
        >>> variables["GAP_"]                   # doctest: +ELLIPSIS 
        [-3.20493949860704, -3.981808446333557, ..., 3.7800671441993616, 3.2396415884531793]
        >>> variables["GAP2"]                   # doctest: +ELLIPSIS 
        [96.92655844699298, 97.39603007168847, ..., 102.14581982070376, 101.58578527761608]
        >>> # note that the variables 'QAF_', 'QAFF_', 'Q_F' and 'Q_I' are not 
        >>> # present in the Variables database after running identities.execute
        >>> variables.names
        ['GAP2', 'GAP_']
        """
        if identities is None:
            identities = ""
        if isinstance(identities, str):
            pass
        elif isinstance(identities, Iterable) and all(isinstance(item, str) for item in identities):
            identities = ';'.join(identities)
        else:
            raise TypeError("'identities': Expected value of type str or a list of str. "
                            f"Got value of type {type(identities).__name__}")
        
        if from_period is None or to_period is None:
            c_sample = cpp_global_variables.get_sample()
            if from_period is None:
                from_period = c_sample.start_period().to_string().decode()
            if to_period is None:
                to_period = c_sample.end_period().to_string().decode()
        
        if isinstance(from_period, Period):
            from_period = str(from_period)
        if not isinstance(from_period, str):
            raise TypeError("'from_period': Expected value of type str or Period. "
                            f"Got value of type {type(from_period).__name__}")

        if isinstance(to_period, Period):
            to_period = str(to_period)
        if not isinstance(to_period, str):
            raise TypeError("'to_period': Expected value of type str or Period. "
                            f"Got value of type {type(to_period).__name__}")

        if var_files is None:
            var_files = ""
        if isinstance(var_files, str):
            pass
        elif isinstance(var_files, Iterable) and all(isinstance(item, str) for item in var_files):
            var_files = ';'.join(var_files)
        else:
            raise TypeError("'var_files': Expected value of type str or a list of str. "
                            f"Got value of type {type(var_files).__name__}")

        if scalar_files is None:
            scalar_files = ""
        if isinstance(scalar_files, str):
            pass
        elif isinstance(scalar_files, Iterable) and all(isinstance(item, str) for item in scalar_files):
            scalar_files = ';'.join(scalar_files)
        else:
            raise TypeError("'scalar_files': Expected value of type str or a list of str. "
                            f"Got value of type {type(scalar_files).__name__}")

        self.database_ptr.execute_identities(from_period.encode(), to_period.encode(), identities.encode(), 
                                            var_files.encode(), scalar_files.encode(), <bint>trace)

    def from_series(self, s: Series):
        r"""
        Copy the pandas Series `s` into the IODE Identities database.
        The identity names to copy are deduced from the index of the Series.

        Parameters
        ----------
        s: Series
            pandas Series containing the identities to copy into the IODE Identities database.

        Notes
        -----
        The index of the passed Series is sorted in alphabetical order before 
        copying to IODE Identities database.

        See Also
        --------
        Identities.to_series

        Examples
        --------
        >>> from iode import identities
        >>> import pandas as pd
        >>> identities.clear()
        >>> len(identities)
        0

        >>> # create the pandas Series
        >>> names = ["CONST", "LOG_T", "EXP_T", "GRT_T", "MAVG_T", "DER_LOG_T"]
        >>> data = ["1", "ln t", "exp t", "grt t", "mavg t", "d(ln t)"]
        >>> s = pd.Series(data=data, index=names, dtype=str, name="Identities")
        >>> # display the pandas series
        >>> s          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        CONST              1
        LOG_T           ln t
        EXP_T          exp t
        GRT_T          grt t
        MAVG_T        mavg t
        DER_LOG_T    d(ln t)
        Name: Identities, dtype: object

        >>> # load into the IODE Identities database
        >>> identities.from_series(s)
        >>> len(identities)
        6
        
        >>> identities.names             # doctest: +ELLIPSIS
        ['CONST', 'DER_LOG_T', 'EXP_T', 'GRT_T', 'LOG_T', 'MAVG_T']
        >>> identities["LOG_T"]
        'ln t'
        >>> identities["DER_LOG_T"]
        'd(ln t)'
        """
        if pd is None:
            raise RuntimeError("pandas library not found")

        for index, value in s.items():
            self._set_object(index, value)

    def to_series(self) -> Series:
        r"""
        Create a pandas Series from the current Identities database.
        The index of the returned Series is build from the Identities names.

        See Also
        --------
        Identities.from_series

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, identities
        >>> import pandas as pd
        >>> identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")
        >>> len(identities)
        48

        >>> # Export the IODE Identities database as a pandas Series
        >>> s = identities.to_series()
        >>> len(s)
        48

        >>> s.index.to_list()               # doctest: +ELLIPSIS
        ['AOUC', 'AOUC_', 'FLGR', ..., 'XW', 'Y', 'YSEFPR', 'YSFICR']
        >>> identities["GAP_"]              # doctest: +NORMALIZE_WHITESPACE
        '100*((QAF_/Q_F)-1)' 
        >>> s["GAP_"]                       # doctest: +NORMALIZE_WHITESPACE
        '100*((QAF_/Q_F)-1)' 
        >>> identities["XTFP"]              # doctest: +NORMALIZE_WHITESPACE
        'grt TFPFHP_'
        >>> s["XTFP"]                       # doctest: +NORMALIZE_WHITESPACE
        'grt TFPFHP_'

        >>> # Export a subset of the IODE Identities database as a pandas Series
        >>> s = identities["X*;*_"].to_series()
        >>> len(s)
        15

        >>> s.index.to_list()               # doctest: +ELLIPSIS
        ['AOUC_', 'GAP_', 'XEX', 'XNATY', ..., 'XTFP', 'XW']
        >>> identities["GAP_"]              # doctest: +NORMALIZE_WHITESPACE
        '100*((QAF_/Q_F)-1)' 
        >>> s["GAP_"]                       # doctest: +NORMALIZE_WHITESPACE
        '100*((QAF_/Q_F)-1)' 
        >>> identities["XTFP"]              # doctest: +NORMALIZE_WHITESPACE
        'grt TFPFHP_'
        >>> s["XTFP"]                       # doctest: +NORMALIZE_WHITESPACE
        'grt TFPFHP_'
        """
        if pd is None:
            raise RuntimeError("pandas library not found")
        
        names = self.names
        data = [self._get_object(name) for name in names]
        return pd.Series(data=data, index=names, dtype=str, name=self.__class__.__name__)

    @property
    def series(self) -> Series:
        r"""
        Create a pandas Series from the current Identities database.
        The index of the returned Series is build from the Identities names.

        See Also
        --------
        Identities.to_series

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, identities
        >>> import pandas as pd
        >>> identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")
        >>> len(identities)
        48

        >>> # Export the IODE Identities database as a pandas Series
        >>> s = identities.series
        >>> len(s)
        48

        >>> s.index.to_list()               # doctest: +ELLIPSIS
        ['AOUC', 'AOUC_', 'FLGR', ..., 'XW', 'Y', 'YSEFPR', 'YSFICR']
        >>> identities["GAP_"]              # doctest: +NORMALIZE_WHITESPACE
        '100*((QAF_/Q_F)-1)' 
        >>> s["GAP_"]                       # doctest: +NORMALIZE_WHITESPACE
        '100*((QAF_/Q_F)-1)' 
        >>> identities["XTFP"]              # doctest: +NORMALIZE_WHITESPACE
        'grt TFPFHP_'
        >>> s["XTFP"]                       # doctest: +NORMALIZE_WHITESPACE
        'grt TFPFHP_'

        >>> # Export a subset of the IODE Identities database as a pandas Series
        >>> s = identities["X*;*_"].series
        >>> len(s)
        15

        >>> s.index.to_list()               # doctest: +ELLIPSIS
        ['AOUC_', 'GAP_', 'XEX', 'XNATY', ..., 'XTFP', 'XW']
        >>> identities["GAP_"]              # doctest: +NORMALIZE_WHITESPACE
        '100*((QAF_/Q_F)-1)' 
        >>> s["GAP_"]                       # doctest: +NORMALIZE_WHITESPACE
        '100*((QAF_/Q_F)-1)' 
        >>> identities["XTFP"]              # doctest: +NORMALIZE_WHITESPACE
        'grt TFPFHP_'
        >>> s["XTFP"]                       # doctest: +NORMALIZE_WHITESPACE
        'grt TFPFHP_'
        """
        return self.to_series()

    def _str_table(self, names: List[str]) -> str:
        columns = {"name": names, "identities": [join_lines(self._get_object(name)) for name in names]}
        return table2str(columns, max_lines=10, justify_funcs={"name": JUSTIFY.LEFT, "identities": JUSTIFY.LEFT})


identities: Identities = Identities._from_ptr()
