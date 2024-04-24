import warnings
import numpy as np
from typing import Union, List, Tuple, Optional

from iode import Variables


def ws_sample_set(from_period: str, to_period: str) -> Optional[Tuple[str, str]]:
    warnings.warn("ws_sample_set() is deprecated. " + 
        "Please use the new syntax:\nvar_db = Variables()\nvar_db.sample = 'from_period:to_period'",DeprecationWarning, stacklevel=2)
    var_db = Variables()
    var_db.sample = f"{from_period}:{to_period}"

def ws_sample_get() -> Optional[Tuple[str, str]]: 
    warnings.warn("ws_sample_get() is deprecated. " + 
        "Please use the new syntax:\nvar_db = Variables()\nvar_db.sample",DeprecationWarning, stacklevel=2)
    var_db = Variables()
    return var_db.sample

def ws_sample_nb_periods() -> int:
    warnings.warn("ws_sample_nb_periods() is deprecated. " + 
        "Please use the new syntax:\nvar_db = Variables()\nvar_db.nb_periods",DeprecationWarning, stacklevel=2)
    var_db = Variables()
    return var_db.nb_periods

def ws_sample_to_string() -> Optional[str]:
    warnings.warn("ws_sample_to_string() is deprecated. " + 
        "Please use the new syntax:\nvar_db = Variables()\nvar_db.sample",DeprecationWarning, stacklevel=2)
    var_db = Variables()
    return var_db.sample

def ws_sample_to_list(from_period: str = "", to_period: str = "", as_floats: bool = False) -> Optional[Union[List[str], np.ndarray]]:
    warnings.warn("ws_sample_to_list() is deprecated. " + 
        "Please use the new syntax:\nvar_db = Variables()\nvar_db.periods",DeprecationWarning, stacklevel=2)
    var_db = Variables()
    return var_db.periods
