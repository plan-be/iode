import warnings
from typing import Union, List

from iode import execute_lec

def exec_lec(lec: str, t: int = -1) -> Union[float, List[float]]:
    warnings.warn("exec_lec() is deprecated. Please use the new syntax:\n"
                  "execute_lec(lec, period)", DeprecationWarning, stacklevel=2)
    return execute_lec(lec, t)
