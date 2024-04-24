import warnings
from typing import Any

from iode import Variables

try:
    import larray as la
    Array = la.Array
    Axis = la.Axis
except ImportError:
    la = None
    Array = Any
    Axis = Any


def larray_to_ws(la_input: Array, time_axis_name: str = 'time', sep: str = "_"):
    warnings.warn("larray_to_ws() is deprecated. " + 
        "Please use the new syntax:\nvar_db = Variables()\nvar_db.from_array(array)",DeprecationWarning, stacklevel=2)
    var_db = Variables()
    var_db.from_array(la_input, time_axis_name, sep)

def ws_to_larray(vars_pattern: str = '*', 
                 vars_axis_name: str = 'vars',     
                 time_axis_name: str = 'time', 
                 split_axis_names = '', 
                 regex = None, 
                 split_sep = '', 
                 time_as_floats: bool = False) -> Array:
    warnings.warn("ws_to_larray() is deprecated. " + 
        "Please use the new syntax:\nvar_db = Variables()\narray = var_db.to_array()",DeprecationWarning, stacklevel=2)
    var_db = Variables()
    array = var_db.to_array(vars_axis_name, time_axis_name, time_as_floats)
    return array

def ws_load_var_to_larray(filename: str, 
                          vars_pattern = '*', 
                          vars_axis_name = 'vars', 
                          time_axis_name = 'time', 
                          split_axis_names = '', 
                          regex = None, 
                          split_sep = '') -> Array:
    warnings.warn("ws_load_var_to_larray() is deprecated. " + 
        "Please use the new syntax:\nvar_db = Variables(filename)\n" + 
        "array = var_db.to_array()",DeprecationWarning, stacklevel=2)
    var_db = Variables(filename)
    array = var_db.to_array(vars_axis_name, time_axis_name)
    return array

def ws_sample_to_larray_axis(axis_name: str = 'time', 
                             from_period:str = '', to_period: str = '', 
                             as_floats: bool = False) -> Axis:
    warnings.warn("ws_sample_to_larray_axis() is deprecated. " + 
        "Please use the new syntax:\nperiods_list = var_db.periods\n" + 
        "time_axis = la.Axis(name='time', labels=nperiods_list)",DeprecationWarning, stacklevel=2)
    var_db = Variables()
    periods_list = var_db.periods
    return Axis(name='time', labels=periods_list)
