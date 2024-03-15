#  IODE EXTENSION FOR PYTHON
#  =========================
#  
#     @header4iode
#  
#  IODE DATA functions (equivalent to $Data* report functions)
#  -----------------------------------------------------------
#  data_update(obj_name: str, obj_value: str, obj_type: int)                                 Create of update an IODE object (cmt, eqs, lst, idt) 
#  data_update_cmt(obj_name: str, obj_value: str)                                            Create or update an IODE comment
#  data_update_eqs(obj_name: str, obj_value: str)                                            Create or update an IODE equation 
#  data_update_idt(obj_name: str, obj_value: str)                                            Create or update an IODE identity
#  data_update_lst(obj_name: str, obj_value: str)                                            Create or update an IODE list
#  data_update_scl(obj_name: str, value: float = None, relax: float = None, stderr: float = None)    Create or update an IODE scalar
#  data_update_var(varname: str, values, operation: str = "L", per_from: str = None)          Create or update an IODE variable starting at a specified period


# TODO (not necessarily all of these functions)
# ----
#  data_update_tbl
#  data_create_* 
#  data_delete_*
#  data_exist_* 
#  data_update_
#  data_append_*
#  data_duplicate_*
#  data_rename_* 
#  data_search
#  data_scan  
#  data_list_*
#  data_list_sort   
#  data_compare_eps 
#  data_compare_* 
#  data_calc_lst      
#  data_calc_var      
#  data_ras_var      
#  data_pattern_*

import warnings

from pyiode_data cimport B_DataUpdate


def data_update(obj_name: str, obj_value: str, obj_type: int): 
    r'''Update an IODE object (cmt, eqs, lst, idt) 
    
    Parameters
    ----------
    obj_name: str
        object name
        
    obj_value: str
        new content of the object
        
    obj_type: int 
        object type
        
    Returns
    -------
    Runtime exception raised on error.

    Examples
    --------
    >>> iode.data_update("ACAF", "Comment of ACAF", iode.K_CMT)

    '''
     
    cmd = obj_name + " " + obj_value
    if B_DataUpdate(_cstr(cmd), obj_type):
        raise RuntimeError(f"{obj_name} update failed")

def data_update_cmt(obj_name: str, obj_value: str):
    warnings.warn("data_update_cmt() is deprecated. " + 
        "Please use the new syntax: Comments[name] = value", DeprecationWarning)
    Comments[obj_name] = obj_value
    
def data_update_eqs(obj_name: str, obj_value: str):
    data_update(obj_name, obj_value, K_EQS)
    
def data_update_idt(obj_name: str, obj_value: str):
    data_update(obj_name, obj_value, K_IDT)
    
def data_update_lst(obj_name: str, obj_value: str):
    data_update(obj_name, obj_value, K_LST)
    
def data_update_scl(obj_name: str, value: float = None, relax: float = None, stderr: float = None):
    cmd = obj_name + " "
    if value is None: cmd += "-- "
    else:             cmd += repr(value)

    if relax is None: relax = 1.0
    cmd += " " + repr(relax)

    if stderr is None: cmd += " -- "
    else:              cmd += " " + repr(stderr)

    if B_DataUpdate(_cstr(cmd), K_SCL):
        raise RuntimeError(f"Scalar {obj_name} update failed")

def data_update_var(varname: str, values, operation: str = "L", per_from: str = None):
    # 
    r'''Update an IODE variable starting at a specified period
    
    Parameters
    ----------
    varname: str
        variable name
        
    values: any numeric iterable (list, tuple, ndarray)
        new values to set in varname
        
    operation: str
        values transformation = {L,l = level | D,d = differences | G,g = growth rates}
        
    per_from: str
        period corresponding to values[0]
        
    Returns
    -------
    Runtime exception raised on error.

    Examples
    --------
    >>> import iode
    >>> iode.ws_load_var("../data/fun.var")
    394
    >>> ACAF = iode.get_var("ACAF")
    >>> ACAF[:7]
    [nan, nan, nan, nan, nan, nan, nan]
    >>> iode.data_update_var("ACAF", [1, 2, 3.1], "L", "1962Y1")
    >>> ACAF = iode.get_var("ACAF")
    >>> ACAF[:7]
    [nan, nan, 1.0, 2.0, 3.1000000000000001, nan, nan]
    ''' 
    cmd = varname + " " + operation
    
    if per_from is None:
        smpl = ws_sample_get()
        per_from = smpl[0]
        
    cmd = cmd + " " + per_from
    cmd = cmd + " " + repr(values)[1:-1]
 
    #print(f"Command:{cmd}")
    if B_DataUpdate(_cstr(cmd), K_VAR) != 0:
        raise RuntimeError(f"Variable {varname} update failed")

