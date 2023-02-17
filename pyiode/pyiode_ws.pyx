#  IODE EXTENSION FOR PYTHON
#  =========================
#
#    @header4iode
# 
#  IODE workspaces functions
#  -------------------------
#   ws_content_(pattern, objtype)
#   ws_content_cmt(pattern)
#   ws_content_eqs(pattern)
#   ws_content_idt(pattern)
#   ws_content_lst(pattern)
#   ws_content_scl(pattern)
#   ws_content_tbl(pattern)
#   ws_content_var(pattern)
#  
#   ws_clear_(filetype)
#   ws_clear_cmt()
#   ws_clear_eqs()
#   ws_clear_idt()
#   ws_clear_lst()
#   ws_clear_scl()
#   ws_clear_tbl()
#   ws_clear_var()
#   ws_clear_all()
#  
#   ws_load_(filename, filetype)
#   ws_load_cmt(filename)
#   ws_load_eqs(filename)
#   ws_load_idt(filename)
#   ws_load_lst(filename)
#   ws_load_scl(filename)
#   ws_load_tbl(filename)
#   ws_load_var(filename)
#  
#   ws_save_(filename, filetype)
#   ws_save_cmt(filename)
#   ws_save_eqs(filename)
#   ws_save_idt(filename)
#   ws_save_lst(filename)
#   ws_save_scl(filename)
#   ws_save_tbl(filename)
#   ws_save_var(filename)
#
# TODO
# ----
#   ws_copy_(filename, filetype, list)
#   ws_copy_cmt(filename, list)
#   ws_copy_eqs(filename, list)
#   ws_copy_idt(filename, list)
#   ws_copy_lst(filename, list)
#   ws_copy_scl(filename, list)
#   ws_copy_tbl(filename, list)
#   ws_copy_var(filename, list, sample)

#   ws_merge_(filename, filetype, list)
#   ws_merge_cmt(filename, list)
#   ws_merge_eqs(filename, list)
#   ws_merge_idt(filename, list)
#   ws_merge_lst(filename, list)
#   ws_merge_scl(filename, list)
#   ws_merge_tbl(filename, list)
#   ws_merge_var(filename, list)
#  
#   ws_extrapolate(...)
#   
#   ws_extrapolate
#   ws_ltohstock  
#   ws_ltohflow 
#   ws_htolsum 
#   ws_htolmean 
#   ws_htollast 
#   ws_seasonadj
#   ws_seasadj 
#   ws_trend 
#   ws_trendstd  
#   ws_import 
#   ws_export  


 

# TODO: rewrite IodeContents with regex or a better algorithm because K_grep() is VERY slow for large databases

def ws_content_(pattern:str = '*', objtype:int = 6):
    r"""Return the names of objects of a given type, satisfying a pattern specification.
    
    Parameters
    ----------
    pattern : string
        string containing wildcards characters like '*' or '?'. 
        Default '*', meaning "all objects".
        
    objtype : int
        IODE object type (0-6, 0 for comments...)
        Default 6 for 
        
    Returns
    -------
    list of strings
        List of object names 

    Examples
    -------
    >>> iode.ws_load_cmt("../fun.cmt")
    >>> result = iode.ws_content("ACA*", 0)
    >>> print(result)
    ['ACAF', 'ACAG']
    """
    cdef char **cnt = IodeContents(cstr(pattern), objtype)
    cdef int nb
    
    res = []
    if cnt != NULL:
        # nb = number of strings in cnt
        nb = 0
        while cnt[nb] != NULL:
            nb = nb + 1
        res = [None] * nb
        
        # Assigns each C string to a value of the list res 
        nb = 0
        while cnt[nb] != NULL:
            s = bytes(cnt[nb])      
            res[nb] = pystr(s)
            nb = nb + 1
    
    SCR_free_tbl(cnt)
    
    return res

def ws_content_cmt(pattern:str = '*'):
    '''Returns the list of comment names corresponding to the given pattern'''
    return ws_content_(pattern, K_CMT)

def ws_content_eqs(pattern:str = '*'):
    '''Returns the list of equation names corresponding to the given pattern'''
    return ws_content_(pattern, K_EQS)

def ws_content_idt(pattern:str = '*'):
    '''Returns the list of identity names corresponding to the given pattern'''
    return ws_content_(pattern, K_IDT)

def ws_content_lst(pattern:str = '*'):
    return ws_content_(pattern, K_LST)

def ws_content_scl(pattern:str = '*'):
    return ws_content_(pattern, K_SCL)

def ws_content_tbl(pattern:str = '*'):
    return ws_content_(pattern, K_TBL)

def ws_content_var(pattern:str = '*'):
    return ws_content_(pattern, K_VAR)


# Clear WS
# --------
def ws_clear_all():
    "Clear all WS"
    return IodeClearAll()    

def ws_clear_(filetype):
    "Clear WS of the given filetype (0..6)"
    return IodeClearWs(filetype)    

def ws_clear_cmt():
    return ws_clear_(K_CMT)

def ws_clear_eqs():
    return ws_clear_(K_EQS)

def ws_clear_idt():
    return ws_clear_(K_IDT)

def ws_clear_lst():
    return ws_clear_(K_LST)

def ws_clear_scl():
    return ws_clear_(K_SCL)

def ws_clear_tbl():
    return ws_clear_(K_TBL)

def ws_clear_var():
    return ws_clear_(K_VAR)


# Load WS
# -------
def ws_load_(filename, filetype):
    "Load iode file"
    return IodeLoad(cstr(filename), filetype)    

def ws_load_cmt(filename):
    return ws_load_(filename, K_CMT)

def ws_load_eqs(filename):
    return ws_load_(filename, K_EQS)

def ws_load_idt(filename):
    return ws_load_(filename, K_IDT)

def ws_load_lst(filename):
    return ws_load_(filename, K_LST)

def ws_load_scl(filename):
    return ws_load_(filename, K_SCL)

def ws_load_tbl(filename):
    return ws_load_(filename, K_TBL)

def ws_load_var(filename):
    return ws_load_(filename, K_VAR)
    

# Save WS
# -------
def ws_save_(filename, filetype):
    return IodeSave(cstr(filename), filetype)    

def ws_save_cmt(filename):
    return ws_save_(filename, K_CMT)  

def ws_save_eqs(filename):
    return ws_save_(filename, K_EQS)  

def ws_save_idt(filename):
    return ws_save_(filename, K_IDT)  

def ws_save_lst(filename):
    return ws_save_(filename, K_LST)  

def ws_save_scl(filename):
    return ws_save_(filename, K_SCL)  

def ws_save_tbl(filename):
    return ws_save_(filename, K_TBL)  

def ws_save_var(filename):
    return ws_save_(filename, K_VAR)  

