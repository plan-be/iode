# IODE EXTENSION FOR PYTHON
# =========================
#
#    @header4iode
# 
# Interface between IODE ws and larray arrays.
# ---------------------------------------------
#   la_to_ws_pos(la_input, int* la_pos, int* ws_pos, int* la_lg, time_axis_name = 'time')
#  
#   larray_to_ws(la_input: la.Array, time_axis_name: str = 'time', sep: str = "_")   | Copies LArray la_input into IODE KV_WS.    
#   ws_to_larray(vars_pattern: str = '*', vars_axis_name: str = 'vars', time_axis_name: str = 'time', split_axis_names = '', regex = None, split_sep = None, time_as_floats: bool = False) -> la.Array | Creates an LArray from the current KV_WS content
#   ws_load_var_to_larray(filename: str, vars_pattern = '*', vars_axis_name = 'vars', time_axis_name = 'time', split_axis_names = None, regex = None, split_sep = None) -> la.Array | Load an IODE var file into an Larray object with 2 axes (vars and time)  
#   larray_get_sample(la_input, time_axis_name = 'time') -> List[Union[str,float]]             | Return the first and last time axis labels as a list of 2 strings

    

cdef la_to_ws_pos(la_input, int* la_pos, int* ws_pos, int* la_lg, time_axis_name = 'time'):
    '''
    Calls C API fonction IodeCalcSamplePosition() to determine which elements of la_input to copy into KV_WS and 
    at which position in the KV_WS sample.
    '''
      
    if time_axis_name not in la_input.axes:
        raise RuntimeError(f"Passed Array object must contain an axis named {time_axis_name}.\nGot axes {repr(la_input.axes)}.")

    time = la_input.axes[time_axis_name]
    IodeCalcSamplePosition(cstr(time.labels[0]), cstr(time.labels[-1]), la_pos, ws_pos, la_lg)
    #print(f"la_pos: {la_pos} - ws_pos: {ws_pos} - la_lg: {la_lg} ")



# TODO: check return values and return an error code 
def larray_to_ws(la_input: la.Array, time_axis_name: str = 'time', sep: str = "_"):
    '''
    Copies Array la_input into IODE KV_WS.
    
    If la_input has more than 2 dimensions, the non-time axes are grouped (with sep as separator) 
    to form the series names.
    '''
    cdef    int     ws_pos
    cdef    int     la_pos
    cdef    int     la_lg
    
    # Retrieve the time_axis_name. Stop if no time_axis_name present in la_input.
    if time_axis_name not in la_input.axes:
        raise RuntimeError(f"Passed Array object must contain an axis named {time_axis_name}.\nGot axes {repr(la_input.axes)}.")
    time = la_input.axes[time_axis_name]

    # Define the KV_WS sample if not yet set
    if not IodeIsSampleSet():
        IodeSetSampleStr(cstr(time.labels[0]), cstr(time.labels[-1]))
    
    # Push the time axis as last axis and combine all other axes 
    la_input = la_input.transpose(..., time_axis_name)
    la_input = la_input.combine_axes(la_input.axes[ : -1], sep = sep)

    # Calculate:
    #   - la_pos: the position of the first element of la_input to be copied into WS
    #   - ws_pos: the position in KV_WS sample where to copy la_input[...la_pos]
    #   - the nb of la_input values to be copied 
    la_to_ws_pos(la_input, &la_pos, &ws_pos, &la_lg, time_axis_name)

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

 
# TODO: review this function logic / parameters 
def ws_to_larray(vars_pattern: str = '*', 
                 vars_axis_name: str = 'vars',     
                 time_axis_name: str = 'time', 
                 split_axis_names = '', 
                 regex = None, 
                 split_sep = '', 
                 time_as_floats: bool = False) -> la.Array:
    '''
    Creates an LArray from the current KV_WS content with time axis labels in the IODE syntax ['2000Q2',...].
    If time_as_floats is True, the time axis labels are doubles [2000.25,...]
    '''
    cdef:
        int i
        np.ndarray[np.double_t, ndim = 2] la_data
        double[ : ,: : 1] data_view
        str v
    
    var_list = ws_content_var(vars_pattern)         # 6 = K_VAR
    
    # Define 2 original axes var and time
    axis_var = la.Axis(var_list, vars_axis_name)
    axis_time = ws_sample_to_larray_axis(axis_name = time_axis_name, as_floats = time_as_floats)

    # Create LArray with 0
    la_res = la.zeros([axis_var, axis_time])
    la_data = la_res.data
    #data_view = la_data
    # print(la_res.info)
    
    # Copy values from KV_WS to la_res
    for i in range(len(var_list)):
        v = var_list[i]
        la_data[i] = get_var_as_ndarray(v, copy = False)
          
    if split_axis_names != '':
        # split_axes(self, axes = None, sep = '_', names = None, regex = None, sort = False, fill_value = nan)
        la_res = la_res.split_axes(axis_var, sep = split_sep, names = split_axis_names, regex = regex)

    return la_res


# Creates an LArray from the current KV_WS content with **integer** sample (old version)
#def ws_to_larray_time_as_doubles(vars_pattern = '*', varsaxis = 'vars', time_axis_name = 'time', split_axis_names = '', regex = None, sep = None):
#
#    return ws_to_larray_gnl(vars_pattern = vars_pattern, varsaxis = varsaxis, time_axis_name = time_axis_name, split_axis_names = split_axis_names, regex = regex, sep = sep, time_as_doubles = True)
 

#def ws_to_larray(vars_pattern = '*', varsaxis = 'vars', time_axis_name = 'time', split_axis_names = '', regex = None, sep = None, time_as_doubles = False):
#
#    return ws_to_larray_gnl(vars_pattern = vars_pattern, varsaxis = varsaxis, time_axis_name = time_axis_name, split_axis_names = split_axis_names, regex = regex, sep = sep, time_as_doubles = time_as_doubles)

    
def ws_load_var_to_larray(filename: str, 
                            vars_pattern = '*', 
                            vars_axis_name = 'vars', 
                            time_axis_name = 'time', 
                            split_axis_names = '', 
                            regex = None, 
                            split_sep = '') -> la.Array:
    
    '''Load an IODE .var file into an Larray object with 2 axes (vars and time) 
    '''

    # Load iode var file
    ws_load_var(filename)
    
    # Create LArray 
    la_res = ws_to_larray(vars_pattern = vars_pattern, 
                    vars_axis_name = vars_axis_name, 
                    time_axis_name = time_axis_name, 
                    split_axis_names = split_axis_names, 
                    regex = regex, 
                    split_sep = split_sep)
    
    return la_res



def larray_get_sample(la_input, time_axis_name = 'time') -> List[Union[str,float]]:
    '''Return the first and last time axis labels as a list of 2 strings'''
    
    time = la_input.axes[time_axis_name]
    return [time.labels[0], time.labels[-1]]



