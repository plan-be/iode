# distutils: language = c++

import numpy as np
import pandas as pd

from libcpp.vector cimport vector
from pyiode.time.sample cimport CSample
from lec cimport execute_lec as cpp_execute_lec
from pyiode.iode_database.cpp_api_database cimport Variables as cpp_global_variables


def execute_lec(lec: str, period: Union[str, int, Period]=None) -> Union[float, List[float], pd.Series]:
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
    >>> execute_lec(lec)        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    time
    1960Y1          NaN
    1961Y1     4.288415
    1962Y1     4.532163
    1963Y1     4.951813
    ...
    2012Y1   -55.559290
    2013Y1   -68.894654
    2014Y1   -83.340625
    2015Y1   -96.410420
    dtype: float64
    >>> variables["ACAF"]       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Workspace: Variables
    nb variables: 1
    filename: ...fun.var
    description: Modèle fun - Simulation 1
    sample: 1960Y1:2015Y1
    mode: LEVEL
    <BLANKLINE>
    name        1960Y1  1961Y1  1962Y1  ...  2013Y1  2014Y1  2015Y1
    ACAF            na      na      na  ...  -68.89  -83.34  -96.41
    <BLANKLINE>
    """
    cdef double c_value
    cdef vector[double] c_values

    if isinstance(period, int) and period >= 0:
        c_value = cpp_execute_lec(<string>lec.encode(), <int>period)
        return c_value if IODE_IS_A_NUMBER(c_value) else np.nan

    if isinstance(period, Period):
        period = str(period)

    if isinstance(period, str) and len(period):
        c_value = cpp_execute_lec(<string>lec.encode(), <string>period.encode())
        return c_value if IODE_IS_A_NUMBER(c_value) else np.nan
    
    # evaluate LEC expression over the whole sample
    c_values = cpp_execute_lec(lec.encode())
    data = [value if IODE_IS_A_NUMBER(value) else np.nan for value in c_values]
    sample = Sample._from_ptr(cpp_global_variables.get_sample(), <bint>False)
    periods = sample.get_period_list()
    series = pd.Series(data, index=periods)
    series.index.name = "time"
    return series
