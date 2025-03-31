from copy import copy
from typing import Union, Tuple, List, Optional
import numpy as np

from cython.operator cimport dereference
from pyiode.util cimport IODE_IS_A_NUMBER
from pyiode.objects.scalar cimport CScalar
from pyiode.objects.scalar cimport hash_value as hash_value_scl


cdef class Scalar:
    cdef bint ptr_owner
    cdef CScalar* c_scalar

    def __cinit__(self):
        self.ptr_owner = False
        self.c_scalar = NULL

    def __init__(self, value: float, relax: float=1.0) -> Scalar:
        if relax < 0.0 or relax > 1.0:
            raise ValueError("Expected 'relax' value between 0.0 and 1.0")   
        if np.isinf(value):
            raise ValueError("Expected 'value' to be a finite number")
        if np.isnan(value):
            value = NA
        self.ptr_owner = <bint>True 
        self.c_scalar = new CScalar(value, relax)

    def __dealloc__(self):
        if self.ptr_owner and self.c_scalar is not NULL:
            del self.c_scalar
            self.c_scalar = NULL

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

    def get_value(self) -> float:
        return self.c_scalar.val if IODE_IS_A_NUMBER(self.c_scalar.val) else np.nan

    def set_value(self, val: float):
        if np.isinf(val):
            raise ValueError("Expected 'value' to be a finite number")
        if np.isnan(val):
            val = NA
        self.c_scalar.val = val
        self.c_scalar.std = NA

    def get_relax(self) -> float:
        return self.c_scalar.relax

    def set_relax(self, value: float):
        if value < 0.0 or value > 1.0:
            raise ValueError("Expected relax value between 0.0 and 1.0")
        self.c_scalar.relax = value
        self.c_scalar.std = NA

    def get_std(self) -> float:
        return self.c_scalar.std if IODE_IS_A_NUMBER(self.c_scalar.std) else np.nan

    def _set_std(self, value: float):
        self.c_scalar.std = value

    def _as_tuple(self) -> Tuple[float, float, float]:
        value = self.c_scalar.val if IODE_IS_A_NUMBER(self.c_scalar.val) else np.nan
        relax = self.c_scalar.relax
        std = self.c_scalar.std if IODE_IS_A_NUMBER(self.c_scalar.std) else np.nan
        return value, relax, std

    def __eq__(self, other: Scalar) -> bool:
        return self.c_scalar == other.c_scalar

    def __hash__(self) -> int:
        return <int>hash_value_scl(dereference(self.c_scalar))
