# distutils: language = c++

from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional

from libcpp.map cimport map
from libcpp.string cimport string
from cython.operator cimport dereference
from pyiode.common cimport EnumIodeEquationTest
from pyiode.objects.equation cimport CEquation
from pyiode.objects.equation cimport hash_value as hash_value_eq


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
    method: str
        estimation method. Possible values are:

            - "LSQ"
            - "ZELLNER"
            - "INSTRUMENTAL"
            - "GLS (3SLS)"
            - "MAX_LIKELIHOOD"
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
    method: int or {'LSQ', 'ZELLNER', 'INSTRUMENTAL', 'GLS', 'MAX_LIKELIHOOD'}, optional
        Method used to estimate the coefficients of the equation.
        Defaults to 'LSQ' (0).
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
    Equation(lec: (ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995),
        method: LSQ,
        sample: 1960Y1:2015Y1,
        comment: ,
        block: ,
        instruments: ,
        tests:
                corr: 0
                dw: 0
                fstat: 0
                loglik: 0
                meany: 0
                r2: 0
                r2adj: 0
                ssres: 0
                stderr: 0
                stderrp: 0
                stdev: 0
        date: )
    """
    cdef string cpp_endogenous
    cdef CEquation* c_equation

    def __cinit__(self, endogenous: str, lec: str, method: Union[int, str] = "LSQ", from_period: Union[str, Period] = "", 
        to_period: Union[str, Period] = "", comment: str = "", instruments: str = "", block: str = "") -> Equation:
        if isinstance(from_period, Period):
            from_period = str(from_period)

        if isinstance(to_period, Period):
            to_period = str(to_period)

        self.cpp_endogenous = endogenous.encode()
        self.c_equation = new CEquation(self.cpp_endogenous, lec.encode(), 0, from_period.encode(), 
                                        to_period.encode(), comment.encode(), instruments.encode(), 
                                        block.encode(), <bint>False)
        self.method = method

    def __dealloc__(self):
        self.cpp_endogenous = b''
        del self.c_equation
        self.c_equation = NULL

    def set_lec(self, lec: str, endogenous: str):
        """
        Update LEC expression of the current equation.

        Examples
        --------
        >>> from iode import Equation
        >>> eq_ACAF = Equation("ACAF", "(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> eq_ACAF.endogenous
        'ACAF'
        >>> eq_ACAF.lec
        '(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)'
        >>> # remove acaf1 from the LEC expression of the ACAF equation
        >>> eq_ACAF.set_lec("(ACAF / VAF[-1]) := acaf2 * GOSF[-1] + acaf4 * (TIME=1995)", "ACAF")
        >>> eq_ACAF.lec
        '(ACAF / VAF[-1]) := acaf2 * GOSF[-1] + acaf4 * (TIME=1995)'
        >>> # wrong name for the endogenous variable
        >>> eq_ACAF.set_lec("(ACAF / VAF[-1]) := acaf2 * GOSF[-1] + acaf4 * (TIME=1995)", "ACAG")
        Traceback (most recent call last):
        ... 
        ValueError: Cannot set LEC '(ACAF / VAF[-1]) := acaf2 * GOSF[-1] + acaf4 * (TIME=1995)' to the equation named 'ACAG'
        """
        self.cpp_endogenous = endogenous.encode()
        self.c_equation.set_lec(lec.encode(), self.cpp_endogenous)

    def set_sample(self, from_period: Union[str, Period] = "", to_period: Union[str, Period] = ""):
        """
        Set the sample for the estimation of coefficients.

        Parameters
        ----------
        from_period: str or Period, optional
            starting period for the estimation.
            Defaults to the starting period of the sample associated with the IODE Variables database.
        to_period: str or Period, optional
            ending period for the estimation.
            Defaults to the ending period of the sample associated with the IODE Variables database.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables, Equation, Sample
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> variables.sample
        '1960Y1:2015Y1'
        >>> eq_ACAF = Equation("ACAF", "(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> eq_ACAF.set_sample("1980Y1", "2010Y1")
        >>> eq_ACAF.sample
        '1980Y1:2010Y1'
        >>> # change only the starting period
        >>> eq_ACAF.set_sample(from_period="1970Y1")
        >>> eq_ACAF.sample
        '1970Y1:2015Y1'
        >>> # change only the ending period
        >>> eq_ACAF.set_sample(to_period="2000Y1")
        >>> eq_ACAF.sample
        '1960Y1:2000Y1'
        """
        if isinstance(from_period, Period):
            from_period = str(from_period)

        if isinstance(to_period, Period):
            to_period = str(to_period)

        self.c_equation.set_sample(from_period.encode(), to_period.encode())

    def get_date_format(self, format: str = "dd-mm-yyyy") -> str:
        """
        Return the date of last estimation in a given format.

        Returns
        -------
        str

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Equation
        >>> eq_ACAF = Equation("ACAF", "(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> # date as default format "dd-mm-yyyy"
        >>> eq_ACAF.date
        ''
        >>> # date with specific format
        >>> eq_ACAF.get_date_format("dd/mm/yyyy")
        ''
        """   
        return self.c_equation.get_date_as_string(format.encode()).decode()

    def get_coefficients_list(self, create_if_not_exit: bool = True) -> List[str]:
        """
        Return the list of coefficients present in the equation.

        Parameters
        ----------
        create_if_not_exit: bool, optional
            Whether or not to create an entry in the Scalars database for each coefficient not found in it. 
            Defaults to True.

        Returns
        -------
        list(str)

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Equation, scalars, variables
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> eq_ACAF = Equation("ACAF", "(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> eq_ACAF         # doctest: +NORMALIZE_WHITESPACE
        Equation(lec: (ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995),
            method: LSQ,
            sample: 1960Y1:2015Y1,
            comment: ,
            block: ,
            instruments: ,
            tests:
                corr: 0
                dw: 0
                fstat: 0
                loglik: 0
                meany: 0
                r2: 0
                r2adj: 0
                ssres: 0
                stderr: 0
                stderrp: 0
                stdev: 0
            date: )
        >>> eq_ACAF.get_coefficients_list()
        ['acaf1', 'acaf2', 'acaf4']
        >>> # clear Scalars database
        >>> scalars.clear()
        >>> # Do not create scalars in the Scalars database
        >>> eq_ACAF.get_coefficients_list(False)
        ['acaf1', 'acaf2', 'acaf4']
        >>> scalars.get_names()
        []
        >>> # create scalars on the flight
        >>> eq_ACAF.get_coefficients_list()
        ['acaf1', 'acaf2', 'acaf4']
        >>> # content of the Scalars database
        >>> scalars.get_names()
        ['acaf1', 'acaf2', 'acaf4']
        """
        return [coeff.decode() for coeff in self.c_equation.get_coefficients_list(create_if_not_exit)]

    def get_variables_list(self, create_if_not_exit: bool = True) -> List[str]:
        """
        Return the list of variables present in the equation.

        Parameters
        ----------
        create_if_not_exit: bool, optional
            Whether or not to create an entry in the Variables database for each variable not found in it. 
            Defaults to True.

        Returns
        -------
        list(str)

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Equation, scalars, variables
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> eq_ACAF = Equation("ACAF", "(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> eq_ACAF         # doctest: +NORMALIZE_WHITESPACE
        Equation(lec: (ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995),
            method: LSQ,
            sample: 1960Y1:2015Y1,
            comment: ,
            block: ,
            instruments: ,
            tests:
                corr: 0
                dw: 0
                fstat: 0
                loglik: 0
                meany: 0
                r2: 0
                r2adj: 0
                ssres: 0
                stderr: 0
                stderrp: 0
                stdev: 0
            date: )
        >>> eq_ACAF.get_variables_list()
        ['ACAF', 'VAF', 'GOSF', 'TIME']
        >>> # clear Variables database + reset vars sample
        >>> variables.clear()
        >>> variables.sample = "1960Y1:2015Y1"
        >>> variables.sample
        '1960Y1:2015Y1'
        >>> # Do not create variables in the Variables database
        >>> eq_ACAF.get_variables_list(False)
        ['ACAF', 'VAF', 'GOSF', 'TIME']
        >>> variables.get_names()
        []
        >>> # create variables on the flight
        >>> eq_ACAF.get_variables_list()
        ['ACAF', 'VAF', 'GOSF', 'TIME']
        >>> # content of the Variables database
        >>> variables.get_names()
        ['ACAF', 'GOSF', 'TIME', 'VAF']
        """
        return [var.decode() for var in self.c_equation.get_variables_list(create_if_not_exit)]

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

    def estimate(self, from_period: Union[str, Period]=None, to_period: Union[str, Period]=None):
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

        Warnings
        --------
        If the present equation belongs to a block, you must use the :meth:`Equations.estimate` method instead.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations, scalars, variables
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

        >>> eq_ACAF = equations["ACAF"]
        >>> eq_ACAF.lec
        '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)'

        >>> # create scalars
        >>> scalars["acaf1"] = 0., 1.
        >>> scalars["acaf2"] = 0., 1.
        >>> scalars["acaf4"] = 0., 1.

        >>> # estimate the ACQF equation
        >>> eq_ACAF.estimate("1980Y1", "1996Y1")
        >>> scalars["acaf1"]
        Scalar(0.0157705, 1, 0.00136949)
        >>> scalars["acaf2"]
        Scalar(-7.96505e-06, 1, 1.48247e-06)
        >>> scalars["acaf4"]
        Scalar(-0.0085027, 1, 0.00208257)
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
        
        cpp_eqs_estimate(self.cpp_endogenous, from_period.encode(), to_period.encode())

    # Attributes access

    @property
    def endogenous(self) -> str:
        return self.cpp_endogenous.decode()

    @property
    def lec(self) -> str:
        return self.c_equation.get_lec().decode()
    
    @property
    def method(self) -> str:
        """
        Estimation method.

        Parameters
        ----------
        value: EqMethod or str
            Possible values are LSQ, ZELLNER, INSTRUMENTAL, GLS, MAX_LIKELIHOOD.
        """
        return self.c_equation.get_method().decode()

    @method.setter
    def method(self, value: Union[EqMethod, str]):
        if isinstance(value, str):
            value = value.upper()
            value = EqMethod[value]
        self.c_equation.set_method(int(value))

    @property
    def sample(self) -> Sample:
        cdef CSample sample = self.c_equation.get_sample()
        return Sample._from_ptr(new CSample(sample), <bint>True)

    @sample.setter
    def sample(self, value: Union[str, Sample]):
        if isinstance(value, Sample):
            value = str(value)
        from_period, to_period = value.split(':')

        self.c_equation.set_sample(from_period.encode(), to_period.encode())

    @property
    def comment(self) -> str:
        return self.c_equation.get_comment().decode()

    @comment.setter
    def comment(self, value: str):
        self.c_equation.set_comment(value.encode())

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

    @property
    def block(self) -> str:
        return self.c_equation.get_block().decode()

    @block.setter
    def block(self, value: Union[str, List[str]]):
        if not isinstance(value, str):
            value = ';'.join(value)
        self.c_equation.set_block(value.encode())

    @property
    def tests(self) -> Dict[str, float]:
        cdef map[string, float] cpp_tests = self.c_equation.get_tests_as_map()
        return {item.first.decode(): item.second for item in cpp_tests}

    @property
    def date(self) -> str:
        return self.get_date_format()

    # Special methods

    def __eq__(self, other: Equation) -> bool:
        return self.c_equation == other.c_equation

    def __str__(self) -> str:
        return self.c_equation.to_string().decode()

    def __repr__(self) -> str:
        return self.c_equation.to_string().decode()

    def __hash__(self) -> int:
        return <int>hash_value_eq(dereference(self.c_equation))


cdef Equation _to_py_equation(string endo, CEquation c_eq):
    str_sample = c_eq.get_sample().to_string().decode()
    from_period, to_period = str_sample.split(':')
    py_eq = Equation(endo.decode(), c_eq.get_lec().decode(), c_eq.get_method().decode(), 
                    from_period, to_period, c_eq.get_comment().decode(), 
                    c_eq.get_instruments().decode(), c_eq.get_block().decode())
    py_eq.c_equation.date = c_eq.date
    for i, test_val in enumerate(c_eq.tests): 
        py_eq.c_equation.set_test(<EnumIodeEquationTest>i, test_val)
    return py_eq
