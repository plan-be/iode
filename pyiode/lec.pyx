from libcpp.vector cimport vector
from pyiode.lec cimport execute_lec as cpp_execute_lec

import numpy as np


def cython_execute_lec(lec: str, period: Union[str, int]=None) -> Union[float, List[float]]:
    cdef double c_value
    cdef vector[double] c_values

    if isinstance(period, int):
        c_value = cpp_execute_lec(<string>lec.encode(), <int>period)
        return c_value if IODE_IS_A_NUMBER(c_value) else np.nan
    elif isinstance(period, str):
        c_value = cpp_execute_lec(<string>lec.encode(), <string>period.encode())
        return c_value if IODE_IS_A_NUMBER(c_value) else np.nan
    else:
        # evaluate LEC expression over the whole sample
        c_values = cpp_execute_lec(lec.encode())
        return [value if IODE_IS_A_NUMBER(value) else np.nan for value in c_values]
