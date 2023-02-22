#  IODE EXTENSION FOR PYTHON
#  =========================
#
#    @header4iode
# 
#  Functions to convert IODE objects from and to python variables.
#  
#  For each object, at least 3 functions are provided: 
#   - py_to_<object_type>() 
#   - <object_type>_to_py()
#   - delete_<object_type>
#  
#  where <object_type> is one of {cmt, eq, idt, lst, scl, tbl, var}.
#  
#  The associated C functions are provided in the source file b_api.c which is part of iodeapi.lib.
#   
#   delete_objects(pattern:str='*', obj_type:int=K_VAR)     delete the objects whose names satisfy the given pattern
#
#   delete_obj(name:str, obj_type:int)->int   | delete the object named name of type obj_type
#   delete_cmt(name:str)->int                 | delete the comment named name
#   delete_eqs(name:str)->int                 | delete the equation named name
#   delete_idt(name:str)->int                 | delete the identity named name
#   delete_lst(name:str)->int                 | delete the list named name
#   delete_scl(name:str)->int                 | delete the scalar named name
#   delete_tbl(name:str)->int                 | delete the table named name
#   delete_var(name:str)->int                 | delete the variable named name
#       
#   cmt_to_py(name)                                         
#   py_to_cmt(name, cmt)
#  
#   eqs_lec_to_py(eq_name:str)
#   eqs_to_py(eq_name:str)
#   py_to_eqs(eq_name:str, lec:str)
#  
#   idt_to_py(name)
#   py_to_idt(name, idt)
#  
#   lst_to_py(name)
#   py_to_lst(name, lst)
#  
#   scl_to_py(name:str)
#   py_to_scl(py_scl:scl)
#  
#   var_to_py(varname)
#   var_to_ndarray(varname, copy = True) 
#   py_to_var(varname, py_values)


def delete_objects(pattern:str='*', obj_type:int=K_VAR)->int:
    r'''Delete one or more IODE objects corresponding to the specified pattern.
    
    Parameters
    ----------
    pattern : str
        string containing wildcards characters like '*' or '?'. 
        Default '*', meaning "all objects".
        
    obj_type : int
        IODE object type (0-6, 0 for comments...)
        Default 6 for variables
        
    Returns
    -------
    int
        0 on success
        -1 on error (no objects found ?) TODO: check this

    Examples
    --------
    >>> iode.wsloadidt("fun.idt")
    >>> nbobjs = len(iode.ws_content('X*', 2))
    >>> rc = iode.delete_objects('X*', 2)
    >>> nbobjs2 = len(iode.ws_content('X*', 2))
    >>> print(f"iode.delete_objects('X*', 2): before: {nbobjs}. After: {nbobjs2}")
    iode.delete_objects('X*', 2): before: 13. After: 0
    '''
    return B_DataDelete(cstr(pattern), obj_type)

# delete_<obj_type> functions
# ---------------------------
def delete_obj(obj_name:str, obj_type:int)->int:
    return IodeDeleteObj(cstr(obj_name), obj_type)

def delete_cmt(name:str)->int:
    return delete_obj(name, K_CMT)

def delete_eqs(name:str)->int:
    return delete_obj(name, K_EQS)

def delete_idt(name:str)->int:
    return delete_obj(name, K_IDT)
    
def delete_lst(name:str)->int:
    return delete_obj(name, K_LST)
   
def delete_scl(name:str)->int:
    return delete_obj(name, K_SCL)

def delete_tbl(name:str)->int:
    return delete_obj(name, K_TBL)

def delete_var(name:str)->int:
    return delete_obj(name, K_VAR)
   

# Set and Get IODE objects
# ------------------------

# Comments
# --------
def cmt_to_py(name:str)->str:
    '''Return the text of an IODE comment'''
    
    cmt = IodeGetCmt(cstr(name))
    return cmt.decode("cp850")

def py_to_cmt(name:str, cmt:str)->int:
    '''Update or create an IODE comment from a python str'''
    
    cmt850 = cmt.encode("cp850")
    return IodeSetCmt(cstr(name), cmt850)

# Equations
# ---------

def eqs_lec_to_py(eq_name:str)->str:
    '''Return an IODE equation LEC form as a string'''
    
    lec = IodeGetEqsLec(cstr(eq_name))
    return pystr(lec)

def eqs_to_py(eq_name:str)->eqs:
    '''Return an IODE equation as an iode.eqs class instance'''
    
    cdef char   *lec, 
    cdef int    method
    cdef char   sample_from[20]
    cdef char   sample_to[20]
    cdef char   *blk
    cdef char   *instr
    cdef float  tests[20]
    
    rc = IodeGetEqs(cstr(eq_name), &lec, &method, sample_from, sample_to, &blk, &instr, tests)
    if rc != 0:
        return None
        
    eq_res = eqs(eq_name, pystr(lec), method, pystr(sample_from), pystr(sample_to),  
                 pystr(blk), pystr(instr),
                 tests[1],
                 tests[2],
                 tests[3],
                 tests[4], # Pas de 5 !
                 tests[6],
                 tests[7],
                 tests[8],
                 tests[9],
                 tests[10])

    return eq_res


# TODO: save other (optional) equation properties like tests, sample, method
def py_to_eqs(eq_name:str, lec:str)->int:
    lec850 = lec.encode("cp850")
    return IodeSetEqs(cstr(eq_name), lec850)

# Identities
# ----------
def idt_to_py(name:str)->str:
    str = IodeGetIdt(cstr(name))
    return str.decode("cp850")

def py_to_idt(name:str, idt:str)->int:
    idt850 = idt.encode("cp850")
    return IodeSetIdt(cstr(name), idt850)
    
# Lists
# -----
def lst_to_py(name:str)->str:
    str = IodeGetLst(cstr(name))
    return str.decode("cp850")

def py_to_lst(name, lst)->int:
    lst850 = lst.encode("cp850")
    return IodeSetLst(cstr(name), lst850)


# Scalars
# -------
def scl_to_py(name:str)->scl:
    #vararray = iodescl_to_ndarray(cstr(name))
    #return vararray
    cdef    double cval
    cdef    double crelax
    cdef    double cstd
    
    cdef rc = IodeGetScl(cstr(name), &cval, &crelax, &cstd)
    if rc == 0:
        res = scl(name, cval, crelax, cstd)
        return res
    else:
        return None


def py_to_scl(py_scl:scl)->int:
    
    return IodeSetScl(cstr(py_scl.name), py_scl.value, py_scl.relax, py_scl.std)

    
# Tables
# ------
# ... not yet implemented ...
# TODO:
#    - tbl_add_line(tbl_name, after_line, [cells], line_type=KT_CELLS)
#    - tbl_add_lines(tbl_name, after_line, [[cells], [cells]...])
#    - tbl_del_line(tbl_name, line)
#    - tbl_create(tbl_name, title, [line1, line2...])
#    - tbl_get_lines(tbl_nname, from_line, to_line)
#    - tbl_get_line_type(tbl_nname, line)  


# Variables
# ---------

# Copy an IODE var into a list
def var_to_py(varname)->List[np.ndarray]:
    ndarray = var_to_ndarray(varname, True)
    return list(ndarray)

# Copy (or refer to) an IODE var into a ndarray
def var_to_ndarray(varname, copy = True)->List[np.ndarray]:
    vararray = iodevar_to_ndarray(cstr(varname), copy)
    return(vararray)

# Copy a ndarray or a list into KV_WS
def py_to_var(varname, py_values)->int:
    cdef double  *c_values
    
    if not IodeIsSampleSet():
        raise RuntimeError("py_to_var impossible : IODE sample not set")
        
    ndarray = np.array(py_values, dtype=np.double) 
    
    c_values = <double*>np.PyArray_DATA(ndarray)
    IodeSetVector(cstr(varname), c_values, 0, 0, -1)
    
