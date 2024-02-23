# distutils: language = c++

from typing import Union, Tuple, List, Optional

from pyiode.objects.scalar cimport Scalar as CScalar
from pyiode.objects.scalar cimport hash_value


# Scalar wrapper class
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#create-cython-wrapper-class 
cdef class Scalar:

    cdef CScalar c_scalar

    def __init__(self, value: float = 0.9, relax: float = 1.0, std: float = nan) -> Scalar:
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
        Scalar(0.9, 1, nan)
        >>> # default relax
        >>> scalar = Scalar(0.5)
        >>> scalar
        Scalar(0.5, 1, nan)
        >>> # specific value and relax
        >>> scalar = Scalar(0.5, 0.8)
        >>> scalar
        Scalar(0.5, 0.8, nan)
        """
        if not isinstance(value, float):
            raise TypeError("Expected value of type 'float' for the argument 'value'.\n" +
                "Got value of type '" + type(value).__name__ + "'")
        if not isinstance(relax, float):
            raise TypeError("Expected value of type 'float' for the argument 'relax'.\n" +
                "Got value of type '" + type(relax).__name__ + "'")  
        if relax < 0.0 or relax > 1.0:
            raise ValueError("Expected 'relax' value between 0.0 and 1.0")
        if not isinstance(std, float):
            raise TypeError("Expected value of type 'float' for the argument 'std'.\n" +
                "Got value of type '" + type(std).__name__ + "'")     
        self.c_scalar = CScalar(value, relax, std)

    # Attributes access

    @property
    def value(self) -> float:
        return self.c_scalar.val

    @value.setter 
    def value(self, val: float):
        if not isinstance(val, float):
            raise TypeError("Expected value of type 'float'.\nGot value of type '" + type(val).__name__ + "'")
        self.c_scalar.val = val
    
    @property
    def relax(self) -> float:
        return self.c_scalar.relax

    @relax.setter
    def relax(self, relax_: float):
        if not isinstance(relax_, float):
            raise TypeError("Expected value of type 'float'.\nGot value of type '" + type(relax_).__name__ + "'")
        if relax_ < 0.0 or relax_ > 1.0:
            raise ValueError("Expected relax value between 0.0 and 1.0")
        self.c_scalar.relax = relax_

    @property
    def std(self) -> float:
        return self.c_scalar.std

    # Special methods

    def __eq__(self, other: Scalar) -> bool:
        if not isinstance(other, Scalar):
            raise TypeError(f"Expected argument of type 'Scalar'.\nGot argument of type '{type(other).__name__}'")
        return self.c_scalar == other.c_scalar

    def __str__(self) -> str:
        return self.c_scalar.to_string().decode("utf-8")

    def __repr__(self) -> str:
        return self.c_scalar.to_string().decode("utf-8")

    def __hash__(self) -> int:
        return <int>hash_value(self.c_scalar)
