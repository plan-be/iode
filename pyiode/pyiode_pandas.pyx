# IODE EXTENSION FOR PYTHON
# =========================
#
#    @header4iode
# 
# Interface between IODE ws and pandas dataframes
# -----------------------------------------------
#  df_to_ws(df_input: pd.DataFrame, time_axis_name: str = 'time')   | Copies DataFrame df_input into IODE KV_WS.    
#  ws_to_df(vars_pattern: str = '*', vars_axis_name: str = 'vars', 
#           time_axis_name: str = 'time', 
#           time_as_floats: bool = False) -> pd.DataFrame           | Creates a DataFrame from the current KV_WS content
    
import pandas as pd


def df_to_ws(df_input: pd.DataFrame, time_axis_name: str = 'time'):
    '''
    Copies DataFrame df_input into IODE KV_WS.
    Sort axes in alphabetical order before copying to KV_WS.
    '''
    if la is None:
        raise RuntimeError("larray library not found")

    la_input = la.from_frame(df_input, sort_rows = True, sort_columns = True)
    return larray_to_ws(la_input, time_axis_name)
   

def ws_to_df(vars_pattern: str = '*', 
                 vars_axis_name: str = 'vars',
                 time_axis_name: str = 'time', 
                 time_as_floats: bool = False) -> pd.DataFrame:
    '''
    Creates a DataFrame from the current KV_WS content with time axis labels in the IODE syntax ['2000Q2',...].
    If time_as_floats is True, the time axis labels are doubles [2000.25,...] instead of strings.
    '''
    if la is None:
        raise RuntimeError("larray library not found")

    la_output = ws_to_larray(vars_pattern = vars_pattern, 
                             vars_axis_name = vars_axis_name,     
                             time_axis_name = time_axis_name, 
                             time_as_floats = time_as_floats)
    return la_output.to_frame()
                