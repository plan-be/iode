from copy import copy
from typing import Union, Tuple, List, Optional

from cython.operator cimport dereference
from pyiode.objects.identity cimport CIdentity


cdef class Identity:
    cdef bint ptr_owner
    cdef CIdentity* c_identity

    def __cinit__(self):
        self.ptr_owner = False
        self.c_identity = NULL

    def __init__(self, lec: str) -> Identity:
        self.ptr_owner = <bint>True 
        self.c_identity = new CIdentity(lec.encode("utf-8"))

    def __dealloc__(self):
        if self.ptr_owner and self.c_identity is not NULL:
            del self.c_identity
            self.c_identity = NULL

    @staticmethod
    cdef Identity _from_ptr(CIdentity* ptr, bint owner=False):
        """
        Factory function to create Identity objects from a given CIdentity pointer.
        """
        # Fast call to __new__() that bypasses the __init__() constructor.
        cdef Identity wrapper = Identity.__new__(Identity)
        wrapper.c_identity = ptr
        wrapper.ptr_owner = owner
        return wrapper

    # for debug purpose only
    def is_pointer_null(self) -> bool:
        return self.c_identity is NULL

    # for debug purpose only
    def is_own_owner(self) -> bool:
        return self.ptr_owner

    def get_coefficients(self) -> List[str]:
        return [coeff.decode() for coeff in self.c_identity.get_coefficients_list(<bint>False)]

    def get_variables(self) -> List[str]:
        return [var.decode() for var in self.c_identity.get_variables_list(<bint>False)]

    def equal(self, other: Identity) -> bool:
        return self.c_identity == other.c_identity

    def _str_(self) -> str:
        return self.c_identity.get_lec().decode("utf-8")
