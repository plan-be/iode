import sys
from copy import copy
from typing import Union, Tuple, List, Dict, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

from iode.common import EqMethod
from iode.iode_cython import Period, Sample
from iode.iode_cython import Equation as CythonEquation


class Equation(CythonEquation):
    r"""
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
    def __init__(self, endogenous: str, lec: str, method: Union[EqMethod, str]=EqMethod.LSQ, from_period: Union[str, Period]='', to_period: Union[str, Period]='', comment: str='', instruments: str='', block: str='') -> Self:
        return CythonEquation.__init__(self, endogenous, lec, method, from_period, to_period, comment, instruments, block)

    @classmethod
    def _new_instance(cls) -> Self:
        instance = cls.__new__(cls)
        return instance

    def get_formated_date(self, format: str='dd-mm-yyyy') -> str:
        r"""
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
        return CythonEquation.get_formated_date(self, format)

    @property
    def coefficients(self) -> List[str]:
        r"""
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
        return CythonEquation.get_coefficients(self)

    def _get_and_create_coefficients(self) -> List[str]:
        r"""
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
        return CythonEquation._get_and_create_coefficients(self)

    @property
    def variables(self) -> List[str]:
        r"""
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
        return CythonEquation.get_variables(self)

    def _get_and_create_variables(self) -> List[str]:
        r"""
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
        return CythonEquation._get_and_create_variables(self)

    def split_equation(self) -> Tuple[str, str]:
        r"""
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
        return CythonEquation.split_equation(self)

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
        return CythonEquation.estimate(self, from_period, to_period)

    @property
    def endogenous(self) -> str:
        return CythonEquation.get_endogenous(self)

    @property
    def lec(self) -> str:
        r"""
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
        return CythonEquation.get_lec(self)

    @lec.setter
    def lec(self, value: str):
        CythonEquation.set_lec(self, value)

    @property
    def method(self) -> str:
        r"""
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
        return CythonEquation.get_method(self)

    @method.setter
    def method(self, value: Union[EqMethod, str]):
        CythonEquation.set_method(self, value)

    @property
    def sample(self) -> Sample:
        r"""
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
        return CythonEquation.get_sample(self)

    @sample.setter
    def sample(self, value: Union[str, Sample]):
        CythonEquation.set_sample(self, value)

    @property
    def comment(self) -> str:
        r"""
        Equation comment.
        """
        return CythonEquation.get_comment(self)

    @comment.setter
    def comment(self, value: str):
        CythonEquation.set_comment(self, value)

    @property
    def instruments(self) -> Union[str, List[str]]:
        r"""
        Instrument(s) used for the estimation.

        Parameters
        ----------
        value: str or list(str)
            If several instruments are required for the estimation, they can be passed either as 
            a unique string in which instruments are separated by a semi colon ';' or as a list of 
            strings.
        """
        return CythonEquation.get_instruments(self)

    @instruments.setter
    def instruments(self, value: Union[str, List[str]]):
        CythonEquation.set_instruments(self, value)

    @property
    def block(self) -> str:
        r"""
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
        return CythonEquation.get_block(self)

    @block.setter
    def block(self, value: Union[str, List[str]]):
        CythonEquation.set_block(self, value)

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
        return CythonEquation.get_tests(self)

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

    def _set_tests_from_list(self, tests: List[float]):
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
        >>> eq_ACAF._set_tests_from_list(test_values)
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
        CythonEquation._set_tests_from_list(self, tests)

    def _set_date(self, value: str, format: str='dd-mm-yyyy'):
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
        CythonEquation._set_date(self, value, format)

    def _as_tuple(self) -> Tuple:
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
        return CythonEquation._as_tuple(self)

    def copy(self) -> Self:
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

    def __eq__(self, other: Self) -> bool:
        return CythonEquation.__eq__(self, other)

    def __copy__(self) -> Self:
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
        copied_eq = Equation(self.endogenous, self.lec, self.method, from_period, to_period, 
                             self.comment, self.instruments, self.block)
        tests = self._get_list_tests()
        copied_eq._set_tests_from_list(tests)
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
        return CythonEquation.__hash__(self)


