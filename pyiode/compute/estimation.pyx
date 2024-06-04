# Import necessary types/classes
from libcpp.string cimport string
from libcpp.vector cimport vector

from pyiode.common cimport EnumIodeAdjustmentMethod
from pyiode.compute.estimation cimport dynamic_adjustment as cpp_dynamic_adjustment
from pyiode.compute.estimation cimport dickey_fuller_test as cpp_dickey_fuller_test
from pyiode.compute.estimation cimport CCorrelationMatrix
from pyiode.iode_database.cpp_api_database cimport KDBScalars as CKDBScalars


def dynamic_adjustment(method: int, eqs: str, c1: str = "c1", c2: str = "c2") -> str:
    r"""
    Transform a LEC equation to add a dynamic adjustment.
    
    Two methods can be used. Given the equation :math:`LHS = RHS`, we have:
    
        - **Partial Adjustment** (0): :math:`d(LHS) = c1 * (RHS - (LHS)[-1])`
        - **Error Correction Model** (1): :math:`d(LHS) = c1 * d(RHS) + c2 * (RHS -LHS)[-1]`
    
    Parameters
    ----------
    method: int    
        Method used for the dynamic adjustment. 
        Possible values are 0 (ADJUSTMENT_PARTIAL) or 1 (ADJUSTMENT_ERROR_CORRECTION_METHOD).
    eqs: str
        LEC equation to dynamically adjust.
    c1: str, optional        
        Name of the first coefficient.
        Default to "c1".
    c2: str, optional    
        Name of the second coefficient. 
        Not used with the *Partial Adjustment* method.
        Default to "c2".

    Returns
    -------
    str
        Dynamically adjusted equation.

    Examples
    --------
    >>> from iode import SAMPLE_DATA_DIR, equations
    >>> from iode import dynamic_adjustment, ADJUSTMENT_PARTIAL, ADJUSTMENT_ERROR_CORRECTION_METHOD
    >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
    >>> lec = equations["ACAF"].lec
    >>> lec
    '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)'
    
    Partial Adjustment

    >>> partial_adjust_eq = dynamic_adjustment(ADJUSTMENT_PARTIAL, lec)
    >>> partial_adjust_eq
    'd((ACAF/VAF[-1])) := c1 * (acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995) -((ACAF/VAF[-1]))[-1])'

    Error Correction Model

    >>> error_corr_adjust_eq = dynamic_adjustment(ADJUSTMENT_ERROR_CORRECTION_METHOD, lec)
    >>> error_corr_adjust_eq
    'd((ACAF/VAF[-1])) := c1 * d(acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)) + c2 * (acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995) -(ACAF/VAF[-1]))[-1]'
    """
    if method not in [ADJUSTMENT_PARTIAL, ADJUSTMENT_ERROR_CORRECTION_METHOD]:
        raise ValueError(f"'method': Possible values are {ADJUSTMENT_PARTIAL} or {ADJUSTMENT_ERROR_CORRECTION_METHOD}. "
                         f"Got value {method} instead.")
    return cpp_dynamic_adjustment(<EnumIodeAdjustmentMethod>method, eqs.encode(), c1.encode(), c2.encode()).decode()


def dickey_fuller_test(lec: str, drift: bool, trend: bool, order: int) -> Scalars:
    """
    Dickey-Fuller tests.

    Tests are saved in scalars whose name is composed of the prefix *df_* and the name of the first series 
    appearing in the formula to be tested. For example, the test for the formula :math:`d(A0GR+A0GF)` is `df_a0gr`.

    Parameters
    ----------
    lec: str
        LEC form to be tested.
    drift: bool
        Whether or not the formula to be estimated must incorporate a constant term. 
    trend: bool
        Whether or not the formula to be estimated should incorporate a trend term.
    order: int 
        Order of the polynomial to be estimated to obtain the tests.
    
    Returns
    -------
    Scalars
        Scalars database containing the results of the Dickey-Fuller tests.

    Examples
    --------
    >>> from iode import SAMPLE_DATA_DIR, scalars, variables
    >>> from iode import dickey_fuller_test
    >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

    >>> # dickey_fuller_test("ACAF", True, True, 3) estimates the equation:
    >>> #     d(ACAF) := df_ * ACAF[-1] +            
    >>> #     df_d +            (DRIFT)          
    >>> #     df_t * t +        (TREND)            
    >>> #     df1 * d(ACAF[-1]) + df2*d(ACAF[-2]) + df3*d(ACAF[-3])  (ORDER)
    >>> df_scalars = dickey_fuller_test("ACAF", True, True, 3)
    >>> df_scalars.get_names("df*")
    ['df1', 'df2', 'df3', 'df_', 'df_d', 'df_t']
    >>> # note: the function dickey_fuller_test() returns a separated Scalars database.
    >>> #       The global database scalars is left unchaged
    >>> scalars.get_names("df*")
    []
    >>> # order 0
    >>> df_scalars["df_"]
    Scalar(0.0132523, 0.0845155, 0.0845155)
    >>> # drift
    >>> df_scalars["df_d"]
    Scalar(13.0806, 1, 6.78675)
    >>> # trend
    >>> df_scalars["df_t"]
    Scalar(-0.492697, 0.187978, 0.187978)
    >>> # order 1
    >>> df_scalars["df1"]
    Scalar(-0.120123, 0.180991, 0.180991)
    >>> # order 2
    >>> df_scalars["df2"]
    Scalar(-0.476959, 0.154505, 0.154505)
    >>> # order 3
    >>> df_scalars["df3"]
    Scalar(-0.211047, 0.170708, 0.170708)
    """
    # Note: cpp_dickey_fuller_test allocates a new CKDBScalars* pointer
    cdef CKDBScalars* df_scalars
    df_scalars = cpp_dickey_fuller_test(lec.encode(), <bint>drift, <bint>trend, order)
    return Scalars._from_ptr(df_scalars, <bint>True)


# CorrelationMatrix wrapper class
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#create-cython-wrapper-class 
cdef class CorrelationMatrix:
    cdef bint ptr_owner
    cdef CCorrelationMatrix* c_correlation_matrix

    def __init__(self):
        # Prevent accidental instantiation from normal Python code
        # since we cannot pass a struct pointer into a Python constructor.
        raise TypeError("This class cannot be instantiated directly.")

    def __cinit__(self):
        self.ptr_owner = False
        self.c_correlation_matrix = NULL

    def __dealloc__(self):
        if self.ptr_owner and self.c_correlation_matrix is not NULL:
            del self.c_correlation_matrix
            self.c_correlation_matrix = NULL

    # see https://cython.readthedocs.io/en/stable/src/userguide/extension_types.html#instantiation-from-existing-c-c-pointers 
    @staticmethod
    cdef CorrelationMatrix from_ptr(CCorrelationMatrix* ptr, bint owner=False):
        """
        Factory function to create CorrelationMatrix objects from a given CCorrelationMatrix pointer.
        """
        # Fast call to __new__() that bypasses the __init__() constructor.
        cdef CorrelationMatrix wrapper = CorrelationMatrix.__new__(CorrelationMatrix)
        wrapper.c_correlation_matrix = ptr
        wrapper.ptr_owner = owner
        return wrapper

    @property
    def shape(self) -> Tuple[int, int]:
        if self.c_correlation_matrix is not NULL:
            return self.c_correlation_matrix.nb_coeffs, self.c_correlation_matrix.nb_coeffs
        else:
            warnings.warn("The correlation matrix has not been defined yet")
            return 0, 0

    @property
    def names(self) -> List[str]:
        if self.c_correlation_matrix is not NULL:
            return [self.c_correlation_matrix.get_name(i).decode() for i in range(len(self))]
        else:
            warnings.warn("The correlation matrix has not been defined yet")
            return None

    def name(self, index: int) -> str:
        if self.c_correlation_matrix is not NULL:
            if index < 0 or index >= self.c_correlation_matrix.nb_coeffs:
                raise ValueError(f"'index': Value must be in range [0, {len(self)}]. "
                                f"Got index value {index} instead")
            return self.c_correlation_matrix.get_name(index).decode()
        else:
            warnings.warn("The correlation matrix has not been defined yet")
            return None

    def __len__(self) -> int:
        if self.c_correlation_matrix is not NULL:
            return self.c_correlation_matrix.nb_coeffs
        else:
            warnings.warn("The correlation matrix has not been defined yet")
            return 0

    def __getitem__(self, key: Tuple[int, int]) -> float:
        if self.c_correlation_matrix is not NULL:
            row, column = key
            if row < 0 or row >= self.c_correlation_matrix.nb_coeffs:
                raise ValueError(f"CorrelationMatrx[row, column]: Value for rwo and column must be in range "
                                f"[0, {len(self)}]. Got row value {row} instead")
            if column < 0 or column >= self.c_correlation_matrix.nb_coeffs:
                raise ValueError(f"CorrelationMatrx[row, column]: Value for rwo and column must be in range "
                                f"[0, {len(self)}]. Got column value {column} instead")
            return self.c_correlation_matrix.get_value(row, column)
        else:
            warnings.warn("The correlation matrix has not been defined yet")
            return None

    def __setitem__(self, key, value):
        warnings.warn("The values of the correlation matrix are read-only.")

    def __delitem__(self, key):
        warnings.warn("The values of the correlation matrix are read-only.")

    def __str__(self):
        if self.c_correlation_matrix is not NULL:
            coeffs_names = self.names
            max_column_length = max(10, max(len(name) for name in coeffs_names) + 1)

            # horizontal header
            top_left = ' ' * max_column_length
            coeffs_names = [name.rjust(max_column_length) for name in coeffs_names]
            s = top_left + ' | ' + '  '.join(coeffs_names) + '\n'
            s += '-' * len(s) + '\n'
            # rows
            for row, name in enumerate(coeffs_names):
                values = [f"{self.c_correlation_matrix.get_value(row, column):>10.6g}".rjust(max_column_length) 
                          for column in range(len(self))]
                s += name.ljust(max_column_length) + ' | ' + '  '.join(values) + '\n'
            return s
        else:
            warnings.warn("The correlation matrix has not been defined yet")
            return None

    def __repr__(self):
        return str(self)
