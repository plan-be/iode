#  IODE EXTENSION FOR PYTHON
#  =========================
#  
#     @header4iode
#  
#  IODE LEC functions 
#  ------------------
# 
#   exec_lec(lec:str, t:int=-1)     Compute a LEC formula using the current WS of VARs and SCLs
#


def exec_lec(lec:str, t:int=-1):
    '''
    Compute a LEC formula using the current WS of VARs and SCLs.
    The formula may be evaluate at a specific position in the sample (t) 
    or on the whole sample (if t is < 0).
    
    Parameters
    ----------
        lec: str
            LEC expression
        t: int
            if >= 0:position in the KV_WS sample of the value to be calculated
            if < 0, the full sample is returned
    
        Returns
        -------
        out: float or list of floats
            if t >= 0: return simple float
            if t < 0: return a list of floats
    '''
    
    cdef    double* cvar
    
    if t >= 0:
        return IodeExecLecT(cstr(lec), t)   # simple value
    else:
        cvar = IodeExecLec(cstr(lec))       # vector of calculated values    
        res = cdouble_to_list(cvar, IodeGetSampleLength())
        SCR_free(cvar)
        return res