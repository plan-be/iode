#  IODE EXTENSION FOR PYTHON
#  =========================
#  
#     @header4iode
#  
#  IODE LEC functions 
#  ------------------
# 
#   exec_lec(lec: str, t: int = -1) -> Union[float, List[float]]  | Compute a LEC formula using the current WS of VARs and SCLs

from lec cimport IodeExecLec, IodeExecLecT
from iode_python cimport SCR_free


def exec_lec(lec: str, t: int = -1) -> Union[float, List[float]]:
    '''
    Compute a LEC formula using the current WS of VARs and SCLs.
    The formula may be evaluate at a specific position in the sample (t) 
    or on the whole sample (if t is < 0).
    
    Parameters
    ----------
    lec: str
        LEC expression
    t: int
        if >= 0: position in the KV_WS sample of the value to be calculated
        if < 0, the full sample is returned
    
    Returns
    -------
    float or list of floats
    if t >= 0: return simple float
    if t < 0: return a list of floats
    '''
    cdef    double* cvar
    
    if t >= 0:
        return IodeExecLecT(_cstr(lec), t)   # simple value
    else:
        cvar = IodeExecLec(_cstr(lec))       # vector of calculated values    
        res = _pyfloats(cvar, variables.nb_periods)
        SCR_free(cvar)
        return res
