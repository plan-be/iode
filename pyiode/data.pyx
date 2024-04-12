import warnings


def data_update(obj_name: str, obj_value: str, obj_type: int): 
    if obj_type == 0:
        data_update_cmt(obj_name, obj_value)
    elif obj_type == 1:
        data_update_eqs(obj_name, obj_value)
    elif obj_type == 2:
        data_update_idt(obj_name, obj_value)
    elif obj_type == 3:
        data_update_lst(obj_name, obj_value)
    elif obj_type == 4:
        data_update_scl(obj_name, obj_value)
    elif obj_type == 5:
        raise NotImplementedError("Please use the syntax:\ntbl_db = Tables()\ntbl_db[name] = table")
    elif obj_type == 6:
        data_update_var(obj_name, obj_value)
    else:
        raise ValueError(f"IODE type {obj_type} is invalid")

def data_update_cmt(obj_name: str, obj_value: str):
    warnings.warn("data_update_cmt() is deprecated. " + 
        "Please use the new syntax:\ncmt_db = Comments()\ncmt_db[name] = value", DeprecationWarning)
    cmt_db = Comments()
    cmt_db[obj_name] = obj_value
    
def data_update_eqs(obj_name: str, obj_value: str):
    warnings.warn("data_update_eqs() is deprecated. " + 
        "Please use the new syntax:\neqs_db = Equations()\neqs_db[name] = new_lec", DeprecationWarning)
    eqs_db = Equations()
    eqs_db[obj_name] = obj_value
    
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
