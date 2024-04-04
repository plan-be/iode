#  IODE EXTENSION FOR PYTHON
#  =========================
#  
#     @header4iode
#  
#  Utilities using explicit C data types in arguments 
#  --------------------------------------------------
#   _pylist(char** c_list)                           Convert a C vector of char* to a python list of python str
#   _pyfloats(double *cvar, int lg)                  Convert a C vector of lg doubles into a list of python floats 

cimport numpy as np
from _cdef cimport IodeGetVector
from sample cimport IodeGetSampleAsDoubles


cdef _pylist(char** c_list):
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
        res[nb] = _pystr(s)
        nb = nb + 1
        
    return res



cdef _pyfloats(double *cvar, int lg):
    '''Convert a C vector of lg doubles into a list of python floats'''
    if cvar == NULL:
        return []

    res = [np.nan] * lg
    for i in range(lg):
        if cvar[i] >= -1.0e37: 
            res[i] = cvar[i]
        
    return res    
