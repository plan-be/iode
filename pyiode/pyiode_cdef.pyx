#  IODE EXTENSION FOR PYTHON
#  =========================
#  
#     @header4iode
#  
#  Utilities using explicit C data types in arguments 
#  --------------------------------------------------
#   pylist(char** c_list)                           Convert a C vector of char* to a python list of python str
#   pyfloats(double *cvar, int lg)                  Convert a C vector of lg doubles into a list of python floats 
#   iodevar_to_ndarray(char *name, int copy = True) Create an numpy array from the content of an IODE variable
#   iodesample_to_ndarray()                         Convert the current WS sample into a numpy 1D array of doubles

cimport numpy as np
from pyiode_cdef cimport IodeGetVector
from pyiode_sample cimport IodeGetSampleAsDoubles


cdef pylist(char** c_list):
    '''
    Convert a C vector of char* to a python list of strings
    
        Parameters
        ----------
        c_list: C char** 
            table of strings (null terminated)
        
        Returns
        -------
        out: list
            list of string (C char* converted to python strings)
    '''
    cdef    int     n
     
    if c_list == NULL:
        return []
        
    # nb = number of strings in cnt
    nb = 0
    while c_list[nb] != NULL:
        nb = nb + 1
    res = [None] * nb
        
    # Assigns each C string to a value of the list res 
    nb = 0
    while c_list[nb] != NULL:
        s = bytes(c_list[nb])      
        res[nb] = pystr(s)
        nb = nb + 1
        
    return res



cdef pyfloats(double *cvar, int lg):
    '''Convert a C vector of lg doubles into a list of python floats'''
    if cvar == NULL:
        return []

    res = [np.nan] * lg
    for i in range(lg):
        if cvar[i] >= -1.0e37: 
            res[i] = cvar[i]
        
    return res    


cdef iodevar_to_ndarray(char * name, bint copy = True):
    '''
    Create an numpy array from the content of an IODE variable (KV_WS[name]). 
    The ndarray data may be either a newly allocated vector, or may point to the IODE memory.
    Be aware that IODE data can be relocated in memory when creating new objects.
    
        Parameters
        ----------
        name: C char* 
            Name of the IODE variable to read 
        copy: C int 
            if not null, a pointer to the IODE data is placed in the resulting ndarray
            if null, the data is allocated 
    
        Returns
        -------
        out: ndarray 
            1-dim numpy array containing the values of KV_WS[name]
    '''
    
    cdef np.npy_intp shape[1]   # int of the size of a pointer (32 ou 64 bits) See https://numpy.org/doc/stable/reference/c-api/dtype.html
    cdef int lg
    cdef double *data_ptr = IodeGetVector(name, &lg)
    
    shape[0] = <np.npy_intp> lg  # vector length
    if data_ptr != NULL:
        # Create an array wrapper around data_ptr 
        # See https://numpy.org/doc/stable/reference/c-api/array.html?highlight=pyarray_simplenewfromdata#c.PyArray_SimpleNewFromData
        vararray = np.PyArray_SimpleNewFromData(1, shape, np.NPY_DOUBLE, data_ptr) # See https://numpy.org/doc/stable/reference/c-api/array.html?highlight=pyarray_simplenewfromdata#c.PyArray_SimpleNewFromData
        
        if(copy):
            # Duplicates if inmemory == 0 (?) => ch³anges to vararray will NOT modify KV_WS
            vararray = vararray.copy()
        vararray[vararray < -1.0e37] = np.nan
    else:
        vararray = np.ndarray(lg, dtype='double')        
        #vararray.fill(0)        
        vararray.fill(np.nan)
    return vararray

 
# Obsolete ? 
# ----------
cdef iodesample_to_ndarray():
    '''Convert the current WS sample into a numpy 1D array of doubles'''
    
    cdef np.npy_intp shape[1]
    cdef int lg
    cdef double *data_ptr = IodeGetSampleAsDoubles(&lg)

    shape[0] = <np.npy_intp> lg
    if data_ptr != NULL:
        vararray = np.PyArray_SimpleNewFromData(1, shape, np.NPY_DOUBLE, data_ptr)
        vararray = vararray.copy()
        vararray[vararray < -1.0e37] = np.nan
    else:
        vararray = np.ndarray(lg, dtype='double')
        vararray.fill(0)
    return vararray

