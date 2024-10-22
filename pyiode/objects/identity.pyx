# distutils: language = c++

from typing import Union, Tuple, List, Optional

from cython.operator cimport dereference
from pyiode.objects.identity cimport CIdentity


# Identity wrapper class
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#create-cython-wrapper-class 
cdef class Identity:
    """
    IODE Identities are formulas (LEC expressions) used to construct series based on other variables. 
    The name of an identity is the one of the series that will be built.

    Parameters
    ----------
    lec: str
        formula (LEC expression) used to construct a series.

    Examples
    --------
    >>> from iode import Identity
    >>> idt = Identity("FLG/VBBP")
    >>> idt
    Identity('FLG/VBBP')
    """
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

    # see https://cython.readthedocs.io/en/stable/src/userguide/extension_types.html#instantiation-from-existing-c-c-pointers 
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

    @property
    def coefficients(self) -> List[str]:
        """
        Return the list of coefficients present in the current identity.

        Returns
        -------
        list(str)

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Identity, scalars, variables
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> idt = Identity("1 - exp((gamma2 + gamma3 * ln(W/ZJ)[-1] + gamma4 * ln(WMIN/ZJ)) / gamma_)")
        >>> idt
        Identity('1 - exp((gamma2 + gamma3 * ln(W/ZJ)[-1] + gamma4 * ln(WMIN/ZJ)) / gamma_)')
        >>> idt.coefficients
        ['gamma2', 'gamma3', 'gamma4', 'gamma_']
        """
        return [coeff.decode() for coeff in self.c_identity.get_coefficients_list(<bint>False)]

    @property
    def variables(self) -> List[str]:
        """
        Return the list of variables present in the identity.

        Returns
        -------
        list(str)

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Identity, scalars, variables
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> idt = Identity("1 - exp((gamma2 + gamma3 * ln(W/ZJ)[-1] + gamma4 * ln(WMIN/ZJ)) / gamma_)")
        >>> idt
        Identity('1 - exp((gamma2 + gamma3 * ln(W/ZJ)[-1] + gamma4 * ln(WMIN/ZJ)) / gamma_)')
        >>> idt.variables
        ['W', 'ZJ', 'WMIN']
        """
        return [var.decode() for var in self.c_identity.get_variables_list(<bint>False)]

    # Special methods

    def __eq__(self, other: Identity) -> bool:
        return self.c_identity == other.c_identity

    def __copy__ (self) -> Identity:
        """
        Return a copy of the current Identity.

        Examples
        --------
        >>> import copy
        >>> from iode import Identity
        >>> idt = Identity("FLG/VBBP")
        >>> idt
        Identity('FLG/VBBP')
        >>> copied_idt = copy.copy(idt)
        >>> copied_idt
        Identity('FLG/VBBP')
        """
        return Identity(str(self))

    def __str__(self) -> str:
        return self.c_identity.get_lec().decode("utf-8")

    def __repr__(self) -> str:
        return f"Identity('{str(self)}')"

    def __hash__(self) -> int:
        return hash(str(self))
