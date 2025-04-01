import sys
from copy import copy
from typing import Union, Tuple, List, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

import numpy as np
from iode.util import iode_number_to_str
from iode.iode_cython import Scalar as CythonScalar


class Scalar(CythonScalar):
    r"""
    IODE Scalars are dimensionless variables defined by a single value. 
    They can be the estimated coefficients in an equation or constant values over time (relax = 0).

    Attributes
    ----------
    value: float
        Value of the scalar.
    relax: float
        Relaxation parameter used in the context of equations estimation.
        For example, setting the relaxation parameter to 0 will 'lock' the coefficient 
        during the estimation process. 
    std: float
        Standard deviation. Calculated during the estimation process.

    Parameters
    ----------
    value: float
        value of the scalar. Defaults to 0.9.
    relax: float
        relax value of the scalar. The value must be between 0.0 and 1.0.
        Defaults to 1.0

    Examples
    --------
    >>> import numpy as np
    >>> from iode import Scalar
    >>> # default relax
    >>> scalar = Scalar(0.9)
    >>> scalar
    Scalar(0.9, 1, na)
    >>> # specific value and relax
    >>> scalar = Scalar(0.9, 0.8)
    >>> scalar
    Scalar(0.9, 0.8, na)
    >>> # Python nan are converted to IODE NA
    >>> scalar = Scalar(np.nan)
    >>> scalar
    Scalar(na, 1, na)
    >>> # Python inf are not accepted
    >>> scalar = Scalar(np.inf)
    Traceback (most recent call last):
    ...
    ValueError: Expected 'value' to be a finite number
    >>> # relax must be between 0.0 and 1.0
    >>> scalar = Scalar(0.9, 1.1)
    Traceback (most recent call last):
    ...
    ValueError: Expected 'relax' value between 0.0 and 1.0
    """
    def __init__(self, value: float, relax: float=1.0) -> Self:
        CythonScalar.__init__(self, value, relax)

    @classmethod
    def _new_instance(cls) -> Self:
        instance = cls.__new__(cls)
        return instance

    @property
    def value(self) -> float:
        r"""
        >>> import numpy as np
        >>> from iode import Scalar
        >>> scalar = Scalar(0.9, 0.8)
        >>> scalar
        Scalar(0.9, 0.8, na)
        >>> scalar.value
        0.9
        >>> scalar.value = 0.8
        >>> scalar.value
        0.8
        >>> # Python nan are converted to IODE NA internally
        >>> # 'scalar.value = np.nan' is equivalent to 
        >>> # 'scalar.value = NA' 
        >>> scalar.value = np.nan
        >>> scalar.value
        nan
        >>> # The numpy isnan can be used to test if  
        >>> # an IODE scalar value is NA or not
        >>> np.isnan(scalar.value)   
        True
        >>> # Python inf are not accepted
        >>> scalar.value = np.inf
        Traceback (most recent call last):
        ...
        ValueError: Expected 'value' to be a finite number
        """
        return CythonScalar.get_value(self)

    @value.setter
    def value(self, val: float):
        CythonScalar.set_value(self, val)

    @property
    def relax(self) -> float:
        return CythonScalar.get_relax(self)

    @relax.setter
    def relax(self, value: float):
        CythonScalar.set_relax(self, value)

    @property
    def std(self) -> float:
        return CythonScalar.get_std(self)

    def _set_std(self, value: float):
        CythonScalar._set_std(self, value)

    def _as_tuple(self) -> Tuple[float, float, float]:
        r"""
        Export the current scalar as a tuple

        Examples
        --------
        >>> from iode import Scalar
        >>> scalar = Scalar(0.9, 0.8)
        >>> scalar._as_tuple()
        (0.9, 0.8, nan)
        """
        return CythonScalar._as_tuple(self)

    def copy(self) -> Self:
        r"""
        Return a copy of the current Scalar.

        Examples
        --------
        >>> from iode import Scalar, scalars, SAMPLE_DATA_DIR
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.scl
        161 objects loaded 
        >>> scalars["acaf1"]
        Scalar(0.0157684, 1, 0.00136871)
        >>> copied_scl = scalars["acaf1"].copy()
        >>> copied_scl
        Scalar(0.0157684, 1, 0.00136871)
        """
        return copy(self)

    def __eq__(self, other: Self) -> bool:
        return CythonScalar.__eq__(self, other)

    def __copy__(self) -> Self:
        r"""
        Return a copy of the current Scalar.

        Examples
        --------
        >>> from copy import copy
        >>> from iode import Scalar, scalars, SAMPLE_DATA_DIR
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.scl
        161 objects loaded 
        >>> scalars["acaf1"]
        Scalar(0.0157684, 1, 0.00136871)
        >>> copied_scl = copy(scalars["acaf1"])
        >>> copied_scl
        Scalar(0.0157684, 1, 0.00136871)
        """
        copied_scalar = Scalar(self.value, self.relax)
        copied_scalar._set_std(self.std)
        return copied_scalar

    def __str__(self) -> str:
        return f"Scalar({iode_number_to_str(self.value)}, {iode_number_to_str(self.relax)}, {iode_number_to_str(self.std)})"

    def __repr__(self) -> str:
        return str(self)

    def __hash__(self) -> int:
        return CythonScalar.__hash__(self)


