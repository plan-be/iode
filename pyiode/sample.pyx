# IODE EXTENSION FOR PYTHON
# =========================
#
#    @header4iode
# 
# Functions to manage ws sample
# 
# Note: this file cannot be compiled independently: it must be "included" in iode.pyx       

# distutils: language = c++

from typing import Union, Tuple, List, Optional, Any
from sample cimport (CSample, IodeIsSampleSet, IodeSetSampleStr, IodeGetSampleAsString, 
                            IodeCreateSampleAsPeriods)
from iode_python cimport free_tbl

try:
    import larray as la
    Axis = la.Axis
except ImportError:
    la = None
    Axis = Any


# Sample wrapper class
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#create-cython-wrapper-class 
cdef class Sample:

    cdef CSample c_sample

    def __init__(self, *args: Union[Sample, Tuple[Union[str, Period], Union[str, Period]]]) -> Sample:
        """
        A sample represents the series of sub periods attached to the IODE variables.
        
        Parameters
        ----------
        start_period: str or Period
            First period of the sample.
        end_period: str or Period
            Last period of the sample.

        Attributes
        ----------
        start: str
            First period of the sample.
        end: str
            Last period of the sample.
        nb_periods
            Total number of sub periods in the sample. 

        Examples
        --------
        >>> from iode import Sample, Period
        >>> # passing two strings
        >>> sample = Sample("1960Y1", "2015Y1")
        >>> sample
        1960Y1:2015Y1
        >>> # passing two Period objects
        >>> start_period = Period("1960Y1")
        >>> end_period = Period("2015Y1")
        >>> sample = Sample(start_period, end_period)
        >>> sample
        1960Y1:2015Y1
        >>> # copy a sample
        >>> sample_copy = Sample(sample)
        >>> sample_copy
        1960Y1:2015Y1
        >>> # error: Incompatible periodicities
        >>> sample = Sample("1960Y1", "2015Q1")
        Traceback (most recent call last):
        ... 
        RuntimeError: Cannot calculate the difference between the periods '2015Q1' and '1960Y1'
        The two periods must share the same periodicity:
        left period periodicity :Q
        right period periodicity:Y
        """
        cdef string start_period
        cdef string end_period  

        def check_period(period) -> string:
            if not isinstance(period, (str, Period)):
                TypeError("Expected str or 'Period' object for the starting period.\n" + 
                    "Got argument of type '" + type(period).__name__ + "'")
            if isinstance(period, Period):
                period = str(period)
            return period.encode("utf-8")

        if len(args) == 1:
            smpl = args[0]
            if not isinstance(smpl, Sample):
                raise TypeError("Expected one argument of type 'Sample' or two arguments of type str or 'Period'.\n" + 
                    "Got argument of type '" + type(smpl).__name__ + "'")
            start_period = check_period(smpl.start)
            end_period = check_period(smpl.end)
            self.c_sample = CSample(start_period, end_period)
        elif len(args) == 2:
            start_period = check_period(args[0])
            end_period = check_period(args[1])
            self.c_sample = CSample(start_period, end_period)
        else:
            ValueError("Expected either one argument of type 'Sample' or two arguments of type str or 'Period'")
    
    def get_period_position(self, period: Union[str, Period]) -> int:
        """
        Position of the 'period' in the sample.

        Returns
        -------
        int

        Examples
        --------
        >>> from iode import Sample
        >>> sample = Sample("1960Y1", "2015Y1")
        >>> sample.get_period_position("1982Y1")
        22
        """
        if not isinstance(period, (str, Period)):
            TypeError("Expected argument of type str or 'Period'.\nGot argument of type '" + type(period).__name__ + "'")
        if isinstance(period, Period):
            period = str(period)
        cdef string str_period = period.encode("utf-8")
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
        >>> from iode import Sample
        >>> sample = Sample("1960Y1", "2015Y1")
        >>> sample.get_list_periods()    #doctest: +ELLIPSIS
        ['1960Y1', '1961Y1', ..., '2014Y1', '2015Y1']
        >>> sample.get_list_periods(as_float=True)    #doctest: +ELLIPSIS
        [1960.0, 1961.0, ..., 2014.0, 2015.0]
        """
        if as_float:
            return self.c_sample.get_list_periods_as_float()
        else:
            return [period.decode("utf-8") for period in self.c_sample.get_list_periods()]

    def intersection(self, other_sample: Sample) -> Sample:
        """
        Compute the intersection between two samples.

        Returns
        -------
        Sample

        Examples
        --------
        >>> from iode import Sample
        >>> sample = Sample("1960Y1", "2015Y1")
        >>> sample
        1960Y1:2015Y1
        >>> sample_2 = Sample("2000Y1", "2040Y1")
        >>> sample_2
        2000Y1:2040Y1
        >>> sample_intersec = sample.intersection(sample_2)
        >>> sample_intersec
        2000Y1:2015Y1
        """
        c_sample_inter = self.c_sample.intersection(other_sample.c_sample)
        str_sample = c_sample_inter.to_string().decode("utf-8")
        return Sample(*str_sample.split(':'))

    # Attributes access

    @property
    def start(self) -> str:
        c_period = self.c_sample.start_period()
        return c_period.to_string().decode("utf-8")

    @property
    def end(self) -> str:
        c_period = self.c_sample.end_period()
        return c_period.to_string().decode("utf-8")

    @property
    def nb_periods(self) -> int:
        return self.c_sample.nb_periods()

    # Special methods

    def __eq__(self, other: Sample) -> bool:
        if not isinstance(other, Sample):
            raise TypeError(f"Expected argument of type 'Sample'.\nGot argument of type '{type(other)}'")
        return self.c_sample == other.c_sample

    def __str__(self) -> str:
        return self.c_sample.to_string().decode("utf-8")

    def __repr__(self) -> str:
        return self.c_sample.to_string().decode("utf-8")


def ws_sample_set(from_period: str, to_period: str) -> Optional[Tuple[str, str]]:
    warnings.warn("ws_sample_set() is deprecated. " + 
        "Please use the new syntax: var_db.sample = 'from_period:to_period'", DeprecationWarning)
    var_db = Variables()
    var_db.sample = f"{from_period}:{to_period}"

def ws_sample_get() -> Optional[Tuple[str, str]]: 
    warnings.warn("ws_sample_get() is deprecated. " + 
        "Please use the new syntax: var_db.sample", DeprecationWarning)
    var_db = Variables()
    return var_db.sample

def ws_sample_nb_periods() -> int:
    warnings.warn("ws_sample_nb_periods() is deprecated. " + 
        "Please use the new syntax: var_db.nb_periods", DeprecationWarning)
    var_db = Variables()
    return var_db.nb_periods

def ws_sample_to_string() -> Optional[str]:
    warnings.warn("ws_sample_to_string() is deprecated. " + 
        "Please use the new syntax: var_db.sample", DeprecationWarning)
    var_db = Variables()
    return var_db.sample

def ws_sample_to_list(from_period: str = "", to_period: str = "", as_floats: bool = False) -> Optional[Union[List[str], np.ndarray]]:
    warnings.warn("ws_sample_to_list() is deprecated. " + 
        "Please use the new syntax: var_db.periods", DeprecationWarning)
    var_db = Variables()
    return var_db.periods

def ws_sample_to_larray_axis(axis_name: str = 'time', 
                             from_period:str = '', to_period: str = '', 
                             as_floats: bool = False) -> Axis:
    warnings.warn("ws_sample_to_larray_axis() is deprecated. " + 
        "Please use the new syntax:\nperiods_list = var_db.periods\n" + 
        "time_axis = la.Axis(name='time', labels=nperiods_list)", DeprecationWarning)
    var_db = Variables()
    periods_list = var_db.periods
    return Axis(name='time', labels=periods_list)
