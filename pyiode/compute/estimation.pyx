# Import necessary types/classes
from libcpp.string cimport string
from libcpp.vector cimport vector

from pyiode.common cimport EnumIodeAdjustmentMethod
from pyiode.compute.estimation cimport dynamic_adjustment as cpp_dynamic_adjustment
from pyiode.compute.estimation cimport dickey_fuller_test as cpp_dickey_fuller_test
from pyiode.iode_database.cpp_api_database cimport KDBScalars as CKDBScalars


def dynamic_adjustment(method: int, eqs: str, c1: str = "c1", c2: str = "c2") -> str:
    r"""
    Transform a LEC equation to add a dynamic adjustment.
    
    Two methods can be used. Given the equation :math:`LHS = RHS`, we have:
    
        - **Partial Adjustment** (0): :math:`d(LHS) = c1 * (RHS - (LHS)[-1])`
        - **Error Correction Model** (1): :math:`d(LHS) = c1 * d(RHS) + c2 * (RHS -LHS)[-1]`
    
    Parameters
    ----------
    method: int    
        Method used for the dynamic adjustment. 
        Possible values are 0 (ADJUSTMENT_PARTIAL) or 1 (ADJUSTMENT_ERROR_CORRECTION_METHOD).
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
    >>> from iode import SAMPLE_DATA_DIR, equations
    >>> from iode import dynamic_adjustment, ADJUSTMENT_PARTIAL, ADJUSTMENT_ERROR_CORRECTION_METHOD
    >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
    >>> lec = equations["ACAF"].lec
    >>> lec
    '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)'
    
    Partial Adjustment

    >>> partial_adjust_eq = dynamic_adjustment(ADJUSTMENT_PARTIAL, lec)
    >>> partial_adjust_eq
    'd((ACAF/VAF[-1])) := c1 * (acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995) -((ACAF/VAF[-1]))[-1])'

    Error Correction Model

    >>> error_corr_adjust_eq = dynamic_adjustment(ADJUSTMENT_ERROR_CORRECTION_METHOD, lec)
    >>> error_corr_adjust_eq
    'd((ACAF/VAF[-1])) := c1 * d(acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)) + c2 * (acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995) -(ACAF/VAF[-1]))[-1]'
    """
    if method not in [ADJUSTMENT_PARTIAL, ADJUSTMENT_ERROR_CORRECTION_METHOD]:
        raise ValueError(f"'method': Possible values are {ADJUSTMENT_PARTIAL} or {ADJUSTMENT_ERROR_CORRECTION_METHOD}. "
                         f"Got value {method} instead.")
    return cpp_dynamic_adjustment(<EnumIodeAdjustmentMethod>method, eqs.encode(), c1.encode(), c2.encode()).decode()


def dickey_fuller_test(lec: str, drift: bool, trend: bool, order: int) -> Scalars:
    """
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
    >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

    >>> # dickey_fuller_test("ACAF", True, True, 3) estimates the equation:
    >>> #     d(ACAF) := df_ * ACAF[-1] +            
    >>> #     df_d +            (DRIFT)          
    >>> #     df_t * t +        (TREND)            
    >>> #     df1 * d(ACAF[-1]) + df2*d(ACAF[-2]) + df3*d(ACAF[-3])  (ORDER)
    >>> df_scalars = dickey_fuller_test("ACAF", True, True, 3)
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
    cdef CKDBScalars* df_scalars
    df_scalars = cpp_dickey_fuller_test(lec.encode(), <bint>drift, <bint>trend, order)
    return Scalars._from_ptr(df_scalars)
