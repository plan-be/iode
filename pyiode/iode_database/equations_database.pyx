# distutils: language = c++

from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

cimport cython
from cython.operator cimport dereference
from pyiode.objects.equation cimport CEquation
from pyiode.iode_database.cpp_api_database cimport hash_value
from pyiode.iode_database.cpp_api_database cimport KDBEquations as CKDBEquations
from pyiode.iode_database.cpp_api_database cimport Equations as cpp_global_equations
from pyiode.iode_database.cpp_api_database cimport Variables as cpp_global_variables
from pyiode.iode_database.cpp_api_database cimport eqs_estimate as cpp_eqs_estimate

from .common import EQ_TEST_NAMES

EquationInput = Union[str, Dict[str, Any], Equation]


@cython.final
cdef class Equations(_AbstractDatabase):
    r"""
    IODE Equations database. 

    Attributes
    ----------
    filename: str
    description: str

    Parameters
    ----------
    filepath: str, optional
        file containing the IODE equations to load.

    Returns
    -------
    Equations

    Examples
    --------
    >>> from iode import equations, SAMPLE_DATA_DIR
    >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
    >>> len(equations)
    274
    >>> equations           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Workspace: Equations
    nb equations: 274
    filename: ...\tests\data\fun.eqs
    <BLANKLINE>
     name                                                    lec                                                method      sample      block    fstat      r2adj     dw     loglik    date
    ACAF        (ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+ acaf4*(TIME=1995)                                           LSQ  1980Y1:1996Y1    ACAF   32.2732     0.7963  2.3293  83.8075 12-06-1998
    ACAG        ACAG := ACAG[-1]+r VBBP[-1]+(0.006*VBBP[-1]*(TIME=2001)-0.008*(TIME=2008))                         LSQ           None    ACAG    0.0000     0.0000  0.0000   0.0000
    AOUC        AOUC:=((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VAFF+VM))[-1]                     LSQ           None    AOUC    0.0000     0.0000  0.0000   0.0000
    BENEF       d BENEF :=d(VBNP-(IT+ITCEE)+SUB-DPUU-(WCF+SSFFIC+WDOM+WBG+ YN)-(GOSH-DPUH+IDH)-DTF-RIDGG+YIDG)     LSQ           None   BENEF    0.0000     0.0000  0.0000   0.0000
    BQY         BQY:=(YK+YN)/PBBP                                                                                  LSQ           None     BQY    0.0000     0.0000  0.0000   0.0000
    ...         ...                                                                                                ...            ...     ...       ...        ...     ...      ...        ...
    YSSF        dln YSSF:=dln WBF_                                                                                 LSQ           None    YSSF    0.0000     0.0000  0.0000   0.0000
    YSSG        YSSG := SSF+SSH-(YSSF+COTRES)                                                                      LSQ           None    YSSG    0.0000     0.0000  0.0000   0.0000
    ZF          grt ZF :=grt PC+ZX-0.05*grt PME                                                                    LSQ           None      ZF    0.0000     0.0000  0.0000   0.0000
    ZJ          grt ZJ :=grt PC +ZX-0.05*grt PME                                                                   LSQ           None      ZJ    0.0000     0.0000  0.0000   0.0000
    ZZF_        ZZF_ := ZZF_[-1]                                                                                   LSQ           None    ZZF_    0.0000     0.0000  0.0000   0.0000
    <BLANKLINE>
    """
    cdef bint ptr_owner
    cdef CKDBEquations* database_ptr

    def __cinit__(self, filepath: str = None) -> Equations:
        self.database_ptr = NULL
        self.abstract_db_ptr = NULL
        self.ptr_owner = False

    def __init__(self, filepath: str = None):
        # Prevent accidental instantiation from normal Python code
        # since we cannot pass a struct pointer into a Python constructor.
        raise TypeError("This class cannot be instantiated directly.")

    def __dealloc__(self):
        # if self.database_ptr points to the C++ global instance Equations 
        # which does not need to be manually deleted 
        if self.ptr_owner and self.database_ptr is not NULL:
            del self.database_ptr
            self.database_ptr = NULL

    # see https://cython.readthedocs.io/en/stable/src/userguide/extension_types.html#instantiation-from-existing-c-c-pointers 
    @staticmethod
    cdef Equations _from_ptr(CKDBEquations* database_ptr = NULL, bint owner=False):
        # call to __new__() that bypasses the __init__() constructor.
        cdef Equations wrapper = Equations.__new__(Equations)
        if database_ptr is not NULL:
            wrapper.ptr_owner = owner
            wrapper.database_ptr = database_ptr
            wrapper.abstract_db_ptr = database_ptr
        else:
            wrapper.ptr_owner = False
            wrapper.database_ptr = &cpp_global_equations
            wrapper.abstract_db_ptr = &cpp_global_equations
        return wrapper

    # TODO: implement KDBAbstract::load() method (for global KDB only)
    def _load(self, filepath: str):
        cdef CKDBEquations* kdb = new CKDBEquations(filepath.encode())
        del kdb

    def _subset(self, pattern: str, copy: bool) -> Equations:
        # call to __new__() that bypasses the __init__() constructor.
        cdef Equations subset_ = Equations.__new__(Equations)
        subset_.database_ptr = subset_.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        return subset_

    def get_lec(self, key: Union[str, int]) -> str:
        r"""
        Get the LEC of an equation in the database.

        Parameters
        ----------
        key : str or int
            Name or index of the equation in the database.

        Returns
        -------
        str

        Examples
        --------
        >>> from iode import equations, SAMPLE_DATA_DIR
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
        >>> equations.get_lec("ACAF")   # doctest: +NORMALIZE_WHITESPACE
        '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)'
        >>> equations.get_lec(0)       # doctest: +NORMALIZE_WHITESPACE
        '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)'
        """
        if isinstance(key, int):
            return self.database_ptr.get_lec(<int>key).decode()
        else:
            return self.database_ptr.get_lec(<string>(key.encode())).decode()

    @property
    def i(self) -> PositionalIndexer:
        r"""
        Allow to select the ith equation in the database.

        Examples
        --------
        >>> from iode import equations, SAMPLE_DATA_DIR
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
        >>> # get the first equation
        >>> equations.i[0]              # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ACAF',
                lec = '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)',
                method = 'LSQ',
                from_period = '1980Y1',
                to_period = '1996Y1',
                block = 'ACAF',
                tests = {corr = 1,
                        dw = 2.32935,
                        fstat = 32.2732,
                        loglik = 83.8075,
                        meany = 0.00818467,
                        r2 = 0.821761,
                        r2adj = 0.796299,
                        ssres = 5.19945e-05,
                        stderr = 0.00192715,
                        stderrp = 23.5458,
                        stdev = 0.0042699},
                date = '12-06-1998')
        >>> # get the last equation
        >>> equations.i[-1]             # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ZZF_',
                lec = 'ZZF_ := ZZF_[-1]',
                method = 'LSQ',
                comment = ' ',
                block = 'ZZF_')
        >>> # update first equation
        >>> equations.i[0] = "(ACAF/VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4*(TIME=1995)"
        >>> equations.i[0]              # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ACAF',
                lec = '(ACAF/VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4*(TIME=1995)',
                method = 'LSQ',
                from_period = '1980Y1',
                to_period = '1996Y1',
                block = 'ACAF',
                tests = {corr = 1,
                        dw = 2.32935,
                        fstat = 32.2732,
                        loglik = 83.8075,
                        meany = 0.00818467,
                        r2 = 0.821761,
                        r2adj = 0.796299,
                        ssres = 5.19945e-05,
                        stderr = 0.00192715,
                        stderrp = 23.5458,
                        stdev = 0.0042699},
                date = '12-06-1998')
        >>> # update last equation
        >>> equations.i[-1] = "ZZF_ := ZZF_[-1] + 1"
        >>> equations.i[-1]             # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ZZF_',
                lec = 'ZZF_ := ZZF_[-1] + 1',
                method = 'LSQ',
                comment = ' ',
                block = 'ZZF_')
        """
        return PositionalIndexer(self)

    def _get_object(self, key: Union[str, int]):
        cdef CEquation* c_eq
        if isinstance(key, int):
            c_eq = self.database_ptr.get(<int>key)
        else:
            key = key.strip()
            c_eq = self.database_ptr.get(<string>(key.encode()))
        
        py_eq = Equation._from_ptr(c_eq, <bint>True, self.database_ptr) 
        return py_eq

    def _set_object(self, key: Union[str, int], value):
        cdef CEquation* c_equation

        if isinstance(key, str):
            key = key.strip()

        # update existing equation
        if isinstance(key, int) or self.database_ptr.contains(key.encode()):
            if isinstance(value, str):
                equation = self._get_object(key)
                equation.lec = value
            elif isinstance(value, Equation):
                equation = value
            elif isinstance(value, dict):
                equation = self._get_object(key)
                if 'lec' in value:
                    equation.lec = value.pop('lec')
                if 'method' in value:
                    equation.method = value.pop('method')
                if 'sample' in value:
                    equation.sample = value.pop('sample')
                if 'comment' in value:
                    equation.comment = value.pop('comment')
                if 'instruments' in value:
                    equation.instruments = value.pop('instruments')
                if 'block' in value:
                    equation.block = value.pop('block')
                if 'tests' in value:
                    warnings.warn(f"Update equation '{key}': 'tests' cannot be updated manually. Skipped new values.")
                    del value['tests']
                if 'date' in value:
                    warnings.warn(f"Update equation '{key}': 'date' cannot be updated manually. Skipped new value.")
                    del value['date']
                if len(value):
                    raise ValueError(f"Update equation '{key}': only 'lec', 'method', 'sample', 'comment', "
                                     f"'instruments' and 'block' keys are accepted. "
                                     f"Got unknown key(s): {';'.join(value.keys())}")
            else:
                raise TypeError(f"Update equation '{key}': Expected input to be of type str or dict or Equation. "
                                f"Got value of type {type(value).__name__}")
            c_equation = (<Equation>equation).c_equation
            if isinstance(key, int):
                self.database_ptr.update(<int>key, dereference(c_equation))
            else:
                self.database_ptr.update(<string>(key.encode()), dereference(c_equation))
        # add a new equation
        else:
            if isinstance(value, str):
                equation = Equation(endogenous=key, lec=value.strip())
            elif isinstance(value, Equation):
                if value.endogenous != key:
                    raise ValueError(f"Name of the endogenous variable '{value.endogenous}' must be the same as "
                                     f"the name of the inserted equation '{key}' in the Equations database")
                equation = value
            elif isinstance(value, (tuple, list)):
                value = [key] + list(value)
                equation = Equation(*value)
            elif isinstance(value, dict):
                value['endogenous'] = key
                equation = Equation(**value)
            else:
                raise TypeError(f"New equation '{key}': Expected input to be of type str or tuple or list or "
                                f"dict or Equation. Got value of type {type(value).__name__}")
            c_equation = (<Equation>equation).c_equation
            self.database_ptr.add(key.encode(), dereference(c_equation))

    def __getitem__(self, key: Union[str, List[str]]) -> Union[Equation, Equations]:
        r"""
        Return the (subset of) equation(s) referenced by `key`.

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
            (the list of) name(s) of the equation(s) to get.
            The list of objects to get can be specified by a pattern or by a list of sub-patterns (e.g. "A*;*_").

        Returns
        -------
        Single equation or a subset of the database.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import equations
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")

        >>> # a) get one equation
        >>> equations["ACAF"]                  # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ACAF',
                 lec = '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)',
                 method = 'LSQ',
                 from_period = '1980Y1',
                 to_period = '1996Y1',
                 block = 'ACAF',
                 tests = {corr = 1,
                          dw = 2.32935,
                          fstat = 32.2732,
                          loglik = 83.8075,
                          meany = 0.00818467,
                          r2 = 0.821761,
                          r2adj = 0.796299,
                          ssres = 5.19945e-05,
                          stderr = 0.00192715,
                          stderrp = 23.5458,
                          stdev = 0.0042699},
                 date = '12-06-1998')

        >>> # b) get a subset of the Equations database using a pattern
        >>> equations_subset = equations["A*"]
        >>> equations_subset.names
        ['ACAF', 'ACAG', 'AOUC']

        >>> # c) get a subset of the Equations database using a list of names
        >>> equations_subset = equations[["ACAF", "AOUC", "BQY", "BVY"]]
        >>> equations_subset.names
        ['ACAF', 'AOUC', 'BQY', 'BVY']
        """
        return super().__getitem__(key)

    def __setitem__(self, key: Union[str, List[str]], value: Union[str, Equation, Dict[str, Any], List[Union[str, Equation, Dict[str, Any]]]]):
        r"""
        Update/add a (subset of) equation(s) referenced by `key` from/to the Equations database.

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
            (the list of) name(s) of the equation(s) to update/add.
            The list of objects to update/add can be specified by a pattern or by a list of sub-patterns 
            (e.g. "A*;*_").
        value: str or Equation or dict(str, ...) or list of any of those
            If str, then it is interpreted as the LEC of the equation.
            If dictionary, then it is interpreted as the values of the attributes of the equation to update or add. 
            The key 'endogenous' is added automatically and does not need to be specified.
            If Equation, then it is used to update an existing equation or to create a new equation if it does not exist yet.
        
        See Also
        --------
        iode.Equation

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import equations, EqMethod
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
        
        >>> # a) add one equation
        >>> equations["BDY"] = "BDY := YN - YK"
        >>> equations["BDY"]                    # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'BDY',
                 lec = 'BDY := YN - YK',
                 method = 'LSQ',
                 from_period = '1960Y1',
                 to_period = '2015Y1')
        
        >>> # b) update one equation        
        >>> equations["ACAF"]                  # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ACAF',
                 lec = '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)',
                 method = 'LSQ',
                 from_period = '1980Y1',
                 to_period = '1996Y1',
                 block = 'ACAF',
                 tests = {corr = 1,
                          dw = 2.32935,
                          fstat = 32.2732,
                          loglik = 83.8075,
                          meany = 0.00818467,
                          r2 = 0.821761,
                          r2adj = 0.796299,
                          ssres = 5.19945e-05,
                          stderr = 0.00192715,
                          stderrp = 23.5458,
                          stdev = 0.0042699},
                 date = '12-06-1998')
        >>> # update only the LEC
        >>> equations["ACAF"] = "(ACAF/VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)"
        >>> equations["ACAF"]                  # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ACAF',
                 lec = '(ACAF/VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)',
                 method = 'LSQ',
                 from_period = '1980Y1',
                 to_period = '1996Y1',
                 block = 'ACAF',
                 tests = {corr = 1,
                          dw = 2.32935,
                          fstat = 32.2732,
                          loglik = 83.8075,
                          meany = 0.00818467,
                          r2 = 0.821761,
                          r2adj = 0.796299,
                          ssres = 5.19945e-05,
                          stderr = 0.00192715,
                          stderrp = 23.5458,
                          stdev = 0.0042699},
                 date = '12-06-1998')
        >>> # update block and sample of a block of equations to estimation (dictionary)
        >>> estim_sample = "2000Y1:2010Y1"
        >>> block = "ACAF; ACAG; AOUC"
        >>> for eq_name in block.split(';'):
        ...     equations[eq_name] = {"sample": estim_sample, "block": block}
        >>> (equations["ACAF"].sample, equations["ACAG"].sample, equations["AOUC"].sample)
        (Sample("2000Y1:2010Y1"), Sample("2000Y1:2010Y1"), Sample("2000Y1:2010Y1"))
        >>> (equations["ACAF"].block, equations["ACAG"].block, equations["AOUC"].block)
        ('ACAF; ACAG; AOUC', 'ACAF; ACAG; AOUC', 'ACAF; ACAG; AOUC')
        >>> # update lec, method, sample and block
        >>> equations["ACAF"].lec = "(ACAF/VAF[-1]) := acaf2 * GOSF[-1] + acaf4 * (TIME=1995)"
        >>> equations["ACAF"].method = EqMethod.MAX_LIKELIHOOD
        >>> # new equation sample is from 1990Y1 to the last year of Variables
        >>> equations["ACAF"].sample = "1990Y1:"
        >>> equations["ACAF"].block = "ACAF"
        >>> equations["ACAF"]                  # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ACAF',
                 lec = '(ACAF/VAF[-1]) := acaf2 * GOSF[-1] + acaf4 * (TIME=1995)',
                 method = 'MAX_LIKELIHOOD',
                 from_period = '1990Y1',
                 to_period = '2015Y1',
                 block = 'ACAF',
                 tests = {corr = 1,
                          dw = 2.32935,
                          fstat = 32.2732,
                          loglik = 83.8075,
                          meany = 0.00818467,
                          r2 = 0.821761,
                          r2adj = 0.796299,
                          ssres = 5.19945e-05,
                          stderr = 0.00192715,
                          stderrp = 23.5458,
                          stdev = 0.0042699},
                 date = '12-06-1998')

        >>> # c) working on a subset
        >>> # 1) get subset
        >>> equations_subset = equations["A*"]
        >>> equations_subset.names
        ['ACAF', 'ACAG', 'AOUC']
        >>> # 2) add a equation to the subset 
        >>> equations_subset["AOUC_"] = "AOUC_ := ((WCRH/QL)/(WCRH/QL)[1990Y1]) * (VAFF/(VM+VAFF))[-1] + PM * (VM/(VAFF+VM))[-1]"
        >>> equations_subset["AOUC_"]               # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'AOUC_',
                 lec = 'AOUC_ := ((WCRH/QL)/(WCRH/QL)[1990Y1]) * (VAFF/(VM+VAFF))[-1] + PM * (VM/(VAFF+VM))[-1]',
                 method = 'LSQ',
                 from_period = '1960Y1',
                 to_period = '2015Y1')
        >>> # --> new equation also appears in the global workspace
        >>> "AOUC_" in equations
        True
        >>> equations["AOUC_"]                      # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'AOUC_',
                 lec = 'AOUC_ := ((WCRH/QL)/(WCRH/QL)[1990Y1]) * (VAFF/(VM+VAFF))[-1] + PM * (VM/(VAFF+VM))[-1]',
                 method = 'LSQ',
                 from_period = '1960Y1',
                 to_period = '2015Y1')
        >>> # 3) update a equation in the subset
        >>> equations_subset["AOUC_"] = "AOUC_ := ((WCRH/QL)/(WCRH/QL)[1990Y1]) * (VAFF/(VM+VAFF))[-1]"
        >>> equations_subset["AOUC_"]           # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'AOUC_',
                 lec = 'AOUC_ := ((WCRH/QL)/(WCRH/QL)[1990Y1]) * (VAFF/(VM+VAFF))[-1]',
                 method = 'LSQ',
                 from_period = '1960Y1',
                 to_period = '2015Y1')
        >>> # --> equation is also updated in the global workspace
        >>> equations["AOUC_"]                  # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'AOUC_',
                 lec = 'AOUC_ := ((WCRH/QL)/(WCRH/QL)[1990Y1]) * (VAFF/(VM+VAFF))[-1]',
                 method = 'LSQ',
                 from_period = '1960Y1',
                 to_period = '2015Y1')  
        """
        super().__setitem__(key, value)

    def __delitem__(self, key):
        """
        Remove the (subset of) equation(s) referenced by `key` from the Equations database.

        Parameters
        ----------
        key: str or list(str)
            (list of) name(s) of the equation(s) to be removed.
            The list of names can be given as a string pattern (e.g. "A*;*_").

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import equations
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")

        >>> # a) delete one equation
        >>> equations.get_names("A*")
        ['ACAF', 'ACAG', 'AOUC']
        >>> del equations["ACAF"]
        >>> equations.get_names("A*")
        ['ACAG', 'AOUC']

        >>> # b) delete several equations at once using a pattern
        >>> del equations["A*"]
        >>> equations.get_names("A*")
        []

        >>> # c) delete several equations at once using a list of names
        >>> equations.get_names("B*")
        ['BENEF', 'BQY', 'BRUGP', 'BVY']
        >>> del equations[["BENEF", "BQY"]]
        >>> equations.get_names("B*")
        ['BRUGP', 'BVY']

        >>> # delete one equation from a subset of the global database
        >>> equations_subset = equations["D*"]
        >>> equations_subset.names
        ['DEBT', 'DPU', 'DPUF', 'DPUG', 'DPUGO', 'DPUH', 'DPUU', 'DTF', 'DTH', 'DTH1', 'DTH1C']
        >>> del equations_subset["DPUGO"]
        >>> equations_subset.names
        ['DEBT', 'DPU', 'DPUF', 'DPUG', 'DPUH', 'DPUU', 'DTF', 'DTH', 'DTH1', 'DTH1C']
        >>> # NOTE: the equation has also been deleted from the global database
        >>> "DPUGO" in equations
        False
        >>> equations.get_names("D*")
        ['DEBT', 'DPU', 'DPUF', 'DPUG', 'DPUH', 'DPUU', 'DTF', 'DTH', 'DTH1', 'DTH1C']
        """
        super().__delitem__(key)

    def estimate(self, from_period: Union[str, Period]=None, to_period: Union[str, Period]=None, list_eqs: Union[str, List[str]]=None):
        r"""
        Estimate an equation or a block of equations.

        At the end of the estimation process, certain variables and scalars are automatically created 
        if the process has converged. These variables and scalars can be used for computational purposes and, 
        as they are part of the global workspace, can be saved for future use.

        The tests resulting from the last estimation are saved as scalars. The same applies to residuals, 
        left-hand and right-hand members of equations.

        Saved tests (as scalars) have the following names (`e<ith>_*` for the <ith> equation of the block):

            - `e<ith>_n`  : number of sample periods 
            - `e<ith>_k`  : number of estimated coefficients 
            - `e<ith>_stdev` : std dev of residuals 
            - `e<ith>_meany` : mean of Y 
            - `e<ith>_ssres` : sum of squares of residuals 
            - `e<ith>_stderr` : std error 
            - `e<ith>_stderrp` : std error percent (in %) 
            - `e<ith>_fstat` : F-Stat 
            - `e<ith>_r2`  : R square 
            - `e<ith>_r2adj`  : adjusted R-squared 
            - `e<ith>_dw` : Durbin-Watson 
            - `e<ith>_loglik`  : Log Likelihood 

        Calculated series are saved in special variables:

            - `_YCALC<i>` : right-hand side of the <ith> equation in the block
            - `_YOBS<i>` : left-hand side of the <ith> equation in the block 
            - `_YRES<i>` : residuals of the <ith> equation of the block 

        Outside the estimation sample, the series values are :math:`NA`.

        Parameters
        ----------
        from_period : str or Period, optional
            The starting period of the execution range. 
            Defaults to the starting period of the current Variables sample.
        to_period : str or Period, optional
            The ending period of the execution range.
            Defaults to the ending period of the current Variables sample.
        list_eqs: str or list(str), optional
            List of equations to be estimated. 
            If not provided, all equations of the present Equations database will be estimated.
            Default to None (all equations).

        Warnings
        --------
        If some equations to be estimated belongs to a same block, the *block* (:meth:`Equation.block`), 
        *method* (:meth:`Equation.method`), and *instruments* (':meth:`Equation.instruments`) of each of 
        them must have been updated before.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations, scalars, variables
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

        >>> equations["ACAF"].lec
        '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)'
        >>> equations["DPUH"].lec
        'dln (DPUH/DPUHO):=dpuh_1+dpuh_2*dln(IHU/PI5)+dln PC'

        >>> # create scalars
        >>> scalars["acaf1"] = 0., 1.
        >>> scalars["acaf2"] = 0., 1.
        >>> scalars["acaf4"] = 0., 1.
        >>> scalars["dpuh_1"] = 0., 1.
        >>> scalars["dpuh_2"] = 0., 1.

        >>> # estimate an equation
        >>> equations.estimate("1980Y1", "1996Y1", "ACAF")
        >>> # or equivalenty
        >>> equations["ACAF"].estimate("1980Y1", "1996Y1")
        >>> scalars["acaf1"]
        Scalar(0.0157705, 1, 0.00136949)
        >>> scalars["acaf2"]
        Scalar(-7.96505e-06, 1, 1.48247e-06)
        >>> scalars["acaf4"]
        Scalar(-0.0085027, 1, 0.00208257)
        >>> scalars["dpuh_1"]
        Scalar(0, 1, na)
        >>> scalars["dpuh_2"]
        Scalar(0, 1, na)

        >>> # estimate a block
        >>> # prepare equations (same block and method)
        >>> block = "ACAF;DPUH"
        >>> for name in block.split(";"):
        ...     equations[name] = {"block": block, "method": "LSQ"}
        >>> equations.estimate("1980Y1", "1996Y1", block)
        >>> scalars["acaf1"]
        Scalar(0.0157705, 1, 0.00136079)
        >>> scalars["acaf2"]
        Scalar(-7.96505e-06, 1, 1.47188e-06)
        >>> scalars["acaf4"]
        Scalar(-0.0085027, 1, 0.00206603)
        >>> scalars["dpuh_1"]
        Scalar(0.0109855, 1, 0.00481857)
        >>> scalars["dpuh_2"]
        Scalar(0.0574893, 1, 0.0368951)
        """
        if from_period is None or to_period is None:
            c_sample = cpp_global_variables.get_sample()
            if from_period is None:
                from_period = c_sample.start_period().to_string().decode()
            if to_period is None:
                to_period = c_sample.end_period().to_string().decode()
        
        if isinstance(from_period, Period):
            from_period = str(from_period)

        if isinstance(to_period, Period):
            to_period = str(to_period)

        if list_eqs is None:
            list_eqs = ';'.join(self.names)

        if not isinstance(list_eqs, str) and isinstance(list_eqs, Iterable) and \
            all(isinstance(item, str) for item in list_eqs):
            list_eqs = ';'.join(list_eqs)
        
        cpp_eqs_estimate(list_eqs.encode(), from_period.encode(), to_period.encode())

    def from_series(self, s: Series):
        r"""
        Copy the pandas Series `s` into the IODE Equations database.
        The equation names to copy are deduced from the index of the Series.

        Parameters
        ----------
        s: Series
            pandas Series containing the equations as LEC expressions to copy into the Equations database.
            All other Equation attributes (method, sample, comment, block, ...) will be set to their default 
            values (see :class:`Equation`).

        Notes
        -----
        The index of the passed Series is sorted in alphabetical order before 
        copying to IODE Equations database.

        See Also
        --------
        Equations.from_frame
        Equations.to_frame

        Examples
        --------
        >>> from iode import equations, variables
        >>> import pandas as pd
        >>> variables.clear()
        >>> variables.sample = "1960Y1:2015Y1"
        >>> equations.clear()
        >>> len(equations)
        0

        >>> # create the pandas Series
        >>> data = {"A": "A := t",
        ...         "B": "B := grt A",
        ...         "C": "C := exp t",
        ...         "D": "ln D := t",
        ...         "E": "1/E := B",
        ...         "F": "d(F) := t + 1"}
        >>> s = pd.Series(data)
        >>> # display the pandas series
        >>> s                   # doctest: +NORMALIZE_WHITESPACE
        A           A := t
        B       B := grt A
        C       C := exp t
        D        ln D := t
        E         1/E := B
        F    d(F) := t + 1
        dtype: object

        >>> # load into the IODE Equations database
        >>> equations.from_series(s)
        >>> len(equations)
        6
        
        >>> equations.names
        ['A', 'B', 'C', 'D', 'E', 'F']
        >>> equations["A"]      # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'A',
                lec = 'A := t',
                method = 'LSQ',
                from_period = '1960Y1',
                to_period = '2015Y1')
        >>> equations["F"]      # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'F',
                lec = 'd(F) := t + 1',
                method = 'LSQ',
                from_period = '1960Y1',
                to_period = '2015Y1')
        """
        if pd is None:
            raise RuntimeError("pandas library not found")

        for index, value in s.items():
            self._set_object(index, value)

    def from_frame(self, df: DataFrame):
        r"""
        Copy the pandas DataFrame `df` into the IODE Equations database.
        The equation names to copy are deduced from the index of the DataFrame.

        Parameters
        ----------
        df: DataFrame
            pandas DataFrame containing the equations to copy into the Equations database.
            The passed DataFrame must at least contains:
                - an index containing the names of the equations
                - a column labeled 'lec' containing the LEC expressions of the equations.
            Other possible columns are 'method', 'sample', 'comment', 'instruments' and 'block'
            (see :class:`Equation`).

        Notes
        -----
        The index of the passed DataFrame is sorted in alphabetical order before 
        copying to IODE Equations database.

        See Also
        --------
        Equations.from_series
        Equations.to_frame

        Examples
        --------
        >>> from iode import equations, variables
        >>> import pandas as pd
        >>> import numpy as np
        >>> variables.clear()
        >>> variables.sample = "1960Y1:2015Y1"
        >>> equations.clear()
        >>> len(equations)
        0

        >>> # create the pandas DataFrame
        >>> columns=["lec", "method", "sample", "corr", "fstat", "r2", "date"]
        >>> data = {"A": ["A := t", "LSQ", "1980Y1:2010Y1", 1.0, 0.79, 0.05, "12-06-1998"],
        ...         "B": ["B := grt A", "LSQ", "1980Y1:2010Y1", 1.0, 0.896, 0.15, "12-06-1998"],
        ...         "C": ["C := exp t", "ZELLNER", "1990Y1:", 1.0, 0.1258, 0.0225, ""],
        ...         "D": ["ln D := t", "MAX_LIKELIHOOD", "", 1.0, 0.358, 0.0698, "12-06-1998"],
        ...         "E": ["1/E := B", "LSQ", ":2010Y1", 1.0, 0.689, 2.23e-03, ""],
        ...         "F": ["d(F) := t + 1", "GLS", "", 1.0, 0.05, 4.568e-04, ""]}
        >>> df = pd.DataFrame.from_dict(data, orient='index', columns=columns)
        >>> # display the pandas series
        >>> df                  # doctest: +NORMALIZE_WHITESPACE
                     lec          method         sample  ...   fstat        r2        date
        A         A := t             LSQ  1980Y1:2010Y1  ...  0.7900  0.050000  12-06-1998
        B     B := grt A             LSQ  1980Y1:2010Y1  ...  0.8960  0.150000  12-06-1998
        C     C := exp t         ZELLNER        1990Y1:  ...  0.1258  0.022500
        D      ln D := t  MAX_LIKELIHOOD                 ...  0.3580  0.069800  12-06-1998
        E       1/E := B             LSQ        :2010Y1  ...  0.6890  0.002230
        F  d(F) := t + 1             GLS                 ...  0.0500  0.000457
        <BLANKLINE>
        [6 rows x 7 columns]

        >>> # load into the IODE Equations database
        >>> equations.from_frame(df)
        >>> len(equations)
        6
        
        >>> equations.names
        ['A', 'B', 'C', 'D', 'E', 'F']
        >>> df.loc["A"]         # doctest: +NORMALIZE_WHITESPACE  
        lec              A := t
        method              LSQ
        sample    1980Y1:2010Y1
        corr                1.0
        fstat              0.79
        r2                 0.05
        date         12-06-1998
        Name: A, dtype: object
        >>> equations["A"]      # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'A',
                 lec = 'A := t',
                 method = 'LSQ',
                 from_period = '1980Y1',
                 to_period = '2010Y1',
                 tests = {corr = 1,
                          dw = 0,
                          fstat = 0.79,
                          loglik = 0,
                          meany = 0,
                          r2 = 0.05,
                          r2adj = 0,
                          ssres = 0,
                          stderr = 0,
                          stderrp = 0,
                          stdev = 0},
                 date = '12-06-1998')
        >>> df.loc["F"]         # doctest: +NORMALIZE_WHITESPACE
        lec       d(F) := t + 1
        method              GLS
        sample
        corr                1.0
        fstat              0.05
        r2             0.000457
        date
        Name: F, dtype: object
        >>> equations["F"]      # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'F',
                 lec = 'd(F) := t + 1',
                 method = 'GLS',
                 from_period = '1960Y1',
                 to_period = '2015Y1',
                 tests = {corr = 1,
                          dw = 0,
                          fstat = 0.05,
                          loglik = 0,
                          meany = 0,
                          r2 = 0.0004568,
                          r2adj = 0,
                          ssres = 0,
                          stderr = 0,
                          stderrp = 0,
                          stdev = 0})
        """
        if pd is None:
            raise RuntimeError("pandas library not found")

        if 'lec' not in df.columns:
            raise ValueError("Expected at least one column with name 'lec'. "
                             f"Got dataframe with column names {df.columns.to_list()}")

        if len(df.columns) == 1:
            self.from_series(df['lec'])
        else:
            test_names = [member.name.lower() for member in EqTest]
            for row in df.itertuples(name='Equation'):
                args = row._asdict()
                # note: equation name = endogenous variable
                endogenous = args.pop('Index')
                if 'endogenous' in args:
                    if args['endogenous'] != endogenous:
                        raise ValueError(f"Mismatch between equation name '{endogenous}' and "
                                         f"its endogenous variable name '{args['endogenous']}'")
                    del args['endogenous']
                sample = args.pop('sample', '')
                if not sample:
                    from_period, to_period = '', ''
                else:
                    from_period, to_period = sample.split(":")
                args["from_period"] = from_period
                args["to_period"] = to_period
                # date and tests cannot be set by the users
                date = args.pop('date', '')
                test_values = [args.pop(test_name, 0.0) for test_name in EQ_TEST_NAMES]

                equation = Equation(endogenous, **args)
                equation._set_tests(test_values)
                equation._set_date(date)

                self._set_object(endogenous, equation)

    def to_frame(self) -> DataFrame:
        r"""
        Create a pandas DataFrame from the current Equations database.
        The index of the returned DataFrame is build from the Equations names.

        See Also
        --------
        Equations.from_series
        Equations.from_frame

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations
        >>> import pandas as pd
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
        >>> len(equations)
        274

        >>> # Export the IODE Equations database as a pandas DataFrame
        >>> df = equations.to_frame()
        >>> len(df)
        274

        >>> df.index.to_list()              # doctest: +ELLIPSIS
        ['ACAF', 'ACAG', 'AOUC', ..., 'YSSG', 'ZF', 'ZJ', 'ZZF_']
        >>> equations["ACAF"]               # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ACAF',
                 lec = '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)',
                 method = 'LSQ',
                 from_period = '1980Y1',
                 to_period = '1996Y1',
                 block = 'ACAF',
                 tests = {corr = 1,
                          dw = 2.32935,
                          fstat = 32.2732,
                          loglik = 83.8075,
                          meany = 0.00818467,
                          r2 = 0.821761,
                          r2adj = 0.796299,
                          ssres = 5.19945e-05,
                          stderr = 0.00192715,
                          stderrp = 23.5458,
                          stdev = 0.0042699},
                 date = '12-06-1998')
        >>> df.loc["ACAF"]                  # doctest: +NORMALIZE_WHITESPACE
        lec            (ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*...
        method                                                       LSQ
        sample                                             1980Y1:1996Y1
        comment
        instruments
        block                                                       ACAF
        corr                                                         1.0
        stdev                                                    0.00427
        meany                                                   0.008185
        ssres                                                   0.000052
        stderr                                                  0.001927
        stderrp                                                23.545813
        fstat                                                  32.273193
        r2                                                      0.821761
        r2adj                                                   0.796299
        dw                                                      2.329346
        loglik                                                 83.807526
        date                                                  12-06-1998
        Name: ACAF, dtype: object  
        >>> equations["YDH_"]               # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'YDH_',
                 lec = 'grt YDH_ :=grt((WBU_+YN+GOSH_+IDH)-(SSF+SSH+DTH)+(SBH+OCUH))',
                 method = 'LSQ',
                 comment = ' ',
                 block = 'YDH_')
        >>> df.loc["YDH_"]                  # doctest: +NORMALIZE_WHITESPACE
        lec            grt YDH_ :=grt((WBU_+YN+GOSH_+IDH)-(SSF+SSH+DT...
        method                                                       LSQ
        sample                                                      None
        comment
        instruments
        block                                                       YDH_
        corr                                                         0.0
        stdev                                                        0.0
        meany                                                        0.0
        ssres                                                        0.0
        stderr                                                       0.0
        stderrp                                                      0.0
        fstat                                                        0.0
        r2                                                           0.0
        r2adj                                                        0.0
        dw                                                           0.0
        loglik                                                       0.0
        date
        Name: YDH_, dtype: object

        >>> # Export a subset of the IODE Equations database as a pandas DataFrame
        >>> df = equations["A*;*_"].to_frame()
        >>> len(df)
        29

        >>> df.index.to_list()              # doctest: +ELLIPSIS
        ['ACAF', 'ACAG', 'AOUC', ..., 'WNF_', 'YDH_', 'ZZF_']
        >>> equations["ACAF"]               # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ACAF',
                 lec = '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)',
                 method = 'LSQ',
                 from_period = '1980Y1',
                 to_period = '1996Y1',
                 block = 'ACAF',
                 tests = {corr = 1,
                          dw = 2.32935,
                          fstat = 32.2732,
                          loglik = 83.8075,
                          meany = 0.00818467,
                          r2 = 0.821761,
                          r2adj = 0.796299,
                          ssres = 5.19945e-05,
                          stderr = 0.00192715,
                          stderrp = 23.5458,
                          stdev = 0.0042699},
                 date = '12-06-1998')
        >>> df.loc["ACAF"]                  # doctest: +NORMALIZE_WHITESPACE
        lec            (ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*...
        method                                                       LSQ
        sample                                             1980Y1:1996Y1
        comment
        instruments
        block                                                       ACAF
        corr                                                         1.0
        stdev                                                    0.00427
        meany                                                   0.008185
        ssres                                                   0.000052
        stderr                                                  0.001927
        stderrp                                                23.545813
        fstat                                                  32.273193
        r2                                                      0.821761
        r2adj                                                   0.796299
        dw                                                      2.329346
        loglik                                                 83.807526
        date                                                  12-06-1998
        Name: ACAF, dtype: object
        >>> equations["YDH_"]               # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'YDH_',
                 lec = 'grt YDH_ :=grt((WBU_+YN+GOSH_+IDH)-(SSF+SSH+DTH)+(SBH+OCUH))',
                 method = 'LSQ',
                 comment = ' ',
                 block = 'YDH_')
        >>> df.loc["YDH_"]                  # doctest: +NORMALIZE_WHITESPACE
        lec            grt YDH_ :=grt((WBU_+YN+GOSH_+IDH)-(SSF+SSH+DT...
        method                                                       LSQ
        sample                                                      None
        comment
        instruments
        block                                                       YDH_
        corr                                                         0.0
        stdev                                                        0.0
        meany                                                        0.0
        ssres                                                        0.0
        stderr                                                       0.0
        stderrp                                                      0.0
        fstat                                                        0.0
        r2                                                           0.0
        r2adj                                                        0.0
        dw                                                           0.0
        loglik                                                       0.0
        date
        Name: YDH_, dtype: object
        """
        if pd is None:
            raise RuntimeError("pandas library not found")
        
        dtype = {"lec": str, "method": str, "sample": str, "comment": str, "instruments": str, "block": str}
        dtype.update({test_name: float for test_name in EQ_TEST_NAMES})
        dtype.update({"date": str})
        # note: [:1] to skip endogenous value which is the same as name
        data = {name: self._get_object(name)._as_tuple()[1:] for name in self.names}
        return pd.DataFrame.from_dict(data, orient='index', columns=list(dtype.keys())).astype(dtype)

    @property
    def df(self) -> DataFrame:
        r"""
        Create a pandas DataFrame from the current Equations database.
        The index of the returned DataFrame is build from the Equations names.

        See Also
        --------
        Equations.to_frame

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations
        >>> import pandas as pd
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
        >>> len(equations)
        274

        >>> # Export the IODE Equations database as a pandas DataFrame
        >>> df = equations.df
        >>> len(df)
        274

        >>> df.index.to_list()              # doctest: +ELLIPSIS
        ['ACAF', 'ACAG', 'AOUC', ..., 'YSSG', 'ZF', 'ZJ', 'ZZF_']
        >>> equations["ACAF"]               # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ACAF',
                 lec = '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)',
                 method = 'LSQ',
                 from_period = '1980Y1',
                 to_period = '1996Y1',
                 block = 'ACAF',
                 tests = {corr = 1,
                          dw = 2.32935,
                          fstat = 32.2732,
                          loglik = 83.8075,
                          meany = 0.00818467,
                          r2 = 0.821761,
                          r2adj = 0.796299,
                          ssres = 5.19945e-05,
                          stderr = 0.00192715,
                          stderrp = 23.5458,
                          stdev = 0.0042699},
                 date = '12-06-1998')
        >>> df.loc["ACAF"]                  # doctest: +NORMALIZE_WHITESPACE
        lec            (ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*...
        method                                                       LSQ
        sample                                             1980Y1:1996Y1
        comment
        instruments
        block                                                       ACAF
        corr                                                         1.0
        stdev                                                    0.00427
        meany                                                   0.008185
        ssres                                                   0.000052
        stderr                                                  0.001927
        stderrp                                                23.545813
        fstat                                                  32.273193
        r2                                                      0.821761
        r2adj                                                   0.796299
        dw                                                      2.329346
        loglik                                                 83.807526
        date                                                  12-06-1998
        Name: ACAF, dtype: object  
        >>> equations["YDH_"]               # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'YDH_',
                 lec = 'grt YDH_ :=grt((WBU_+YN+GOSH_+IDH)-(SSF+SSH+DTH)+(SBH+OCUH))',
                 method = 'LSQ',
                 comment = ' ',
                 block = 'YDH_')
        >>> df.loc["YDH_"]                  # doctest: +NORMALIZE_WHITESPACE
        lec            grt YDH_ :=grt((WBU_+YN+GOSH_+IDH)-(SSF+SSH+DT...
        method                                                       LSQ
        sample                                                      None
        comment
        instruments
        block                                                       YDH_
        corr                                                         0.0
        stdev                                                        0.0
        meany                                                        0.0
        ssres                                                        0.0
        stderr                                                       0.0
        stderrp                                                      0.0
        fstat                                                        0.0
        r2                                                           0.0
        r2adj                                                        0.0
        dw                                                           0.0
        loglik                                                       0.0
        date
        Name: YDH_, dtype: object

        >>> # Export a subset of the IODE Equations database as a pandas DataFrame
        >>> df = equations["A*;*_"].df
        >>> len(df)
        29

        >>> df.index.to_list()              # doctest: +ELLIPSIS
        ['ACAF', 'ACAG', 'AOUC', ..., 'WNF_', 'YDH_', 'ZZF_']
        >>> equations["ACAF"]               # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ACAF',
                 lec = '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)',
                 method = 'LSQ',
                 from_period = '1980Y1',
                 to_period = '1996Y1',
                 block = 'ACAF',
                 tests = {corr = 1,
                          dw = 2.32935,
                          fstat = 32.2732,
                          loglik = 83.8075,
                          meany = 0.00818467,
                          r2 = 0.821761,
                          r2adj = 0.796299,
                          ssres = 5.19945e-05,
                          stderr = 0.00192715,
                          stderrp = 23.5458,
                          stdev = 0.0042699},
                 date = '12-06-1998')
        >>> df.loc["ACAF"]                  # doctest: +NORMALIZE_WHITESPACE
        lec            (ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*...
        method                                                       LSQ
        sample                                             1980Y1:1996Y1
        comment
        instruments
        block                                                       ACAF
        corr                                                         1.0
        stdev                                                    0.00427
        meany                                                   0.008185
        ssres                                                   0.000052
        stderr                                                  0.001927
        stderrp                                                23.545813
        fstat                                                  32.273193
        r2                                                      0.821761
        r2adj                                                   0.796299
        dw                                                      2.329346
        loglik                                                 83.807526
        date                                                  12-06-1998
        Name: ACAF, dtype: object
        >>> equations["YDH_"]               # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'YDH_',
                 lec = 'grt YDH_ :=grt((WBU_+YN+GOSH_+IDH)-(SSF+SSH+DTH)+(SBH+OCUH))',
                 method = 'LSQ',
                 comment = ' ',
                 block = 'YDH_')
        >>> df.loc["YDH_"]                  # doctest: +NORMALIZE_WHITESPACE
        lec            grt YDH_ :=grt((WBU_+YN+GOSH_+IDH)-(SSF+SSH+DT...
        method                                                       LSQ
        sample                                                      None
        comment
        instruments
        block                                                       YDH_
        corr                                                         0.0
        stdev                                                        0.0
        meany                                                        0.0
        ssres                                                        0.0
        stderr                                                       0.0
        stderrp                                                      0.0
        fstat                                                        0.0
        r2                                                           0.0
        r2adj                                                        0.0
        dw                                                           0.0
        loglik                                                       0.0
        date
        Name: YDH_, dtype: object
        """
        return self.to_frame()      

    def _str_table(self, names: List[str]) -> str:
        data = []
        for name in names:
            eq = self._get_object(name)
            tests = eq.tests
            data += [[join_lines(eq.lec), eq.method, str(eq.sample), eq.block, tests["fstat"], 
                     tests["r2adj"], tests["dw"], tests["loglik"], eq.date]]
        # transpose data
        data = [list(row) for row in zip(*data)]
        columns = {"name": names, "lec": data[0], "method": data[1], "sample": data[2], "block": data[3], 
                   "fstat": data[4], "r2adj": data[5], "dw": data[6], "loglik": data[7], "date": data[8]}
        return table2str(columns, max_lines=10, justify_funcs={"name": JUSTIFY.LEFT, "lec": JUSTIFY.LEFT})

    def __hash__(self) -> int:
        """
        Return a hash value for the current Equations database.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
        >>> len(equations)
        274
        >>> original_hash = hash(equations)

        >>> # modify one equation
        >>> eq = equations["ACAF"]
        >>> original_lec = eq.lec
        >>> eq.lec = "ACAF := 1"
        >>> equations["ACAF"] = eq
        >>> original_hash == hash(equations)
        False
        >>> eq.lec = original_lec  # revert the change
        >>> equations["ACAF"] = eq
        >>> original_hash == hash(equations)
        True

        >>> # delete a equation
        >>> original_eq = equations["ACAF"]
        >>> del equations["ACAF"]
        >>> original_hash == hash(equations)
        False
        >>> equations["ACAF"] = original_eq
        >>> original_hash == hash(equations)
        True

        >>> # add a equation
        >>> equations["NEW"] = "NEW := 0"
        >>> original_hash == hash(equations)
        False
        >>> del equations["NEW"]
        >>> original_hash == hash(equations)
        True
        """
        return hash_value(dereference(self.database_ptr))


equations: Equations = Equations._from_ptr()
