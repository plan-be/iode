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
from pyiode.time.sample cimport CSample

try:
    import larray as la
    Axis = la.Axis
except ImportError:
    la = None
    Axis = Any

# TODO : add Period ?
_ALLOWED_TYPES_FOR_PERIOD = {'str': str, 'float': float}


# Sample wrapper class
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#create-cython-wrapper-class 
cdef class Sample:
    """
    A sample represents the series of sub periods attached to the IODE variables or to the estimation process.

    Parameters
    ----------
    start_period: str or Period
    	First period of the sample.
    	If str, it must be a valid period string (e.g. '1960Y1').
    end_period: str or Period
    	Last period of the sample.
        If str, it must be a valid period string (e.g. '2015Y1').

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
    >>> from iode import Sample
    >>> Sample("1982Y1", "2020Y1")
    Sample("1982Y1:2020Y1")
    >>> Sample("1982Y1:2020Y1")
    Sample("1982Y1:2020Y1")
    """
    cdef bint ptr_owner
    cdef CSample* c_sample

    def __init__(self, start_period: Union[str, Period], end_period: Union[str, Period]=None):
        if isinstance(start_period, Period):
            start_period = str(start_period)
        if end_period is not None and isinstance(end_period, Period):
            end_period = str(end_period)
        
        if end_period is None: 
            if ':' not in start_period:
                raise ValueError("When only one parameter is passed to Sample(), it is considered as "
                                 "a string representation of the desired sample and must include a colon ':'")
            start_period, end_period = start_period.split(':')

        self.ptr_owner = True
        self.c_sample = new CSample(start_period.encode(), end_period.encode())

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
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")    # doctest: +ELLIPSIS
        Loading .../fun.var
        394 objects loaded
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

    def get_period_list(self, astype: Union[type(Any), str] = str) -> List[Any]:
        """
        List of all periods of the sample.
        Periods are exported as string (default) or as float.

        Parameters
        ----------
        astype: type or str
            Allowed returned type for periods are str and float.
            Default to str.

        Returns
        -------
        list(str) or list(float)

        Examples
        --------
        >>> from iode import variables, SAMPLE_DATA_DIR
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")    # doctest: +ELLIPSIS
        Loading .../fun.var
        394 objects loaded
        >>> variables.sample.get_period_list()    # doctest: +ELLIPSIS
        ['1960Y1', '1961Y1', ..., '2014Y1', '2015Y1']
        >>> variables.sample.get_period_list(astype=float)    # doctest: +ELLIPSIS
        [1960.0, 1961.0, ..., 2014.0, 2015.0]
        """
        if self.c_sample is NULL:
            raise RuntimeError("'sample' is not defined")
        if isinstance(astype, str):
            astype = _ALLOWED_TYPES_FOR_PERIOD[astype]
        
        if astype == float:
            return self.c_sample.get_list_periods_as_float()
        elif astype == str:
            return [period.decode() for period in self.c_sample.get_list_periods()]
        else:
            raise ValueError(f"'astype': type {astype.__name__} is not allowed.")

    def intersection(self, other_sample: Sample) -> Sample:
        """
        Compute the intersection between two samples.

        Returns
        -------
        Sample

        Examples
        --------
        >>> from iode import variables, SAMPLE_DATA_DIR
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")    # doctest: +ELLIPSIS
        Loading .../fun.var
        394 objects loaded
        >>> variables.sample
        Sample("1960Y1:2015Y1")
        >>> variables_2 = variables.copy()
        >>> variables_2.sample = "2000Y1:2040Y1"
        >>> variables_2.sample
        Sample("2000Y1:2040Y1")
        >>> sample_intersec = variables.sample.intersection(variables_2.sample)
        >>> sample_intersec
        Sample("2000Y1:2015Y1")
        """
        if self.c_sample is NULL:
            raise RuntimeError("'sample' is not defined")
        if other_sample.c_sample is NULL:
            raise RuntimeError("Passed sample to 'intersection(sample)' represents an empty sample")
        cdef CSample c_sample_inter = self.c_sample.intersection(dereference(other_sample.c_sample))
        return Sample._from_ptr(new CSample(c_sample_inter), <bint>True)

    # Attributes access

    @property
    def start(self) -> Period:
        if self.c_sample is NULL:
            warnings.warn("'sample' is not defined")
            return None
        else:
            c_period = self.c_sample.start_period()
            return Period(c_period.to_string().decode()) if c_period.p_y > 0 else None

    @property
    def end(self) -> Period:
        if self.c_sample is NULL:
            warnings.warn("'sample' is not defined")
            return None
        else:
            c_period = self.c_sample.end_period()
            return Period(c_period.to_string().decode()) if c_period.p_y > 0 else None

    @property
    def nb_periods(self) -> int:
        if self.c_sample is NULL:
            warnings.warn("'sample' is not defined")
            return 0
        else:
            return self.c_sample.nb_periods()

    # Special methods

    def __len__(self) -> int:
        if self.c_sample is NULL:
            warnings.warn("'sample' is not defined")
            return 0
        else:
            return self.c_sample.nb_periods()

    def __eq__(self, other: Sample) -> bool:
        if self.c_sample is NULL:
            warnings.warn("'sample' in 'sample == other' is not defined")
            return False
        elif other.c_sample is NULL:
            warnings.warn("other sample in 'sample == other' is not defined")
            return False
        else:
            return str(self) == str(other)

    def __str__(self) -> str:
        if self.c_sample is NULL or (self.start is None and self.end is None):
            warnings.warn("'sample' is not defined")
            return str(None)
        else:
            s_start = str(self.start) if self.start is not None else ""
            s_end = str(self.end) if self.end is not None else ""
            return s_start + ":" + s_end

    def __repr__(self) -> str:
        if self.c_sample is NULL or (self.start is None and self.end is None):
            warnings.warn("'sample' is not defined")
            return str(None)
        else:
            return f'Sample("{self.start}:{self.end}")'
