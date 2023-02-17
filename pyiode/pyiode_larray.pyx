# IODE EXTENSION FOR PYTHON
# =========================
#
#    @header4iode
# 
# Interface between IODE ws and larray arrays.
# ---------------------------------------------
#   la_to_ws_pos(la_input, int* la_pos, int* ws_pos, int* la_lg, timeaxis = 'time')
#  
#   larray_to_ws(la_input, timeaxis:str='time', sep:str="_")
#   ws_to_larray_time_as_doubles(vars_pattern='*', varsaxis='vars', timeaxis='time', axis_names='', regex=None, sep=None):
#   ws_to_larray(vars_pattern = '*', varsaxis = 'vars', timeaxis = 'time', axis_names='', regex=None, sep=None):
#   ws_load_var_py(filename:str, vars_pattern='*', varsaxis='vars', timeaxis='time', axis_names=None, regex=None, sep=None):
#   lasample(la_input, timeaxis = 'time')                               Return the first and last time axis labels as a list of 2 strings
    


cdef la_to_ws_pos(la_input, int* la_pos, int* ws_pos, int* la_lg, timeaxis = 'time'):
    '''
    Calls C API fonction IodeCalcSamplePosition() to determine which elements of la_input to copy into KV_WS and 
    at which position in the KV_WS sample.
    '''
      
    if timeaxis not in la_input.axes:
        raise RuntimeError(f"Passed Array object must contain an axis named {timeaxis}.\nGot axes {repr(la_input.axes)}.")

    time = la_input.axes[timeaxis]
    IodeCalcSamplePosition(cstr(time.labels[0]), cstr(time.labels[-1]), la_pos, ws_pos, la_lg)
    #print(f"la_pos:{la_pos} - ws_pos:{ws_pos} - la_lg:{la_lg} ")


 
def larray_to_ws(la_input, timeaxis:str='time', sep:str="_"):
    '''
    Copies Array la_input into IODE KV_WS.
    
    If la_input has more than 2 dimensions, the non-time axes are grouped (with sep as separator) 
    to form the series names.
    '''
    cdef    int     ws_pos
    cdef    int     la_pos
    cdef    int     la_lg
    
    # Retrieve the timeaxis. Stop if no timeaxis present in la_input.
    if timeaxis not in la_input.axes:
        raise RuntimeError(f"Passed Array object must contain an axis named {timeaxis}.\nGot axes {repr(la_input.axes)}.")
    time = la_input.axes[timeaxis]

    # Define the KV_WS sample if not yet set
    if not IodeIsSampleSet():
        IodeSetSampleStr(cstr(time.labels[0]), cstr(time.labels[-1]))
    
    # Push the time axis as last axis and combine all other axes 
    la_input = la_input.transpose(..., timeaxis)
    la_input = la_input.combine_axes(la_input.axes[:-1], sep=sep)

    # Calculate:
    #   - la_pos: the position of the first element of la_input to be copied into WS
    #   - ws_pos: the position in KV_WS sample where to copy la_input[...la_pos]
    #   - the nb of la_input values to be copied 
    la_to_ws_pos(la_input, &la_pos, &ws_pos, &la_lg, timeaxis)

    # List of variable names
    vars = la_input.axes[0]

    # Copy each line of la_input into KV_WS on the time intersection of la_input and KV_WS
    #values = <double*>np.PyArray_DATA(la_input[vars[0]].data)

    if la_input.dtype == "double":
        if la_input.data.data.c_contiguous:
            for v, la_line_data in zip(vars.labels, la_input.data):
                # Save the vector of doubles in KV_WS
                values = < double * > np.PyArray_DATA(la_line_data)
                IodeSetVector(cstr(v), values, la_pos, ws_pos, la_lg)
        else:
            for v, la_line_data in zip(vars.labels, la_input.data):
                la_line_data = la_line_data.copy()  # copy if non contiguous
                # Save the vector of doubles in KV_WS
                values = < double * > np.PyArray_DATA(la_line_data)
                IodeSetVector(cstr(v), values, la_pos, ws_pos, la_lg)
    else:
        for v, la_line_data in zip(vars.labels, la_input.data):
            la_line_data = la_line_data.astype("double") # astype creates a copy

            # Save the vector of doubles in KV_WS
            values = <double*>np.PyArray_DATA(la_line_data)
            IodeSetVector(cstr(v), values, la_pos, ws_pos, la_lg)

 
# Creates an LArray from the current KV_WS content with IODE sample
def ws_to_larray_gnl(vars_pattern:str='*', varsaxis:str='vars', timeaxis:str='time', axis_names='', regex=None, sep=None, time_as_doubles=False):
    cdef:
        int i
        np.ndarray[np.double_t, ndim=2] la_data
        double[:,::1] data_view
        str v
    
    var_list = ws_content_var(vars_pattern)         # 6 = K_VAR
    
    # Define 2 original axes var and time
    axis_var = la.Axis(var_list, varsaxis)
    if time_as_doubles:
        axis_time = la.Axis(ws_sample_as_list_of_doubles(), timeaxis)
    else:
        axis_time = ws_sample_as_axis(timeaxis)

    # Create LArray with 0
    la_res = la.zeros([axis_var, axis_time])
    la_data = la_res.data
    #data_view = la_data
    # print(la_res.info)
    
    # Copy values from KV_WS to la_res
    for i in range(len(var_list)):
        v = var_list[i]
        la_data[i] = var_to_ndarray(v, copy=False)
          
    if axis_names != '':
        # split_axes(self, axes=None, sep='_', names=None, regex=None, sort=False, fill_value=nan)
        la_res = la_res.split_axes(axis_var, sep=sep, names=axis_names, regex=regex)

    return la_res


# Creates an LArray from the current KV_WS content with **integer** sample (old version)
def ws_to_larray_time_as_doubles(vars_pattern='*', varsaxis='vars', timeaxis='time', axis_names='', regex=None, sep=None):

    return ws_to_larray_gnl(vars_pattern=vars_pattern, varsaxis=varsaxis, timeaxis=timeaxis, axis_names=axis_names, regex=regex, sep=sep, time_as_doubles=True)
 

def ws_to_larray(vars_pattern = '*', varsaxis = 'vars', timeaxis = 'time', axis_names='', regex=None, sep=None):

    return ws_to_larray_gnl(vars_pattern=vars_pattern, varsaxis=varsaxis, timeaxis=timeaxis, axis_names=axis_names, regex=regex, sep=sep, time_as_doubles=False)

    
# Load an IODE .var file 
# Create a LA object with 2 axes (vars and time) or
def ws_load_var_py(filename:str, vars_pattern='*', varsaxis='vars', timeaxis='time', axis_names=None, regex=None, sep=None):

    # Load iode var file
    rc = ws_load_var(filename)
    if rc <= 0:
        # raise an exception ?
        return None

    # Create LArray 
    la_res = ws_to_larray(vars_pattern=vars_pattern, varsaxis=varsaxis, timeaxis=timeaxis, axis_names=axis_names, regex=regex, sep=sep)
    
    return la_res



def lasample(la_input, timeaxis = 'time'):
    '''Return the first and last time axis labels as a list of 2 strings'''
    
    time = la_input.axes[timeaxis]
    return [time.labels[0], time.labels[-1]]


# OBSOLETE CODE
# -------------

#def larray_to_ws(la_input, timeaxis = 'time', sep="_"):
#    cdef    void* values
#    cdef    void* timedbl
#    
#    # Check there are 2 axes in la_input (ndim)
#    ndim = np.PyArray_NDIM(la_input.data)
#    print(f'Ndim = {ndim}')
#
#    if timeaxis not in la_input.axes:
#        raise RuntimeError(f"Passed Array object must contain an axis named {timeaxis}.\nGot axes {repr(la_input.axes)}.")
#        
#    time = la_input.axes[timeaxis]
#    
#    # push the time axis as last axis
#    la_input = la_input.transpose(..., timeaxis)
#    
#    # combine all axes expect time
#    la_input = la_input.combine_axes(la_input.axes[:-1], sep=sep)
#    
#    print("la_input info:\n", la_input.info)
#    
#    vars = la_input.axes[0]
#    # time = la_input.axes[timeaxis]
#    nvars = np.PyArray_DIM(la_input.data, 0)
#    ntime = np.PyArray_DIM(la_input.data, 1)
#
#    # Pointer to the first element in la_input ?
#    #  values = np.PyArray_DATA(la_input.data)
#    #cdef IodeSetVector_v1(char *name, double *value, int len) 
#    
#    # Manage the sample
#    # define the la_input sample in a IODE SAMPLE struct
#    
#    # timedbl =  np.PyArray_DATA(time.data)
#    # IodeSetSample(timedbl)
#    print(f"Label 0: {type(time.labels[0])} -> {time.labels[0]}-{time.labels[-1]}")
#    print(f"SAMPLE {time[0]} - {time[-1]}")
#    
#    # Redefine the WS sample 
#    IodeSetSample(time.labels[0], time.labels[-1])
#    
#    # Copy each line of la_input into KV_WS on the selected sample
#    for v in vars:
#        #setvector(v, la_input[v])
#        # Get a pointer to the values of v (double)
#        values = np.PyArray_DATA(la_input[v].data)
#        #IodeSetVector_v1(cstr(v), values, ntime) # temp fn defined in the local versionof s_iode.c
#        PyIodePrint(cstr(v), values, ntime) # temp fn defined in the local versionof s_iode.c
#        #print("{} = {}".format(v, la_input[v]))
# 
#
#def larray_to_ws_int_time(la_input, timeaxis = 'time', sep="_"):
#    cdef    double*   values
#    cdef    int     time1, time2
#    
#    # Check the existence of a timeaxis
#    if timeaxis not in la_input.axes:
#        raise RuntimeError(f"Passed Array object must contain an axis named {timeaxis}.\nGot axes {repr(la_input.axes)}.")
#    
#    # Define the KV_WS sample if not yet set    
#    time = la_input.axes[timeaxis]
#    time1 = time.labels[0]
#    time2 = time.labels[-1]
#    if not IodeIsSampleSet():
#        IodeSetSample(time1, time2)
#    
#    # Push the time axis as last axis and combine all other axes
#    la_input = la_input.transpose(..., timeaxis)
#    la_input = la_input.combine_axes(la_input.axes[:-1], sep=sep)
#    #print("la_input info:\n", la_input.info)
#    
#    vars = la_input.axes[0]
#    # time = la_input.axes[timeaxis]
#    ntime = np.PyArray_DIM(la_input.data, 1)
#
#    # Copy each line of la_input into KV_WS on la_input sample
#    for v in vars:
#        #setvector(v, la_input[v])
#        # Get a pointer to the values of v (double)
#        values = <double*>np.PyArray_DATA(la_input[v].data)
#        IodeSetVector_v1(cstr(v), values, time1, time2) # temp fn defined in the local versionof s_iode.c
#        PyIodePrint(cstr(v), values, ntime) # temp fn defined in the local versionof s_iode.c
#        #print("{} = {}".format(v, la_input[v]))
# 
#def larray_to_ws2(la_input, timeaxis = 'time', sep="_"):
#    cdef    double  *values
#    #cdef    char    *str_per1
#    #cdef    char    *str_per2
#    
#    # Check the existence of a timeaxis
#    if timeaxis not in la_input.axes:
#        raise RuntimeError(f"Passed Array object must contain an axis named {timeaxis}.\nGot axes {repr(la_input.axes)}.")
#    
#    # Define the KV_WS sample if not yet set    
#    time = la_input.axes[timeaxis]
#    #str_per1 = cstr(time.labels[0])
#    #str_per2 = cstr(time.labels[-1])
#    if not IodeIsSampleSet():
#        # IodeSetSampleStr(str_per1, str_per2)
#        IodeSetSampleStr(cstr(time.labels[0]), cstr(time.labels[-1]))
#    
#    # Push the time axis as last axis and combine all other axes
#    la_input = la_input.transpose(..., timeaxis)
#    la_input = la_input.combine_axes(la_input.axes[:-1], sep=sep)
#    #print("la_input info:\n", la_input.info)
#    
#    vars = la_input.axes[0]
#    # time = la_input.axes[timeaxis]
#    
#
#    # Copy each line of la_input into KV_WS on la_input sample
#    for v in vars:
#        #setvector(v, la_input[v])
#        # Get a pointer to the values of v (double)
#        values = <double*>np.PyArray_DATA(la_input[v].data)
#        #IodeSetVector_v2(cstr(v), values, str_per1, str_per2) # temp fn defined in the local versionof s_iode.c
#        IodeSetVector_v2(cstr(v), values, cstr(time.labels[0]), cstr(time.labels[-1]))
#        #ntime = np.PyArray_DIM(la_input.data, 1)
#        #PyIodePrint(cstr(v), values, ntime) # temp fn defined in the local version of s_iode.c
#        #print("{} = {}".format(v, la_input[v]))
#
# 
#def larray_to_ws3(la_input, timeaxis = 'time', sep="_"):
#    '''
#    Calculate the position and size of la_input data in KV_WS
#    via a call to the C fb la_to_ws_pos() to avoid calculating these
#    values for each series.
#    
#    TODO: accept Arrays s with time axis in any position.
#    
#    '''
#    cdef    double  *values
#    cdef    int     ws_pos
#    cdef    int     la_pos
#    cdef    int     la_lg
#    
#    # Retrieve the timeaxis. Stop if no timeaxis present in la_input.
#    if timeaxis not in la_input.axes:
#        raise RuntimeError(f"Passed Array object must contain an axis named {timeaxis}.\nGot axes {repr(la_input.axes)}.")
#    time = la_input.axes[timeaxis]
#
#    if(la_input.axes[-1].name != timeaxis):
#        raise RuntimeError(f"Passed Array time axis is not in last position. Passed Array axes = {repr(la_input.axes)}.")
#    
#    # Define the KV_WS sample if not yet set
#    if not IodeIsSampleSet():
#        IodeSetSampleStr(cstr(time.labels[0]), cstr(time.labels[-1]))
#    
#    # Push the time axis as last axis and combine all other axes 
#    # Warning: if timeaxis is not the last one, the data are not contiguous anymore
#    #          and the result will be erroneous
#    
#    la_input = la_input.transpose(..., timeaxis)
#    la_input = la_input.combine_axes(la_input.axes[:-1], sep=sep)
#    #print("la_input info:\n", la_input.info)
#    
#    vars = la_input.axes[0]
#    # time = la_input.axes[timeaxis]
#    
#    la_to_ws_pos(la_input, &la_pos, &ws_pos, &la_lg, timeaxis)
#    
#    # Copy each line of la_input into KV_WS on la_input sample
#    for v in vars:
#        #setvector(v, la_input[v])
#        # Get a pointer to the values of v (double)
#        values = <double*>np.PyArray_DATA(la_input[v].astype(float, copy=False).data)
#        #IodeSetVector_v2(cstr(v), values, str_per1, str_per2) # temp fn defined in the local versionof s_iode.c
#        IodeSetVector(cstr(v), values, la_pos, ws_pos, la_lg)
#        #ntime = np.PyArray_DIM(la_input.data, 1)
#        #PyIodePrint(cstr(v), values, ntime) # temp fn defined in the local versionof s_iode.c
#        #print("{} = {}".format(v, la_input[v]))


