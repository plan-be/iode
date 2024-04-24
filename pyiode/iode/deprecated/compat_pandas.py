import warnings
from typing import Any

from iode import Variables

try:
    import pandas as pd
    DataFrame = pd.DataFrame
except ImportError:
    pd = None
    DataFrame = Any


def df_to_ws(df_input: DataFrame, time_axis_name: str = 'time'):
    warnings.warn("df_to_ws() is deprecated. " + 
        "Please use the new syntax:\nvar_db = Variables()\nvar_db.from_frame(df)",DeprecationWarning, stacklevel=2)
    var_db = Variables()
    var_db.from_frame(df_input)

def ws_to_df(vars_pattern: str = '*', vars_axis_name: str = 'vars', time_axis_name: str = 'time', 
             time_as_floats: bool = False) -> DataFrame:
    warnings.warn("ws_to_df() is deprecated. " + 
        "Please use the new syntax:\nvar_db = Variables()\ndf = var_db.to_frame()",DeprecationWarning, stacklevel=2)
    var_db = Variables()
    df = var_db.to_frame(vars_axis_name, time_axis_name, time_as_floats)
    return df
