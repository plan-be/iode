from copy import copy
from typing import Union, Tuple, List, Optional
import numpy as np

from libcpp.memory cimport shared_ptr, make_shared
from cython.operator cimport dereference

from pyiode.util cimport IODE_IS_A_NUMBER
from pyiode.objects.scalar cimport CScalar
from pyiode.objects.scalar cimport hash_value as hash_value_scl


cdef class Scalar:
    cdef shared_ptr[CScalar] scl_ptr
    cdef CScalar* c_scalar

    def __cinit__(self):
        self.c_scalar = NULL

    def __init__(self, value: float, relax: float) -> Scalar:
        self.scl_ptr = make_shared[CScalar](value, relax)
        self.c_scalar = self.scl_ptr.get()

    def __dealloc__(self):
        self.scl_ptr.reset()
        self.c_scalar = NULL

    cdef update_ptr(self, shared_ptr[CScalar] scl_ptr):
        self.scl_ptr = scl_ptr
        self.c_scalar = self.scl_ptr.get()

    @staticmethod
    cdef Scalar _from_ptr(shared_ptr[CScalar] scl_ptr):
        """
        Factory function to create Scalar objects from a given CScalar pointer.
        """
        # Fast call to __new__() that bypasses the __init__() constructor.
        cdef Scalar wrapper = Scalar.__new__(Scalar)
        if scl_ptr.get() == NULL:
            return None
        wrapper.update_ptr(scl_ptr)
        return wrapper

    def get_value(self) -> float:
        return self.c_scalar.value if IODE_IS_A_NUMBER(self.c_scalar.value) else np.nan

    def set_value(self, value: float):
        self.c_scalar.value = value
        self.c_scalar.std = NA

    def get_relax(self) -> float:
        return self.c_scalar.relax

    def set_relax(self, value: float):
        self.c_scalar.relax = value
        self.c_scalar.std = NA

    def get_std(self) -> float:
        return self.c_scalar.std if IODE_IS_A_NUMBER(self.c_scalar.std) else np.nan

    def _set_std(self, value: float):
        self.c_scalar.std = value

    def _as_tuple(self) -> Tuple[float, float, float]:
        value = self.c_scalar.value if IODE_IS_A_NUMBER(self.c_scalar.value) else np.nan
        relax = self.c_scalar.relax
        std = self.c_scalar.std if IODE_IS_A_NUMBER(self.c_scalar.std) else np.nan
        return value, relax, std

    def equal(self, other: Scalar) -> bool:
        return self.c_scalar == other.c_scalar

    def __hash__(self) -> int:
        return <int>hash_value_scl(dereference(self.c_scalar))
