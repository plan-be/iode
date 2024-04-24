import numpy as np
import warnings
from typing import List, Union, Optional 

from iode import (COMMENTS, EQUATIONS, IDENTITIES, LISTS, SCALARS, TABLES, VARIABLES, 
                  Comments, Equations, Identities, Lists, Scalars, Tables, 
                  Variables, Equation, Scalar)
from .workspace import ws_content


def delete_objects(pattern: str = '*', obj_type: int = VARIABLES):
    names = ws_content(pattern, obj_type)
    for name in names:
        delete_obj(name, obj_type)

# delete_<obj_type> functions
# ---------------------------

def delete_obj(obj_name: str, obj_type: int):
    if obj_type == COMMENTS:
        delete_cmt(obj_name, obj_type)
    elif obj_type == EQUATIONS:
        delete_eqs(obj_name, obj_type)
    elif obj_type == IDENTITIES:
        delete_idt(obj_name, obj_type)
    elif obj_type == LISTS:
        delete_lst(obj_name, obj_type)
    elif obj_type == SCALARS:
        delete_scl(obj_name, obj_type)
    elif obj_type == TABLES:
        delete_tbl(obj_name, obj_type)
    elif obj_type == VARIABLES:
        delete_var(obj_name, obj_type)
    else:
        raise ValueError(f"IODE type {obj_type} is invalid")

def delete_cmt(name: str):
    warnings.warn("delete_cmt() is deprecated. " + 
        "Please use the new syntax: del cmt_db[name]",DeprecationWarning, stacklevel=2)
    cmt_db = Comments()
    del cmt_db[name]

def delete_eqs(name: str):
    warnings.warn("delete_eqs() is deprecated. " + 
        "Please use the new syntax: del eqs_db[name]",DeprecationWarning, stacklevel=2)
    eqs_db = Equations()
    del eqs_db[name]

def delete_idt(name: str):
    warnings.warn("delete_idt() is deprecated. " + 
        "Please use the new syntax: del idt_db[name]",DeprecationWarning, stacklevel=2)
    idt_db = Identities()
    del idt_db[name]
    
def delete_lst(name: str):
    warnings.warn("delete_lst() is deprecated. " + 
        "Please use the new syntax: del lst_db[name]",DeprecationWarning, stacklevel=2)
    lst_db = Lists()
    del lst_db[name]
   
def delete_scl(name: str):
    warnings.warn("delete_lst() is deprecated. " + 
        "Please use the new syntax:\nscl_db = Scalars()\n del scl_db[name]",DeprecationWarning, stacklevel=2)
    scl_db = Scalars()
    del scl_db[name]

def delete_tbl(name: str):
    warnings.warn("delete_tbl() is deprecated. " + 
        "Please use the new syntax:\ntbl_db = Tables()\n del tbl_db[name]",DeprecationWarning, stacklevel=2)
    tbl_db = Tables()
    del tbl_db[name]

def delete_var(name: str):
    warnings.warn("delete_var() is deprecated. " + 
        "Please use the new syntax: del var_db[name]",DeprecationWarning, stacklevel=2)
    var_db = Variables()
    del var_db[name]

# Set and Get IODE objects
# ------------------------

# Comments
# --------

def get_cmt(name: str) -> str:
    warnings.warn("get_cmt() is deprecated. " + 
        "Please use the new syntax:\ncmt_db = Comments()\ncmt_db[name]",DeprecationWarning, stacklevel=2)
    cmt_db = Comments()
    return cmt_db[name]

def set_cmt(name: str, cmt: str):
    warnings.warn("set_cmt() is deprecated. " + 
        "Please use the new syntax:\ncmt_db = Comments()\ncmt_db[name] = value",DeprecationWarning, stacklevel=2)
    cmt_db = Comments()
    cmt_db[name] = cmt

# Equations
# ---------

def get_eqs_lec(eq_name: str) -> str:
    warnings.warn("get_eqs_lec() is deprecated. " + 
        "Please use the new syntax:\neqs_db = Equations()\neqs_db[name].lec",DeprecationWarning, stacklevel=2)
    eqs_db = Equations()
    return eqs_db[eq_name].lec

def get_eqs(eq_name: str) -> Equation:
    warnings.warn("get_eqs() is deprecated. " + 
        "Please use the new syntax:\neqs_db = Equations()\neqs_db[name]",DeprecationWarning, stacklevel=2)
    eqs_db = Equations()
    return eqs_db[eq_name]

def set_eqs(eq_name: str, lec: str):
    warnings.warn("set_eqs() is deprecated. " + 
        "Please use the new syntax:\neqs_db = Equations()\neqs_db[name] = lec",DeprecationWarning, stacklevel=2)
    eqs_db = Equations()
    eqs_db[eq_name] = lec

# Identities
# ----------

def get_idt(name: str) -> str:
    '''Return the LEC formula of an IODE identity '''
    warnings.warn("get_idt() is deprecated. " + 
        "Please use the new syntax:\nidt_db = Identities()\nidt_db[name]",DeprecationWarning, stacklevel=2)
    idt_db = Identities()
    return idt_db[name]

def set_idt(name: str, idt: str):
    '''Update or create an identity'''
    warnings.warn("set_idt() is deprecated. " + 
        "Please use the new syntax:\nidt_db = Identities()\nidt_db[name] = value",DeprecationWarning, stacklevel=2)
    idt_db = Identities()
    idt_db[name] = idt
    
# Lists
# -----

def get_lst(name: str) -> str:
    '''Return a list as a string'''
    warnings.warn("get_lst() is deprecated. " + 
        "Please use the new syntax:\nlst_db = Lists()\nlst_db[name]",DeprecationWarning, stacklevel=2)
    lst_db = Lists()
    return lst_db[name]

def set_lst(name: str, lst: str):
    '''Update or create a list from a string'''
    warnings.warn("set_lst() is deprecated. " + 
        "Please use the new syntax:\nlst_db = Lists()\nlst_db[name] = value",DeprecationWarning, stacklevel=2)
    lst_db = Lists()
    lst_db[name] = lst

# Scalars
# -------

def get_scl(name: str) -> Scalar:
    '''Get an IODE scalar in an iode.Scalar class instance'''
    warnings.warn("get_scl() is deprecated. " + 
        "Please use the new syntax:\nscl_db = Scalars()\nscl_db[name]",DeprecationWarning, stacklevel=2)
    scl_db = Scalars()
    return scl_db[name]

def set_scl(name: str, scalar: Scalar):
    '''Create or update an IODE scalar from an iode.Scalar class instance'''
    warnings.warn("set_scl() is deprecated. " + 
        "Please use the new syntax:\nscl_db = Scalars()\nscl_db[name] = value",DeprecationWarning, stacklevel=2)
    scl_db = Scalars()
    scl_db[name] = scalar

# Variables
# ---------

def get_var(varname: str) -> List[float]:
    warnings.warn("get_var() is deprecated. " + 
        "Please use the new syntax:\nvar_db = Variables()\nvar_db[name]",DeprecationWarning, stacklevel=2)
    var_db = Variables()
    return var_db[varname]

# Copy (or refer to) an IODE var into a ndarray
def get_var_as_ndarray(varname: str, copy: bool = True) -> np.ndarray:
    warnings.warn("get_var_as_ndarray() is deprecated. " + 
        "Please use the new syntax: np.asarray(var_db[name])",DeprecationWarning, stacklevel=2)
    var_db = Variables()
    return np.asarray(var_db[varname])

# Copy a ndarray or a list into KV_WS
def set_var(varname: str, py_values):
    warnings.warn("set_var() is deprecated. " + 
        "Please use the new syntax:\nvar_db = Variables()\nvar_db[name] = values",DeprecationWarning, stacklevel=2)
    var_db = Variables()
    var_db[varname] = py_values

def data_update(obj_name: str, obj_value: str, obj_type: int): 
    if obj_type == COMMENTS:
        data_update_cmt(obj_name, obj_value)
    elif obj_type == EQUATIONS:
        data_update_eqs(obj_name, obj_value)
    elif obj_type == IDENTITIES:
        data_update_idt(obj_name, obj_value)
    elif obj_type == LISTS:
        data_update_lst(obj_name, obj_value)
    elif obj_type == SCALARS:
        data_update_scl(obj_name, obj_value)
    elif obj_type == TABLES:
        raise NotImplementedError("Please use the syntax:\ntbl_db = Tables()\ntbl_db[name] = table")
    elif obj_type == VARIABLES:
        data_update_var(obj_name, obj_value)
    else:
        raise ValueError(f"IODE type {obj_type} is invalid")

# Data update
# -----------

def data_update_cmt(obj_name: str, obj_value: str):
    warnings.warn("data_update_cmt() is deprecated. " + 
        "Please use the new syntax:\ncmt_db = Comments()\ncmt_db[name] = value",DeprecationWarning, stacklevel=2)
    cmt_db = Comments()
    cmt_db[obj_name] = obj_value
    
def data_update_eqs(obj_name: str, obj_value: str):
    warnings.warn("data_update_eqs() is deprecated. " + 
        "Please use the new syntax:\neqs_db = Equations()\neqs_db[name] = new_lec",DeprecationWarning, stacklevel=2)
    eqs_db = Equations()
    eqs_db[obj_name] = obj_value
    
def data_update_idt(obj_name: str, obj_value: str):
    warnings.warn("data_update_idt() is deprecated. " + 
        "Please use the new syntax:\nidt_db = Identities()\nidt_db[name] = value",DeprecationWarning, stacklevel=2)
    idt_db = Identities()
    idt_db[obj_name] = obj_value
    
def data_update_lst(obj_name: str, obj_value: str):
    warnings.warn("data_update_lst() is deprecated. " + 
        "Please use the new syntax:\nlst_db = Lists()\nlst_db[name] = value",DeprecationWarning, stacklevel=2)
    lst_db = Lists()
    lst_db[obj_name] = obj_value
    
def data_update_scl(obj_name: str, value: float = None, relax: float = None, stderr: float = None):
    warnings.warn("data_update_scl() is deprecated. " + 
        "Please use the new syntax:\nscl_db = Scalars()\nscl_db[name] = value, relax",DeprecationWarning, stacklevel=2)
    scl_db = Scalars()
    scl_db[obj_name] = value, relax

def data_update_var(varname: str, values, operation: str = "L", per_from: str = None):
    warnings.warn("data_update_var() is deprecated. " + 
        "Please use the new syntax:\nvar_db = Variables()\nvar_db[name, period] = value\n" + 
        "var_db[name, period_start:period_end] = [vector, of , values]",DeprecationWarning, stacklevel=2)
    var_db = Variables()
    var_db[varname, per_from:] = values

# Miscellaneous
# -------------

def idt_execute(sample: Optional[Union[str, List[str]]] = None, 
                idt_list: Optional[Union[str, List[str]]] = None, 
                var_files: Optional[Union[str, List[str]]] = None, 
                scl_files: Optional[Union[str, List[str]]] = None,
                trace: bool = False):
    r'''
    Execute a list of identities
    
    Parameters
    ----------
    sample: Optional[Union[str, List[str]]] = None
            range of period on which the identities must be calculated
            if sample is None or empty, the WS sample is used 
            
    idt_list: Optional[Union[str, List[str]]] = None
            list of identities to execute
            if empty, all identities are executed
            
    var_files: Optional[Union[str, List[str]]] = None
            list of files the needed variables must be read from. "WS" means current current Variable WS.
            if empty, only the current KV_WS is used
            
    scl_files: Optional[Union[str, List[str]]] = None
            list of files the needed scalars must be read from. "WS" means current current WS.
            if empty, only the current workspace is used
                        
    trace: bool = False
            optional trace indicating the source of the variables and scalars. For example:
            
            Execution of identities
            
            Parameters
                Execution sample : 1960Y1:2015Y1
            
                Variables loaded
                    From WS : AOUC AOUC_ COTRES DEBT DTF DTH EFMY EFXY EX FLG FLGR GAP2 GAP_ GOSF HF  
                    
                Scalars loaded
                    From ../data/fun : gamma gamma2 gamma3 gamma4 knf3
    '''
    warnings.warn("idt_execute() is deprecated. " + 
        "Please use the new syntax:\nidt_db = Identities()\nidt_db.execute(identities, from_period, " + 
        "to_period, var_files, scalar_files, trace)",DeprecationWarning, stacklevel=2)
    idt_db = Identities()
    from_period, to_period = sample.split(';')
    idt_db.execute(idt_list, from_period, to_period, var_files, scl_files, trace)
