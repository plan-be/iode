from copy import copy
from typing import Union, Tuple, List, Optional

from libcpp.memory cimport shared_ptr, make_shared
from cython.operator cimport dereference

from pyiode.objects.identity cimport CIdentity


cdef class Identity:
    cdef shared_ptr[CIdentity] idt_ptr
    cdef CIdentity* c_identity

    def __cinit__(self):
        self.c_identity = NULL

    def __init__(self, lec: str) -> Identity:
        self.idt_ptr = make_shared[CIdentity](lec.encode("utf-8"))
        self.c_identity = self.idt_ptr.get()

    def __dealloc__(self):
        self.idt_ptr.reset()
        self.c_identity = NULL

    cdef update_ptr(self, shared_ptr[CIdentity] idt_ptr):
        self.idt_ptr = idt_ptr
        self.c_identity = self.idt_ptr.get()

    @staticmethod
    cdef Identity _from_ptr(shared_ptr[CIdentity] idt_ptr):
        """
        Factory function to create Identity objects from a given CIdentity pointer.
        """
        # Fast call to __new__() that bypasses the __init__() constructor.
        cdef Identity wrapper = Identity.__new__(Identity)
        if idt_ptr.get() == NULL:
            return None
        wrapper.update_ptr(idt_ptr)
        return wrapper

    def get_lec(self) -> str:
        return self.c_identity.get_lec().decode()

    def set_lec(self, value: str):
        value = value.strip()
        self.c_identity.set_lec(value.encode())

    def get_coefficients(self) -> List[str]:
        return [coeff.decode() for coeff in self.c_identity.get_coefficients_list(<bint>False)]

    def get_variables(self) -> List[str]:
        return [var.decode() for var in self.c_identity.get_variables_list(<bint>False)]

    def equal(self, other: Identity) -> bool:
        return dereference(self.c_identity) == dereference(other.c_identity)

    def _str_(self) -> str:
        return self.c_identity.get_lec().decode()
