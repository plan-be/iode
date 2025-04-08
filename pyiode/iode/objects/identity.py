import sys
from copy import copy
from typing import Union, Tuple, List, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

from iode.iode_cython import Identity as CythonIdentity


class Identity:
    r"""
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
    def __init__(self, lec: str) -> Self:
        self._cython_instance = CythonIdentity(lec)

    @classmethod
    def get_instance(cls) -> Self:
        instance = cls.__new__(cls)
        instance._cython_instance = CythonIdentity.__new__(CythonIdentity)
        return instance

    @property
    def coefficients(self) -> List[str]:
        r"""
        Return the list of coefficients present in the current identity.

        Returns
        -------
        list(str)

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Identity, scalars, variables
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.scl
        161 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> idt = Identity("1 - exp((gamma2 + gamma3 * ln(W/ZJ)[-1] + gamma4 * ln(WMIN/ZJ)) / gamma_)")
        >>> idt
        Identity('1 - exp((gamma2 + gamma3 * ln(W/ZJ)[-1] + gamma4 * ln(WMIN/ZJ)) / gamma_)')
        >>> idt.coefficients
        ['gamma2', 'gamma3', 'gamma4', 'gamma_']
        """
        return self._cython_instance.get_coefficients()

    @property
    def variables(self) -> List[str]:
        r"""
        Return the list of variables present in the identity.

        Returns
        -------
        list(str)

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Identity, scalars, variables
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.scl
        161 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> idt = Identity("1 - exp((gamma2 + gamma3 * ln(W/ZJ)[-1] + gamma4 * ln(WMIN/ZJ)) / gamma_)")
        >>> idt
        Identity('1 - exp((gamma2 + gamma3 * ln(W/ZJ)[-1] + gamma4 * ln(WMIN/ZJ)) / gamma_)')
        >>> idt.variables
        ['W', 'ZJ', 'WMIN']
        """
        return self._cython_instance.get_variables()

    def copy(self) -> Self:
        r"""
        Return a copy of the current Identity.

        Examples
        --------
        >>> from iode import Identity
        >>> idt = Identity("FLG/VBBP")
        >>> idt
        Identity('FLG/VBBP')
        >>> copied_idt = idt.copy()
        >>> copied_idt
        Identity('FLG/VBBP')
        """
        return copy(self)

    def __eq__(self, other: Self) -> bool:
        return self._cython_instance.equal(other)

    def __copy__(self) -> Self:
        r"""
        Return a copy of the current Identity.

        Examples
        --------
        >>> from copy import copy
        >>> from iode import Identity
        >>> idt = Identity("FLG/VBBP")
        >>> idt
        Identity('FLG/VBBP')
        >>> copied_idt = copy(idt)
        >>> copied_idt
        Identity('FLG/VBBP')
        """
        return Identity(str(self))

    def __str__(self) -> str:
        return self._cython_instance._str_()

    def __repr__(self) -> str:
        return f"Identity('{str(self)}')"

    def __hash__(self) -> int:
        return hash(str(self))


