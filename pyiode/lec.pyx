# distutils: language = c++

import numpy as np

from libcpp.vector cimport vector
from lec cimport execute_lec as cpp_execute_lec


def execute_lec(lec: str, period: Union[str, int, Period]=None) -> Union[float, List[float]]:
    r"""
    Compute a LEC formula using the current Variables and Scalars databases.
    The formula may be evaluate at a specific period or on the whole sample 
    (no value for *period* is passed).
    
    Parameters
    ----------
    lec: str
        LEC expression to be evaluated.
    period: str or int or Period, optional
        Period at which the LEC formula is evaluated.
        If the passed value is of type int, then it represents 
        the position of the period in the current sample.
        If the passed value is None, empty string or -1, the LEC expression 
        is evaluated over the whole sample.
        Default to None.
    
    Returns
    -------
    float or list(float)

    Examples
    --------
    >>> from iode import SAMPLE_DATA_DIR
    >>> from iode import execute_lec, equations, scalars, variables
    >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")        # doctest: +ELLIPSIS
    Loading .../fun.eqs
    274 objects loaded
    >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")          # doctest: +ELLIPSIS
    Loading .../fun.scl
    161 objects loaded
    >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS
    Loading .../fun.var
    394 objects loaded

    >>> equations["ACAF"].lec       # doctest: +NORMALIZE_WHITESPACE
    '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)'
    >>> variables["ACAF", "2000Y1"]
    10.046610792200543
    >>> lec = "(acaf1 + acaf2 * GOSF[-1] + acaf4*(TIME=1995)) * VAF[-1]"
    >>> # pass a period value as string
    >>> execute_lec(lec, "2000Y1")
    10.046610792200543
    >>> # pass a period position in the sample
    >>> t = variables.sample.index("2000Y1")
    >>> t
    40
    >>> execute_lec(lec, t)
    10.046610792200543
    >>> # compute the LEC formula over the whole sample
    >>> execute_lec(lec)        # doctest: +ELLIPSIS
    [nan, 4.2884154594335815, 4.532163174288473, ..., -83.34062511080091, -96.41041982848331]
    >>> variables["ACAF"]       # doctest: +ELLIPSIS
    [nan, nan, nan, ..., -83.34062511080091, -96.41041982848331]
    """
    cdef double c_value
    cdef vector[double] c_values

    # evaluate LEC expression over the whole sample
    if period is None:
        c_values = cpp_execute_lec(lec.encode())
        return [value if IODE_IS_A_NUMBER(value) else np.nan for value in c_values]

    if isinstance(period, int):
        if period >= 0:
            c_value = cpp_execute_lec(<string>lec.encode(), <int>period)
            return c_value if IODE_IS_A_NUMBER(c_value) else np.nan
        # evaluate LEC expression over the whole sample
        else:
            c_values = cpp_execute_lec(lec.encode())
            return [value if IODE_IS_A_NUMBER(value) else np.nan for value in c_values]

    if isinstance(period, Period):
        period = str(period)

    if isinstance(period, str):
        if len(period):
            c_value = cpp_execute_lec(<string>lec.encode(), <string>period.encode())
            return c_value if IODE_IS_A_NUMBER(c_value) else np.nan
        # evaluate LEC expression over the whole sample
        else:
           c_values = cpp_execute_lec(lec.encode())
           return [value if IODE_IS_A_NUMBER(value) else np.nan for value in c_values]
