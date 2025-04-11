import sys
import warnings
from collections.abc import Iterable
from typing import Union, Tuple, List, Optional, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

from iode.iode_cython import suppress_msgs, reset_msgs
from iode.time.period import Period
from iode.time.sample import Sample
from iode.objects.equation import Equation
from iode.iode_database.equations_database import Equations
from iode.iode_database.scalars_database import Scalars
from iode.iode_database.variables_database import variables

from iode.iode_cython import AdjustmentMethod, EqMethod
from iode.iode_cython import cython_dickey_fuller_test, cython_dynamic_adjustment
from iode.iode_cython import CythonCorrelationMatrix
from iode.iode_cython import CythonEditAndEstimateEquations


def dynamic_adjustment(method: Union[AdjustmentMethod, str], eqs: str, c1: str='c1', c2: str='c2') -> str:
    r"""
    Transform a LEC equation to add a dynamic adjustment.
    
    Two methods can be used. Given the equation :math:`LHS = RHS`, we have:
    
        - **Partial Adjustment** (PARTIAL): :math:`d(LHS) = c1 * (RHS - (LHS)[-1])`
        - **Error Correction Model** (ERROR_CORRECTION): :math:`d(LHS) = c1 * d(RHS) + c2 * (RHS -LHS)[-1]`
    
    Parameters
    ----------
    method: AdjustmentMethod or str 
        Method used for the dynamic adjustment. 
        Possible values are PARTIAL or ERROR_CORRECTION.
    eqs: str
        LEC equation to dynamically adjust.
    c1: str, optional        
        Name of the first coefficient.
        Default to "c1".
    c2: str, optional    
        Name of the second coefficient. 
        Not used with the *Partial Adjustment* method.
        Default to "c2".

    Returns
    -------
    str
        Dynamically adjusted equation.

    Examples
    --------
    >>> from iode import SAMPLE_DATA_DIR, equations, dynamic_adjustment, AdjustmentMethod
    >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Loading .../fun.eqs
    274 objects loaded 
    >>> lec = equations["ACAF"].lec                         # doctest: +NORMALIZE_WHITESPACE
    >>> lec
    '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)'
    
    Partial Adjustment

    >>> partial_adjust_eq = dynamic_adjustment(AdjustmentMethod.PARTIAL, lec)
    >>> partial_adjust_eq
    'd((ACAF/VAF[-1])) := c1 * (acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995) -((ACAF/VAF[-1]))[-1])'

    Error Correction Model

    >>> error_corr_adjust_eq = dynamic_adjustment(AdjustmentMethod.ERROR_CORRECTION, lec)
    >>> error_corr_adjust_eq
    'd((ACAF/VAF[-1])) := c1 * d(acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)) + c2 * (acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995) -(ACAF/VAF[-1]))[-1]'
    """
    if not isinstance(method, (AdjustmentMethod, str, int)):
        raise TypeError("'method' must be of type AdjustmentMethod, str or int. "
                        f"Got value of type {type(method).__name__} instead.")
    if isinstance(method, str):
        method = method.upper()
        method = AdjustmentMethod[AdjustmentMethod]
    method = int(method)
    return cython_dynamic_adjustment(method, eqs, c1, c2)

def dickey_fuller_test(lec: str, drift: bool, trend: bool, order: int) -> Scalars:
    r"""
    Dickey-Fuller tests.

    Tests are saved in scalars whose name is composed of the prefix *df_* and the name of the first series 
    appearing in the formula to be tested. For example, the test for the formula :math:`d(A0GR+A0GF)` is `df_a0gr`.

    Parameters
    ----------
    lec: str
        LEC form to be tested.
    drift: bool
        Whether or not the formula to be estimated must incorporate a constant term. 
    trend: bool
        Whether or not the formula to be estimated should incorporate a trend term.
    order: int 
        Order of the polynomial to be estimated to obtain the tests.
    
    Returns
    -------
    Scalars
        Scalars database containing the results of the Dickey-Fuller tests.

    Examples
    --------
    >>> from iode import SAMPLE_DATA_DIR, scalars, variables
    >>> from iode import dickey_fuller_test
    >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Loading .../fun.var
    394 objects loaded

    >>> # dickey_fuller_test("ACAF", True, True, 3) estimates the equation:
    >>> #     d(ACAF) := df_ * ACAF[-1] +            
    >>> #     df_d +            (DRIFT)          
    >>> #     df_t * t +        (TREND)            
    >>> #     df1 * d(ACAF[-1]) + df2*d(ACAF[-2]) + df3*d(ACAF[-3])  (ORDER)
    >>> df_scalars = dickey_fuller_test("ACAF", True, True, 3)
    Estimating : iteration 1 (||eps|| = 2.20454)
    <BLANKLINE>
    Estimating : iteration 2 (||eps|| = 2.39047e-10)
    <BLANKLINE>
    Solution reached after 2 iteration(s). Creating results file ...
    <BLANKLINE>
    >>> df_scalars.get_names("df*")
    ['df1', 'df2', 'df3', 'df_', 'df_d', 'df_t']
    >>> # note: the function dickey_fuller_test() returns a separated Scalars database.
    >>> #       The global database scalars is left unchaged
    >>> scalars.get_names("df*")
    []
    >>> # order 0
    >>> df_scalars["df_"]
    Scalar(0.0132523, 0.0845155, 0.0845155)
    >>> # drift
    >>> df_scalars["df_d"]
    Scalar(13.0806, 1, 6.78675)
    >>> # trend
    >>> df_scalars["df_t"]
    Scalar(-0.492697, 0.187978, 0.187978)
    >>> # order 1
    >>> df_scalars["df1"]
    Scalar(-0.120123, 0.180991, 0.180991)
    >>> # order 2
    >>> df_scalars["df2"]
    Scalar(-0.476959, 0.154505, 0.154505)
    >>> # order 3
    >>> df_scalars["df3"]
    Scalar(-0.211047, 0.170708, 0.170708)
    """
    scalars_db = Scalars.get_instance()
    scalars_db._cython_instance = cython_dickey_fuller_test(scalars_db._cython_instance, lec, drift, trend, order)
    if scalars_db._cython_instance is None:
        raise RuntimeError("Dickey-Fuller test failed. No scalars database created.")
    return scalars_db


class CorrelationMatrix:
    def __init__(self):
        raise TypeError("This class cannot be instantiated directly")

    @classmethod
    def get_instance(cls) -> Self:
        instance = cls.__new__(cls)
        instance._cython_instance = CythonCorrelationMatrix.__new__(CythonCorrelationMatrix)
        return instance

    @property
    def shape(self) -> Tuple[int, int]:
        if self._cython_instance.is_undefined():
            warnings.warn("The correlation matrix has not been defined yet")
            return 0, 0
        return self._cython_instance.get_shape()

    @property
    def names(self) -> List[str]:
        if self._cython_instance.is_undefined():
            warnings.warn("The correlation matrix has not been defined yet")
            return []
        return self._cython_instance.get_names()

    def name(self, index: int) -> str:
        if self._cython_instance.is_undefined():
            warnings.warn("The correlation matrix has not been defined yet")
            return -1
        if index < 0 or index >= len(self):
            raise ValueError(f"'index': value must be in range [0, {len(self)}]. "
                             f"Got index value {index} instead")
        return self._cython_instance.name(index)

    def __len__(self) -> int:
        return self._cython_instance.__len__()

    def __getitem__(self, key: Tuple[int, int]) -> float:
        row, column = key
        shape = self.shape
        if row < 0 or row >= shape[0]:
            raise ValueError(f"CorrelationMatrix[row, column]: Value for rwo and column must be in range "
                            f"[0, {shape[0]}]. Got row value {row} instead")
        if column < 0 or column >= shape[1]:
            raise ValueError(f"CorrelationMatrix[row, column]: Value for rwo and column must be in range "
                            f"[0, {shape[1]}]. Got column value {column} instead")
        return self._cython_instance.__getitem__((row, column))

    def __setitem__(self, key, value):
        warnings.warn("The values of the correlation matrix are read-only.")

    def __delitem__(self, key):
        warnings.warn("The values of the correlation matrix are read-only.")

    def __str__(self):
        coeffs_names = self.names
        max_column_length = max(10, max(len(name) for name in coeffs_names) + 1)

        # horizontal header
        top_left = ' ' * max_column_length
        coeffs_names = [name.rjust(max_column_length) for name in coeffs_names]
        s = top_left + ' | ' + '  '.join(coeffs_names) + '\n'
        s += '-' * len(s) + '\n'
        # rows
        for row, name in enumerate(coeffs_names):
            values = [f"{self[row, column]:>10.6g}".rjust(max_column_length) for column in range(len(self))]
            s += name.ljust(max_column_length) + ' | ' + '  '.join(values) + '\n'
        return s

    def __repr__(self):
        return str(self)


class EditAndEstimateEquations:
    r"""
    Special separate Equations and Scalars databases to make estimations and merge results 
    into the global Scalars database if results are satisfying.

    Warnings
    --------
    Dedicated to be used only by the developers in the code of the Graphical User Interface for IODE
    """
    def __init__(self, from_period: Union[str, Period]=None, to_period: Union[str, Period]=None):
        if from_period is None or to_period is None:
            vars_sample = variables.sample
            if vars_sample.start is None or vars_sample.end is None:
                warnings.warn("The sample of the Variables workspace is not defined. "
                              "Set estimation sample as undefined.")
                from_period, to_period = '', ''
            else:
                if from_period is None:
                    from_period = vars_sample.start
                if to_period is None:
                    to_period = vars_sample.end
        
        if isinstance(from_period, Period):
            from_period = str(from_period)

        if isinstance(to_period, Period):
            to_period = str(to_period)

        self._cython_instance = CythonEditAndEstimateEquations(from_period, to_period)

    @property
    def sample(self) -> Sample:
        r"""
        Estimation sample.

        Parameters
        ----------
        value: str or slice(str, str) or tuple(str, str) or Sample
            New estimation sample as either string 'start_period:last_period' or 
            slice 'start_period':'last_period' or tuple 'start_period', 'last_period'.
        
        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations, scalars, variables
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded 
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.scl
        161 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> variables.sample
        Sample("1960Y1:2015Y1")

        >>> from iode.compute.estimation import EditAndEstimateEquations
        >>> estimation = EditAndEstimateEquations()
        >>> estimation.sample
        Sample("1960Y1:2015Y1")
        >>> estimation.sample = "1980Y1:1996Y1"
        >>> estimation.sample
        Sample("1980Y1:1996Y1")
        """
        sample = Sample.get_instance()
        sample._cython_instance = self._cython_instance.get_sample()
        if sample._cython_instance is None:
            warnings.warn("Estimation sample has not been defined yet.")
            return None
        return sample

    @sample.setter
    def sample(self, value: Union[str, slice, Tuple[Union[str, Period], Union[str, Period]], Sample]):
        if isinstance(value, str):
            if ':' not in value:
                raise ValueError(f"Missing colon ':' in the definition of the estimation sample. "
                                 f"Got value '{value}'")
            from_period, to_period = value.split(':')
        elif isinstance(value, slice):
            sample = self.sample
            from_period = sample.start if value.start is None else value.start
            to_period = sample.end if value.stop is None else value.stop
        elif isinstance(value, tuple) and len(value) == 2:
            from_period, to_period = value
        elif isinstance(value, Sample):
            from_period = value.start
            to_period = value.end
        
        if isinstance(from_period, Period):
            from_period = str(from_period)
        
        if isinstance(to_period, Period):
            to_period = str(to_period)
        
        self._cython_instance.set_sample(from_period, to_period)

    @property
    def block(self) -> str:
        r"""
        Update the 'block', 'kdb_eqs', 'kdb_scl', 'v_equations' and 'current_eq' C++ attributes 
        given the passed values for the 'block' and 'current_eq_name'. 
        
        What the C++ method set_block() does:

            - Step 1:  reset attributes 'block', 'v_equations' and 'current_eq'
            - Step 2:  generate a list of equations names from the passed argument 'block'
            - Step 3:  add the current equation name to the block if not referenced in it 
            - Step 4:  check each name if is valid. If there is an invalid name, throw an error
            - Step 5:  for each equation name from the block:
                       a. check if the equation is already present in the local database 'kdb_eqs':
                       - no  -> check if in the global database:
                                - yes -> copy equation from the global database to 'kdb_eqs'. 
                                - no  -> add a new equation with LEC '<name> := 0' to 'kdb_eqs'.
                       - yes -> does nothing.
                       b. add the equation name to the vector 'v_equations'.
            - Step 6: copy the list of equations names separated by ';' to the 'block' attribute
            - Step 7: move the equations iterator to the current equation or the first equation of the block

        Parameters
        ----------
        value: str or tuple(str, str)
            Represent both the list of equations to estimate ('block') and the name of the currently 
            displayed (edited) equation (in the GUI) ('current_eq_name').
            If the passed value is a simple string (not a tuple), 'current_eq_name' is set to an empty string.
        
        Notes
        -----
        Equivalent to the ODE_blk_check() function from o_est.c from the old GUI (developed by Jean-Marc Paul)

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations, scalars, variables
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded 
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.scl
        161 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded

        >>> from iode.compute.estimation import EditAndEstimateEquations
        >>> estimation = EditAndEstimateEquations("1980Y1", "1996Y1")

        >>> # ====== block = ACAF;DPUH ======
        >>> # set_block("new_block", "currently_displayed_equation")
        >>> #
        >>> estimation.block = "ACAF;DPUH", "ACAF"
        >>> estimation.block
        'ACAF;DPUH'
        >>> # ---- equations list ----
        >>> estimation.equations_list
        ['ACAF', 'DPUH']
        >>> equations_res = estimation.equations_db
        >>> equations_res.names
        ['ACAF', 'DPUH']
        >>> # ---- scalars list ----
        >>> estimation.update_scalars()
        >>> scalars_res = estimation.scalars_db
        >>> scalars_res.names
        ['acaf1', 'acaf2', 'acaf4', 'dpuh_1', 'dpuh_2']
        >>> # ---- current equation ----
        >>> current_eq = estimation.current_equation
        >>> current_eq.endogenous
        'ACAF'
        >>> current_eq.lec           # doctest: +NORMALIZE_WHITESPACE
        '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)'             
        >>> # ---- next equation ---- 
        >>> next_eq = estimation.next_equation
        >>> next_eq.endogenous
        'DPUH'
        >>> next_eq.lec           # doctest: +NORMALIZE_WHITESPACE
        'dln (DPUH/DPUHO):=dpuh_1+dpuh_2*dln(IHU/PI5)+dln PC'
        >>> # ---- go back to first eq ----
        >>> next_eq = estimation.next_equation
        >>> next_eq.endogenous
        'ACAF'
        >>> next_eq.lec           # doctest: +NORMALIZE_WHITESPACE
        '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)'

        >>> # ====== add a non existing equation to the block ======
        >>> # set_block("new_block", "currently_displayed_equation")
        >>> #
        >>> estimation.block = "ACAF;DPUH;TEST", "DPUH"
        >>> estimation.block
        'ACAF;DPUH;TEST'
        >>> # ---- equations list ----
        >>> estimation.equations_list
        ['ACAF', 'DPUH', 'TEST']
        >>> equations_res = estimation.equations_db
        >>> equations_res.names
        ['ACAF', 'DPUH', 'TEST']
        >>> # ---- scalars list ----
        >>> estimation.update_scalars()
        >>> scalars_res = estimation.scalars_db
        >>> scalars_res.names
        ['acaf1', 'acaf2', 'acaf4', 'dpuh_1', 'dpuh_2']
        >>> # ---- current equation ----
        >>> current_eq = estimation.current_equation
        >>> current_eq.endogenous
        'DPUH'
        >>> current_eq.lec           # doctest: +NORMALIZE_WHITESPACE
        'dln (DPUH/DPUHO):=dpuh_1+dpuh_2*dln(IHU/PI5)+dln PC'
        >>> # ---- next equation ----
        >>> next_eq = estimation.next_equation
        >>> next_eq.endogenous
        'TEST'
        >>> next_eq.lec           # doctest: +NORMALIZE_WHITESPACE
        'TEST := 0'
        >>> # ---- next equation ----
        >>> next_eq = estimation.next_equation
        >>> next_eq.endogenous
        'ACAF'
        >>> next_eq.lec           # doctest: +NORMALIZE_WHITESPACE
        '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)'

        >>> # ====== remove an equation from the block ======
        >>> # set_block("new_block", "currently_displayed_equation")
        >>> #
        >>> estimation.block = "ACAF;TEST", "ACAF"
        >>> estimation.block
        'ACAF;TEST'
        >>> # ---- equations list ----
        >>> estimation.equations_list
        ['ACAF', 'TEST']
        >>> equations_res = estimation.equations_db
        >>> equations_res.names
        ['ACAF', 'DPUH', 'TEST']
        >>> # ---- scalars list ----
        >>> estimation.update_scalars()
        >>> scalars_res = estimation.scalars_db
        >>> scalars_res.names
        ['acaf1', 'acaf2', 'acaf4']
        >>> # ---- current equation ----
        >>> current_eq = estimation.current_equation
        >>> current_eq.endogenous
        'ACAF'
        >>> current_eq.lec           # doctest: +NORMALIZE_WHITESPACE
        '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)'
        >>> # ---- next equation ----
        >>> next_eq = estimation.next_equation
        >>> next_eq.endogenous
        'TEST'
        >>> next_eq.lec           # doctest: +NORMALIZE_WHITESPACE
        'TEST := 0'

        >>> # ====== currently displayed equation not in the block -> add it to the block ======
        >>> # set_block("new_block", "currently_displayed_equation")
        >>> #
        >>> estimation.block = "ACAF;TEST", "DPUH"
        >>> estimation.block
        'ACAF;TEST;DPUH'
        >>> # ---- equations list ----
        >>> estimation.equations_list
        ['ACAF', 'TEST', 'DPUH']
        >>> equations_res = estimation.equations_db
        >>> equations_res.names
        ['ACAF', 'DPUH', 'TEST']
        >>> # ---- scalars list ----
        >>> estimation.update_scalars()
        >>> scalars_res = estimation.scalars_db
        >>> scalars_res.names
        ['acaf1', 'acaf2', 'acaf4', 'dpuh_1', 'dpuh_2']
        >>> # ---- current equation ----
        >>> current_eq = estimation.current_equation
        >>> current_eq.endogenous
        'DPUH'
        >>> current_eq.lec           # doctest: +NORMALIZE_WHITESPACE
        'dln (DPUH/DPUHO):=dpuh_1+dpuh_2*dln(IHU/PI5)+dln PC'

        >>> # ====== some scalars does not exist yet ======
        >>> #
        >>> del scalars["dpuh_1"]
        >>> del scalars["dpuh_2"]
        >>> estimation_new_coeffs = EditAndEstimateEquations("1980Y1", "1996Y1")
        >>> # set_block("new_block", "currently_displayed_equation")
        >>> estimation_new_coeffs.block = "ACAF;DPUH", "ACAF"
        >>> # ---- equations list ----
        >>> estimation_new_coeffs.equations_list
        ['ACAF', 'DPUH']
        >>> # ---- scalars list ----
        >>> estimation_new_coeffs.update_scalars()
        >>> scalars_res = estimation_new_coeffs.scalars_db
        >>> scalars_res.names
        ['acaf1', 'acaf2', 'acaf4', 'dpuh_1', 'dpuh_2']
        >>> scalars_res["dpuh_1"]
        Scalar(0.9, 1, na)
        >>> scalars_res["dpuh_2"]
        Scalar(0.9, 1, na)
        """
        return self._cython_instance.get_block()

    @block.setter
    def block(self, value: Union[str, List[str], Tuple[Union[str, List[str]], str]]):
        if not isinstance(value, tuple):
            _block = value
            current_eq_name = ""
        else:
            _block, current_eq_name = value

        if not isinstance(_block, str):
            _block = ';'.join(_block)
        
        self._cython_instance.set_block(_block, current_eq_name)

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
        >>> from iode import SAMPLE_DATA_DIR, equations, scalars, variables, EqMethod
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded 
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.scl
        161 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded

        >>> from iode.compute.estimation import EditAndEstimateEquations
        >>> estimation = EditAndEstimateEquations("1980Y1", "1996Y1")
        >>> estimation.block = "ACAF;DPUH", "ACAF"
        
        >>> estimation.method
        'LSQ'
        >>> estimation.method = EqMethod.ZELLNER
        >>> estimation.method
        'ZELLNER'
        >>> estimation.method = "MAX_LIKELIHOOD"
        >>> estimation.method
        'MAX_LIKELIHOOD'
        """
        return self._cython_instance.get_method()

    @method.setter
    def method(self, value: Union[EqMethod, str, int]):
        if not isinstance(value, (EqMethod, str, int)):
            raise TypeError("'value' must be of type EqMethod, str or int. "
                            f"Got value of type {type(value).__name__} instead.")
        if isinstance(value, str):
            value = value.upper()
            value = EqMethod[value]
        value = int(value)
        self._cython_instance.set_method(value)

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

        Notes
        -----
        Equivalent to the ODE_blk_instr() function of o_est.c from the old GUI (developed by Jean-Marc Paul)
        
        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations, scalars, variables
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded 
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.scl
        161 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded

        >>> from iode.compute.estimation import EditAndEstimateEquations
        >>> estimation = EditAndEstimateEquations("1980Y1", "1996Y1")
        >>> estimation.block = "ACAF;DPUH", "ACAF"
        >>> estimation.instruments
        ''

        >>> # one instrument
        >>> estimation.instruments = "one_instrument"
        >>> estimation.instruments
        'one_instrument'

        >>> # several instruments
        >>> estimation.instruments = "several;instruments;as;unique;string"
        >>> estimation.instruments
        ['several', 'instruments', 'as', 'unique', 'string']
        >>> estimation.instruments = ["several", "instruments", "as", "list"]
        >>> estimation.instruments
        ['several', 'instruments', 'as', 'list']
        """
        return self._cython_instance.get_instruments()

    @instruments.setter
    def instruments(self, value: Union[str, List[str]]):
        if not isinstance(value, str):
            value = ';'.join(value)
        self._cython_instance.set_instruments(value)

    def copy_eq_tests_values(self):
        r"""
        Extract and save the resulting values for the tests for each equations in the 
        local estimation Equations database.

        See :meth:`EditAndEstimateEquations.estimate` for the examples.

        Notes
        -----
        Equivalent to the ODE_blk_cur_atests() function from o_est.c from the old GUI (developed by Jean-Marc Paul)
        """
        self._cython_instance.copy_eq_tests_values()

    def update_scalars(self):
        r"""
        Update the local estimation Scalars database 'kdb_scl'.
        
        What the C++ method update_scalars() does:

            - Step 1: for each equation in the local estimation Equations database, get the list if corresponding scalars
            - Step 2: remove duplicated scalar names
            - Step 3: for each scalar name, check if it is already present in the local database 'kdb_scl':
	                  - no  -> check if in the global Scalars database
                               - yes -> copy scalars from the global database to 'kdb_scl'. 
	                           - no  -> add a new scalar with value = 0.0 and relax = 1.0 to 'kdb_scl'.
	                  - yes -> does nothing.
            - Step 4: remove the scalars associated with equations which are not in the present block to estimate
        
        See :meth:`EditAndEstimateEquations.estimate` for the examples.

        Notes
        -----
        Equivalent to the ODE_blk_coef() function from o_est.c from the old GUI (developed by Jean-Marc Paul)
        """
        self._cython_instance.update_scalars()

    @property
    def scalars_db(self) -> Scalars:
        r"""
        Local estimation Scalars database.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations, scalars, variables
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded 
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.scl
        161 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded

        >>> from iode.compute.estimation import EditAndEstimateEquations
        >>> estimation = EditAndEstimateEquations("1980Y1", "1996Y1")
        >>> estimation.block = "ACAF;DPUH", "ACAF"
        >>> estimation.block
        'ACAF;DPUH'

        >>> # ---- equations list ----
        >>> estimation.equations_list
        ['ACAF', 'DPUH']
        >>> equations_res = estimation.equations_db
        >>> equations_res.names
        ['ACAF', 'DPUH']
        
        >>> # ---- scalars list ----
        >>> estimation.update_scalars()
        >>> scalars_res = estimation.scalars_db
        >>> scalars_res.names
        ['acaf1', 'acaf2', 'acaf4', 'dpuh_1', 'dpuh_2']
        """
        scalars_db = Scalars.get_instance()
        scalars_db._cython_instance = self._cython_instance.get_scalars_db(scalars_db._cython_instance)
        if scalars_db._cython_instance is None:
            warnings.warn("Estimation Scalars database has not been defined yet.")
            return None
        return scalars_db

    @property
    def equations_list(self) -> List[str]:
        return self._cython_instance.get_equations_list()

    @property
    def equations_db(self) -> Equations:
        r"""
        Local estimation Equations database.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations, scalars, variables
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded 
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.scl
        161 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded

        >>> from iode.compute.estimation import EditAndEstimateEquations
        >>> estimation = EditAndEstimateEquations("1980Y1", "1996Y1")
        >>> estimation.block = "ACAF;DPUH", "ACAF"
        >>> estimation.block
        'ACAF;DPUH'

        >>> # ---- equations list ----
        >>> estimation.equations_list
        ['ACAF', 'DPUH']
        >>> equations_res = estimation.equations_db
        >>> equations_res.names
        ['ACAF', 'DPUH']
        """
        equations_db = Equations.get_instance()
        equations_db._cython_instance = self._cython_instance.get_equations_db(equations_db._cython_instance)
        if equations_db._cython_instance is None:
            warnings.warn("Estimation Equations database has not been defined yet.")
            return None
        return equations_db

    def update_current_equation(self, lec: str, comment: str):
        r"""
        Update the LEC and comment of the current equation
        
        Notes
        -----
        Equivalent to the ODE_blk_save_cur() function from o_est.c from the old GUI (developed by Jean-Marc Paul)
        """
        self._cython_instance.update_current_equation(lec, comment)

    @property
    def current_equation(self) -> Equation:
        r"""
        Return the currently displayed equation in the GUI.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations, scalars, variables
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded 
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.scl
        161 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded

        >>> from iode.compute.estimation import EditAndEstimateEquations
        >>> estimation = EditAndEstimateEquations("1980Y1", "1996Y1")
        >>> estimation.block = "ACAF;DPUH", "ACAF"
        >>> estimation.block
        'ACAF;DPUH'

        >>> # ---- current equation ----
        >>> current_eq = estimation.current_equation
        >>> current_eq.endogenous
        'ACAF'
        >>> current_eq.lec           # doctest: +NORMALIZE_WHITESPACE
        '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)' 

        >>> # ---- next equation ---- 
        >>> next_eq = estimation.next_equation
        >>> next_eq.endogenous
        'DPUH'
        >>> next_eq.lec           # doctest: +NORMALIZE_WHITESPACE
        'dln (DPUH/DPUHO):=dpuh_1+dpuh_2*dln(IHU/PI5)+dln PC'

        >>> # ---- go back to first eq ----
        >>> next_eq = estimation.next_equation
        >>> next_eq.endogenous
        'ACAF'
        >>> next_eq.lec           # doctest: +NORMALIZE_WHITESPACE
        '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)'
        """
        eq = Equation.get_instance()
        eq._cython_instance = self._cython_instance.get_current_equation(eq._cython_instance)
        if eq._cython_instance is None:
            warnings.warn("Couldn't get the current equation")
            return None
        return eq

    @property
    def next_equation(self) -> Equation:
        r"""
        Move the equations iterator to the next one of the block if any or to the  
        first equation if the iterator was pointing to the last equation of the block.
        Then return the newly displayed equation.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations, scalars, variables
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded 
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.scl
        161 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded

        >>> from iode.compute.estimation import EditAndEstimateEquations
        >>> estimation = EditAndEstimateEquations("1980Y1", "1996Y1")
        >>> estimation.block = "ACAF;DPUH", "ACAF"
        >>> estimation.block
        'ACAF;DPUH'

        >>> # ---- current equation ----
        >>> current_eq = estimation.current_equation
        >>> current_eq.endogenous
        'ACAF'
        >>> current_eq.lec           # doctest: +NORMALIZE_WHITESPACE
        '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)' 

        >>> # ---- next equation ---- 
        >>> next_eq = estimation.next_equation
        >>> next_eq.endogenous
        'DPUH'
        >>> next_eq.lec           # doctest: +NORMALIZE_WHITESPACE
        'dln (DPUH/DPUHO):=dpuh_1+dpuh_2*dln(IHU/PI5)+dln PC'
        
        >>> # ---- go back to first eq ----
        >>> next_eq = estimation.next_equation
        >>> next_eq.endogenous
        'ACAF'
        >>> next_eq.lec           # doctest: +NORMALIZE_WHITESPACE
        '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)'
        """
        eq = Equation.get_instance()
        eq._cython_instance = self._cython_instance.get_next_equation(eq._cython_instance)
        if eq._cython_instance is None:
            warnings.warn("Couldn't get the next equation")
            return None
        return eq

    @property
    def correlation_matrix(self) -> CorrelationMatrix:
        r"""
        Coefficients correlation matrix.
        
        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations, scalars, variables, Equation
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> equations.clear()
        >>> scalars.clear()

        >>> equations["ACAF"] = Equation("ACAF", "(ACAF/VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> equations["DPUH"] = Equation("DPUH", "dln(DPUH/DPUHO) := dpuh_1 + dpuh_2 * dln(IHU/PI5) + dln(PC)")

        >>> from iode.compute.estimation import EditAndEstimateEquations
        >>> estimation = EditAndEstimateEquations("1980Y1", "1996Y1")
        >>> estimation.block = "ACAF;DPUH", "ACAF"
        >>> success = estimation.estimate()                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Estimating : iteration 1 (||eps|| = 2.01246)
        <BLANKLINE>
        Estimating : iteration 2 (||eps|| = 7.77305e-08)
        <BLANKLINE>
        Solution reached after 2 iteration(s). Creating results file ...
        <BLANKLINE>

        >>> corr_matrix = estimation.correlation_matrix
        >>> corr_matrix.names
        ['acaf1', 'acaf2', 'acaf4', 'dpuh_1', 'dpuh_2']
        >>> corr_matrix.shape
        (5, 5)
        >>> corr_matrix         # doctest: +NORMALIZE_WHITESPACE
                   |      acaf1       acaf2       acaf4      dpuh_1      dpuh_2
        ------------------------------------------------------------------------
             acaf1 |          1   -0.935266    0.200167   0.0448324  -0.0372903
             acaf2 |  -0.935266           1   -0.300833  -0.0016619   0.0395814
             acaf4 |   0.200167   -0.300833           1  0.00037477  -0.00892588
            dpuh_1 |  0.0448324  -0.0016619  0.00037477           1  -0.0419869
            dpuh_2 | -0.0372903   0.0395814  -0.00892588  -0.0419869           1
        <BLANKLINE>
        """
        corr_matrix = CorrelationMatrix.get_instance()
        corr_matrix._cython_instance = self._cython_instance.get_correlation_matrix(corr_matrix._cython_instance)
        if corr_matrix._cython_instance is None:
            warnings.warn("Couldn't get the correlation matrix")
            return None
        return corr_matrix

    def get_observed_values(self, name: str) -> List[float]:
        r"""
        Observed values (saved as the Variable `_YOBS`).

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations, scalars, variables, Equation
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> equations.clear()
        >>> scalars.clear()

        >>> equations["ACAF"] = Equation("ACAF", "(ACAF/VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> equations["DPUH"] = Equation("DPUH", "dln(DPUH/DPUHO) := dpuh_1 + dpuh_2 * dln(IHU/PI5) + dln(PC)")

        >>> from iode.compute.estimation import EditAndEstimateEquations
        >>> estimation = EditAndEstimateEquations("1980Y1", "1996Y1")
        >>> estimation.block = "ACAF;DPUH", "ACAF"
        >>> success = estimation.estimate()                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Estimating : iteration 1 (||eps|| = 2.01246)
        <BLANKLINE>
        Estimating : iteration 2 (||eps|| = 7.77305e-08)
        <BLANKLINE>
        Solution reached after 2 iteration(s). Creating results file ...
        <BLANKLINE>

        >>> estimation.get_observed_values("ACAF")      # doctest: +ELLIPSIS
        [0.011412041862997465, 0.016028202180810566, ..., -0.002985052229901132, 0.00695696806902914]
        >>> estimation.get_fitted_values("ACAF")        # doctest: +ELLIPSIS
        [0.01256212379902523, 0.01249107519078205, ..., -0.002985052229901133, 0.004490336374473415]
        >>> estimation.get_residual_values("ACAF")      # doctest: +ELLIPSIS
        [-0.0011500819360277647, 0.0035371269900285156, ..., 8.673617379884035e-19, 0.002466631694555725]

        >>> estimation.get_observed_values("DPUH")      # doctest: +ELLIPSIS
        [0.06044527980207867, 0.08768972383253629, ..., 0.0424313077256923, 0.0064336499579307135]
        >>> estimation.get_fitted_values("DPUH")        # doctest: +ELLIPSIS
        [0.07361898875462985, 0.06423949088326213, ..., 0.028792670295126173, 0.032048802201331064]
        >>> estimation.get_residual_values("DPUH")      # doctest: +ELLIPSIS
        [-0.013173708952551183, 0.02345023294927416, ..., 0.013638637430566126, -0.02561515224340035]
        """
        observed_values = self._cython_instance.get_observed_values(name)
        if not observed_values:
            warnings.warn(f"No observed values found for '{name}'", RuntimeWarning)
            return []
        return observed_values

    def get_fitted_values(self, name: str) -> List[float]:
        r"""
        Fitted values for estimation (saved as the Variable `_CALC`)

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations, scalars, variables, Equation
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> equations.clear()
        >>> scalars.clear()

        >>> equations["ACAF"] = Equation("ACAF", "(ACAF/VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> equations["DPUH"] = Equation("DPUH", "dln(DPUH/DPUHO) := dpuh_1 + dpuh_2 * dln(IHU/PI5) + dln(PC)")

        >>> from iode.compute.estimation import EditAndEstimateEquations
        >>> estimation = EditAndEstimateEquations("1980Y1", "1996Y1")
        >>> estimation.block = "ACAF;DPUH", "ACAF"
        >>> success = estimation.estimate()                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Estimating : iteration 1 (||eps|| = 2.01246)
        <BLANKLINE>
        Estimating : iteration 2 (||eps|| = 7.77305e-08)
        <BLANKLINE>
        Solution reached after 2 iteration(s). Creating results file ...
        <BLANKLINE>

        >>> estimation.get_observed_values("ACAF")      # doctest: +ELLIPSIS
        [0.011412041862997465, 0.016028202180810566, ..., -0.002985052229901132, 0.00695696806902914]
        >>> estimation.get_fitted_values("ACAF")        # doctest: +ELLIPSIS
        [0.01256212379902523, 0.01249107519078205, ..., -0.002985052229901133, 0.004490336374473415]
        >>> estimation.get_residual_values("ACAF")      # doctest: +ELLIPSIS
        [-0.0011500819360277647, 0.0035371269900285156, ..., 8.673617379884035e-19, 0.002466631694555725]

        >>> estimation.get_observed_values("DPUH")      # doctest: +ELLIPSIS
        [0.06044527980207867, 0.08768972383253629, ..., 0.0424313077256923, 0.0064336499579307135]
        >>> estimation.get_fitted_values("DPUH")        # doctest: +ELLIPSIS
        [0.07361898875462985, 0.06423949088326213, ..., 0.028792670295126173, 0.032048802201331064]
        >>> estimation.get_residual_values("DPUH")      # doctest: +ELLIPSIS
        [-0.013173708952551183, 0.02345023294927416, ..., 0.013638637430566126, -0.02561515224340035]
        """
        fitted_values = self._cython_instance.get_fitted_values(name)
        if not fitted_values:
            warnings.warn(f"No fitted values found for '{name}'", RuntimeWarning)
            return []
        return fitted_values

    def get_residual_values(self, name: str) -> List[float]:
        r"""
        Difference between the observations and the estimate values (saved as the Variable `YRES`).

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations, scalars, variables, Equation
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> equations.clear()
        >>> scalars.clear()

        >>> equations["ACAF"] = Equation("ACAF", "(ACAF/VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> equations["DPUH"] = Equation("DPUH", "dln(DPUH/DPUHO) := dpuh_1 + dpuh_2 * dln(IHU/PI5) + dln(PC)")

        >>> from iode.compute.estimation import EditAndEstimateEquations
        >>> estimation = EditAndEstimateEquations("1980Y1", "1996Y1")
        >>> estimation.block = "ACAF;DPUH", "ACAF"
        >>> success = estimation.estimate()                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Estimating : iteration 1 (||eps|| = 2.01246)
        <BLANKLINE>
        Estimating : iteration 2 (||eps|| = 7.77305e-08)
        <BLANKLINE>
        Solution reached after 2 iteration(s). Creating results file ...
        <BLANKLINE>

        >>> estimation.get_observed_values("ACAF")      # doctest: +ELLIPSIS
        [0.011412041862997465, 0.016028202180810566, ..., -0.002985052229901132, 0.00695696806902914]
        >>> estimation.get_fitted_values("ACAF")        # doctest: +ELLIPSIS
        [0.01256212379902523, 0.01249107519078205, ..., -0.002985052229901133, 0.004490336374473415]
        >>> estimation.get_residual_values("ACAF")      # doctest: +ELLIPSIS
        [-0.0011500819360277647, 0.0035371269900285156, ..., 8.673617379884035e-19, 0.002466631694555725]

        >>> estimation.get_observed_values("DPUH")      # doctest: +ELLIPSIS
        [0.06044527980207867, 0.08768972383253629, ..., 0.0424313077256923, 0.0064336499579307135]
        >>> estimation.get_fitted_values("DPUH")        # doctest: +ELLIPSIS
        [0.07361898875462985, 0.06423949088326213, ..., 0.028792670295126173, 0.032048802201331064]
        >>> estimation.get_residual_values("DPUH")      # doctest: +ELLIPSIS
        [-0.013173708952551183, 0.02345023294927416, ..., 0.013638637430566126, -0.02561515224340035]
        """
        residual_values = self._cython_instance.get_residual_values(name)
        if not residual_values:
            warnings.warn(f"No residual values found for '{name}'", RuntimeWarning)
            return []
        return residual_values

    def estimate(self, quiet: bool=False) -> bool:
        r"""
        Estimate the current block of equations (which is not necessarily all the equations 
        in the local Equations database 'kdb_eqs').

        Parameters
        ----------
        quiet: bool, optional
            If True, the estimation will be silent (no printout). 
            Default is False.

        Returns
        -------
        True if the estimation was successful, False otherwise.

        Notes
        -----
        Equivalent to ODE_blk_est() from o_est.c from the old GUI (developed by Jean-Marc Paul).

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations, scalars, variables, Equation
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> equations.clear()
        >>> scalars.clear()

        >>> equations["ACAF"] = Equation("ACAF", "(ACAF/VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> equations["DPUH"] = Equation("DPUH", "dln(DPUH/DPUHO) := dpuh_1 + dpuh_2 * dln(IHU/PI5) + dln(PC)")

        >>> from iode.compute.estimation import EditAndEstimateEquations
        >>> estimation = EditAndEstimateEquations("1980Y1", "1996Y1")
        >>> estimation.block = "ACAF;DPUH", "ACAF"
        >>> estimation.block
        'ACAF;DPUH'

        >>> scalars_est = estimation.scalars_db
        >>> scalars_est.names
        []
        >>> estimation.update_scalars()
        >>> scalars_est.names
        ['acaf1', 'acaf2', 'acaf4', 'dpuh_1', 'dpuh_2']
        >>> scalars_est["acaf1"]
        Scalar(0.9, 1, na)
        >>> scalars_est["dpuh_1"]
        Scalar(0.9, 1, na)

        >>> equations_est = estimation.equations_db
        >>> equations_est["ACAF"]               # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ACAF',
                 lec = '(ACAF/VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)',
                 method = 'LSQ',
                 from_period = '1960Y1',
                 to_period = '2015Y1')
        >>> equations_est["DPUH"]               # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'DPUH',
                 lec = 'dln(DPUH/DPUHO) := dpuh_1 + dpuh_2 * dln(IHU/PI5) + dln(PC)',
                 method = 'LSQ',
                 from_period = '1960Y1',
                 to_period = '2015Y1')

        >>> estimation.is_done
        False
        >>> success = estimation.estimate()                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Estimating : iteration 1 (||eps|| = 2.01246)
        <BLANKLINE>
        Estimating : iteration 2 (||eps|| = 7.77305e-08)
        <BLANKLINE>
        Solution reached after 2 iteration(s). Creating results file ...
        <BLANKLINE>
        >>> estimation.is_done
        True

        >>> scalars_est["acaf1"]
        Scalar(0.0157705, 1, 0.00136079)
        >>> scalars_est["dpuh_1"]
        Scalar(0.0109855, 1, 0.00481857)

        >>> corr_matrix = estimation.correlation_matrix
        >>> corr_matrix.names
        ['acaf1', 'acaf2', 'acaf4', 'dpuh_1', 'dpuh_2']
        >>> corr_matrix.shape
        (5, 5)
        >>> corr_matrix         # doctest: +NORMALIZE_WHITESPACE
                   |      acaf1       acaf2       acaf4      dpuh_1      dpuh_2
        ------------------------------------------------------------------------
             acaf1 |          1   -0.935266    0.200167   0.0448324  -0.0372903
             acaf2 |  -0.935266           1   -0.300833  -0.0016619   0.0395814
             acaf4 |   0.200167   -0.300833           1  0.00037477  -0.00892588
            dpuh_1 |  0.0448324  -0.0016619  0.00037477           1  -0.0419869
            dpuh_2 | -0.0372903   0.0395814  -0.00892588  -0.0419869           1
        <BLANKLINE>

        >>> estimation.copy_eq_tests_values()
        >>> equations_est["ACAF"]             # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ACAF',
                lec = '(ACAF/VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)',
                method = 'LSQ',
                from_period = '1960Y1',
                to_period = '2015Y1',
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
                         stdev = 0.0042699})
        >>> equations_est["DPUH"]             # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'DPUH',
                lec = 'dln(DPUH/DPUHO) := dpuh_1 + dpuh_2 * dln(IHU/PI5) + dln(PC)',
                method = 'LSQ',
                from_period = '1960Y1',
                to_period = '2015Y1',
                tests = {corr = 0.126096,
                         dw = 3.15593,
                         fstat = 3.51611,
                         loglik = 43.5743,
                         meany = 0.0505132,
                         r2 = 0.189895,
                         r2adj = 0.135888,
                         ssres = 0.00591031,
                         stderr = 0.01985,
                         stderrp = 39.2966,
                         stdev = 0.0213538})

        >>> # global Equations and Scalars databases are left intact by the method estimate()
        >>> equations["ACAF"]                   # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ACAF',
                 lec = '(ACAF/VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)',
                 method = 'LSQ',
                 from_period = '1960Y1',
                 to_period = '2015Y1')
        >>> equations["DPUH"]                   # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'DPUH',
                 lec = 'dln(DPUH/DPUHO) := dpuh_1 + dpuh_2 * dln(IHU/PI5) + dln(PC)',
                 method = 'LSQ',
                 from_period = '1960Y1',
                 to_period = '2015Y1')
        >>> scalars["acaf1"]
        Scalar(0.9, 1, na)
        >>> scalars["dpuh_1"]
        Scalar(0.9, 1, na)

        >>> # save results in the global databases
        >>> new_eqs = estimation.save()
        >>> new_eqs
        []
        >>> equations["ACAF"]             # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ACAF',
                 lec = '(ACAF/VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)',
                 method = 'LSQ',
                 from_period = '1980Y1',
                 to_period = '1996Y1',
                 block = 'ACAF;DPUH',
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
                 date = ...)
        >>> equations["DPUH"]             # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Equation(endogenous = 'DPUH',
                 lec = 'dln(DPUH/DPUHO) := dpuh_1 + dpuh_2 * dln(IHU/PI5) + dln(PC)',
                 method = 'LSQ',
                 from_period = '1980Y1',
                 to_period = '1996Y1',
                 block = 'ACAF;DPUH',
                 tests = {corr = 0.126096,
                          dw = 3.15593,
                          fstat = 3.51611,
                          loglik = 43.5743,
                          meany = 0.0505132,
                          r2 = 0.189895,
                          r2adj = 0.135888,
                          ssres = 0.00591031,
                          stderr = 0.01985,
                          stderrp = 39.2966,
                          stdev = 0.0213538},
                 date = ...)
        >>> scalars["acaf1"]
        Scalar(0.0157705, 1, 0.00136079)
        >>> scalars["dpuh_1"]
        Scalar(0.0109855, 1, 0.00481857)
        """
        if quiet:
            suppress_msgs()

        try:
            self._cython_instance.estimate()
            success = True
        except Exception as e:
            warnings.warn(str(e), RuntimeWarning)
            success = False
        
        if quiet:
            reset_msgs()
        return success

    @property
    def is_done(self) -> bool:
        return self._cython_instance.get_is_done()

    def save(self, from_period: Union[str, List[str]]=None, to_period: Union[str, List[str]]=None) -> List[str]:
        r"""
        - copy the equations referenced in the vector 'v_equations' from the local Equations database 
          to the global Equations database,
        - if estimation -> create/update the scalars containing the results of the estimated equation(s),
        - merge the local Scalars database into the global Scalars database,
        - return the list of names of the new equations.

        See :meth:`EditAndEstimateEquations.estimate` for the examples.
        
        Parameters
        ----------
        from_period: str or Period, optional
            Starting period to copy if no estimation has been done
        to_period: str or Period, optional
            Ending period to copy if no estimation has been done
        
        Returns
        -------
        list(str)
            List of the names of the new equations

        Notes
        -----
        Equivalent to the ODE_blk_save() function from o_est.c from the old GUI (developed by Jean-Marc Paul).
        """
        if from_period is None:
            from_period = ''
        if isinstance(from_period, Period):
            from_period = str(from_period)

        if to_period is None:
            to_period = ''
        if isinstance(to_period, Period):
            to_period = str(to_period)
        
        return self._cython_instance.save(from_period, to_period)
