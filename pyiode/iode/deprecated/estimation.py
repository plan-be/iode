import warnings
from typing import Union, List 

from iode import equations


def eqs_estimate(eq_list: Union[str, List[str]], afrom: str, ato: str):
    warnings.warn("eqs_estimate() is deprecated. Please use the new syntax:\n" + 
                  "equations.estimate(from_period, to_period, list_eqs)", DeprecationWarning, stacklevel=2)
    equations.estimate(afrom, ato, eq_list)
