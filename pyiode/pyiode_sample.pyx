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
#     ws_sample_set(per_from="", per_to="")                                 | Set KV_WS sample
#     ws_sample_get()                                                       | Get KV_WS sample
#     ws_sample_nb_periods()                                                | Return the number of observations in the current KV_WS.
#     ws_sample_to_string()                                                 | Return the current sample definition in a string: "from to", e.g.: "2000Y1 2020Y1"
#     ws_sample_to_list(per_from="", per_to="", as_floats:bool=False)       | Return the current sample definition in a list
#     ws_sample_to_larray_axis(axis_name = 'time', as_floats:bool=False))   | Return the current sample definition as an larray axis
  

def ws_sample_set(per_from:str, per_to:str):
    '''
    Set the KV_WS sample and return the new sample
    
    Example: if the current sample is 200Y1 2005Y1:
    ws_sample_set("", "") )              => ["2002Y1", "2005Y1"]
    ws_sample_set("2002Y1", "") )        => ["2002Y1", "2005Y1"]
    ws_sample_set("", "2010Y1") )        => ["2002Y1", "2010Y1"]
    ws_sample_set("2001Y1", "2020Y1") )  => ["2001Y1", "2020Y1"]
    ws_sample_set("2001Q1", "2020Q3") )  => error 
    '''

    cursample = ws_sample_get()
    if per_from == "" and per_to == "":
        return cursample

    if not IodeIsSampleSet() and (per_from == "" or per_to == ""):
        raise RuntimeError(f"ws_sample_set(): sample insufficiently undefined.") 

    if per_from == "":
        rc = IodeSetSampleStr(cstr(cursample[0]), cstr(per_to))
    elif per_to == "":
        rc = IodeSetSampleStr(cstr(per_from), cstr(cursample[1]))
    else: 
        rc = IodeSetSampleStr(cstr(per_from), cstr(per_to))

    if rc != 0:
        raise RuntimeError(f"ws_sample_set(): cannot set sample '{per_from}' '{per_to}'.") 
        
    return ws_sample_get()


def ws_sample_get()->List[str]:
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


def ws_sample_nb_periods()->int:
    '''Return the number of observations in the current KV_WS.'''
    
    return IodeGetSampleLength()


def ws_sample_to_string()->str:
    '''
    Return the current sample definition in a string: "from to", e.g.: "2000Y1 2020Y1"
    or "" if the sample is undefined.
    '''
    
    if not IodeIsSampleSet():
        return ""
    else:
        lst = ws_sample_get()
        smpl = f"{lst[0]} {lst[1]}"
        return smpl
 

#  TODO: if as_floats is True and per_from and per_to are not empty, 
#        return the sample per_from to per_to in floats instead of the KV_WS sample
def ws_sample_to_list(per_from="", per_to="", as_floats:bool=False)->List[str]:
    '''Return the current sample definition or the sample [per_from, per_to] in a list
    e.g.: 
        ["2000Y1", "2001Y1", ..., "2010Y1"] 
        or 
        [] (if the sample is undefined)
    
   
    '''

    cdef char **smpl

    if as_floats: 
        vararray = iodesample_to_ndarray()
        return vararray
    
    else:
        if per_from == '' or per_to == '':
            if not IodeIsSampleSet():
                return []
            else:
                smpl = IodeGetSampleAsPeriods()
        else:
            smpl = IodeCreateSampleAsPeriods(cstr(per_from), cstr(per_to))
    
        lst = pylist(smpl)
        SCR_free_tbl(smpl)
        return lst 
        

#  TODO: if as_floats is True and per_from and per_to are not empty, 
#        return the sample per_from to per_to in floats instead of the KV_WS sample
def ws_sample_to_larray_axis(axis_name='time', per_from='', per_to='', as_floats:bool=False)->la.Axis:
    '''
    Return the current sample or the sample [per_from, per_to] definition as an Axis.
        e.g.: Axis(["2000Y1", "2001Y1", ..., "2010Y1"], "time")
        
    If as_floats == True, the list items are floats.
       e.g.: [2000.0, 2000.25, 2000.50,2000.75,...]
    '''
    
    lst = ws_sample_to_list(per_from, per_to, as_floats)
    ax = la.Axis(lst, axis_name)
    return ax 