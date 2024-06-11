# distutils: language = c++

from typing import Union, Tuple, List, Optional

from pyiode.util cimport L_ISAN
from pyiode.objects.scalar cimport CScalar
from pyiode.objects.scalar cimport hash_value as hash_value_scl


# Scalar wrapper class
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#create-cython-wrapper-class 
cdef class Scalar:
    """
    An IODE scalar represents the parameters of an equation that can be estimated.
    It is represented by 'value' value, a 'relax' value and a standard deviation ('std') value.

    Attributes
    ----------
    value: float
    relax: float
    std: float

    Parameters
    ----------
    value: float
        value of the scalar. Defaults to 0.9.
    relax: float
        relax value of the scalar. The value must be between 0.0 and 1.0.
        Defaults to 1.0

    Examples
    --------
    >>> from iode import Scalar
    >>> # default scalar
    >>> scalar = Scalar()
    >>> scalar
    Scalar(0.9, 1, na)
    >>> # default relax
    >>> scalar = Scalar(0.5)
    >>> scalar
    Scalar(0.5, 1, na)
    >>> # specific value and relax
    >>> scalar = Scalar(0.5, 0.8)
    >>> scalar
    Scalar(0.5, 0.8, na)
    """

    cdef CScalar c_scalar

    def __cinit__(self, value: float = 0.9, relax: float = 1.0) -> Scalar:
        if not isinstance(value, float):
            raise TypeError("Expected value of type 'float' for the argument 'value'.\n" +
                "Got value of type '" + type(value).__name__ + "'")
        if not isinstance(relax, float):
            raise TypeError("Expected value of type 'float' for the argument 'relax'.\n" +
                "Got value of type '" + type(relax).__name__ + "'")  
        if relax < 0.0 or relax > 1.0:
            raise ValueError("Expected 'relax' value between 0.0 and 1.0")    
        self.c_scalar = CScalar(value, relax)

    # Attributes access

    @property
    def value(self) -> float:
        return self.c_scalar.val

    @value.setter 
    def value(self, val: float):
        self.c_scalar.val = val
    
    @property
    def relax(self) -> float:
        return self.c_scalar.relax

    @relax.setter
    def relax(self, relax_: float):
        if relax_ < 0.0 or relax_ > 1.0:
            raise ValueError("Expected relax value between 0.0 and 1.0")
        self.c_scalar.relax = relax_

    @property
    def std(self) -> float:
        return self.c_scalar.std

    # Special methods

    def __eq__(self, other: Scalar) -> bool:
        return self.c_scalar == other.c_scalar

    def __str__(self) -> str:
        return f"Scalar({_iode_number_to_str(self.value)}, {_iode_number_to_str(self.relax)}, {_iode_number_to_str(self.std)})"

    def __repr__(self) -> str:
        return str(self)

    def __hash__(self) -> int:
        return <int>hash_value_scl(self.c_scalar)
