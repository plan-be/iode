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
from pyiode_sample cimport (CSample, IodeIsSampleSet, IodeSetSampleStr, IodeGetSampleAsString, 
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
    '''
    Set the sample of the Variables database and return the new sample
    
    Parameters
    ----------
    from_period: str
        starting period.
    to_period: str
        ending period.

    Returns
    -------
    tuple(str, str)
        starting and ending periods of the new sample.

    Examples
    --------
    >>> import iode
    >>> iode.ws_load_var("../data/fun.var")
    394
    >>> iode.ws_sample_get()
    ('1960Y1', '2015Y1')
    >>> # does nothing
    >>> iode.ws_sample_set("", "")              
    ('1960Y1', '2015Y1')
    >>> # change only starting period
    >>> iode.ws_sample_set("2002Y1", "")        
    ('2002Y1', '2015Y1')
    >>> # change only ending period
    >>> iode.ws_sample_set("", "2010Y1")        
    ('2002Y1', '2010Y1')
    >>> iode.ws_sample_set("2001Y1", "2020Y1")  
    ('2001Y1', '2020Y1')
    >>> iode.ws_sample_set("2001Q1", "2020Q3") 
    Traceback (most recent call last):
    ... 
    RuntimeError: ws_sample_set(): cannot set sample '2001Q1' '2020Q3'
    '''
    cursample = ws_sample_get()
    if from_period == "" and to_period == "":
        return cursample

    if not IodeIsSampleSet() and (from_period == "" or to_period == ""):
        raise RuntimeError(f"ws_sample_set(): sample insufficiently undefined.") 

    if from_period == "":
        rc = IodeSetSampleStr(cstr(cursample[0]), cstr(to_period))
    elif to_period == "":
        rc = IodeSetSampleStr(cstr(from_period), cstr(cursample[1]))
    else: 
        rc = IodeSetSampleStr(cstr(from_period), cstr(to_period))

    if rc != 0:
        raise RuntimeError(f"ws_sample_set(): cannot set sample '{from_period}' '{to_period}'") 
        
    return ws_sample_get()


def ws_sample_get() -> Optional[Tuple[str, str]]: 
    '''
    Return the current sample lower and upper bounds if set, otherwise return None.

    Returns
    -------
    tuple(str, str) or None

    Examples
    --------
    >>> import iode
    >>> iode.ws_load_var("../data/fun.var")
    394
    >>> iode.ws_sample_get()
    ('1960Y1', '2015Y1')
    '''
    cdef char* c_str

    if not IodeIsSampleSet():
        return None
    else:
        c_str = IodeGetSampleAsString()
        start, end = pystr(c_str).split(" ")
        SCR_free(c_str)
        return start, end


def ws_sample_nb_periods() -> int:
    '''
    Return the number of observations in the current KV_WS.
    
    Returns
    -------
    int

    Examples
    --------
    >>> import iode
    >>> iode.ws_load_var("../data/fun.var")
    394
    >>> iode.ws_sample_get()
    ('1960Y1', '2015Y1')
    >>> iode.ws_sample_nb_periods()
    56
    '''
    return IodeGetSampleLength()


def ws_sample_to_string() -> Optional[str]:
    '''
    Return the current sample definition as a string "from_period to_period" 
    (e.g.: "2000Y1 2020Y1") or None if the sample is undefined.

    Returns
    -------
    str or None

    Examples
    --------
    >>> import iode
    >>> iode.ws_load_var("../data/fun.var")
    394
    >>> iode.ws_sample_to_string()
    '1960Y1 2015Y1'
    '''
    if not IodeIsSampleSet():
        return
    else:
        start, end = ws_sample_get()
        return f"{start} {end}"
 

#  TODO: if as_floats is True and from_period and to_period are not empty, 
#        return the sample from_period to to_period in floats instead of the KV_WS sample
def ws_sample_to_list(from_period: str = "", to_period: str = "", as_floats: bool = False) -> Optional[Union[List[str], np.ndarray]]:
    '''
    Return the list of sub periods of the current sample.
    Return None if the sample is not set.

    Parameters
    ----------
    from_period: str, optional
        starting period of the list to return. 
        Defaults to starting period of the current sample.
    to_period: str, optional
        ending period of the list to return.
        Defaults to ending period of the current sample.
    as_floats: bool, optional
        whether or not to return the periods as floats.
        Defaults to False.
    
    Returns
    -------
    list(str) or list(float) or None

    Examples
    --------
    >>> import iode
    >>> iode.ws_load_var("../data/fun.var")
    394
    >>> iode.ws_sample_to_list()    #doctest: +ELLIPSIS
    ['1960Y1', '1961Y1', ..., '2014Y1', '2015Y1']
    >>> iode.ws_sample_to_list(from_period="2010Y1")
    ['2010Y1', '2011Y1', '2012Y1', '2013Y1', '2014Y1', '2015Y1']
    >>> iode.ws_sample_to_list(to_period="1965Y1")
    ['1960Y1', '1961Y1', '1962Y1', '1963Y1', '1964Y1', '1965Y1']
    >>> iode.ws_sample_to_list(as_floats=True)
    array([ 1960.,  1961.,  1962.,  1963.,  1964.,  1965.,  1966.,  1967.,
            1968.,  1969.,  1970.,  1971.,  1972.,  1973.,  1974.,  1975.,
            1976.,  1977.,  1978.,  1979.,  1980.,  1981.,  1982.,  1983.,
            1984.,  1985.,  1986.,  1987.,  1988.,  1989.,  1990.,  1991.,
            1992.,  1993.,  1994.,  1995.,  1996.,  1997.,  1998.,  1999.,
            2000.,  2001.,  2002.,  2003.,  2004.,  2005.,  2006.,  2007.,
            2008.,  2009.,  2010.,  2011.,  2012.,  2013.,  2014.,  2015.])
    '''
    cdef char **smpl

    if not IodeIsSampleSet():
        return

    if as_floats: 
        vararray = iodesample_to_ndarray()
        return vararray
    else:
        start, end = ws_sample_get()
        if not from_period:
            from_period = start
        if not to_period:
            to_period = end
        smpl = IodeCreateSampleAsPeriods(cstr(from_period), cstr(to_period))
        lst = pylist(smpl)
        free_tbl(smpl)
        return lst 
        

#  TODO: if as_floats is True and from_period and to_period are not empty, 
#        return the sample from_period to to_period in floats instead of the KV_WS sample
def ws_sample_to_larray_axis(axis_name: str = 'time', 
                             from_period:str = '', to_period: str = '', 
                             as_floats: bool = False) -> Axis:
    '''
    Return the current sample or the sample [from_period, to_period] definition as an Axis.
        e.g.: Axis(["2000Y1", "2001Y1", ..., "2010Y1"], "time")
        
    If as_floats == True, the list items are floats.
       e.g.: [2000.0, 2000.25, 2000.50,2000.75,...]
    '''
    if la is None:
        raise RuntimeError("larray library not found")

    lst = ws_sample_to_list(from_period, to_period, as_floats)
    ax = Axis(lst, axis_name)
    return ax 