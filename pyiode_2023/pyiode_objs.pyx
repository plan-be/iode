#  IODE EXTENSION FOR PYTHON
#  =========================
#
#    @header4iode
# 
#  Functions to convert IODE objects from and to python variables/classes.
#  
#  For each object, at least 3 functions are provided: 
#   - set_<object_type>()       
#   - get_<object_type>_to_py() 
#   - delete_<object_type>()      
#  
#  where <object_type> is one of {cmt, eq, idt, lst, scl, tbl, var}.
#  
#  The associated C functions are provided in the source file b_api.c which is part of iodeapi.lib.
#   
#  List of functions
#  -----------------
#   delete_objects(pattern: str = '*', obj_type: int = VARIABLES)  |   delete the objects whose names satisfy the given pattern
#
#   delete_obj(name: str, obj_type: int) | delete the object named name of type obj_type
#   delete_cmt(name: str)                | delete the comment named name
#   delete_eqs(name: str)                | delete the equation named name
#   delete_idt(name: str)                | delete the identity named name
#   delete_lst(name: str)                | delete the list named name
#   delete_scl(name: str)                | delete the scalar named name
#   delete_tbl(name: str)                | delete the table named name
#   delete_var(name: str)                | delete the variable named name
#       
#   get_cmt(name) -> str                 | return the text of an IODE comment                                        
#   set_cmt(name, cmt)                   | update or create an IODE comment from a python str
#  
#   get_eqs_lec(eq_name: str) -> str     | return an IODE equation LEC form as a string
#   get_eqs(eq_name: str) -> Equation    | return an IODE equation as an iode.Equation class instance
#   set_eqs(eq_name: str, lec: str)      | update an equation lec form
#  
#   get_idt(name) -> str                 | return the LEC formula of an IODE identity 
#   set_idt(name, idt)                   | update or create an identity
#  
#   get_lst(name) -> str                 | return an IODE list as a string
#   set_lst(name, lst)                   | update or create a list fro a string
#  
#   get_scl(name: str) -> Scalar         | return an IODE scalar in a iode.Scalar class instance
#   set_scl(py_scl: Scalar)              | create or update an IODE scalar from an iode.Scalar class instance
#  
#   get_var(varname: str) -> List[float] | get an IODE variable in a list of floats  
#   get_var_as_ndarray(varname: str, copy = True) -> np.ndarray | get an IODE variable in a numpy ndarray
#   set_var(varname, py_values)          | create or update an IODE variable from a list of floats or a ndarray


def delete_objects(pattern: str = '*', obj_type: int = VARIABLES):
    r'''Delete one or more IODE objects corresponding to the specified pattern.
    
    Parameters
    ----------
    pattern: str
        string containing wildcards characters like '*' or '?'. 
        Default '*', meaning "all objects".
        
    obj_type: int
        IODE object type (0-6, 0 for comments...)
        Default 6 for variables
        
    Returns
    -------
    Raise an exception on error

    Examples
    --------
    >>> iode.wsloadidt("fun.idt")
    >>> nbobjs = len(iode.ws_content('X*', IDENTITIES))
    >>> iode.delete_objects('X*', IDENTITIES)
    >>> nbobjs2 = len(iode.ws_content('X*', IDENTITIES))
    >>> print(f"iode.delete_objects('X*', IDENTITIES): before: {nbobjs}. After: {nbobjs2}")
    iode.delete_objects('X*', IDENTITIES2): before: 13. After: 0
    '''
    if B_DataDelete(cstr(pattern), obj_type):
        raise RuntimeError(f"Delete '{pattern}' of type {obj_type} failed")

# delete_<obj_type> functions
# ---------------------------
def delete_obj(obj_name: str, obj_type: int):
    if IodeDeleteObj(cstr(obj_name), obj_type): 
        raise RuntimeError(f"Variable {obj_name} cannot be deleted")

def delete_cmt(name: str):
    delete_obj(name, COMMENTS)

def delete_eqs(name: str):
    return delete_obj(name, EQUATIONS)

def delete_idt(name: str):
    return delete_obj(name, IDENTITIES)
    
def delete_lst(name: str):
    return delete_obj(name, LISTS)
   
def delete_scl(name: str):
    return delete_obj(name, SCALARS)

def delete_tbl(name: str):
    return delete_obj(name, TABLES)

def delete_var(name: str):
    return delete_obj(name, VARIABLES)
   

# Set and Get IODE objects
# ------------------------

# Comments
# --------

def get_cmt(name: str) -> str:
    '''Return the text of an IODE comment'''
    
    cmt850 = IodeGetCmt(cstr(name))
    return pystr(cmt850)

def set_cmt(name: str, cmt: str):
    '''Update or create an IODE comment from a python str'''
    
    if IodeSetCmt(cstr(name), cstr(cmt)):
        raise RuntimeError(f"Comment {name} cannot be set")


# Equations
# ---------

def get_eqs_lec(eq_name: str) -> str:
    '''Return an IODE equation LEC form as a string'''
    
    lec850 = IodeGetEqsLec(cstr(eq_name))
    return pystr(lec850)

def get_eqs(eq_name: str) -> Equation:
    '''Return an IODE equation as an iode.Equation class instance'''
    
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
        
    eq_res = Equation(eq_name, pystr(lec), method, pystr(sample_from), pystr(sample_to),  
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
def set_eqs(eq_name: str, lec: str):
    if IodeSetEqs(cstr(eq_name), cstr(lec)):
        raise RuntimeError(f"Equation {eq_name} cannot be set")

# Identities
# ----------
def get_idt(name: str) -> str:
    '''Return the LEC formula of an IODE identity '''
    idt850 = IodeGetIdt(cstr(name))
    return pystr(idt850)

def set_idt(name: str, idt: str):
    '''Update or create an identity'''
    if IodeSetIdt(cstr(name), cstr(idt)):
        raise RuntimeError(f"Identity {name} cannot be set")
    
# Lists
# -----
def get_lst(name: str) -> str:
    '''Return a list as a string'''
    lst850 = IodeGetLst(cstr(name))
    return pystr(lst850)

def set_lst(name: str, lst: str):
    '''Update or create a list fro a string'''
    if IodeSetLst(cstr(name), cstr(lst)):
        raise RuntimeError(f"List {name} cannot be set")


# Scalars
# -------
def get_scl(name: str) -> Scalar:
    '''Get an IODE scalar in an iode.Scalar class instance'''
    #vararray = iodescl_to_ndarray(cstr(name))
    #return vararray
    cdef    double cval
    cdef    double crelax
    cdef    double cstd
    
    cdef rc = IodeGetScl(cstr(name), &cval, &crelax, &cstd)
    if rc == 0:
        res = Scalar(name, cval, crelax, cstd)
        return res
    else:
        return None


def set_scl(py_scl: Scalar):
    '''Create or update an IODE scalar from an iode.Scalar class instance'''
    if IodeSetScl(cstr(py_scl.name), py_scl.value, py_scl.relax, py_scl.std):
        raise RuntimeError(f"Scalar {py_scl.name} cannot be set")

    
# Tables
# ------
# ... not yet implemented ...
# TODO:
#    - tbl_add_line(tbl_name, after_line, [cells], line_type = KT_CELLS)
#    - tbl_add_lines(tbl_name, after_line, [[cells], [cells]...])
#    - tbl_del_line(tbl_name, line)
#    - tbl_create(tbl_name, title, [line1, line2...])
#    - tbl_get_lines(tbl_nname, from_line, to_line)
#    - tbl_get_line_type(tbl_nname, line)  


# Variables
# ---------

def get_var(varname: str) -> List[float]:
    '''Get an IODE variable in a list of floats'''
    ndarray = get_var_as_ndarray(varname, True)
    return list(ndarray)

# Copy (or refer to) an IODE var into a ndarray
def get_var_as_ndarray(varname: str, copy: bool = True) -> np.ndarray:
    '''Get an IODE variable in a numpy ndarray'''
    vararray = iodevar_to_ndarray(cstr(varname), copy)
    return vararray 

# Copy a ndarray or a list into KV_WS
def set_var(varname: str, py_values):
    '''Create or update an IODE variable from a list of floats or a ndarray'''
    cdef double  *c_values
    
    if not IodeIsSampleSet():
        raise RuntimeError("py_to_var impossible: IODE sample not set")
        
    ndarray = np.array(py_values, dtype = np.double) 
    
    c_values = <double*>np.PyArray_DATA(ndarray)
    pos =  IodeSetVector(cstr(varname), c_values, 0, 0, -1)
    if pos < 0:
        raise RuntimeError(f"Variable {varname} cannot be set")
    
