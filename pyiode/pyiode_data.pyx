#  IODE EXTENSION FOR PYTHON
#  =========================
#  
#     @header4iode
#  
#  IODE DATA functions (equivalent to $Data* report functions)
#  -----------------------------------------------------------
#  data_update_(obj_name:str, obj_value:str, obj_type:int)                                 Create of update an IODE object (cmt, eqs, lst, idt) 
#  data_update_cmt(obj_name:str, obj_value:str)                                            Create or update an IODE comment
#  data_update_eqs(obj_name:str, obj_value:str)                                            Create or update an IODE equation 
#  data_update_idt(obj_name:str, obj_value:str)                                            Create or update an IODE identity
#  data_update_lst(obj_name:str, obj_value:str)                                            Create or update an IODE list
#  data_update_scl(obj_name:str, value:float=None, relax:float=None, stderr:float=None)    Create or update an IODE scalar
#  data_update_var(varname:str, values, operation:str = "L", per_from:str = None)          Create or update an IODE variable starting at a specified period


# TODO (not necessarily all these functions)
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


def data_update_(obj_name:str, obj_value:str, obj_type:int): 
    r'''Update an IODE object (cmt, eqs, lst, idt) 
    
    Parameters
    ----------
    obj_name : str
        object name
        
    obj_value : str
        new content of the object
        
    obj_type : int 
        object type
        
    Returns
    -------
    int
        0 on success
        -1 on error 

    Examples
    --------
    >>> rc = iode.data_update_("ACAF", "Comment of ACAF", "C")

    '''
     
    cmd = obj_name + " " + obj_value
    return B_DataUpdate(cstr(cmd), obj_type)

def data_update_cmt(obj_name:str, obj_value:str):
    '''Create or update a comment'''
    return data_update_(obj_name, obj_value, K_CMT)
    
def data_update_eqs(obj_name:str, obj_value:str):
    return data_update_(obj_name, obj_value, K_EQS)
    
def data_update_idt(obj_name:str, obj_value:str):
    return data_update_(obj_name, obj_value, K_IDT)
    
def data_update_lst(obj_name:str, obj_value:str):
    return data_update_(obj_name, obj_value, K_LST)
    
def data_update_scl(obj_name:str, value:float=None, relax:float=None, stderr:float=None):
    cmd = obj_name + " "
    if value == None: cmd += "-- "
    else:             cmd += repr(value)

    if relax == None: relax = 1.0
    cmd += " " + repr(relax)

    if stderr == None: cmd += " -- "
    else:              cmd += " " + repr(stderr)

    return B_DataUpdate(cstr(cmd), K_SCL)


def data_update_var(varname:str, values, operation:str = "L", per_from:str = None):
    # 
    r'''Update an IODE variable starting at a specified period
    
    Parameters
    ----------
    varname : str
        variable name
        
    values : any numeric iterable (list, tupple, ndarray)
        new values to set in varname
        
    operation : str
        values transformation = {L,l = level | D,d = differences | G,g = growth rates}
        
    per_from : str
        period corresponding to values[0]
        
    Returns
    -------
    int
        0 on success
        -1 on error (no objects found ?) TODO: check this

    Examples
    --------
    >>> iode.ws_load_var("fun.var")
    >>> rc = iode.data_update_var("ACAF", [1, 2, 3.1], "2000Y1")

    '''
     
    cmd = varname + " " + operation
    
    if per_from is None:
        smpl = ws_sample()
        per_from = smpl[0]
        
    cmd = cmd + " " + per_from
    cmd = cmd + " " + repr(values)
    #for x in values:
    #    cmd += f" {x}"
 
    #print(f"Command:{cmd}")
    return B_DataUpdate(cstr(cmd), K_VAR)

