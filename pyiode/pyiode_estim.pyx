# IODE EXTENSION FOR PYTHON
# =========================
#
#    @header4iode
# 
# IODE estimation function
# ------------------------
#  eqs_estimate(eq_list, afrom:str, ato:str)  | Estimate an equation or a block of equations on the given sample.

#  TODO: add optional parameters to include all available estimation parameters (method, block...)
def eqs_estimate(eq_list, afrom:str, ato:str)->int:
    '''
    Estimate an equation or a block of equations on the given sample.
    The estimation method and the instruments must be specified in the equation before the estimation.
    '''
    if type(eq_list) == list:
        eq_list = ','.join(eq_list)

    rc = IodeEstimate(cstr(eq_list), cstr(afrom), cstr(ato))
    return rc


