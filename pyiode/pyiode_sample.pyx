# IODE EXTENSION FOR PYTHON
# =========================
#
#    @header4iode
# 
# Functions to manage ws sample
# 
# Note: this file cannot be compiled independently: it must be "included" in iode.pyx       

# distutils: language = c++

from typing import Union, Tuple, Optional


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