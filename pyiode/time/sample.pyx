# IODE EXTENSION FOR PYTHON
# =========================
#
#    @header4iode
# 
# Functions to manage ws sample
# 
# Note: this file cannot be compiled independently: it must be "included" in iode.pyx       

# distutils: language = c++

from cython.operator cimport dereference

from typing import Union, Tuple, List, Optional, Any
from time.sample cimport CSample

try:
    import larray as la
    Axis = la.Axis
except ImportError:
    la = None
    Axis = Any


# Sample wrapper class
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#create-cython-wrapper-class 
cdef class Sample:
    """
    A sample represents the series of sub periods attached to the IODE variables or to the estimation process.

    Attributes
    ----------
    start: str
        First period of the sample.
    end: str
        Last period of the sample.
    nb_periods: int
        Total number of sub periods in the sample. 

    Examples
    --------
    >>> from iode import variables, SAMPLE_DATA_DIR
    >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
    >>> variables.sample
    '1960Y1:2015Y1'
    """
    cdef bint ptr_owner
    cdef CSample* c_sample

    def __init__(self):
        # Prevent accidental instantiation from normal Python code
        # since we cannot pass a struct pointer into a Python constructor.
        raise TypeError("This class cannot be instantiated directly.")

    def __cinit__(self):
        self.ptr_owner = False
        self.c_sample = NULL

    def __dealloc__(self):
        if self.ptr_owner and self.c_sample is not NULL:
            del self.c_sample
            self.c_sample = NULL

    # see https://cython.readthedocs.io/en/stable/src/userguide/extension_types.html#instantiation-from-existing-c-c-pointers 
    @staticmethod
    cdef Sample _from_ptr(CSample* ptr, bint owner=False):
        """
        Factory function to create Sample objects from a given CSample pointer.
        """
        # Fast call to __new__() that bypasses the __init__() constructor.
        cdef Sample wrapper = Sample.__new__(Sample)
        wrapper.c_sample = ptr
        wrapper.ptr_owner = owner
        return wrapper
    
    def index(self, period: Union[str, Period]) -> int:
        """
        Position of the 'period' in the sample.

        Returns
        -------
        int

        Raises
        ------
        IndexError
            If the 'period' has not been found in the sample.

        Examples
        --------
        >>> from iode import variables, SAMPLE_DATA_DIR
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> variables.sample.index("1982Y1")
        22
        >>> variables.sample.index("2020Y1")
        Traceback (most recent call last):
        ... 
        IndexError: The period '2020Y1' is not in the sample '1960Y1:2015Y1'
        """
        if self.c_sample is NULL:
            raise RuntimeError("'sample' is not defined")
        if isinstance(period, Period):
            period = str(period)
        cdef string str_period = period.encode()
        return self.c_sample.get_period_position(str_period)

    def get_list_periods(self, as_float: bool = False) -> Union[List[str], List[float]]:
        """
        List of all periods of the sample.
        Periods are exported as string (default) or as float

        Returns
        -------
        list(str) or list(float)

        Examples
        --------
        >>> from iode import variables, SAMPLE_DATA_DIR
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> variables.sample.get_list_periods()    #doctest: +ELLIPSIS
        ['1960Y1', '1961Y1', ..., '2014Y1', '2015Y1']
        >>> variables.sample.get_list_periods(as_float=True)    #doctest: +ELLIPSIS
        [1960.0, 1961.0, ..., 2014.0, 2015.0]
        """
        if self.c_sample is NULL:
            raise RuntimeError("'sample' is not defined")
        if as_float:
            return self.c_sample.get_list_periods_as_float()
        else:
            return [period.decode() for period in self.c_sample.get_list_periods()]

    def intersection(self, other_sample: Sample) -> Sample:
        """
        Compute the intersection between two samples.

        Returns
        -------
        Sample

        Examples
        --------
        >>> from iode import variables, SAMPLE_DATA_DIR
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> variables.sample
        '1960Y1:2015Y1'
        >>> variables_2 = variables.copy()
        >>> variables_2.sample = "2000Y1:2040Y1"
        >>> variables_2.sample
        '2000Y1:2040Y1'
        >>> sample_intersec = variables.sample.intersection(variables_2.sample)
        >>> sample_intersec
        '2000Y1:2015Y1'
        """
        if self.c_sample is NULL:
            raise RuntimeError("'sample' is not defined")
        if other_sample.c_sample is NULL:
            raise RuntimeError("Passed sample to 'intersection(sample)' represents an empty sample")
        cdef CSample c_sample_inter = self.c_sample.intersection(dereference(other_sample.c_sample))
        return Sample._from_ptr(new CSample(c_sample_inter), <bint>True)

    # Attributes access

    @property
    def start(self) -> str:
        if self.c_sample is NULL:
            warnings.warns("'sample' is not defined")
            return None
        else:
            c_period = self.c_sample.start_period()
            return c_period.to_string().decode()

    @property
    def end(self) -> str:
        if self.c_sample is NULL:
            warnings.warns("'sample' is not defined")
            return None
        else:
            c_period = self.c_sample.end_period()
            return c_period.to_string().decode()

    @property
    def nb_periods(self) -> int:
        if self.c_sample is NULL:
            warnings.warns("'sample' is not defined")
            return 0
        else:
            return self.c_sample.nb_periods()

    # Special methods

    def __len__(self) -> int:
        if self.c_sample is NULL:
            warnings.warns("'sample' is not defined")
            return 0
        else:
            return self.c_sample.nb_periods()

    def __eq__(self, other: Sample) -> bool:
        if self.c_sample is NULL:
            warnings.warns("'sample' in 'sample == other' is not defined")
            return False
        elif other.c_sample is NULL:
            warnings.warns("other sample in 'sample == other' is not defined")
            return False
        else:
            return self.c_sample == other.c_sample

    def __str__(self) -> str:
        if self.c_sample is NULL:
            warnings.warns("'sample' is not defined")
            return ''
        else:
            return self.c_sample.to_string().decode()

    def __repr__(self) -> str:
        if self.c_sample is NULL:
            warnings.warns("'sample' is not defined")
            return ''
        else:
            return repr(str(self))
