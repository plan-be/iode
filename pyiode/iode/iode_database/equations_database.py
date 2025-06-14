import sys
import warnings
from pathlib import Path
from typing import Union, Tuple, List, Dict, Optional, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

import pandas as pd
from iode.common import EqTest, PrintEquationsAs, PrintEquationsLecAs
from iode.time.period import Period
from iode.util import join_lines, table2str, JUSTIFY
from iode.iode_database.abstract_database import IodeDatabase, PositionalIndexer
from iode.objects.equation import Equation

from iode.util import suppress_msgs, enable_msgs
from iode.iode_cython import Equations as CythonEquations

EquationInput = Union[str, Dict[str, Any], Equation]


class Equations(IodeDatabase):
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
    >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Loading .../fun.eqs
    274 objects loaded 
    >>> len(equations)
    274
    >>> equations                                           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
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
    def __init__(self, filepath: str=None):
        raise TypeError("This class cannot be instantiated directly.")

    @classmethod
    def get_instance(cls) -> Self:
        instance = cls.__new__(cls)
        instance._cython_instance = CythonEquations()
        instance = cls._get_instance(instance)
        return instance

    def _load(self, filepath: str):
        self._cython_instance._load(filepath)

    def _subset(self, pattern: str, copy: bool) -> Self:
        instance = Equations.get_instance()
        instance._cython_instance = self._cython_instance.initialize_subset(instance._cython_instance, pattern, copy)
        instance = self._subset_(instance, copy)
        return instance

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
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded 
        >>> equations.get_lec("ACAF")   # doctest: +NORMALIZE_WHITESPACE
        '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)'
        >>> equations.get_lec(0)       # doctest: +NORMALIZE_WHITESPACE
        '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)'
        """
        name = self._single_object_key_to_name(key)
        if not name in self:
            raise KeyError(f"Name '{name}' not found in the {type(self).__name__} workspace")
        return self._cython_instance.get_lec(name)

    @property
    def i(self) -> PositionalIndexer:
        r"""
        Allow to select the ith equation in the database.

        Examples
        --------
        >>> from iode import equations, SAMPLE_DATA_DIR
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded 
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

    def _get_object(self, key: Union[str, int]) -> Equation:
        name = self._single_object_key_to_name(key)
        if not name in self:
            raise KeyError(f"Name '{name}' not found in the {type(self).__name__} workspace")
        eq = Equation.get_instance()
        eq._cython_instance = self._cython_instance._get_object(name, eq._cython_instance)
        return eq

    def _set_object(self, key: Union[str, int], value):
        name = self._single_object_key_to_name(key)

        if isinstance(value, pd.Series):
            value = value.to_dict()

        # update existing equation
        if name in self:
            if isinstance(value, str):
                equation = self._get_object(name)
                equation.lec = value
            elif isinstance(value, Equation):
                equation = value
            elif isinstance(value, dict):
                equation = self._get_object(name)
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
                    warnings.warn(f"Cannot update equation '{name}': 'tests' cannot be updated manually. Skipped new values.")
                    del value['tests']
                if 'date' in value:
                    warnings.warn(f"Cannot update equation '{name}': 'date' cannot be updated manually. Skipped new value.")
                    del value['date']
                if len(value):
                    raise ValueError(f"Cannot update equation '{name}': only 'lec', 'method', 'sample', 'comment', "
                                     f"'instruments' and 'block' keys are accepted. "
                                     f"Got unknown key(s): {';'.join(value.keys())}")
            else:
                raise TypeError(f"Cannot update equation '{name}': Expected input to be of type str or dict or Equation. "
                                f"Got value of type {type(value).__name__}")
        # add a new equation
        else:
            if isinstance(value, str):
                equation = Equation(endogenous=name, lec=value.strip())
            elif isinstance(value, Equation):
                equation = value
            elif isinstance(value, (tuple, list)):
                value = [name] + list(value)
                equation = Equation(*value)
            elif isinstance(value, dict):
                value['endogenous'] = name
                equation = Equation(**value)
            else:
                raise TypeError(f"Cannot add equation '{name}': Expected input to be of type str or tuple or list or "
                                f"dict or Equation. Got value of type {type(value).__name__}")

        self._cython_instance._set_object(name, equation._cython_instance)

    def __getitem__(self, key: Union[str, List[str]]) -> Union[Equation, Self]:
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
            The list of equations to get can be specified by a pattern or by a list of sub-patterns (e.g. "A*;*_").

        Returns
        -------
        Single equation or a subset of the database.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import equations
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded 

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
            The list of equations to update/add can be specified by a pattern or by a list of sub-patterns 
            (e.g. "A*;*_").
        value: str or Equation or dict(str, ...) or pandas.DataFrame or Equations
            If str, then it is interpreted as the LEC of the equation.
            The key 'endogenous' is added automatically and does not need to be specified.
            If pandas DataFrame, then only the columns 'endogenous', 'lec', 'method', 'sample', 'block', 
            'instruments' and 'comment' are used. The columns 'endogenous' and 'lec' are mandatory.
            The tests values as well the (estimation) date of equations are set/updated when performing the estimation.
        
        See Also
        --------
        iode.Equation

        Examples
        --------
        >>> import pandas as pd
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import equations, variables, Equation, EqMethod
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        
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
        >>> # new equation sample is from 1990Y1 to 2015Y1
        >>> equations["ACAF"].sample = "1990Y1:2015Y1"
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

        >>> # c) add/update several equations at once        
        >>> # 1) using a dict of values
        >>> eq_ACAF = Equation("ACAF", "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+ acaf4*(TIME=1995)", 
        ...                    method=EqMethod.ZELLNER, from_period='1980Y1', to_period='1996Y1')
        >>> eq_ACAG = Equation("ACAG", "ACAG := ACAG[-1]+r VBBP[-1]+(0.006*VBBP[-1]*(TIME=2001)-0.008*(TIME=2008))", 
        ...                    method=EqMethod.ZELLNER, from_period='1980Y1', to_period='1996Y1')
        >>> eq_AOUC = Equation("AOUC", "AOUC:=((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VAFF+VM))[-1]", 
        ...                    method=EqMethod.ZELLNER, from_period='1980Y1', to_period='1996Y1')
        >>> values = {"ACAF": eq_ACAF, "ACAG": eq_ACAG, "AOUC": eq_AOUC}
        >>> equations["ACAF, ACAG, AOUC"] = values
        >>> equations["ACAF, ACAG, AOUC"]              # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Equations
        nb equations: 3
        filename: ...fun.eqs
        <BLANKLINE>
        name	                                     lec                                      	 method	   sample   	...
        ACAF	(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+ acaf4*(TIME=1995)                        ZELLNER	1980Y1:1996Y1	...	    
        ACAG	ACAG := ACAG[-1]+r VBBP[-1]+(0.006*VBBP[-1]*(TIME=2001)-0.008*(TIME=2008))      ZELLNER	1980Y1:1996Y1	...    
        AOUC	AOUC:=((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VAFF+VM))[-1]  ZELLNER	1980Y1:1996Y1	...	
        <BLANKLINE>

        >>> # 2) using a pandas DataFrame
        >>> data = []
        >>> data.append(["ACAF", "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+ acaf4*(TIME=1995)", "GLS", '1980Y1:1996Y1'])
        >>> data.append(["ACAG", "ACAG := ACAG[-1]+r VBBP[-1]+(0.006*VBBP[-1]*(TIME=2001)-0.008*(TIME=2008))", "GLS", '1980Y1:1996Y1'])
        >>> data.append(["AOUC", "AOUC:=((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VAFF+VM))[-1]", "GLS", '1980Y1:1996Y1'])
        >>> df = pd.DataFrame(data, index=["ACAF", "ACAG", "AOUC"], columns=["endogenous", "lec", "method", "sample"])
        >>> equations["ACAF, ACAG, AOUC"] = df
        >>> equations["ACAF, ACAG, AOUC"]               # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Equations
        nb equations: 3
        filename: ...fun.eqs
        <BLANKLINE>
        name	                                     lec                                      	method	    sample    ...
        ACAF	(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+ acaf4*(TIME=1995)                      	   GLS	1980Y1:1996Y1 ...
        ACAG	ACAG := ACAG[-1]+r VBBP[-1]+(0.006*VBBP[-1]*(TIME=2001)-0.008*(TIME=2008))    	   GLS	1980Y1:1996Y1 ...
        AOUC	AOUC:=((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VAFF+VM))[-1]	   GLS	1980Y1:1996Y1 ...
        <BLANKLINE>
        
        >>> # 3) using another Equations database (subset)
        >>> equations_subset = equations["ACAF, ACAG, AOUC"].copy()
        >>> equations_subset["ACAF"].method = EqMethod.MAX_LIKELIHOOD
        >>> equations_subset["ACAG"].method = EqMethod.MAX_LIKELIHOOD
        >>> equations_subset["AOUC"].method = EqMethod.MAX_LIKELIHOOD
        >>> equations["ACAF, ACAG, AOUC"] = equations_subset
        >>> equations["ACAF, ACAG, AOUC"]               # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Equations
        nb equations: 3
        filename: ...fun.eqs
        <BLANKLINE>
        name	                                     lec                                      	    method    	    sample    ...
        ACAF	(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+ acaf4*(TIME=1995)                      	MAX_LIKELIHOOD	1980Y1:1996Y1 ...
        ACAG	ACAG := ACAG[-1]+r VBBP[-1]+(0.006*VBBP[-1]*(TIME=2001)-0.008*(TIME=2008))    	MAX_LIKELIHOOD	1980Y1:1996Y1 ...
        AOUC	AOUC:=((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VAFF+VM))[-1]	MAX_LIKELIHOOD	1980Y1:1996Y1 ...
        <BLANKLINE>

        >>> # d) working on a subset
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
        r"""
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
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded 

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

    @property
    def coefficients(self) -> List[str]:
        r"""
        Return the list of coefficients (scalars) used in the equations of 
        (the subset of) the database.
        
        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import equations
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded 
        >>> equations["A*"].coefficients
        ['acaf1', 'acaf2', 'acaf4']
        """
        return super()._coefficients()

    @property
    def variables(self) -> List[str]:
        r"""
        Return the list of variables used in the equations of (the subset of) the database.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import equations
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded 
        >>> equations["A*"].variables
        ['ACAF', 'ACAG', 'AOUC', 'GOSF', 'PM', 'QL', 'TIME', 'VAF', 'VAFF', 'VBBP', 'VM', 'WCRH']
        """
        return super()._variables()

    def estimate(self, from_period: Union[str, Period]=None, to_period: Union[str, Period]=None, 
                 list_eqs: Union[str, List[str]]=None, quiet: bool=False) -> bool:
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
        quiet : bool, optional
            If True, suppresses the log messages during the estimation process
            Default to False.

        Returns
        -------
        bool
            True if the estimation process has converged, False otherwise.

        Warnings
        --------
        If some equations to be estimated belongs to a same block, the *block* (:meth:`Equation.block`), 
        *method* (:meth:`Equation.method`), and *instruments* (':meth:`Equation.instruments`) of each of 
        them must have been updated before.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations, scalars, variables
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded

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
        >>> success = equations.estimate("1980Y1", "2000Y1", "ACAF")      # doctest: +NORMALIZE_WHITESPACE
        Estimating : iteration 1 (||eps|| = 0.173205)
        <BLANKLINE>
        Estimating : iteration 2 (||eps|| = 9.24137e-09)
        <BLANKLINE>
        Solution reached after 2 iteration(s). Creating results file ...
        <BLANKLINE>
        >>> success
        True
        >>> # or equivalenty
        >>> success = equations["ACAF"].estimate("1980Y1", "2000Y1")      # doctest: +NORMALIZE_WHITESPACE
        Estimating : iteration 1 (||eps|| = 1.93451e-13)
        <BLANKLINE>
        Solution reached after 1 iteration(s). Creating results file ...
        <BLANKLINE>
        >>> scalars["acaf1"]
        Scalar(0.0150646, 1, 0.00118455)
        >>> scalars["acaf2"]
        Scalar(-6.90855e-06, 1, 1.07873e-06)
        >>> scalars["acaf4"]
        Scalar(-0.00915675, 1, 0.00209541)
        >>> scalars["dpuh_1"]
        Scalar(0, 1, na)
        >>> scalars["dpuh_2"]
        Scalar(0, 1, na)
        >>> equations["ACAF"]                           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ACAF',
                lec = '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)',
                method = 'LSQ',
                from_period = '1980Y1',
                to_period = '2000Y1',
                block = 'ACAF',
                tests = {corr = 1,
                         dw = 1.87449,
                         fstat = 34.6629,
                         loglik = 102.07,
                         meany = 0.0075289,
                         r2 = 0.793875,
                         r2adj = 0.770973,
                         ssres = 7.37883e-05,
                         stderr = 0.00202468,
                         stderrp = 26.8922,
                         stdev = 0.00423072},
                date = '...')
        >>> # observed values (left hand side of the equation)
        >>> variables["_YOBS0", "1980Y1:2000Y1"]        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1980Y1:2000Y1
        mode: LEVEL
        <BLANKLINE>
         name      1980Y1  1981Y1  1982Y1  ...  1998Y1  1999Y1  2000Y1
        _YOBS0       0.01    0.02    0.01  ...    0.01    0.00    0.00
        <BLANKLINE>
        >>> # fitted values (right hand side of the equation)
        >>> variables["_YCALC0", "1980Y1:2000Y1"]       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1980Y1:2000Y1
        mode: LEVEL
        <BLANKLINE>
          name     1980Y1  1981Y1  1982Y1  ...  1998Y1  1999Y1  2000Y1
        _YCALC0      0.01    0.01    0.01  ...    0.00    0.00    0.00
        <BLANKLINE>
        >>> # residuals values
        >>> variables["_YRES0", "1980Y1:2000Y1"]        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1980Y1:2000Y1
        mode: LEVEL
        <BLANKLINE>
         name      1980Y1  1981Y1  1982Y1  ...  1998Y1  1999Y1  2000Y1
        _YRES0      -0.00    0.00   -0.00  ...    0.00   -0.00   -0.00
        <BLANKLINE>

        >>> # estimate a block
        >>> # prepare equations (same block and method)
        >>> block = "ACAF;DPUH"
        >>> for name in block.split(";"):
        ...     equations[name] = {"block": block, "method": "LSQ"}
        >>> success = equations.estimate("1980Y1", "2000Y1", block)           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Estimating : iteration 1 (||eps|| = 0.141421)
        <BLANKLINE>
        Estimating : iteration 2 (||eps|| = 1.56772e-12)
        <BLANKLINE>
        Solution reached after 2 iteration(s). Creating results file ...
        <BLANKLINE>
        Estimating : iteration 1 (||eps|| = 2.00302e-12)
        <BLANKLINE>
        Solution reached after 1 iteration(s). Creating results file ...
        <BLANKLINE>
        >>> success
        True
        >>> scalars["acaf1"]
        Scalar(0.0150646, 1, 0.00117649)
        >>> scalars["acaf2"]
        Scalar(-6.90855e-06, 1, 1.07029e-06)
        >>> scalars["acaf4"]
        Scalar(-0.00915675, 1, 0.00207864)
        >>> scalars["dpuh_1"]
        Scalar(0.0120668, 1, 0.00388436)
        >>> scalars["dpuh_2"]
        Scalar(0.0547555, 1, 0.0312223)
        >>> equations["ACAF"]                           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ACAF',
                lec = '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)',
                method = 'LSQ',
                from_period = '1980Y1',
                to_period = '2000Y1',
                block = 'ACAF;DPUH',
                tests = {corr = 1,
                         dw = 1.87449,
                         fstat = 34.6629,
                         loglik = 102.07,
                         meany = 0.0075289,
                         r2 = 0.793875,
                         r2adj = 0.770973,
                         ssres = 7.37883e-05,
                         stderr = 0.00202468,
                         stderrp = 26.8922,
                         stdev = 0.00423072},
                date = '...')
        >>> # observed values for ACAF
        >>> variables["_YOBS0", "1980Y1:2000Y1"]        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1980Y1:2000Y1
        mode: LEVEL
        <BLANKLINE>
         name      1980Y1  1981Y1  1982Y1  ...  1998Y1  1999Y1  2000Y1
        _YOBS0       0.01    0.02    0.01  ...    0.01    0.00    0.00
        <BLANKLINE>
        >>> # fitted values for ACAF
        >>> variables["_YCALC0", "1980Y1:2000Y1"]       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1980Y1:2000Y1
        mode: LEVEL
        <BLANKLINE>
          name     1980Y1  1981Y1  1982Y1  ...  1998Y1  1999Y1  2000Y1
        _YCALC0      0.01    0.01    0.01  ...    0.00    0.00    0.00
        <BLANKLINE>
        >>> # residuals values for ACAF
        >>> variables["_YRES0", "1980Y1:2000Y1"]        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1980Y1:2000Y1
        mode: LEVEL
        <BLANKLINE>
         name      1980Y1  1981Y1  1982Y1  ...  1998Y1  1999Y1  2000Y1
        _YRES0      -0.00    0.00   -0.00  ...    0.00   -0.00   -0.00
        <BLANKLINE>
        >>> equations["DPUH"]                           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Equation(endogenous = 'DPUH',
                lec = 'dln (DPUH/DPUHO):=dpuh_1+dpuh_2*dln(IHU/PI5)+dln PC',
                method = 'LSQ',
                from_period = '1980Y1',
                to_period = '2000Y1',
                block = 'ACAF;DPUH',
                tests = {corr = 0.126322,
                         dw = 3.2465,
                         fstat = 8.71231,
                         loglik = 55.855,
                         meany = 0.0466199,
                         r2 = 0.314384,
                         r2adj = 0.278299,
                         ssres = 0.00601874,
                         stderr = 0.0177982,
                         stderrp = 38.1772,
                         stdev = 0.0209507},
                date = '...')
        >>> # observed values for DPUH
        >>> variables["_YOBS1", "1980Y1:2000Y1"]        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1980Y1:2000Y1
        mode: LEVEL
        <BLANKLINE>
         name      1980Y1  1981Y1  1982Y1  ...  1998Y1  1999Y1  2000Y1
        _YOBS1       0.06    0.09    0.07  ...    0.03    0.02    0.04
        <BLANKLINE>
        >>> # fitted values for DPUH
        >>> variables["_YCALC1", "1980Y1:2000Y1"]       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1980Y1:2000Y1
        mode: LEVEL
        <BLANKLINE>
          name     1980Y1  1981Y1  1982Y1  ...  1998Y1  1999Y1  2000Y1
        _YCALC1      0.07    0.07    0.08  ...    0.02    0.02    0.03
        <BLANKLINE>
        >>> # residuals values for DPUH
        >>> variables["_YRES1", "1980Y1:2000Y1"]        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1980Y1:2000Y1
        mode: LEVEL
        <BLANKLINE>
         name      1980Y1  1981Y1  1982Y1  ...  1998Y1  1999Y1  2000Y1
        _YRES1      -0.01    0.02   -0.02  ...    0.00    0.00    0.01
        <BLANKLINE>
        """
        if list_eqs is None:
            list_eqs = self.names
        if isinstance(list_eqs, str):
            list_eqs = self.get_names(list_eqs)
        if not isinstance(list_eqs, (list, tuple)):
            raise TypeError("list_eqs must be a string or a list of strings")

        try:
            if quiet:
                suppress_msgs()
            success = True
            for eq_name in list_eqs:
                eq = self[eq_name]
                success &= eq._estimate(from_period, to_period)
            if quiet:
                enable_msgs()
            return success
        except Exception as e:
            if quiet:
                enable_msgs()
            raise e

    def estimate_step_wise(self, eq_names: Union[str, List[str]], from_period: Union[str, Period]=None, 
                           to_period: Union[str, Period]=None, lec_condition: str=None, test: str="r2") -> bool:
        r"""
        Estimates (an) equation(s) and searches for the best possible test values for all 
        possible combinations of coefficients.

        Parameters
        ----------
        eq_names : str or list(str)
            Name(s) of the equation(s) to be estimated.
            The list of names can be given as a string pattern (e.g. "A*;*_").
        from_period : str, optional
            The starting period for the estimation.
            Defaults to the starting period of the current Variables sample.
        to_period : str, optional
            The ending period for the estimation.
            Defaults to the ending period of the current Variables sample.
        lec_condition : str
            Boolean condition written as a LEC expression which is evaluated for each 
            combination of coefficients. Defaults to None (no condition).
        test : str, {r2|fstat}
            Test name. It must be either 'r2' or 'fstat'.
            Defaults to 'r2'.

        Returns
        -------
        bool
            True if the estimation process has converged, False otherwise.
            
        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations, scalars, variables, Scalar
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.scl
        161 objects loaded
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> coefficient_names = equations["ACAF"].coefficients
        >>> coefficient_names
        ['acaf1', 'acaf2', 'acaf4']

        Estimate the equation for the test 'r2' with no condition

        >>> # reset the coefficients
        >>> scalars[coefficient_names] = Scalar(0.9, 1.)
        >>> # estimate 
        >>> success = equations.estimate_step_wise("ACAF", "1980Y1", "2000Y1")      # doctest: +NORMALIZE_WHITESPACE
        Estimating : iteration 1 (||eps|| = 1.27279)
        <BLANKLINE>
        Estimating : iteration 2 (||eps|| = 2.4346e-08)
        <BLANKLINE>
        Solution reached after 2 iteration(s). Creating results file ...
        <BLANKLINE>
        ACAF: scalars : acaf1 acaf2 , r2=0.575200
        Estimating : iteration 1 (||eps|| = 1.27279)
        <BLANKLINE>
        Estimating : iteration 2 (||eps|| = 1.75111e-10)
        <BLANKLINE>
        Solution reached after 2 iteration(s). Creating results file ...
        <BLANKLINE>
        ACAF: scalars : acaf1 acaf4 , r2=0.324238
        Estimating : iteration 1 (||eps|| = 1.27279)
        <BLANKLINE>
        Estimating : iteration 2 (||eps|| = 4.13603e-08)
        <BLANKLINE>
        Solution reached after 2 iteration(s). Creating results file ...
        <BLANKLINE>
        ACAF: scalars : acaf2 acaf4 , r2=-1.058215
        Estimating : iteration 1 (||eps|| = 1.55885)
        <BLANKLINE>
        Estimating : iteration 2 (||eps|| = 5.30092e-08)
        <BLANKLINE>
        Solution reached after 2 iteration(s). Creating results file ...
        <BLANKLINE>
        ACAF: scalars : acaf1 acaf2 acaf4 , r2=0.793875
        Estimating : iteration 1 (||eps|| = 1.55885)
        <BLANKLINE>
        Estimating : iteration 2 (||eps|| = 5.30092e-08)
        <BLANKLINE>
        Solution reached after 2 iteration(s). Creating results file ...
        <BLANKLINE>
        ACAF: scalars : acaf1 acaf2 acaf4 , r2=0.793875
        >>> success
        True
        >>> # r2
        >>> equations["ACAF"].tests["r2"]
        0.7938754558563232
        >>> # display the estimated coefficients
        >>> for name in coefficient_names:
        ...     print(f"{name} -> {scalars[name]}")
        acaf1 -> Scalar(0.0150646, 1, 0.00118455)
        acaf2 -> Scalar(-6.90855e-06, 1, 1.07873e-06)
        acaf4 -> Scalar(-0.00915675, 1, 0.00209541)

        Estimate the equation for the test 'fstat' with no condition

        >>> # reset the coefficients
        >>> scalars[coefficient_names] = Scalar(0.9, 1.)
        >>> # estimate 
        >>> success = equations.estimate_step_wise("ACAF", "1980Y1", "2000Y1", test="fstat")        # doctest: +NORMALIZE_WHITESPACE
        Estimating : iteration 1 (||eps|| = 1.27279)
        <BLANKLINE>
        Estimating : iteration 2 (||eps|| = 2.4346e-08)
        <BLANKLINE>
        Solution reached after 2 iteration(s). Creating results file ...
        <BLANKLINE>
        ACAF: scalars : acaf1 acaf2 , fstat=25.726931
        Estimating : iteration 1 (||eps|| = 1.27279)
        <BLANKLINE>
        Estimating : iteration 2 (||eps|| = 1.75111e-10)
        <BLANKLINE>
        Solution reached after 2 iteration(s). Creating results file ...
        <BLANKLINE>
        ACAF: scalars : acaf1 acaf4 , fstat=9.116395
        Estimating : iteration 1 (||eps|| = 1.27279)
        <BLANKLINE>
        Estimating : iteration 2 (||eps|| = 4.13603e-08)
        <BLANKLINE>
        Solution reached after 2 iteration(s). Creating results file ...
        <BLANKLINE>
        ACAF: scalars : acaf2 acaf4 , fstat=-9.768702
        Estimating : iteration 1 (||eps|| = 1.55885)
        <BLANKLINE>
        Estimating : iteration 2 (||eps|| = 5.30092e-08)
        <BLANKLINE>
        Solution reached after 2 iteration(s). Creating results file ...
        <BLANKLINE>
        ACAF: scalars : acaf1 acaf2 acaf4 , fstat=34.662926
        Estimating : iteration 1 (||eps|| = 1.55885)
        <BLANKLINE>
        Estimating : iteration 2 (||eps|| = 5.30092e-08)
        <BLANKLINE>
        Solution reached after 2 iteration(s). Creating results file ...
        <BLANKLINE>
        ACAF: scalars : acaf1 acaf2 acaf4 , fstat=34.662926
        >>> success
        True
        >>> # fstat
        >>> equations["ACAF"].tests["fstat"]
        34.662925720214844
        >>> # display the estimated coefficients
        >>> for name in coefficient_names:
        ...     print(f"{name} -> {scalars[name]}")
        acaf1 -> Scalar(0.0150646, 1, 0.00118455)
        acaf2 -> Scalar(-6.90855e-06, 1, 1.07873e-06)
        acaf4 -> Scalar(-0.00915675, 1, 0.00209541)

        Estimate the equation for the test 'r2' with condition (acaf2 > 0)

        >>> # write the condition (all coefficients > 0)
        >>> lec_condition = "acaf2 > 0"
        >>> lec_condition
        'acaf2 > 0'
        >>> # reset the coefficients
        >>> scalars[coefficient_names] = Scalar(0.9, 1.)
        >>> # estimate 
        >>> success = equations.estimate_step_wise("ACAF", "1980Y1", "2000Y1", lec_condition)       # doctest: +NORMALIZE_WHITESPACE
        Estimating : iteration 1 (||eps|| = 1.27279)
        <BLANKLINE>
        Estimating : iteration 2 (||eps|| = 2.4346e-08)
        <BLANKLINE>
        Solution reached after 2 iteration(s). Creating results file ...
        <BLANKLINE>
        ACAF: scalars : acaf1 acaf2 , r2=0.575200
        Estimating : iteration 1 (||eps|| = 1.27279)
        <BLANKLINE>
        Estimating : iteration 2 (||eps|| = 1.75111e-10)
        <BLANKLINE>
        Solution reached after 2 iteration(s). Creating results file ...
        <BLANKLINE>
        ACAF: scalars : acaf1 acaf4 , r2=0.324238
        Estimating : iteration 1 (||eps|| = 1.27279)
        <BLANKLINE>
        Estimating : iteration 2 (||eps|| = 4.13603e-08)
        <BLANKLINE>
        Solution reached after 2 iteration(s). Creating results file ...
        <BLANKLINE>
        ACAF: scalars : acaf2 acaf4 , r2=-1.058215
        Estimating : iteration 1 (||eps|| = 1.55885)
        <BLANKLINE>
        Estimating : iteration 2 (||eps|| = 5.30092e-08)
        <BLANKLINE>
        Solution reached after 2 iteration(s). Creating results file ...
        <BLANKLINE>
        ACAF: scalars : acaf1 acaf2 acaf4 , r2=0.793875
        Estimating : iteration 1 (||eps|| = 1.27279)
        <BLANKLINE>
        Estimating : iteration 2 (||eps|| = 4.13603e-08)
        <BLANKLINE>
        Solution reached after 2 iteration(s). Creating results file ...
        <BLANKLINE>
        ACAF: scalars : acaf2 acaf4 , r2=-1.058215
        >>> success
        True
        >>> # r2
        >>> equations["ACAF"].tests["r2"]
        -1.0582152605056763
        >>> # display the estimated coefficients
        >>> for name in coefficient_names:
        ...     print(f"{name} -> {scalars[name]}")
        acaf1 -> Scalar(0, 0, 0)
        acaf2 -> Scalar(5.76933e-06, 1, 1.26806e-06)
        acaf4 -> Scalar(-0.0104115, 1, 0.00643766)

        Estimate the equation for the test 'fstat' with condition (acaf2 > 0)

        >>> # reset the coefficients
        >>> scalars[coefficient_names] = Scalar(0.9, 1.)
        >>> # estimate 
        >>> success = equations.estimate_step_wise("ACAF", "1980Y1", "2000Y1", lec_condition, "fstat")      # doctest: +NORMALIZE_WHITESPACE
        Estimating : iteration 1 (||eps|| = 1.27279)
        <BLANKLINE>
        Estimating : iteration 2 (||eps|| = 2.4346e-08)
        <BLANKLINE>
        Solution reached after 2 iteration(s). Creating results file ...
        <BLANKLINE>
        ACAF: scalars : acaf1 acaf2 , fstat=25.726931
        Estimating : iteration 1 (||eps|| = 1.27279)
        <BLANKLINE>
        Estimating : iteration 2 (||eps|| = 1.75111e-10)
        <BLANKLINE>
        Solution reached after 2 iteration(s). Creating results file ...
        <BLANKLINE>
        ACAF: scalars : acaf1 acaf4 , fstat=9.116395
        Estimating : iteration 1 (||eps|| = 1.27279)
        <BLANKLINE>
        Estimating : iteration 2 (||eps|| = 4.13603e-08)
        <BLANKLINE>
        Solution reached after 2 iteration(s). Creating results file ...
        <BLANKLINE>
        ACAF: scalars : acaf2 acaf4 , fstat=-9.768702
        Estimating : iteration 1 (||eps|| = 1.55885)
        <BLANKLINE>
        Estimating : iteration 2 (||eps|| = 5.30092e-08)
        <BLANKLINE>
        Solution reached after 2 iteration(s). Creating results file ...
        <BLANKLINE>
        ACAF: scalars : acaf1 acaf2 acaf4 , fstat=34.662926
        Estimating : iteration 1 (||eps|| = 1.27279)
        <BLANKLINE>
        Estimating : iteration 2 (||eps|| = 4.13603e-08)
        <BLANKLINE>
        Solution reached after 2 iteration(s). Creating results file ...
        <BLANKLINE>
        ACAF: scalars : acaf2 acaf4 , fstat=-9.768702
        >>> success
        True
        >>> # fstat
        >>> equations["ACAF"].tests["fstat"]
        -9.768701553344727
        >>> # display the estimated coefficients
        >>> for name in coefficient_names:
        ...     print(f"{name} -> {scalars[name]}")
        acaf1 -> Scalar(0, 0, 0)
        acaf2 -> Scalar(5.76933e-06, 1, 1.26806e-06)
        acaf4 -> Scalar(-0.0104115, 1, 0.00643766)
        """
        if isinstance(eq_names, str):
            eq_names = self.get_names(eq_names)
        if not isinstance(eq_names, (list, tuple)):
            raise TypeError("eq_names must be a string or a list of strings")
        
        success = True
        for eq_name in eq_names:
            if eq_name not in self.names:
                raise ValueError(f"Equation '{eq_name}' not found in the Equations database")
            eq = self[eq_name]
            success &= eq._estimate_step_wise(from_period, to_period, lec_condition, test)
            if success:
                eq.sample = f"{from_period}:{to_period}"
            else:
                msg = f"Estimation of the equation '{eq_name}' for the period range [{from_period}, {to_period}]"
                if lec_condition:
                    msg += f" with condition:\n\t'{lec_condition}'\nfailed. "
                else:
                    msg += "failed.\n"
                msg += "No coefficients have been estimated."
                warnings.warn(msg)
        return success   

    def copy_from(self, input_files: Union[str, List[str]], names: Union[str, List[str]]='*'):
        r"""
        Copy (a subset of) equations from the input file(s) 'input_files' into the current database.

        Parameters
        ----------
        input_file: str or list(str)
            file(s) from which the copied equations are read.
        names: str or list(str)
            list of equations to copy from the input file(s).
            Defaults to load all equations from the input file(s). 
        """
        input_files, names = self._copy_from(input_files, names)
        self._cython_instance.copy_from(input_files, names)

    def from_series(self, s: pd.Series):
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
        if not (self.is_global_workspace or self.is_detached):
            # check that all names in the pandas object are present in the current subset 
            self._check_same_names(self.names, s.index.tolist())

        for index, value in s.items():
            self._set_object(index, value)

    def from_frame(self, df: pd.DataFrame):
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
        if not (self.is_global_workspace or self.is_detached):
            # check that all names in the pandas object are present in the current subset 
            self._check_same_names(self.names, df.index.to_list())

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
                if ':' not in sample:
                    from_period, to_period = '', ''
                else:
                    from_period, to_period = sample.split(":")
                args["from_period"] = from_period
                args["to_period"] = to_period
                # date and tests cannot be set by the users
                date = args.pop('date', '')
                test_values = [args.pop(test_name, 0.0) for test_name in test_names]

                equation = Equation(endogenous, **args)
                equation._set_tests_from_list(test_values)
                equation._set_date(date)

                self._set_object(endogenous, equation)

    def to_frame(self) -> pd.DataFrame:
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
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded 
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
        test_names = [member.name.lower() for member in EqTest]
        dtype = {"lec": str, "method": str, "sample": str, "comment": str, "instruments": str, "block": str}
        dtype.update({test_name: float for test_name in test_names})
        dtype.update({"date": str})
        # note: [:1] to skip endogenous value which is the same as name
        data = {name: self._get_object(name)._as_tuple()[1:] for name in self.names}
        return pd.DataFrame.from_dict(data, orient='index', columns=list(dtype.keys())).astype(dtype)

    @property
    def df(self) -> pd.DataFrame:
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
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded 
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

    @property
    def print_nb_decimals(self) -> int:
        r"""
        Number of decimals to print.

        Parameters
        ----------
        value: int
            number of decimals to print (equations coeffs, scalars and computed tables values).

        Examples
        --------
        >>> from iode import equations
        >>> equations.print_nb_decimals = 4
        >>> equations.print_nb_decimals
        4
        """
        return self._cython_instance._get_print_nb_decimals()

    @print_nb_decimals.setter
    def print_nb_decimals(self, value: int):
        self._cython_instance._set_print_nb_decimals(value)

    @property
    def print_equations_as(self) -> PrintEquationsAs:
        r"""
        Whether to print only the LEC formula, or the LEC formula and the comment, 
        or the LEC formula, the comment and the estimation results of the IODE equations 
        in the current database.

        Parameters
        ----------
        value: PrintEquationsAs
            Possible values are: PrintEquationsAs.EQ_ONLY, PrintEquationsAs.EQ_COMMENTS, 
            PrintEquationsAs.EQ_COMMENTS_ESTIMATION

        Examples
        --------
        >>> from iode import equations, PrintEquationsAs
        >>> equations.print_equations_as
        <PrintEquationsAs.EQ_ONLY: 0>
        >>> equations.print_equations_as = PrintEquationsAs.EQ_COMMENTS
        >>> equations.print_equations_as
        <PrintEquationsAs.EQ_COMMENTS: 1>
        >>> equations.print_equations_as = "EQ_COMMENTS_ESTIMATION"
        >>> equations.print_equations_as
        <PrintEquationsAs.EQ_COMMENTS_ESTIMATION: 2>
        """
        i_value = self._cython_instance.get_print_equations_as()
        return PrintEquationsAs(i_value)

    @print_equations_as.setter
    def print_equations_as(self, value: Union[PrintEquationsAs, str, int]):
        if not isinstance(value, (PrintEquationsAs, str, int)):
            raise TypeError(f"Expected value of type PrintEquationsAs, str or int. "
                            f"Got {type(value).__name__} instead.")
        if isinstance(value, int) and not (0 <= value <= 2):
            raise ValueError(f"Invalid value {value}. Expected value between 0 and 2.")
        if isinstance(value, str):
            upper_str = value.upper()
            if upper_str not in PrintEquationsAs.__members__:
                raise ValueError(f"Invalid value '{value}'. "
                                 f"Expected one of {', '.join(PrintEquationsAs.__members__.keys())}. ")
            value = PrintEquationsAs[upper_str]
        value = str(int(value))
        self._cython_instance.set_print_equations_as(value)

    @property
    def print_equations_lec_as(self) -> PrintEquationsLecAs:
        r"""
        Whether to print the LEC formula of each equation: 
          
          - as is,
          - with coefficients replaced by their values,
          - with coefficients replaced by their values + t-tests. 

        Parameters
        ----------
        value: PrintEquationsLecAs
            Possible values are: PrintEquationsLecAs.AS_IS, 
            PrintEquationsLecAs.COEFFS_TO_VALUES, 
            PrintEquationsLecAs.COEFFS_TO_VALUES_TTEST

        Examples
        --------
        >>> from iode import equations, PrintEquationsLecAs
        >>> equations.print_equations_lec_as
        <PrintEquationsLecAs.AS_IS: 0>
        >>> equations.print_equations_lec_as = PrintEquationsLecAs.COEFFS_TO_VALUES
        >>> equations.print_equations_lec_as
        <PrintEquationsLecAs.COEFFS_TO_VALUES: 1>
        >>> equations.print_equations_lec_as = "COEFFS_TO_VALUES_TTEST"
        >>> equations.print_equations_lec_as
        <PrintEquationsLecAs.COEFFS_TO_VALUES_TTEST: 2>
        """
        i_value = self._cython_instance.get_print_equations_lec_as()
        return PrintEquationsLecAs(i_value) 

    @print_equations_lec_as.setter
    def print_equations_lec_as(self, value: Union[PrintEquationsLecAs, str, int]):
        if not isinstance(value, (PrintEquationsLecAs, str, int)):
            raise TypeError(f"Expected value of type PrintEquationsLecAs, str or int. "
                            f"Got {type(value).__name__} instead.")
        if isinstance(value, int) and not (0 <= value <= 2):
            raise ValueError(f"Invalid value {value}. Expected value between 0 and 2.")
        if isinstance(value, str):
            upper_str = value.upper()
            if upper_str not in PrintEquationsLecAs.__members__:
                raise ValueError(f"Invalid value '{value}'. "
                                 f"Expected one of {', '.join(PrintEquationsLecAs.__members__.keys())}. ")
            value = PrintEquationsLecAs[upper_str]
        value = str(int(value))
        self._cython_instance.set_print_equations_lec_as(value)

    def print_to_file(self, filepath: Union[str, Path], names: Union[str, List[str]]=None, format: str=None):
        r"""
        Print the list equations defined by `names` to the file `filepath` using the format `format`.

        Argument `format` must be in the list:
        - 'H' (HTML file)
        - 'M' (MIF file)
        - 'R' (RTF file)
        - 'C' (CSV file)

        If argument `format` is null (default), the *A2M* format will be used to print the output.

        If the filename does not contain an extension, it is automatically added based on 
        the value of `format`.

        If `names` is a string, it is considered as a *pattern* and the function will print 
        all equations matching the pattern. The following characters in *pattern* have a 
        special meaning:
        
            - `*` : any character sequence, even empty
            - `?` : any character (one and only one)
            - `@` : any alphanumerical char [A-Za-z0-9]
            - `&` : any non alphanumerical char
            - `|` : any alphanumeric character or none at the beginning and end of a string 
            - `!` : any non-alphanumeric character or none at the beginning and end of a string 
            - `\` : escape the next character

        If `names` is None, print all equations of the (subset of the) current database.

        Parameters
        ----------
        filepath: str or Path
            path to the file to print.
            If the filename does not contain an extension, it is automatically 
            added based on the value of the format argument.
        names: str or list of str, optional
            pattern or list of names of the equations to print.
            If None, print all equations of the (subset of the) current database.
            Defaults to None.
        format: str, optional
            format of the output file. Possible values are: 'H' (HTML file), 
            'M' (MIF file), 'R' (RTF file) or 'C' (CSV file).
            Defaults to None meaning that the equations will be dumped in the *A2M* format.

        Examples
        --------
        >>> from iode import equations, scalars, SAMPLE_DATA_DIR
        >>> from iode import PrintEquationsAs, PrintEquationsLecAs
        >>> output_dir = getfixture('tmp_path')
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.scl
        161 objects loaded
        >>> equations.print_nb_decimals = 4
        
        >>> equations.print_equations_as = PrintEquationsAs.EQ_ONLY
        >>> equations.print_equations_lec_as = PrintEquationsLecAs.AS_IS
        >>> equations.print_to_file(output_dir / "equations_lec_only.csv", ["ACAF", "ACAG"])              # doctest: +ELLIPSIS
        Printing IODE objects definition to file '...equations_lec_only.csv'...
        Printing ACAF ...
        Printing ACAG ...
        Print done
        >>> with open(output_dir / "equations_lec_only.csv") as f:                     # doctest: +NORMALIZE_WHITESPACE
        ...     print(f.read())
        ...
        " - (ACAF/VAF[-1 ]) := acaf1 + acaf2*GOSF[-1 ] + acaf4*(TIME= 1995)"
        " - ACAG := ACAG[-1 ] + r VBBP[-1 ] + (0.006*VBBP[-1 ]*(TIME= 2001) -0.008*(TIME= 2008) )"

        >>> equations.print_equations_as = PrintEquationsAs.EQ_ONLY
        >>> equations.print_equations_lec_as = PrintEquationsLecAs.COEFFS_TO_VALUES
        >>> equations.print_to_file(output_dir / "equations_coeffs_as_values.csv", ["ACAF", "ACAG"])      # doctest: +ELLIPSIS
        Printing IODE objects definition to file '...equations_coeffs_as_values.csv'...
        Printing ACAF ...
        Printing ACAG ...
        Print done
        >>> with open(output_dir / "equations_coeffs_as_values.csv") as f:                     # doctest: +NORMALIZE_WHITESPACE
        ...     print(f.read())
        ...
        " - (ACAF/VAF[-1 ]) := 0.0158 + -0.0000*GOSF[-1 ] + -0.0085*(TIME= 1995)"
        " - ACAG := ACAG[-1 ] + r VBBP[-1 ] + (0.006*VBBP[-1 ]*(TIME= 2001) -0.008*(TIME= 2008) )"        
        <BLANKLINE>

        >>> equations.print_equations_as = PrintEquationsAs.EQ_ONLY
        >>> equations.print_equations_lec_as = PrintEquationsLecAs.COEFFS_TO_VALUES_TTEST
        >>> equations.print_to_file(output_dir / "equations_coeffs_ttest.csv", ["ACAF", "ACAG"])          # doctest: +ELLIPSIS
        Printing IODE objects definition to file '...equations_coeffs_ttest.csv'...
        Printing ACAF ...
        Printing ACAG ...
        Print done
        >>> with open(output_dir / "equations_coeffs_ttest.csv") as f:                     # doctest: +NORMALIZE_WHITESPACE
        ...     print(f.read())
        ...
        " - (ACAF/VAF[-1 ]) := 0.0158(11.5206)  + -0.0000(-5.3691) *GOSF[-1 ] + -0.0085(-4.0825) *(TIME= 1995)"
        " - ACAG := ACAG[-1 ] + r VBBP[-1 ] + (0.006*VBBP[-1 ]*(TIME= 2001) -0.008*(TIME= 2008) )"        
        <BLANKLINE>

        >>> equations.print_equations_as = PrintEquationsAs.EQ_COMMENTS
        >>> equations.print_equations_lec_as = PrintEquationsLecAs.AS_IS
        >>> equations.print_to_file(output_dir / "equations_lec_cmt.csv", ["ACAF", "ACAG"])               # doctest: +ELLIPSIS
        Printing IODE objects definition to file '...equations_lec_cmt.csv'...
        Printing ACAF ...
        Printing ACAG ...
        Print done
        >>> with open(output_dir / "equations_lec_cmt.csv") as f:                     # doctest: +NORMALIZE_WHITESPACE
        ...     print(f.read())
        ...
        " - (ACAF/VAF[-1 ]) := acaf1 + acaf2*GOSF[-1 ] + acaf4*(TIME= 1995)"
        " - ACAG := ACAG[-1 ] + r VBBP[-1 ] + (0.006*VBBP[-1 ]*(TIME= 2001) -0.008*(TIME= 2008) )"

        >>> equations.print_equations_as = PrintEquationsAs.EQ_COMMENTS_ESTIMATION
        >>> equations.print_equations_lec_as = PrintEquationsLecAs.AS_IS
        >>> equations.print_to_file(output_dir / "equations_lec_cmt_estim.csv", ["ACAF", "ACAG"])         # doctest: +ELLIPSIS
        Printing IODE objects definition to file '...equations_lec_cmt_estim.csv'...
        Printing ACAF ...
        Printing ACAG ...
        Print done
        >>> with open(output_dir / "equations_lec_cmt_estim.csv") as f:                     # doctest: +NORMALIZE_WHITESPACE
        ...     print(f.read())
        ...
        " - (ACAF/VAF[-1 ]) := acaf1 + acaf2*GOSF[-1 ] + acaf4*(TIME= 1995)"
        " - Estimation : L on 1980Y1-1996Y1"
        " - Block : ACAF"
        " - Tests :"
        " - St dev of residuals : 0.004270"
        " - Mean of YOBS : 0.008185"
        " - Sum of Squares of Residuals : 0.000052"
        " - Standard error (%) : 0.001927 (23.545813)"
        " - F-stat : 32.273193"
        " - R2 (R2 adjusted) : 0.821761 (0.796299)"
        " - Durbin-Watson : 2.329346"
        " - Log Likelihood : 83.807526"
        " - Coefficient values (relax, stderr, t-stat) :"
        " - acaf1 : 0.0158 (1, 0.0014, 11.5206)"
        " - acaf2 : -0.0000 (1, 0.0000, -5.3691)"
        " - acaf4 : -0.0085 (1, 0.0021, -4.0825)"
        " - ACAG := ACAG[-1 ] + r VBBP[-1 ] + (0.006*VBBP[-1 ]*(TIME= 2001) -0.008*(TIME= 2008) )"        
        <BLANKLINE>
        """
        super().print_to_file(filepath, names, format)

    def __hash__(self) -> int:
        r"""
        Return a hash value for the current Equations database.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded 
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
        return super().__hash__()


equations: Equations = Equations.get_instance()
