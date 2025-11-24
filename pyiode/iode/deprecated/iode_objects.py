import numpy as np
import warnings
from typing import List, Union, Optional 

from iode import (IodeType, comments, equations, identities, lists, scalars, tables, variables,
                  Equation, Scalar)
from .workspace import ws_content


def delete_objects(pattern: str = '*', obj_type: int = IodeType.VARIABLES):
    names = ws_content(pattern, obj_type)
    for name in names:
        delete_obj(name, obj_type)

# delete_<obj_type> functions
# ---------------------------

def delete_obj(obj_name: str, obj_type: int):
    if obj_type == IodeType.COMMENTS:
        delete_cmt(obj_name, obj_type)
    elif obj_type == IodeType.EQUATIONS:
        delete_eqs(obj_name, obj_type)
    elif obj_type == IodeType.IDENTITIES:
        delete_idt(obj_name, obj_type)
    elif obj_type == IodeType.LISTS:
        delete_lst(obj_name, obj_type)
    elif obj_type == IodeType.SCALARS:
        delete_scl(obj_name, obj_type)
    elif obj_type == IodeType.TABLES:
        delete_tbl(obj_name, obj_type)
    elif obj_type == IodeType.VARIABLES:
        delete_var(obj_name, obj_type)
    else:
        raise ValueError(f"IODE type {obj_type} is invalid")

def delete_cmt(name: str):
    warnings.warn("delete_cmt() is deprecated. " + 
        "Please use the new syntax: del comments[name]",DeprecationWarning, stacklevel=2)
    del comments[name]

def delete_eqs(name: str):
    warnings.warn("delete_eqs() is deprecated. " + 
        "Please use the new syntax: del equations[name]",DeprecationWarning, stacklevel=2)
    del equations[name]

def delete_idt(name: str):
    warnings.warn("delete_idt() is deprecated. " + 
        "Please use the new syntax: del identities[name]",DeprecationWarning, stacklevel=2)
    del identities[name]
    
def delete_lst(name: str):
    warnings.warn("delete_lst() is deprecated. " + 
        "Please use the new syntax: del lists[name]",DeprecationWarning, stacklevel=2)
    del lists[name]
   
def delete_scl(name: str):
    warnings.warn("delete_lst() is deprecated. " + 
        "Please use the new syntax: del scalars[name]",DeprecationWarning, stacklevel=2)
    del scalars[name]

def delete_tbl(name: str):
    warnings.warn("delete_tbl() is deprecated. " + 
        "Please use the new syntax: del tables[name]",DeprecationWarning, stacklevel=2)
    del tables[name]

def delete_var(name: str):
    warnings.warn("delete_var() is deprecated. " + 
        "Please use the new syntax: del variables[name]",DeprecationWarning, stacklevel=2)
    del variables[name]

# Set and Get IODE objects
# ------------------------

# Comments
# --------

def get_cmt(name: str) -> str:
    warnings.warn("get_cmt() is deprecated. " + 
        "Please use the new syntax:\ncomments[name]",DeprecationWarning, stacklevel=2)
    return comments[name]

def set_cmt(name: str, cmt: str):
    warnings.warn("set_cmt() is deprecated. " + 
        "Please use the new syntax:\ncomments[name] = value",DeprecationWarning, stacklevel=2)
    comments[name] = cmt

# Equations
# ---------

def get_eqs_lec(eq_name: str) -> str:
    warnings.warn("get_eqs_lec() is deprecated. " + 
        "Please use the new syntax: equations[name].lec",DeprecationWarning, stacklevel=2)
    return equations[eq_name].lec

def get_eqs(eq_name: str) -> Equation:
    warnings.warn("get_eqs() is deprecated. " + 
        "Please use the new syntax: equations[name]",DeprecationWarning, stacklevel=2)
    return equations[eq_name]

def set_eqs(eq_name: str, lec: str):
    warnings.warn("set_eqs() is deprecated. " + 
        "Please use the new syntax: equations[name] = lec",DeprecationWarning, stacklevel=2)
    equations[eq_name] = lec

# Identities
# ----------

def get_idt(name: str) -> str:
    '''Return the LEC formula of an IODE identity '''
    warnings.warn("get_idt() is deprecated. " + 
        "Please use the new syntax: identities[name]",DeprecationWarning, stacklevel=2)
    return identities[name]

def set_idt(name: str, idt: str):
    '''Update or create an identity'''
    warnings.warn("set_idt() is deprecated. " + 
        "Please use the new syntax: identities[name] = value",DeprecationWarning, stacklevel=2)
    identities[name] = idt
    
# Lists
# -----

def get_lst(name: str) -> str:
    '''Return a list as a string'''
    warnings.warn("get_lst() is deprecated. " + 
        "Please use the new syntax: lists[name]",DeprecationWarning, stacklevel=2)
    return lists[name]

def set_lst(name: str, lst: str):
    '''Update or create a list from a string'''
    warnings.warn("set_lst() is deprecated. " + 
        "Please use the new syntax: lists[name] = value",DeprecationWarning, stacklevel=2)
    lists[name] = lst

# Scalars
# -------

def get_scl(name: str) -> Scalar:
    '''Get an IODE scalar in an iode.Scalar class instance'''
    warnings.warn("get_scl() is deprecated. " + 
        "Please use the new syntax: scalars[name]",DeprecationWarning, stacklevel=2)
    return scalars[name]

def set_scl(name: str, scalar: Scalar):
    '''Create or update an IODE scalar from an iode.Scalar class instance'''
    warnings.warn("set_scl() is deprecated. " + 
        "Please use the new syntax: scalars[name] = value",DeprecationWarning, stacklevel=2)
    scalars[name] = scalar

# Variables
# ---------

def get_var(varname: str) -> List[float]:
    warnings.warn("get_var() is deprecated. " + 
        "Please use the new syntax: variables[name]",DeprecationWarning, stacklevel=2)
    return variables[varname]

# Copy (or refer to) an IODE var into a ndarray
def get_var_as_ndarray(varname: str, copy: bool = True) -> np.ndarray:
    warnings.warn("get_var_as_ndarray() is deprecated. " + 
        "Please use the new syntax: np.asarray(variables[name])",DeprecationWarning, stacklevel=2)
    return np.asarray(variables[varname])

# Copy a ndarray or a list into the global Variables database
def set_var(varname: str, py_values):
    warnings.warn("set_var() is deprecated. " + 
        "Please use the new syntax: variables[name] = values",DeprecationWarning, stacklevel=2)
    variables[varname] = py_values

def data_update(obj_name: str, obj_value: str, obj_type: int): 
    if obj_type == IodeType.COMMENTS:
        data_update_cmt(obj_name, obj_value)
    elif obj_type == IodeType.EQUATIONS:
        data_update_eqs(obj_name, obj_value)
    elif obj_type == IodeType.IDENTITIES:
        data_update_idt(obj_name, obj_value)
    elif obj_type == IodeType.LISTS:
        data_update_lst(obj_name, obj_value)
    elif obj_type == IodeType.SCALARS:
        data_update_scl(obj_name, obj_value)
    elif obj_type == IodeType.TABLES:
        raise NotImplementedError("Please use the syntax: tables[name] = table")
    elif obj_type == IodeType.VARIABLES:
        data_update_var(obj_name, obj_value)
    else:
        raise ValueError(f"IODE type {obj_type} is invalid")

# Data update
# -----------

def data_update_cmt(obj_name: str, obj_value: str):
    warnings.warn("data_update_cmt() is deprecated. " + 
        "Please use the new syntax:\ncomments[name] = value",DeprecationWarning, stacklevel=2)
    comments[obj_name] = obj_value
    
def data_update_eqs(obj_name: str, obj_value: str):
    warnings.warn("data_update_eqs() is deprecated. " + 
        "Please use the new syntax: equations[name] = new_lec",DeprecationWarning, stacklevel=2)
    equations[obj_name] = obj_value
    
def data_update_idt(obj_name: str, obj_value: str):
    warnings.warn("data_update_idt() is deprecated. " + 
        "Please use the new syntax: identities[name] = value",DeprecationWarning, stacklevel=2)
    identities[obj_name] = obj_value
    
def data_update_lst(obj_name: str, obj_value: str):
    warnings.warn("data_update_lst() is deprecated. " + 
        "Please use the new syntax: lists[name] = value",DeprecationWarning, stacklevel=2)
    lists[obj_name] = obj_value
    
def data_update_scl(obj_name: str, value: float = None, relax: float = None, stderr: float = None):
    warnings.warn("data_update_scl() is deprecated. " + 
        "Please use the new syntax: scalars[name] = value, relax",DeprecationWarning, stacklevel=2)
    scalars[obj_name] = value, relax

def data_update_var(varname: str, values, operation: str = "L", per_from: str = None):
    warnings.warn("data_update_var() is deprecated. " + 
        "Please use the new syntax: variables[name, period] = value\n" + 
        "variables[name, period_start:period_end] = [vector, of , values]",DeprecationWarning, stacklevel=2)
    variables[varname, per_from:] = values

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
            if empty, only the current global Variables database is used
            
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
        "Please use the new syntax: identities.execute(identities, from_period, " + 
        "to_period, var_files, scalar_files, trace)",DeprecationWarning, stacklevel=2)
    from_period, to_period = sample.split(';')
    identities.execute(idt_list, from_period, to_period, var_files, scl_files, trace)
