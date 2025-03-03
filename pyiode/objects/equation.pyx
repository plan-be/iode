# distutils: language = c++

from copy import copy
from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional

from libcpp.map cimport map
from libcpp.string cimport string
from cython.operator cimport dereference
from libc.string cimport memset

from pyiode.common cimport IodeEquationMethod, IodeEquationTest
from pyiode.objects.equation cimport EQ
from pyiode.objects.equation cimport CEquation
from pyiode.objects.equation cimport hash_value as hash_value_eq
from pyiode.iode_database.cpp_api_database cimport KDBEquations as CKDBEquations


# Equation wrapper class
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#create-cython-wrapper-class 
cdef class Equation:
    """
    IODE equation.

    Attributes
    ----------
    endogenous: str
        name of the endogenous variable. It must the same as the name of the present 
        equation in the Equations database. 
    lec: str
        LEC expression of the equation.
    method: EqMethod
        estimation method. Possible values are:
            - LSQ
            - ZELLNER
            - INSTRUMENTAL
            - GLS
            - MAX_LIKELIHOOD
    sample: Sample
        estimaton sample.
    comment: str
    instruments: str
        estimation instruments.
    block: str
        block of equations (to estimate) to which the equation belong.
    tests: dict(str, float)
        tests values associated with the estimation of the equation.
    date: str
        last time the equation has been estimated.

    Parameters
    ----------
    endogenous: str
        Name of the endogenous variable. It must the same as the name of the present 
        equation in the Equations database. 
    lec: str
        LEC expression of the equation.
    method: EqMethod or str, optional
        Method used to estimate the coefficients of the equation.
        Defaults to LSQ.
    from_period: str or Period, optional
        Starting period for the estimation.
        Defaults to the starting period of the sample associated with the IODE Variables database.
    to_period: str or Period, optional
        Ending period for the estimation.
        Defaults to the ending period of the sample associated with the IODE Variables database.
    comment: str, optional
        Defaults to empty.
    instruments: str, optional
        Defaults to empty.
    block: str, optional
        block of equations (to estimate) to which the equation belong.
        Defaults to empty.

    Examples
    --------
    >>> from iode import Equation, variables
    >>> variables.sample = "1960Y1:2015Y1"
    >>> eq_ACAF = Equation("ACAF", "(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
    >>> eq_ACAF         # doctest: +NORMALIZE_WHITESPACE
    Equation(endogenous = 'ACAF',
             lec = '(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)',
             method = 'LSQ',
             from_period = '1960Y1',
             to_period = '2015Y1')
    """
    cdef bint ptr_owner
    cdef CEquation* c_equation
    cdef CKDBEquations* c_database

    def __cinit__(self):
        self.ptr_owner = False
        self.c_database = NULL
        self.c_equation = NULL

    def __init__(self, endogenous: str, lec: str, method: Union[EqMethod, str] = EqMethod.LSQ, 
        from_period: Union[str, Period] = "", to_period: Union[str, Period] = "", comment: str = "", 
        instruments: str = "", block: str = "") -> Equation:
        if isinstance(from_period, Period):
            from_period = str(from_period)
        if isinstance(to_period, Period):
            to_period = str(to_period)

        self.ptr_owner = <bint>True
        self.c_database = NULL
        self.c_equation = new CEquation(endogenous.encode(), lec.encode(), <IodeEquationMethod>(0), from_period.encode(), 
                                        to_period.encode(), comment.encode(), instruments.encode(), block.encode(), <bint>False)
        self.method = method

    def __dealloc__(self):
        if self.ptr_owner and self.c_equation is not NULL:
            del self.c_equation
            self.c_equation = NULL
            self.c_database = NULL

    # see https://cython.readthedocs.io/en/stable/src/userguide/extension_types.html#instantiation-from-existing-c-c-pointers 
    @staticmethod
    cdef Equation _from_ptr(CEquation* ptr, bint owner=False, CKDBEquations* c_database=NULL):
        """
        Factory function to create Equation objects from a given CEquation pointer.
        """
        # Fast call to __new__() that bypasses the __init__() constructor.
        cdef Equation wrapper = Equation.__new__(Equation)
        wrapper.c_equation = ptr
        wrapper.c_database = c_database
        wrapper.ptr_owner = owner
        return wrapper

    def get_formated_date(self, format: str = "dd-mm-yyyy") -> str:
        """
        Return the date of last estimation in a given format.

        Returns
        -------
        str

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded 
        >>> # date as default format "dd-mm-yyyy"
        >>> equations["ACAF"].date
        '12-06-1998'
        >>> # date with specific format
        >>> equations["ACAF"].get_formated_date("dd/mm/yyyy")
        '12/06/1998'
        """   
        return self.c_equation.get_date_as_string(format.encode()).decode()

    @property
    def coefficients(self) -> List[str]:
        """
        Return the list of coefficients present in the current equation.

        Returns
        -------
        list(str)

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Equation, scalars, variables
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")      # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.scl
        161 objects loaded
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")    # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> eq_ACAF = Equation("ACAF", "(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> eq_ACAF         # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ACAF',
                 lec = '(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)',
                 method = 'LSQ',
                 from_period = '1960Y1',
                 to_period = '2015Y1')
        >>> eq_ACAF.coefficients
        ['acaf1', 'acaf2', 'acaf4']
        """
        return [coeff.decode() for coeff in self.c_equation.get_coefficients_list(<bint>False)]

    def _get_and_create_coefficients(self) -> List[str]:
        """
        Return the list of coefficients present in the equation. 
        Create an entry in the Scalars database for each coefficient not found in it. 

        Returns
        -------
        list(str)

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Equation, scalars, variables
        >>> scalars.clear()
        >>> eq_ACAF = Equation("ACAF", "(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> eq_ACAF         # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ACAF',
                 lec = '(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)',
                 method = 'LSQ',
                 from_period = '1960Y1',
                 to_period = '2015Y1')
        >>> # create scalars on the flight
        >>> eq_ACAF._get_and_create_coefficients()
        ['acaf1', 'acaf2', 'acaf4']
        >>> scalars.names
        ['acaf1', 'acaf2', 'acaf4']
        """
        return [coeff.decode() for coeff in self.c_equation.get_coefficients_list(<bint>True)]

    @property
    def variables(self) -> List[str]:
        """
        Return the list of variables present in the equation.

        Returns
        -------
        list(str)

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Equation, scalars, variables
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.scl
        161 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> eq_ACAF = Equation("ACAF", "(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> eq_ACAF         # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ACAF',
                 lec = '(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)',
                 method = 'LSQ',
                 from_period = '1960Y1',
                 to_period = '2015Y1')
        >>> eq_ACAF.variables
        ['ACAF', 'VAF', 'GOSF', 'TIME']
        """
        return [var.decode() for var in self.c_equation.get_variables_list(<bint>False)]

    def _get_and_create_variables(self) -> List[str]:
        """
        Return the list of variables present in the equation.
        Create an entry in the Variables database for each variable not found in it. 

        Returns
        -------
        list(str)

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Equation, scalars, variables
        >>> variables.clear()
        >>> variables.sample = "1960Y1:2015Y1"
        >>> eq_ACAF = Equation("ACAF", "(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> eq_ACAF         # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ACAF',
                 lec = '(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)',
                 method = 'LSQ',
                 from_period = '1960Y1',
                 to_period = '2015Y1')
        >>> # create variables on the flight
        >>> eq_ACAF._get_and_create_variables()
        ['ACAF', 'VAF', 'GOSF', 'TIME']
        >>> variables.names
        ['ACAF', 'GOSF', 'TIME', 'VAF']
        """
        return [var.decode() for var in self.c_equation.get_variables_list(<bint>True)]

    def split_equation(self) -> Tuple[str, str]:
        """
        Split an equation into its left-hand side and its right-hand side.

        Returns
        -------
        tuple(str, str)

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Equation
        >>> eq_ACAF = Equation("ACAF", "(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> lhs, rhs = eq_ACAF.split_equation()
        >>> # left-hand side
        >>> lhs
        '(ACAF / VAF[-1])'
        >>> # right-hand side
        >>> rhs
        'acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)'
        """
        lhs, rhs = self.c_equation.split_equation()
        return lhs.decode(), rhs.decode() 

    def estimate(self, from_period: Union[str, Period]=None, to_period: Union[str, Period]=None) -> bool:
        r"""
        Estimate the present equation.

        At the end of the estimation process, certain variables and scalars are automatically created 
        if the process has converged. These variables and scalars can be used for computational purposes and, 
        as they are part of the global workspace, can be saved for future use.

        The tests resulting from the last estimation are saved as scalars. The same applies to residuals, 
        left-hand and right-hand members of equations.

        Saved tests (as scalars) have the following names:

            - `e0_n` : number of sample periods 
            - `e0_k` : number of estimated coefficients 
            - `e0_stdev` : std dev of residuals 
            - `e0_meany` : mean of Y 
            - `e0_ssres` : sum of squares of residuals 
            - `e0_stderr` : std error 
            - `e0_stderrp` : std error percent (in %) 
            - `e0_fstat` : F-Stat 
            - `e0_r2` : R square 
            - `e0_r2adj` : adjusted R-squared 
            - `e0_dw` : Durbin-Watson 
            - `e0_loglik` : Log Likelihood 

        Calculated series are saved in special variables:

            - `_YCALC0` : right-hand side of the equation
            - `_YOBS0` : left-hand side of the equation
            - `_YRES0` : residuals of the equation

        Outside the estimation sample, the series values are :math:`NA`.

        Parameters
        ----------
        from_period : str or Period, optional
            The starting period of the execution range. 
            Defaults to the starting period of the current Variables sample.
        to_period : str or Period, optional
            The ending period of the execution range.
            Defaults to the ending period of the current Variables sample.

        Returns
        -------
        bool
            True if the estimation process has converged, False otherwise.

        Warnings
        --------
        If the present equation belongs to a block, you must use the :meth:`Equations.estimate` method instead.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations, scalars, variables
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded

        >>> eq_ACAF = equations["ACAF"]
        >>> eq_ACAF.lec
        '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)'

        >>> # create scalars
        >>> scalars["acaf1"] = 0., 1.
        >>> scalars["acaf2"] = 0., 1.
        >>> scalars["acaf4"] = 0., 1.

        >>> # estimate the ACQF equation
        >>> success = eq_ACAF.estimate("1980Y1", "1996Y1")
        Estimating : iteration 1 (||eps|| = 0.173205)
        <BLANKLINE>
        Estimating : iteration 2 (||eps|| = 5.16075e-09)
        <BLANKLINE>
        Solution reached after 2 iteration(s). Creating results file ...
        <BLANKLINE>
        >>> success
        True
        >>> scalars["acaf1"]
        Scalar(0.0157705, 1, 0.00136949)
        >>> scalars["acaf2"]
        Scalar(-7.96505e-06, 1, 1.48247e-06)
        >>> scalars["acaf4"]
        Scalar(-0.0085027, 1, 0.00208257)
        >>> equations["ACAF"]                           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ACAF',
                lec = '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)',
                method = 'LSQ',
                from_period = '1980Y1',
                to_period = '1996Y1',
                block = 'ACAF',
                tests = {corr = 1,
                        dw = 2.33007,
                        fstat = 32.2851,
                        loglik = 83.8101,
                        meany = 0.00818467,
                        r2 = 0.821815,
                        r2adj = 0.79636,
                        ssres = 5.19787e-05,
                        stderr = 0.00192685,
                        stderrp = 23.5422,
                        stdev = 0.0042699},
                date = '...')
        >>> # observed values (left hand side of the equation)
        >>> variables["_YOBS0", "1980Y1:1996Y1"]        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1980Y1:1996Y1
        mode: LEVEL
        <BLANKLINE>
         name       1980Y1  1981Y1  1982Y1  ...  1994Y1  1995Y1  1996Y1
        _YOBS0        0.01    0.02    0.01  ...    0.00   -0.00    0.01
        <BLANKLINE>
        >>> # fitted values (right hand side of the equation)
        >>> variables["_YCALC0", "1980Y1:1996Y1"]       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1980Y1:1996Y1
        mode: LEVEL
        <BLANKLINE>
          name      1980Y1  1981Y1  1982Y1  ...  1994Y1  1995Y1  1996Y1
        _YCALC0       0.01    0.01    0.01  ...    0.01   -0.00    0.00
        <BLANKLINE>
        >>> # residuals values
        >>> variables["_YRES0", "1980Y1:1996Y1"]        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1980Y1:1996Y1
        mode: LEVEL
        <BLANKLINE>
         name       1980Y1  1981Y1  1982Y1  ...  1994Y1  1995Y1  1996Y1
        _YRES0       -0.00    0.00   -0.00  ...   -0.00   -0.00    0.00
        <BLANKLINE>
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
        
        try:
            cpp_eqs_estimate(self.c_equation.get_endo(), from_period.encode(), to_period.encode())
            return True
        except Exception as e:
            warnings.warn(str(e), RuntimeWarning)
            return False

    cdef void update_global_database(self):
        if self.c_database is not NULL:
            self.c_database.update(<string>(self.endogenous.encode("utf-8")), 
                                   dereference(self.c_equation))

    cdef void reset_date_and_tests(self):
        self.c_equation.date = 0L
        memset(&self.c_equation.tests, 0, EQS_NBTESTS * sizeof(float))

    # Attributes access

    @property
    def endogenous(self) -> str:
        return self.c_equation.get_endo().decode()

    @property
    def lec(self) -> str:
        """
        LEC expression of the current equation.

        Paramaters
        ----------
        value: str
            New LEC expression.

        Examples
        --------
        >>> from iode import Equation
        >>> eq_ACAF = Equation("ACAF", "(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> eq_ACAF.endogenous
        'ACAF'
        >>> eq_ACAF.lec
        '(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)'
        >>> # remove acaf1 from the LEC expression of the ACAF equation
        >>> eq_ACAF.lec = "(ACAF / VAF[-1]) := acaf2 * GOSF[-1] + acaf4 * (TIME=1995)"
        >>> eq_ACAF.lec
        '(ACAF / VAF[-1]) := acaf2 * GOSF[-1] + acaf4 * (TIME=1995)'
        >>> # wrong name for the endogenous variable
        >>> eq_ACAF.lec = "(ACAF_ / VAF[-1]) := acaf2 * GOSF[-1] + acaf4 * (TIME=1995)"
        Traceback (most recent call last):
        ... 
        ValueError: Cannot set LEC '(ACAF_ / VAF[-1]) := acaf2 * GOSF[-1] + acaf4 * (TIME=1995)' to the equation named 'ACAF'
        """
        return self.c_equation.get_lec().decode()
    
    @lec.setter
    def lec(self, value: str):
        value = value.strip()
        self.c_equation.set_lec(value.encode())
        self.reset_date_and_tests()
        self.update_global_database()

    @property
    def method(self) -> str:
        """
        Estimation method.

        Parameters
        ----------
        value: EqMethod or str
            Possible values are LSQ, ZELLNER, INSTRUMENTAL, GLS, MAX_LIKELIHOOD.

        Examples
        --------
        >>> from iode import Equation, EqMethod
        >>> eq_ACAF = Equation("ACAF", "(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> # default value
        >>> eq_ACAF.method
        'LSQ'
        >>> eq_ACAF.method = EqMethod.GLS
        >>> eq_ACAF.method
        'GLS'
        >>> eq_ACAF.method = 'MAX_LIKELIHOOD'
        >>> eq_ACAF.method
        'MAX_LIKELIHOOD'
        """
        return EqMethod(<int>(self.c_equation.get_method_as_int())).name

    @method.setter
    def method(self, value: Union[EqMethod, str]):
        if isinstance(value, str):
            value = value.upper()
            # warning: In the IODE ascii and binary files, the method is stored as a string (char*).
            #          In particular: 
            #              - if no method have been specifed, the method is stored as an empty string
            #              - the GLS method is returned as 'GLS (3SLS)'   
            if not value:
                value = EqMethod.LSQ         
            elif "GLS" in value:
                value = EqMethod.GLS
            else:
                value = EqMethod[value]
        value = int(value)
        self.c_equation.set_method(<IodeEquationMethod>(value))
        self.reset_date_and_tests()
        self.update_global_database()

    @property
    def sample(self) -> Sample:
        """
        Estimation sample of the current equation.

        Parameters
        ----------
        value: str or Sample
            New estimation sample

        Examples
        --------
        >>> from iode import Equation, variables
        >>> variables.clear()
        >>> eq_ACAF = Equation("ACAF", "(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> eq_ACAF.sample
        None

        >>> # set Variables sample
        >>> variables.sample = "1960Y1:2015Y1"
        >>> variables.sample
        Sample("1960Y1:2015Y1")

        >>> # specify starting and ending periods
        >>> eq_ACAF.sample = "1980Y1:2000Y1"
        >>> eq_ACAF.sample
        Sample("1980Y1:2000Y1")

        >>> # specify only the starting period 
        >>> # -> ending period = ending period of the Variables sample
        >>> eq_ACAF.sample = "1990Y1:"
        >>> eq_ACAF.sample
        Sample("1990Y1:2015Y1")

        >>> # specify only the ending period 
        >>> # -> starting period = starting period of the Variables sample
        >>> eq_ACAF.sample = ":2010Y1"
        >>> eq_ACAF.sample
        Sample("1960Y1:2010Y1")

        >>> # specify nothing
        >>> # new equation sample = Variables sample
        >>> eq_ACAF.sample = ":"
        >>> eq_ACAF.sample
        Sample("1960Y1:2015Y1")
        """
        cdef CSample sample = self.c_equation.get_sample()
        return Sample._from_ptr(new CSample(sample), <bint>True)

    @sample.setter
    def sample(self, value: Union[str, Sample]):
        if isinstance(value, Sample):
            value = str(value)
        if ':' not in value:
            raise ValueError("New sample value must contain the colon character ':'")
        from_period, to_period = value.split(':')

        self.c_equation.set_sample(from_period.encode(), to_period.encode())
        self.reset_date_and_tests()
        self.update_global_database()

    @property
    def comment(self) -> str:
        """
        Equation comment.
        """
        return self.c_equation.get_comment().decode()

    @comment.setter
    def comment(self, value: str):
        self.c_equation.set_comment(value.encode())
        self.update_global_database()

    @property
    def instruments(self) -> Union[str, List[str]]:
        """
        Instrument(s) used for the estimation.

        Parameters
        ----------
        value: str or list(str)
            If several instruments are required for the estimation, they can be passed either as 
            a unique string in which instruments are separated by a semi colon ';' or as a list of 
            strings.
        """
        _instruments = self.c_equation.get_instruments().decode().split(';')
        return _instruments[0] if len(_instruments) == 1 else _instruments

    @instruments.setter
    def instruments(self, value: Union[str, List[str]]):
        if not isinstance(value, str):
            value = ';'.join(value)
        self.c_equation.set_instruments(value.encode())
        self.reset_date_and_tests()
        self.update_global_database()

    @property
    def block(self) -> str:
        """
        Estimation block of equations to which the equation belongs.

        Parameters
        ----------
        value: str or list(str)
           
        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded 
        >>> equations["ACAF"].block             # doctest: +NORMALIZE_WHITESPACE
        'ACAF'
        """
        return self.c_equation.get_block().decode()

    @block.setter
    def block(self, value: Union[str, List[str]]):
        if not isinstance(value, str):
            value = ';'.join(value)
        self.c_equation.set_block(value.encode())
        self.reset_date_and_tests()
        self.update_global_database()

    @property
    def tests(self) -> Dict[str, float]:
        r"""
        Estimation tests.

        Returns
        -------
        dict(str, float)

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded 
        >>> equations["ACAF"].tests             # doctest: +NORMALIZE_WHITESPACE
        {'corr': 1.0, 'dw': 2.329345941543579, 'fstat': 32.273193359375, 'loglik': 83.80752563476562, 
        'meany': 0.008184665814042091, 'r2': 0.8217613697052002, 'r2adj': 0.7962986826896667, 
        'ssres': 5.1994487876072526e-05, 'stderr': 0.0019271461060270667, 'stderrp': 23.545812606811523, 
        'stdev': 0.004269900266081095}
        """
        cdef map[string, float] cpp_tests = self.c_equation.get_tests_as_map()
        return {item.first.decode(): item.second for item in cpp_tests}

    @property
    def date(self) -> str:
        r"""
        Date of the last estimation.
        
        Returns
        -------
        str

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded 
        >>> equations["ACAF"].date
        '12-06-1998'
        """
        return self.get_formated_date()

    # misc

    def _set_tests(self, tests: List[float]):
        r"""
        Examples
        --------
        >>> from iode import Equation, variables
        >>> variables.sample = "1960Y1:2015Y1"
        >>> eq_ACAF = Equation("ACAF", "(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> eq_ACAF         # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ACAF',
                 lec = '(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)',
                 method = 'LSQ',
                 from_period = '1960Y1',
                 to_period = '2015Y1')
        >>> test_values = [1.0, 2.32935, 32.2732, 83.8075, 0.00818467, 0.821761, 0.796299, 
        ...                5.19945e-05, 0.00192715, 23.5458, 0.0042699]
        >>> eq_ACAF._set_tests(test_values)
        >>> eq_ACAF         # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ACAF',
                 lec = '(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)',
                 method = 'LSQ',
                 from_period = '1960Y1',
                 to_period = '2015Y1',
                 tests = {corr = 1,
                          dw = 23.5458,
                          fstat = 0.796299,
                          loglik = 0.0042699,
                          meany = 32.2732,
                          r2 = 5.19945e-05,
                          r2adj = 0.00192715,
                          ssres = 83.8075,
                          stderr = 0.00818467,
                          stderrp = 0.821761,
                          stdev = 2.32935})
        """
        if len(tests) != len(EqTest):
            raise ValueError("Cannot set equation test values. "
                             f"Expected vector of size {len(EqTest)} but got vector of size {len(tests)}.")
        for i, value in enumerate(tests):
            self.c_equation.set_test(<IodeEquationTest>i, value)
        self.update_global_database()

    def _set_date(self, value: str, format: str = "dd-mm-yyyy"):
        r"""
        Examples
        --------
        >>> from iode import Equation, variables
        >>> variables.sample = "1960Y1:2015Y1"
        >>> eq_ACAF = Equation("ACAF", "(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> eq_ACAF         # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ACAF',
                 lec = '(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)',
                 method = 'LSQ',
                 from_period = '1960Y1',
                 to_period = '2015Y1')
        >>> eq_ACAF.date
        ''
        >>> eq_ACAF._set_date("18-06-2024")
        >>> eq_ACAF.date
        '18-06-2024'
        >>> eq_ACAF._set_date("20/06/2024", "dd/mm/yyyy")
        >>> eq_ACAF.date
        '20-06-2024'
        >>> eq_ACAF._set_date("")
        >>> eq_ACAF.date
        ''
        """
        if not value:
            self.c_equation.reset_date()
        else:    
            self.c_equation.set_date(value.encode(), format.encode())
        self.update_global_database()

    def _as_tuple(self):
        r"""
        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded 
        >>> equations["ACAF"]           # doctest: +NORMALIZE_WHITESPACE
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
        >>> equations["ACAF"]._as_tuple()         # doctest: +NORMALIZE_WHITESPACE
        ('ACAF', '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)', 'LSQ', '1980Y1:1996Y1', '', '', 'ACAF', 
        1.0, 0.004269900266081095, 0.008184665814042091, 5.1994487876072526e-05, 0.0019271461060270667, 23.545812606811523, 
        32.273193359375, 0.8217613697052002, 0.7962986826896667, 2.329345941543579, 83.80752563476562, '12-06-1998')
        """
        tests = [self.c_equation.tests[i] for i in range(len(EqTest))]
        return self.endogenous, self.lec, self.method, str(self.sample), self.comment, self.instruments, \
               self.block, *tests, self.date

    def copy(self) -> Equation:
        r"""
        Return a copy of the current Equation.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations, variables
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> equations["ACAF"]           # doctest: +NORMALIZE_WHITESPACE
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
        >>> copied_eq = equations["ACAF"].copy()
        >>> copied_eq                   # doctest: +NORMALIZE_WHITESPACE
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
        """
        return copy(self)

    # Special methods

    def __eq__(self, other: Equation) -> bool:
        return self.c_equation == other.c_equation

    def __copy__(self) -> Equation:
        r"""
        Return a copy of the current Equation.

        Examples
        --------
        >>> from copy import copy
        >>> from iode import SAMPLE_DATA_DIR, equations, variables
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> equations["ACAF"]           # doctest: +NORMALIZE_WHITESPACE
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
        >>> copied_eq = copy(equations["ACAF"])
        >>> copied_eq                   # doctest: +NORMALIZE_WHITESPACE
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
        """
        from_period = self.sample.start
        to_period = self.sample.end
        tests = [self.c_equation.tests[i] for i in range(len(EqTest))]
        copied_eq = Equation(self.endogenous, self.lec, self.method, from_period, to_period, 
                             self.comment, self.instruments, self.block)
        copied_eq._set_tests(tests)
        copied_eq._set_date(self.date)
        return copied_eq

    def __str__(self) -> str:
        sample = self.sample
        tests = self.tests
        indent = " " * len("Equation(")

        s = [f"endogenous = {self.endogenous}"]
        s += [f"lec = {self.lec}"]
        s += [f"method = {self.method}"]
        if len(sample):
            s+= [f"sample = {sample}"]
        if self.comment:
            s += [f"comment = {self.comment}"]
        if self.block:
            s += [f"block = {self.block}"]
        if self.instruments:
            s += [f"instruments = {self.instruments}"]
        if tests['corr'] > 0.0:
            indent_tests = " " * len("tests = ")
            s += ["tests = " + f",\n{indent}{indent_tests}".join(f"{key} = {value:g}" 
                                for key, value in self.tests.items())]
        if self.date:
            s += [f"date = {self.date}"]

        return "Equation(" + f",\n{indent}".join(s) + ")"

    def __repr__(self) -> str:
        sample = self.sample
        tests = self.tests
        indent = " " * len("Equation(")

        s = [f"endogenous = {repr(self.endogenous)}"]
        s += [f"lec = {repr(self.lec)}"]
        s += [f"method = {repr(self.method)}"]
        if len(sample):
            s+= [f"from_period = '{sample.start}'", f"to_period = '{sample.end}'"]
        if self.comment:
            s += [f"comment = {repr(self.comment)}"]
        if self.block:
            s += [f"block = {repr(self.block)}"]
        if self.instruments:
            s += [f"instruments = {repr(self.instruments)}"]
        if tests['corr'] > 0.0:
            indent_tests = " " * len("tests = {")
            s += ["tests = {" + f",\n{indent}{indent_tests}".join(f"{key} = {value:g}" 
                                for key, value in self.tests.items()) + "}"]
        if self.date:
            s += [f"date = {repr(self.date)}"]

        return "Equation(" + f",\n{indent}".join(s) + ")" 

    def __hash__(self) -> int:
        return <int>hash_value_eq(dereference(self.c_equation))
