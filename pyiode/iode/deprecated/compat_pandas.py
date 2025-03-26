import warnings
import pandas as pd
from typing import Any

from iode import variables



def df_to_ws(df_input: pd.DataFrame, time_axis_name: str = 'time'):
    warnings.warn("df_to_ws() is deprecated. " + 
        "Please use the new syntax: variables.from_frame(df)",DeprecationWarning, stacklevel=2)
    variables.from_frame(df_input)

def ws_to_df(vars_pattern: str = '*', vars_axis_name: str = 'vars', time_axis_name: str = 'time', 
             time_as_floats: bool = False) -> pd.DataFrame:
    warnings.warn("ws_to_df() is deprecated. " + 
        "Please use the new syntax: df = variables.to_frame()",DeprecationWarning, stacklevel=2)
    df = variables.to_frame(vars_axis_name, time_axis_name, time_as_floats)
    return df
