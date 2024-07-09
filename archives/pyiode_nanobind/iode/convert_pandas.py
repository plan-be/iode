from .common import TEST_DATA_DIR
from .iode_python_api import (__set_vars_from_ndarray_contiguous, get_var_as_ndarray, nan,
                              ws_content_var, ws_sample_to_list, ws_sample_to_float_list)

import warnings
from typing import Any

try:
    import numpy as np
    import pandas as pd
    DataFrame = pd.DataFrame
except ImportError:
    pd = None
    DataFrame = Any


def df_to_ws(df: DataFrame):
    """
    Merge input DataFrame into IODE Variables.
    Assuming the Variables values are represented by the rows of 
    the input DataFrame and the columns represent valid IODE periods.

    Parameters
    ----------
    df: pandas DataFrame
        Input DataFrame containing the Variables to be copied in the 
        current IODE workspace.
    
    Examples
    --------
    >>> import iode
    >>> import pandas as pd

    >>> data = {"1991Y1": [0, 0.5, 1], "1992Y1": [2, 2.5, 3], "1993Y1": [4, 4.5, 5]}
    >>> index = ["AA", "BB", "CC"]
    >>> df = pd.DataFrame(data=data, index=index)

    >>> iode.df_to_ws(df)
    >>> len(iode.ws_content_var("*"))
    3
    >>> AA = iode.get_var("AA")
    >>> AA[1]
    2.0
    """
    if not pd:
        raise Exception("df_to_ws() is not available because the pandas package is not installed")

    # if index = multiindex -> flatten
    if df.index.nlevels > 1:
        df.index = ['_'.join(col) for col in df.index.values]

    # no multiindex accepted for the columns
    if df.columns.nlevels > 1:
        raise RuntimeError("The columns of the DataFrame must not be a mutliindex")

    var_names = df.index.to_list()
    periods = df.columns.to_list()
    # na_value=nan converts Numpy NaN to IODE NaN values
    data = df.to_numpy(dtype=float, copy=False, na_value=nan)
    data = np.ascontiguousarray(data)

    __set_vars_from_ndarray_contiguous(var_names, periods[0], periods[-1], data)


def ws_to_df(pattern: str = "*", 
             index_name: str = "vars", 
             columns_name: str = "time", 
             periods_as_floats: bool = False, 
             iode_to_numpy_nan=True) -> DataFrame:
    """
    create a pandas DataFrame from the current IODE Variables workspace.
    
    Parameters
    ----------
    pattern: str
        Pattern used to select which Variables to export to the output DataFrame.
        Defaults to all Variables.
    index_name: str
        Name of the DataFrame index.
        Defaults to 'vars'.
    columns_name: str
        Name of the DataFrame columns.
        Defaults to 'time'.
    periods_as_floats: bool
        Whether or not to convert IODE periods into float.
        Defaults to False.
    iode_to_numpy_nan: bool
        Whether or not to convert IODE NaN to Numpy NaN values.
        Defaults to True.

    Examples
    --------
    >>> import iode
    >>> import pandas as pd

    >>> filename = str(iode.TEST_DATA_DIR / "fun.var")
    >>> iode.ws_load_var(filename)
    394
    >>> df = iode.ws_to_df()

    >>> # check index and columns
    >>> var_names = df.index.to_list()
    >>> var_names[:5]
    ['ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']
    >>> var_names[-5:]
    ['ZJ', 'ZKF', 'ZKFO', 'ZX', 'ZZF_']

    >>> periods = df.columns.to_list()
    >>> periods[:5]
    ['1960Y1', '1961Y1', '1962Y1', '1963Y1', '1964Y1']
    >>> periods[-5:]
    ['2011Y1', '2012Y1', '2013Y1', '2014Y1', '2015Y1']

    >>> # Check values
    >>> print(round(df.loc["ACAF", "1990Y1"], 3))
    23.771
    """
    if not pd:
        raise Exception("df_to_ws() is not available because the pandas package is not installed")
    
    var_names = ws_content_var(pattern)
    if not len(var_names):
        warnings.warn("Variables workspace is empty")
        return None

    periods = ws_sample_to_float_list() if periods_as_floats else ws_sample_to_list()
    if not len(periods):
        warnings.warn("Sample not set")
        return None

    data = np.empty((len(var_names), len(periods)), dtype=float, order='C')
    for i, var_name in enumerate(var_names):
        data[i] = get_var_as_ndarray(var_name)

    if iode_to_numpy_nan:
        data = np.where(data < nan * (1. + 1e-8), np.nan, data)

    df = pd.DataFrame(data, index=var_names, columns=periods)
    df.index.name = index_name
    df.columns.name = columns_name
    return df
