# distutils: language = c++

from collections.abc import Iterable
from typing import Union, Tuple, List, Optional, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

cimport cython
from cython.operator cimport dereference
from pyiode.iode_database.cpp_api_database cimport hash_value
from pyiode.iode_database.cpp_api_database cimport KDBIdentities as CKDBIdentities
from pyiode.iode_database.cpp_api_database cimport Identities as cpp_global_identities
from pyiode.iode_database.cpp_api_database cimport Variables as cpp_global_variables
from pyiode.iode_database.cpp_api_database cimport KCPTR, KIPTR, KLPTR, KVPTR


cdef class Identities(IodeDatabase):
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
    >>> identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Loading .../fun.idt
    48 objects loaded 
    >>> len(identities)
    48
    >>> identities                                          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
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

    @staticmethod
    def __init_instance(instance: Identities) -> Self:
        instance.ptr_owner = False
        instance.database_ptr = &cpp_global_identities
        instance.abstract_db_ptr = &cpp_global_identities
        return instance

    @classmethod
    def get_instance(cls) -> Self:
        instance = cls.__new__(cls)
        return cls.__init_instance(instance)

    @classmethod
    def _new_instance(cls) -> Self:
        instance = cls.__new__(cls)
        return instance

    # TODO: implement KDBAbstract::load() method (for global KDB only)
    def _load(self, filepath: str):
        cdef CKDBIdentities* kdb = new CKDBIdentities(filepath.encode())
        del kdb

    def _subset(self, pattern: str, copy: bool) -> Identities:
        subset_db: Identities = self._new_instance()
        subset_db.database_ptr = subset_db.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        return subset_db

    @property
    def i(self) -> PositionalIndexer:
        """
        Allow to select the ith identity in the database.

        Examples
        --------
        >>> from iode import identities, SAMPLE_DATA_DIR
        >>> identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.idt
        48 objects loaded 
        >>> # get the first identity
        >>> identities.i[0]
        Identity('((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VM+VAFF))[-1]')
        >>> # get the last identity
        >>> identities.i[-1]
        Identity('YSFIC/(TWGP*ZJ)')
        >>> # update first identity
        >>> identities.i[0] = 't'
        >>> identities.i[0]
        Identity('t')
        >>> # update last identity
        >>> identities.i[-1] = 't'
        >>> identities.i[-1]
        Identity('t')
        """
        return PositionalIndexer(self)

    def _get_object(self, key: Union[str, int]):
        cdef CIdentity* c_identity
        if isinstance(key, int):
            c_identity = self.database_ptr.get(<int>key)
        else:
            key  = key.strip()
            c_identity = self.database_ptr.get(<string>(key.encode()))
        
        # self.database_ptr.get() does not allocate a new C++ Identity instance
        py_identity = Identity._from_ptr(c_identity, <bint>False) 
        return py_identity

    def _set_object(self, key: Union[str, int], value: Union[str, Identity]):
        if isinstance(value, Identity):
            value = str(value)
        value = value.strip()

        if isinstance(key, int):
            self.database_ptr.update(<int>key, <string>(value.encode()))
        else:
            key = key.strip()
            if self.database_ptr.contains(key.encode()):
                self.database_ptr.update(<string>(key.encode()), <string>(value.encode()))
            else:
                self.database_ptr.add(key.encode(), value.encode())

    def __getitem__(self, key: Union[str, List[str]]) -> Union[Identity, Identities]:
        r"""
        Return the (subset of) identity(ies) referenced by `key`.

        The `key` can represent a single object name (e.g. "ACAF") or a list of object names ("ACAF;ACAG;AOUC") 
        or a pattern (e.g. "A*") or a list of sub-patterns (e.g. "A*;*_").
        
        If the `key` represents a list of object names or of sub-patterns, each name or sub-pattern is separated 
        by a `separator` character which is either a whitespace ` `, or a comma `,`, or a semi-colon `;`, or a 
        tabulation `\t`, or a newline `\n`.

        A (sub-)`pattern` is a list of characters representing a group of object names. 
        It includes some special characters which have a special meaning:
        
            - `*` : any character sequence, even empty
            - `?` : any character (one and only one)
            - `@` : any alphanumerical char [A-Za-z0-9]
            - `&` : any non alphanumerical char
            - `|` : any alphanumeric character or none at the beginning and end of a string 
            - `!` : any non-alphanumeric character or none at the beginning and end of a string 
            - `\` : escape the next character

        Note that the `key` can contain references to IODE lists which are prefixed with the symbol `$`.

        Parameters
        ----------
        key: str or list(str)
            (the list of) name(s) of the identity(ies) to get.
            The list of identities to get can be specified by a pattern or by a list of sub-patterns (e.g. "A*;*_").

        Returns
        -------
        Single identity or a subset of the database.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import identities
        >>> identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.idt
        48 objects loaded 

        >>> # a) get one Identity
        >>> identities["AOUC"]
        Identity('((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VM+VAFF))[-1]')

        >>> # b) get a subset of the Identities database using a pattern
        >>> identities_subset = identities["X*"]
        >>> identities_subset.names
        ['XEX', 'XNATY', 'XPOIL', 'XPWMAB', 'XPWMS', 'XPWXAB', 'XPWXS', 'XQWXAB', 'XQWXS', 'XQWXSS', 'XRLBER', 'XTFP', 'XW']

        >>> # c) get a subset of the Identities database using a list of names
        >>> identities_subset = identities[["XEX", "XPWMAB", "XPWMS", "XQWXS", "XTFP"]]
        >>> identities_subset.names
        ['XEX', 'XPWMAB', 'XPWMS', 'XQWXS', 'XTFP']
        """
        return super().__getitem__(key)

    def __setitem__(self, key: Union[str, List[str]], value: Union[str, Identity, List[Union[str, Identity]]]):
        r"""
        Update/add a (subset of) identity(ies) referenced by `key` from/to the Identities database.

        The `key` can represent a single object name (e.g. "ACAF") or a list of object names ("ACAF;ACAG;AOUC") 
        or a pattern (e.g. "A*") or a list of sub-patterns (e.g. "A*;*_").
        
        If the `key` represents a list of object names or of sub-patterns, each name or sub-pattern is separated 
        by a `separator` character which is either a whitespace ` `, or a comma `,`, or a semi-colon `;`, or a 
        tabulation `\t`, or a newline `\n`.

        A (sub-)`pattern` is a list of characters representing a group of object names. 
        It includes some special characters which have a special meaning:
        
            - `*` : any character sequence, even empty
            - `?` : any character (one and only one)
            - `@` : any alphanumerical char [A-Za-z0-9]
            - `&` : any non alphanumerical char
            - `|` : any alphanumeric character or none at the beginning and end of a string 
            - `!` : any non-alphanumeric character or none at the beginning and end of a string 
            - `\` : escape the next character

        Note that the `key` can contain references to IODE lists which are prefixed with the symbol `$`.

        Parameters
        ----------
        key: str or list(str)
            (the list of) name(s) of the identity(ies) to update/add.
            The list of identities to update/add can be specified by a pattern or by a list of sub-patterns 
            (e.g. "A*;*_").
        value: str, Identity, dict(str, str or Identity) or pd.Series(str) or Identities
            (new) identity(ies) value(s).

        Examples
        --------
        >>> import pandas as pd
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import identities
        >>> identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.idt
        48 objects loaded 
        
        >>> # a) add one identity
        >>> identities["BDY"] = "YN - YK"
        >>> identities["BDY"]
        Identity('YN - YK')

        >>> # b) update one identity
        >>> identities["AOUC"]
        Identity('((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VM+VAFF))[-1]')
        >>> identities["AOUC"] = '(WCRH / WCRH[1990Y1]) * (VAFF / (VM+VAFF))[-1] + PM * (VM / (VM+VAFF))[-1]'
        >>> identities["AOUC"]
        Identity('(WCRH / WCRH[1990Y1]) * (VAFF / (VM+VAFF))[-1] + PM * (VM / (VM+VAFF))[-1]')

        >>> # c) add/update several identities at once
        >>> # 1) using a dict of values
        >>> values = {"GAP2": "0.9 * 100*(QAFF_/(Q_F+Q_I))", "GAP_": "0.9 * 100*((QAF_/Q_F)-1)", 
        ...           "GOSFR": "0.9 * (GOSF/VAF_)"}
        >>> identities["GAP2, GAP_, GOSFR"] = values
        >>> identities["GAP2, GAP_, GOSFR"]             # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE               
        Workspace: Identities
        nb identities: 3
        filename: ...fun.idt
        <BLANKLINE>
        name	         identities        
        GAP2 	0.9 * 100*(QAFF_/(Q_F+Q_I))
        GAP_ 	0.9 * 100*((QAF_/Q_F)-1)   
        GOSFR	0.9 * (GOSF/VAF_)   

        >>> # 2) using a pandas series
        >>> data = ["0.8 * 100*(QAFF_/(Q_F+Q_I))", "0.8 * 100*((QAF_/Q_F)-1)", "0.8 * (GOSF/VAF_)"]
        >>> series = pd.Series(data, index=["GAP2", "GAP_", "GOSFR"])
        >>> identities["GAP2, GAP_, GOSFR"] = series
        >>> identities["GAP2, GAP_, GOSFR"]             # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Identities
        nb identities: 3
        filename: ...fun.idt
        <BLANKLINE>
        name	         identities        
        GAP2 	0.8 * 100*(QAFF_/(Q_F+Q_I))
        GAP_ 	0.8 * 100*((QAF_/Q_F)-1)   
        GOSFR	0.8 * (GOSF/VAF_) 

        >>> # 3) using another Identities database (subset)
        >>> identities_subset = identities["GAP2, GAP_, GOSFR"].copy()
        >>> identities_subset["GAP2"] = "0.7 * 100*(QAFF_/(Q_F+Q_I))"
        >>> identities_subset["GAP_"] = "0.7 * 100*((QAF_/Q_F)-1)"
        >>> identities_subset["GOSFR"] = "0.7 * (GOSF/VAF_)"
        >>> identities["GAP2, GAP_, GOSFR"] = identities_subset
        >>> identities["GAP2, GAP_, GOSFR"]             # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Identities
        nb identities: 3
        filename: ...fun.idt
        <BLANKLINE>
        name	         identities        
        GAP2 	0.7 * 100*(QAFF_/(Q_F+Q_I))
        GAP_ 	0.7 * 100*((QAF_/Q_F)-1)   
        GOSFR	0.7 * (GOSF/VAF_) 

        >>> # d) working on a subset
        >>> # 1) get subset
        >>> identities_subset = identities["X*"]
        >>> identities_subset.names
        ['XEX', 'XNATY', 'XPOIL', 'XPWMAB', 'XPWMS', 'XPWXAB', 'XPWXS', 'XQWXAB', 'XQWXS', 'XQWXSS', 'XRLBER', 'XTFP', 'XW']
        >>> # 2) add an identity to the subset 
        >>> identities_subset["XDPU"] = "grt DPU"
        >>> identities_subset["XDPU"]
        Identity('grt DPU')
        >>> # --> new identity also appears in the global workspace
        >>> "XDPU" in identities
        True
        >>> identities["XDPU"]
        Identity('grt DPU')
        >>> # 3) update an identity in the subset
        >>> identities_subset["XDPU"] = "0"
        >>> identities_subset["XDPU"]
        Identity('0')
        >>> # --> identity is also updated in the global workspace
        >>> identities["XDPU"]
        Identity('0')
        """
        super().__setitem__(key, value)

    def __delitem__(self, key):
        """
        Remove the (subset of) identity(ies) referenced by `key` from the Identities database.

        Parameters
        ----------
        key: str or list(str)
            (list of) name(s) of the identity(ies) to be removed.
            The list of names can be given as a string pattern (e.g. "A*;*_").

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import identities
        >>> identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.idt
        48 objects loaded 

        >>> # a) delete one identity
        >>> identities.get_names("W*")
        ['W', 'WBGR', 'WCRH', 'WMINR', 'WO']
        >>> del identities["W"]
        >>> identities.get_names("W*")
        ['WBGR', 'WCRH', 'WMINR', 'WO']

        >>> # b) delete several identities at once using a pattern
        >>> del identities["W*"]
        >>> identities.get_names("W*")
        []

        >>> # c) delete several identities at once using a list of names
        >>> identities.get_names("XP*")
        ['XPOIL', 'XPWMAB', 'XPWMS', 'XPWXAB', 'XPWXS']
        >>> del identities[["XPOIL", "XPWXS"]]
        >>> identities.get_names("XP*")
        ['XPWMAB', 'XPWMS', 'XPWXAB']

        >>> # delete one identity from a subset of the global database
        >>> identities_subset = identities["Y*"]
        >>> identities_subset.names
        ['Y', 'YSEFPR', 'YSFICR']
        >>> del identities_subset["Y"]
        >>> identities_subset.names
        ['YSEFPR', 'YSFICR']
        >>> # NOTE: the identity has also been deleted from the global database
        >>> "Y" in identities
        False
        >>> identities.get_names("Y*")
        ['YSEFPR', 'YSFICR']
        """
        super().__delitem__(key)

    @property
    def coefficients(self) -> List[str]:
        """
        Return the list of coefficients (scalars) used in the identities of 
        (the subset of) the database.
        
        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import identities
        >>> identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.idt
        48 objects loaded 
        >>> identities["A*;N*"].coefficients
        ['gamma2', 'gamma3', 'gamma4', 'gamma_']
        """
        return self._coefficients()
    
    @property
    def variables(self) -> List[str]:
        """
        Return the list of variables used in the identities of (the subset of) the database.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import identities
        >>> identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.idt
        48 objects loaded 
        >>> identities["A*;N*"].variables                       # doctest: +ELLIPSIS
        ['KNFFY', 'NFYH', 'PKF', 'PM', 'QAFF', ..., 'WCF', 'WCRH', 'WMIN', 'ZJ']
        """
        return self._variables()

    def copy_from(self, input_files: Union[str, List[str]], names: Union[str, List[str]]='*'):
        """
        Copy (a subset of) identities from the input file(s) 'input_files' into the current database.

        Parameters
        ----------
        input_file: str or list(str)
            file(s) from which the copied identities are read.
        names: str or list(str)
            list of identities to copy from the input file(s).
            Defaults to load all identities from the input file(s). 
        """
        if not (self.is_global_workspace or self.is_detached):
            raise RuntimeError("Cannot call 'copy_from' method on a subset of a workspace")
        input_files, names = self._copy_from(input_files, names)
        self.database_ptr.copy_from(input_files.encode(), names.encode())

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
        >>> identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.idt
        48 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> sample = variables.sample
        >>> sample
        Sample("1960Y1:2015Y1")
        >>> identities["GAP_"]
        Identity('100*((QAF_/Q_F)-1)')
        >>> identities["GAP2"]
        Identity('100*(QAFF_/(Q_F+Q_I))')
        >>> # reset variables GAP_ and GAP2
        >>> variables["GAP_"] = 0.
        >>> variables["GAP_"]                   # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE 
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  1962Y1  ...  2013Y1  2014Y1  2015Y1
        GAP_          0.00    0.00    0.00  ...    0.00    0.00    0.00
        <BLANKLINE>
        >>> variables["GAP2"] = 0.
        >>> variables["GAP2"]                   # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE 
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  1962Y1  ...  2013Y1  2014Y1  2015Y1
        GAP2          0.00    0.00    0.00  ...    0.00    0.00    0.00
        <BLANKLINE>

        >>> # compute GAP_ and GAP2 (assuming Scalars and Variables are already loaded)
        >>> identities.execute("GAP_;GAP2")
        >>> variables["GAP_"]                   # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  1962Y1  ...  2013Y1  2014Y1  2015Y1
        GAP_         -3.20   -3.98   -2.12  ...    4.06    3.78    3.24
        <BLANKLINE>
        >>> variables["GAP2"]                   # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  1962Y1  ...  2013Y1  2014Y1  2015Y1
        GAP2         96.93   97.40   98.37  ...  102.50  102.15  101.59
        <BLANKLINE>

        >>> # compute GAP_ and GAP2 over a subset of the sample
        >>> variables["GAP_"] = 0.
        >>> variables["GAP2"] = 0.
        >>> identities.execute("GAP_;GAP2", "2000Y1", "2005Y1")
        >>> variables["GAP_", "2000Y1:2005Y1"]      # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 2000Y1:2005Y1
        mode: LEVEL
        <BLANKLINE>
        name        2000Y1  2001Y1  2002Y1  2003Y1  2004Y1  2005Y1
        GAP_          4.51    3.31    2.62    3.46    5.48    5.58
        <BLANKLINE>
        >>> variables["GAP2", "2000Y1:2005Y1"]      # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 2000Y1:2005Y1
        mode: LEVEL
        <BLANKLINE>
        name        2000Y1  2001Y1  2002Y1  2003Y1  2004Y1  2005Y1
        GAP2        104.61  103.06  102.17  102.82  104.47  104.36
        <BLANKLINE>

        >>> # compute GAP_ and GAP2 assuming Variables are not already loaded
        >>> variables.clear()
        >>> variables.sample = "1960Y1:2015Y1"
        >>> variables.names
        []
        >>> # setting the var_files argument will fetch the required values of 
        >>> # 'QAF_', 'QAFF_', 'Q_F' and 'Q_I' from the passed Variables file
        >>> identities.execute("GAP_;GAP2", var_files=f"{SAMPLE_DATA_DIR}/fun.var")
        >>> variables["GAP_"]                   # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE 
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  1962Y1  1963Y1  ...  2012Y1  2013Y1  2014Y1  2015Y1
        GAP_         -3.20   -3.98   -2.12   -2.65  ...    3.61    4.06    3.78    3.24
        <BLANKLINE>
        >>> variables["GAP2"]                   # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE 
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  1962Y1  1963Y1  ...  2012Y1  2013Y1  2014Y1  2015Y1
        GAP2         96.93   97.40   98.37   97.71  ...  102.19  102.50  102.15  101.59
        <BLANKLINE>
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
        Identity('ln t')
        >>> identities["DER_LOG_T"]
        Identity('d(ln t)')
        """
        if pd is None:
            raise RuntimeError("pandas library not found")

        if not (self.is_global_workspace or self.is_detached):
            # check that all names in the pandas object are present in the current subset 
            self._check_same_names(self.names, s.index.tolist())

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
        >>> identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.idt
        48 objects loaded 
        >>> len(identities)
        48

        >>> # Export the IODE Identities database as a pandas Series
        >>> s = identities.to_series()
        >>> len(s)
        48

        >>> s.index.to_list()               # doctest: +ELLIPSIS
        ['AOUC', 'AOUC_', 'FLGR', ..., 'XW', 'Y', 'YSEFPR', 'YSFICR']
        >>> identities["GAP_"]              # doctest: +NORMALIZE_WHITESPACE
        Identity('100*((QAF_/Q_F)-1)')
        >>> s["GAP_"]                       # doctest: +NORMALIZE_WHITESPACE
        '100*((QAF_/Q_F)-1)'
        >>> identities["XTFP"]              # doctest: +NORMALIZE_WHITESPACE
        Identity('grt TFPFHP_')
        >>> s["XTFP"]                       # doctest: +NORMALIZE_WHITESPACE
        'grt TFPFHP_'

        >>> # Export a subset of the IODE Identities database as a pandas Series
        >>> s = identities["X*;*_"].to_series()
        >>> len(s)
        15

        >>> s.index.to_list()               # doctest: +ELLIPSIS
        ['AOUC_', 'GAP_', 'XEX', 'XNATY', ..., 'XTFP', 'XW']
        >>> identities["GAP_"]              # doctest: +NORMALIZE_WHITESPACE
        Identity('100*((QAF_/Q_F)-1)')
        >>> s["GAP_"]                       # doctest: +NORMALIZE_WHITESPACE
        '100*((QAF_/Q_F)-1)'
        >>> identities["XTFP"]              # doctest: +NORMALIZE_WHITESPACE
        Identity('grt TFPFHP_')
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
        >>> identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.idt
        48 objects loaded 
        >>> len(identities)
        48

        >>> # Export the IODE Identities database as a pandas Series
        >>> s = identities.series
        >>> len(s)
        48

        >>> s.index.to_list()               # doctest: +ELLIPSIS
        ['AOUC', 'AOUC_', 'FLGR', ..., 'XW', 'Y', 'YSEFPR', 'YSFICR']
        >>> identities["GAP_"]              # doctest: +NORMALIZE_WHITESPACE
        Identity('100*((QAF_/Q_F)-1)')
        >>> s["GAP_"]                       # doctest: +NORMALIZE_WHITESPACE
        '100*((QAF_/Q_F)-1)'
        >>> identities["XTFP"]              # doctest: +NORMALIZE_WHITESPACE
        Identity('grt TFPFHP_')
        >>> s["XTFP"]                       # doctest: +NORMALIZE_WHITESPACE
        'grt TFPFHP_'

        >>> # Export a subset of the IODE Identities database as a pandas Series
        >>> s = identities["X*;*_"].series
        >>> len(s)
        15

        >>> s.index.to_list()               # doctest: +ELLIPSIS
        ['AOUC_', 'GAP_', 'XEX', 'XNATY', ..., 'XTFP', 'XW']
        >>> identities["GAP_"]              # doctest: +NORMALIZE_WHITESPACE
        Identity('100*((QAF_/Q_F)-1)')
        >>> s["GAP_"]                       # doctest: +NORMALIZE_WHITESPACE
        '100*((QAF_/Q_F)-1)'
        >>> identities["XTFP"]              # doctest: +NORMALIZE_WHITESPACE
        Identity('grt TFPFHP_')
        >>> s["XTFP"]                       # doctest: +NORMALIZE_WHITESPACE
        'grt TFPFHP_'
        """
        return self.to_series()

    def _str_table(self, names: List[str]) -> str:
        columns = {"name": names, "identities": [join_lines(str(self._get_object(name))) for name in names]}
        return table2str(columns, max_lines=10, justify_funcs={"name": JUSTIFY.LEFT, "identities": JUSTIFY.LEFT})

    def print_to_file(self, filepath: Union[str, Path], names: Union[str, List[str]]=None, format: str=None) -> None:
        """
        Print the list identities defined by `names` to the file `filepath` using the format `format`.

        Argument `format` must be in the list:
        - 'H' (HTML file)
        - 'M' (MIF file)
        - 'R' (RTF file)
        - 'C' (CSV file)

        If argument `format` is null (default), the *A2M* format will be used to print the output.

        If the filename does not contain an extension, it is automatically added based on 
        the value of `format`.

        If `names` is a string, it is considered as a *pattern* and the function will print 
        all identities matching the pattern. The following characters in *pattern* have a 
        special meaning:
        
            - `*` : any character sequence, even empty
            - `?` : any character (one and only one)
            - `@` : any alphanumerical char [A-Za-z0-9]
            - `&` : any non alphanumerical char
            - `|` : any alphanumeric character or none at the beginning and end of a string 
            - `!` : any non-alphanumeric character or none at the beginning and end of a string 
            - `\` : escape the next character

        If `names` is None, print all identities of the (subset of the) current database.

        Parameters
        ----------
        filepath: str or Path
            path to the file to print.
            If the filename does not contain an extension, it is automatically 
            added based on the value of the format argument.
        names: str or list of str, optional
            pattern or list of names of the identities to print.
            If None, print all identities of the (subset of the) current database.
            Defaults to None.
        format: str, optional
            format of the output file. Possible values are: 'H' (HTML file), 
            'M' (MIF file), 'R' (RTF file) or 'C' (CSV file).
            Defaults to None meaning that the identities will be dumped in the *A2M* format.

        Examples
        --------
        >>> from iode import identities, SAMPLE_DATA_DIR
        >>> identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")               # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.idt
        48 objects loaded
        >>> identities.print_to_file(output_dir / "identities.csv", "G*")     # doctest: +ELLIPSIS
        Printing IODE objects definition to file '...identities.csv'...
        Printing GAP2 ...
        Printing GAP_ ...
        Printing GOSFR ...
        Print done
        >>> with open(output_dir / "identities.csv") as f:                     # doctest: +NORMALIZE_WHITESPACE
        ...     print(f.read())
        ...
        " - GAP2 : 100*(QAFF_/(Q_F + Q_I) )"
        " - GAP_ : 100*((QAF_/Q_F) -1)"
        " - GOSFR : GOSF/VAF_"
        """
        self._print_to_file(filepath, names, format)

    def __hash__(self) -> int:
        """
        Return a hash value for the current Identities database.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, identities
        >>> identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.idt
        48 objects loaded 
        >>> len(identities)
        48
        >>> original_hash = hash(identities)
        
        >>> # rename 1 identity
        >>> identities.rename("FLGR", "FLGR_")
        >>> original_hash == hash(identities)
        False
        >>> identities.rename("FLGR_", "FLGR")  # revert the change
        >>> original_hash == hash(identities)
        True

        >>> # modify one identity
        >>> original_lec = identities["FLGR"]
        >>> identities["FLGR"] = "1"
        >>> original_hash == hash(identities)
        False
        >>> identities["FLGR"] = original_lec  # revert the change
        >>> original_hash == hash(identities)
        True

        >>> # delete a identity
        >>> original_identity = identities["FLGR"]
        >>> del identities["FLGR"]
        >>> original_hash == hash(identities)
        False
        >>> identities["FLGR"] = original_identity
        >>> original_hash == hash(identities)
        True

        >>> # add a identity
        >>> identities["NEW"] = "1"
        >>> original_hash == hash(identities)
        False
        >>> del identities["NEW"]
        >>> original_hash == hash(identities)
        True
        """
        return hash_value(dereference(self.database_ptr))


identities: Identities = Identities.get_instance()
