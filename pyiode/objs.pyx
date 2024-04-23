import numpy as np
import warnings


def delete_objects(pattern: str = '*', obj_type: int = VARIABLES):
    from iode import ws_content
    names = ws_content(pattern, obj_type)
    for name in names:
        delete_obj(name, obj_type)

# delete_<obj_type> functions
# ---------------------------

def delete_obj(obj_name: str, obj_type: int):
    if obj_type == 0:
        delete_cmt(obj_name, obj_type)
    elif obj_type == 1:
        delete_eqs(obj_name, obj_type)
    elif obj_type == 2:
        delete_idt(obj_name, obj_type)
    elif obj_type == 3:
        delete_lst(obj_name, obj_type)
    elif obj_type == 4:
        delete_scl(obj_name, obj_type)
    elif obj_type == 5:
        delete_tbl(obj_name, obj_type)
    elif obj_type == 6:
        delete_var(obj_name, obj_type)
    else:
        raise ValueError(f"IODE type {obj_type} is invalid")

def delete_cmt(name: str):
    warnings.warn("delete_cmt() is deprecated. " + 
        "Please use the new syntax: del cmt_db[name]", DeprecationWarning)
    cmt_db = Comments()
    del cmt_db[name]

def delete_eqs(name: str):
    warnings.warn("delete_eqs() is deprecated. " + 
        "Please use the new syntax: del eqs_db[name]", DeprecationWarning)
    eqs_db = Equations()
    del eqs_db[name]

def delete_idt(name: str):
    warnings.warn("delete_idt() is deprecated. " + 
        "Please use the new syntax: del idt_db[name]", DeprecationWarning)
    idt_db = Identities()
    del idt_db[name]
    
def delete_lst(name: str):
    warnings.warn("delete_lst() is deprecated. " + 
        "Please use the new syntax: del lst_db[name]", DeprecationWarning)
    lst_db = Lists()
    del lst_db[name]
   
def delete_scl(name: str):
    warnings.warn("delete_lst() is deprecated. " + 
        "Please use the new syntax:\nscl_db = Scalars()\n del scl_db[name]", DeprecationWarning)
    scl_db = Scalars()
    del scl_db[name]

def delete_tbl(name: str):
    warnings.warn("delete_tbl() is deprecated. " + 
        "Please use the new syntax:\ntbl_db = Tables()\n del tbl_db[name]", DeprecationWarning)
    tbl_db = Tables()
    del tbl_db[name]

def delete_var(name: str):
    warnings.warn("delete_var() is deprecated. " + 
        "Please use the new syntax: del var_db[name]", DeprecationWarning)
    var_db = Variables()
    del var_db[name]

# Set and Get IODE objects
# ------------------------

# Comments
# --------

def get_cmt(name: str) -> str:
    warnings.warn("get_cmt() is deprecated. " + 
        "Please use the new syntax:\ncmt_db = Comments()\ncmt_db[name]", DeprecationWarning)
    cmt_db = Comments()
    return cmt_db[name]

def set_cmt(name: str, cmt: str):
    warnings.warn("set_cmt() is deprecated. " + 
        "Please use the new syntax:\ncmt_db = Comments()\ncmt_db[name] = value", DeprecationWarning)
    cmt_db = Comments()
    cmt_db[name] = cmt

# Equations
# ---------

def get_eqs_lec(eq_name: str) -> str:
    warnings.warn("get_eqs_lec() is deprecated. " + 
        "Please use the new syntax:\neqs_db = Equations()\neqs_db[name].lec", DeprecationWarning)
    eqs_db = Equations()
    return eqs_db[eq_name].lec

def get_eqs(eq_name: str) -> Equation:
    warnings.warn("get_eqs() is deprecated. " + 
        "Please use the new syntax:\neqs_db = Equations()\neqs_db[name]", DeprecationWarning)
    eqs_db = Equations()
    return eqs_db[eq_name]

def set_eqs(eq_name: str, lec: str):
    warnings.warn("set_eqs() is deprecated. " + 
        "Please use the new syntax:\neqs_db = Equations()\neqs_db[name] = lec", DeprecationWarning)
    eqs_db = Equations()
    eqs_db[eq_name] = lec

# Identities
# ----------

def get_idt(name: str) -> str:
    '''Return the LEC formula of an IODE identity '''
    warnings.warn("get_idt() is deprecated. " + 
        "Please use the new syntax:\nidt_db = Identities()\nidt_db[name]", DeprecationWarning)
    idt_db = Identities()
    return idt_db[name]

def set_idt(name: str, idt: str):
    '''Update or create an identity'''
    warnings.warn("set_idt() is deprecated. " + 
        "Please use the new syntax:\nidt_db = Identities()\nidt_db[name] = value", DeprecationWarning)
    idt_db = Identities()
    idt_db[name] = idt
    
# Lists
# -----

def get_lst(name: str) -> str:
    '''Return a list as a string'''
    warnings.warn("get_lst() is deprecated. " + 
        "Please use the new syntax:\nlst_db = Lists()\nlst_db[name]", DeprecationWarning)
    lst_db = Lists()
    return lst_db[name]

def set_lst(name: str, lst: str):
    '''Update or create a list from a string'''
    warnings.warn("set_lst() is deprecated. " + 
        "Please use the new syntax:\nlst_db = Lists()\nlst_db[name] = value", DeprecationWarning)
    lst_db = Lists()
    lst_db[name] = lst

# Scalars
# -------

def get_scl(name: str) -> Scalar:
    '''Get an IODE scalar in an iode.Scalar class instance'''
    warnings.warn("get_scl() is deprecated. " + 
        "Please use the new syntax:\nscl_db = Scalars()\nscl_db[name]", DeprecationWarning)
    scl_db = Scalars()
    return scl_db[name]

def set_scl(name: str, scalar: Scalar):
    '''Create or update an IODE scalar from an iode.Scalar class instance'''
    warnings.warn("set_scl() is deprecated. " + 
        "Please use the new syntax:\nscl_db = Scalars()\nscl_db[name] = value", DeprecationWarning)
    scl_db = Scalars()
    scl_db[name] = scalar

# Variables
# ---------

def get_var(varname: str) -> List[float]:
    warnings.warn("get_var() is deprecated. " + 
        "Please use the new syntax:\nvar_db = Variables()\nvar_db[name]", DeprecationWarning)
    var_db = Variables()
    return var_db[varname]

# Copy (or refer to) an IODE var into a ndarray
def get_var_as_ndarray(varname: str, copy: bool = True) -> np.ndarray:
    warnings.warn("get_var_as_ndarray() is deprecated. " + 
        "Please use the new syntax: np.asarray(var_db[name])", DeprecationWarning)
    var_db = Variables()
    return np.asarray(var_db[varname])

# Copy a ndarray or a list into KV_WS
def set_var(varname: str, py_values):
    warnings.warn("set_var() is deprecated. " + 
        "Please use the new syntax:\nvar_db = Variables()\nvar_db[name] = values", DeprecationWarning)
    var_db = Variables()
    var_db[varname] = py_values
