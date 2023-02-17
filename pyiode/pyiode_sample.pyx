# IODE EXTENSION FOR PYTHON
# =========================
#
#    @header4iode
# 
# Functions to manage ws sample
# 
# Note: this file cannot be compiled independently: it must be "included" in iode.pyx 

# List of functions
# -----------------
#     ws_sample(per_from = None, per_to = None)          | Set or get the KV_WS sample
#     ws_sample_get()                                    | Get the KV_WS sample
#     ws_sample_length()                                 | Return the number of observations in the current KV_WS.
#     ws_sample_as_string()                              | Return the current sample definition in a string: "from to", e.g.: "2000Y1 2020Y1"
#     ws_sample_as_list()                                | Return the current sample definition in a list
#     ws_sample_as_axis(axis_name = 'time')              | Return the current sample definition as an Axis
#     ws_sample_as_list_of_doubles()                     | Return the current sample definition in a list of float
#     ws_sample_as_axis_of_doubles(axis_name = 'time')   | Return the current sample definition in an Axis with float labels.
  

def ws_sample(per_from=None, per_to=None):
    '''
    Set or get the KV_WS sample.
    
    Example: if the current sample is 200Y1 2005Y1:
    ws_sample()                      => ["2000Y1", "2005Y1"] (no change)
    ws_sample("2002Y1") )            => ["2002Y1", "2005Y1"]
    ws_sample(None, "2010Y1") )      => ["2002Y1", "2010Y1"]
    ws_sample("2001Y1", "2020Y1") )  => ["2001Y1", "2020Y1"]
    '''

    cursample = ws_sample_get()
    if per_from is None and per_to is None:
        return cursample

    # print(f"not IodeIsSampleSet() ? {not IodeIsSampleSet()}");
    # print(f"per_from = {per_from} -- per_to = {per_to}");
    
    if not IodeIsSampleSet() and (per_from is None or per_to is None):
        return cursample

    if per_from is None:
        IodeSetSampleStr(cstr(cursample[0]), cstr(per_to))
    elif per_to is None:
        IodeSetSampleStr(cstr(per_from), cstr(cursample[1]))
    else: 
        IodeSetSampleStr(cstr(per_from), cstr(per_to))

    return ws_sample_get()



def ws_sample_get():
    '''
    Return the current sample lower and upper bounds in a list, e.g.: ["2000Y1", "2010Y1"]
    or [None, None] if the sample is undefined.
    '''

    cdef char* c_str

    if not IodeIsSampleSet():
        return [None, None]
    else:
        c_str = IodeGetSampleAsString()
        py_lst = pystr(c_str).split(" ")
        SCR_free(c_str)
        return py_lst


def ws_sample_length():
    '''Return the number of observations in the current KV_WS.'''
    
    return IodeGetSampleLength()


def ws_sample_as_string():
    '''
    Return the current sample definition in a string: "from to", e.g.: "2000Y1 2020Y1"
    or "" if the sample is undefined.
    '''
    
    if not IodeIsSampleSet():
        return ""
    else:
        lst = ws_sample()
        smpl = f"{lst[0]} {lst[1]}"
        return(smpl)
 

def ws_sample_as_list():
    '''Return the current sample definition in a list
    e.g.: 
        ["2000Y1", "2001Y1", ..., "2010Y1"] 
        or 
        [] (if the sample is undefined)
    '''

    cdef char **smpl
    
    if not IodeIsSampleSet():
        return []
    else:
        smpl = IodeGetSampleAsAxis()
        lst = pylist(smpl)
        SCR_free_tbl(smpl)
        return(lst)


def ws_sample_as_axis(axis_name='time'):
    '''Return the current sample definition as an Axis, e.g.: Axis(["2000Y1", "2001Y1", ..., "2010Y1"], "time")'''
    
    lst = ws_sample_as_list()
    ax = la.Axis(lst, axis_name)
    return(ax)


def ws_sample_as_list_of_doubles():
    '''Return the current sample definition in a list of float.
     e.g.: [2000.0, 2000.25, 2000.50,2000.75,...]
     '''
    vararray = iodesample_to_ndarray()
    return(vararray)


def ws_sample_as_axis_of_doubles(axis_name='time'):
    '''Return the current sample definition in an Axis with float labels.
     e.g.: Axis([2000.0, 2000.25, 2000.50,2000.75,...], "time")
     '''
    lst = ws_sample_as_list()
    ax = la.Axis(lst, axis_name)
    return(ax)
