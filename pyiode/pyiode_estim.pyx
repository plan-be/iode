# IODE EXTENSION FOR PYTHON
# =========================
#
#    @header4iode
# 
# IODE estimation function
# ------------------------
#  eqs_estimate(eq_list, afrom:str, ato:str)  | Estimate an equation or a block of equations on the given sample.

from pyiode_estim cimport IodeEstimate


#  TODO: add optional parameters to include all available estimation parameters (method, block...)
def eqs_estimate(eq_list: Union[str, List[str]], afrom: str, ato: str):
    '''
    Estimate an equation or a block of equations on the given sample.
    The estimation method and the instruments must be specified in the equation before the estimation.
    '''
    eq_list = arg_to_str(eq_list, sep = ',')
    
    if IodeEstimate(cstr(eq_list), cstr(afrom), cstr(ato)):
        raise RuntimeError(f"Estimation of {eq_list} failed")


