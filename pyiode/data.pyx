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

import warnings

from data cimport B_DataUpdate


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
        "Please use the new syntax:\ncmt_db = Comments()\ncmt_db[name] = value", DeprecationWarning)
    cmt_db = Comments()
    cmt_db[obj_name] = obj_value
    
def data_update_eqs(obj_name: str, obj_value: str):
    data_update(obj_name, obj_value, K_EQS)
    
def data_update_idt(obj_name: str, obj_value: str):
    warnings.warn("data_update_idt() is deprecated. " + 
        "Please use the new syntax:\nidt_db = Identities()\nidt_db[name] = value", DeprecationWarning)
    idt_db = Identities()
    idt_db[obj_name] = obj_value
    
def data_update_lst(obj_name: str, obj_value: str):
    warnings.warn("data_update_lst() is deprecated. " + 
        "Please use the new syntax:\nlst_db = Lists()\nlst_db[name] = value", DeprecationWarning)
    lst_db = Lists()
    lst_db[obj_name] = obj_value
    
def data_update_scl(obj_name: str, value: float = None, relax: float = None, stderr: float = None):
    warnings.warn("data_update_scl() is deprecated. " + 
        "Please use the new syntax:\nscl_db = Scalars()\nscl_db[name] = value, relax", DeprecationWarning)
    scl_db = Scalars()
    scl_db[obj_name] = value, relax

def data_update_var(varname: str, values, operation: str = "L", per_from: str = None):
    warnings.warn("data_update_var() is deprecated. " + 
        "Please use the new syntax:\nvar_db = Variables()\nvar_db[name, period] = value\n" + 
        "var_db[name, period_start:period_end] = [vector, of , values]", DeprecationWarning)
    var_db = Variables()
    var_db[varname, per_from:] = values
