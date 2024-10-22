# distutils: language = c++

from typing import Union, Tuple, List, Optional

from pyiode.util cimport IODE_IS_A_NUMBER
from cython.operator cimport dereference
from pyiode.objects.scalar cimport CScalar
from pyiode.objects.scalar cimport hash_value as hash_value_scl


# Scalar wrapper class
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#create-cython-wrapper-class 
cdef class Scalar:
    """
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
    >>> from iode import Scalar
    >>> # default relax
    >>> scalar = Scalar(0.9)
    >>> scalar
    Scalar(0.9, 1, na)
    >>> # specific value and relax
    >>> scalar = Scalar(0.9, 0.8)
    >>> scalar
    Scalar(0.9, 0.8, na)
    """
    cdef bint ptr_owner
    cdef CScalar* c_scalar

    def __cinit__(self):
        self.ptr_owner = False
        self.c_scalar = NULL

    def __init__(self, value: float, relax: float = 1.0) -> Scalar:  
        if relax < 0.0 or relax > 1.0:
            raise ValueError("Expected 'relax' value between 0.0 and 1.0")   
        self.ptr_owner = <bint>True 
        self.c_scalar = new CScalar(value, relax)

    def __dealloc__(self):
        if self.ptr_owner and self.c_scalar is not NULL:
            del self.c_scalar
            self.c_scalar = NULL

    # see https://cython.readthedocs.io/en/stable/src/userguide/extension_types.html#instantiation-from-existing-c-c-pointers 
    @staticmethod
    cdef Scalar _from_ptr(CScalar* ptr, bint owner=False):
        """
        Factory function to create Scalar objects from a given CScalar pointer.
        """
        # Fast call to __new__() that bypasses the __init__() constructor.
        cdef Scalar wrapper = Scalar.__new__(Scalar)
        wrapper.c_scalar = ptr
        wrapper.ptr_owner = owner
        return wrapper

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

    # misc

    def _as_tuple(self) -> Tuple[float, float, float]:
        """
        Export the current scalar as a tuple

        Examples
        --------
        >>> from iode import Scalar
        >>> scalar = Scalar(0.9, 0.8)
        >>> scalar._as_tuple()
        (0.9, 0.8, -2e+37)
        """
        return self.c_scalar.val, self.c_scalar.relax, self.c_scalar.std

    # Special methods

    def __eq__(self, other: Scalar) -> bool:
        return self.c_scalar == other.c_scalar

    def __copy__ (self) -> Scalar:
        """
        Return a copy of the current Scalar.

        Examples
        --------
        >>> import copy
        >>> from iode import Scalar, scalars, SAMPLE_DATA_DIR
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
        >>> scalars["acaf1"]
        Scalar(0.0157684, 1, 0.00136871)
        >>> copied_scl = copy.copy(scalars["acaf1"])
        >>> copied_scl
        Scalar(0.0157684, 1, 0.00136871)
        """
        copied_scl = Scalar(self.value, self.relax)
        copied_scl.c_scalar.std = self.c_scalar.std
        return copied_scl

    def __str__(self) -> str:
        return f"Scalar({_iode_number_to_str(self.value)}, {_iode_number_to_str(self.relax)}, {_iode_number_to_str(self.std)})"

    def __repr__(self) -> str:
        return str(self)

    def __hash__(self) -> int:
        return <int>hash_value_scl(dereference(self.c_scalar))
