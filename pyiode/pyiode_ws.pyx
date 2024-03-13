#  IODE EXTENSION FOR PYTHON
#  =========================
#
#    @header4iode
#
#  IODE workspaces functions
#  -------------------------
#   ws_content(pattern, objtype) -> List[str]       | Return the names of all objects of a given type, satisfying a pattern specification  
#   ws_content_cmt(pattern) -> List[str]            | Returns the list of comment names corresponding to the given pattern
#   ws_content_eqs(pattern) -> List[str]            | Returns the list of equation names corresponding to the given pattern
#   ws_content_idt(pattern) -> List[str]            | Returns the list of identity names corresponding to the given pattern
#   ws_content_lst(pattern) -> List[str]            | Returns the list of list names corresponding to the given pattern
#   ws_content_scl(pattern) -> List[str]            | Returns the list of scalar names corresponding to the given pattern
#   ws_content_tbl(pattern) -> List[str]            | Returns the list of table names corresponding to the given pattern
#   ws_content_var(pattern) -> List[str]            | Returns the list of variable names corresponding to the given pattern
#
#   ws_clear(filetype: int) | Clear WS of the given filetype (K_CMT..K_VAR)
#   ws_clear_cmt()          | Clear the comment WS
#   ws_clear_eqs()          | Clear the equation WS
#   ws_clear_idt()          | Clear the identity WS
#   ws_clear_lst()          | Clear the list WS
#   ws_clear_scl()          | Clear the scalar WS
#   ws_clear_tbl()          | Clear the table WS
#   ws_clear_var()          | Clear the varaible WS
#   ws_clear_all()          | Clear all WS           
#
#   ws_load(filename: str, filetype: int) -> int | Load an IODE file and return the number of read objects
#   ws_load_cmt(filename: str) -> int            | Load a comment file and return the number of read objects
#   ws_load_eqs(filename: str) -> int            | Load a equation file and return the number of read objects
#   ws_load_idt(filename: str) -> int            | Load a identity file and return the number of read objects
#   ws_load_lst(filename: str) -> int            | Load a list file and return the number of read objects
#   ws_load_scl(filename: str) -> int            | Load a scalar file and return the number of read objects
#   ws_load_tbl(filename: str) -> int            | Load a table file and return the number of read objects
#   ws_load_var(filename: str) -> int            | Load a variable file and return the number of read objects
#
#   ws_save(filename: str, filetype: int) | Save the current IODE workspace of a given type
#   ws_save_cmt(filename: str)            | Save the current workspace of comments
#   ws_save_eqs(filename: str)            | Save the current workspace of equations
#   ws_save_idt(filename: str)            | Save the current workspace of identities
#   ws_save_lst(filename: str)            | Save the current workspace of lists
#   ws_save_scl(filename: str)            | Save the current workspace of scalars
#   ws_save_tbl(filename: str)            | Save the current workspace of tables
#   ws_save_var(filename: str)            | Save the current workspace of variables
#

#   ws_ltoh_flow(filename: str, varlist)                         |
#   ws_ltoh_stock(filename: str, varlist, method: int = LTOH_CS) |
#   ws_htol_sum(filename: str, varlist, method: int)             |
#   ws_htol_mean(filename: str, varlist, method: int)            |
#   ws_htol_last(filename: str, varlist, method: int)            |
#  
#  
#  TODO
#  ----
#   ws_copy(filename, filetype, list)
#   ws_copy_cmt(filename, list)
#   ws_copy_eqs(filename, list)
#   ws_copy_idt(filename, list)
#   ws_copy_lst(filename, list)
#   ws_copy_scl(filename, list)
#   ws_copy_tbl(filename, list)
#   ws_copy_var(filename, list, sample)

#   ws_merge(filename, filetype, list)
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
#   ws_seasonadj
#   ws_seasadj
#   ws_trend
#   ws_trendstd
#   ws_import
#   ws_export

# TODO: rewrite IodeContents with regex or a better algorithm because K_grep() is VERY slow for large databases

import warnings

from pyiode_ws cimport (IodeLoad, IodeSave, IodeClearWs, IodeClearAll, IodeContents, 
                        B_WsHtoLLast, B_WsHtoLMean, B_WsHtoLSum, B_WsLtoHStock, B_WsLtoHFlow)
from iode_python cimport free_tbl


def ws_content(pattern: str = '*', objtype: int = 6) -> List[str]:
    r"""Return the names of objects of a given type, satisfying a pattern specification.

    Parameters
    ----------
    pattern: string
        string containing wildcards characters like '*' or '?'.
        Default '*', meaning "all objects".

    objtype: int
        IODE object type (0-6, 0 for comments...)
        Default 6 for

    Returns
    -------
    List[str]
        List of object names

    Examples
    -------
    >>> from iode import Comments, ws_content
    >>> Comments.load("../data/fun.cmt")
    >>> names = iode.ws_content("ACA*", 0)
    >>> names
    ['ACAF', 'ACAG']
    """
    cdef char **cnt = IodeContents(_cstr(pattern), objtype)
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
            res[nb] = _pystr(s)
            nb = nb + 1

    free_tbl(cnt)

    return res

def ws_content_cmt(pattern: str = '*') -> List[str]:
    warnings.warn("ws_content_cmt() is deprecated. " + 
        "Please use the new syntax: Comments.get_names(pattern)", DeprecationWarning)
    return Comments.get_names(pattern)

def ws_content_eqs(pattern: str = '*') -> List[str]:
    '''Returns the list of equation names corresponding to the given pattern'''
    return ws_content(pattern, K_EQS)

def ws_content_idt(pattern: str = '*') -> List[str]:
    '''Returns the list of identity names corresponding to the given pattern'''
    return ws_content(pattern, K_IDT)

def ws_content_lst(pattern: str = '*') -> List[str]:
    return ws_content(pattern, K_LST)

def ws_content_scl(pattern: str = '*') -> List[str]:
    return ws_content(pattern, K_SCL)

def ws_content_tbl(pattern: str = '*') -> List[str]:
    return ws_content(pattern, K_TBL)

def ws_content_var(pattern: str = '*') -> List[str]:
    return ws_content(pattern, K_VAR)


# Clear WS
# --------
def ws_clear_all():
    '''Clear all WS'''
    if IodeClearAll():
        raise RuntimeError(f"Cannot clear all workspaces.")

def ws_clear(filetype: int):
    '''Clear WS of the given filetype (K_CMT..K_VAR)'''
    if IodeClearWs(filetype):
        raise RuntimeError(f"Workspace of type {filetype} cannot be cleared")

def ws_clear_cmt():
    warnings.warn("ws_clear_cmt() is deprecated. " + 
        "Please use the new syntax: Comments.clear()", DeprecationWarning)
    Comments.clear()

def ws_clear_eqs():
    ws_clear(K_EQS)

def ws_clear_idt():
    ws_clear(K_IDT)

def ws_clear_lst():
    ws_clear(K_LST)

def ws_clear_scl():
    ws_clear(K_SCL)

def ws_clear_tbl():
    ws_clear(K_TBL)

def ws_clear_var():
    ws_clear(K_VAR)


# Load WS
# -------
def ws_load(filename: str, filetype: int) -> int:
    '''Load an IODE file and return the number of read objects'''
    
    nb = IodeLoad(_cstr(filename), filetype)
    if nb < 0:
        raise RuntimeError(f"File {filename} of type {filetype} cannot be loaded")
    return nb    

def ws_load_cmt(filename: str):
    warnings.warn("ws_load_cmt() is deprecated. " + 
        "Please use the new syntax: Comments.load(filepath)", DeprecationWarning)
    Comments.load(filename)

def ws_load_eqs(filename: str) -> int:
    return ws_load(filename, K_EQS)

def ws_load_idt(filename: str) -> int:
    return ws_load(filename, K_IDT)

def ws_load_lst(filename: str) -> int:
    return ws_load(filename, K_LST) 
                             
def ws_load_scl(filename: str) -> int:
    return ws_load(filename, K_SCL) 
                             
def ws_load_tbl(filename: str) -> int:
    return ws_load(filename, K_TBL) 
                             
def ws_load_var(filename: str) -> int:
    return ws_load(filename, K_VAR)


# Save WS
# -------
def ws_save(filename: str, filetype: int):
    '''Save the current IODE workspace of a given type'''
    if IodeSave(_cstr(filename), filetype):
        raise RuntimeError(f"Workspace of type {filetype} cannot be saved in file {filename}.")

def ws_save_cmt(filename: str):
    warnings.warn("ws_save_cmt() is deprecated. " + 
        "Please use the new syntax: Comments.save(filepath)", DeprecationWarning)
    Comments.save(filename)

def ws_save_eqs(filename: str):
    '''Save the current equation workspace'''
    ws_save(filename, K_EQS)

def ws_save_idt(filename: str):
    '''Save the current identity workspace'''
    ws_save(filename, K_IDT)

def ws_save_lst(filename: str):
    '''Save the current list workspace'''
    ws_save(filename, K_LST)

def ws_save_scl(filename: str):
    '''Save the current scalar workspace'''
    ws_save(filename, K_SCL)

def ws_save_tbl(filename: str):
    '''Save the current table workspace'''
    ws_save(filename, K_TBL)

def ws_save_var(filename: str):
    '''Save the current variable workspace'''
    ws_save(filename, K_VAR)


# High to Low
# -----------
def ws_htol(filename: str, varlist, series_type: int):
    #if type(varlist) == list:
    if isinstance(varlist, list):
        varlist = ' '.join(varlist)

    arg = f"{filename} {varlist}"
    if series_type == HTOL_LAST:
        if B_WsHtoLLast(_cstr(arg)):
            raise RuntimeError(f"ws_htol_last() on file {filename} failed.")
    elif series_type == HTOL_MEAN:
        if B_WsHtoLMean(_cstr(arg)):
            raise RuntimeError(f"ws_htol_mean() on file {filename} failed.")
    else:
        if B_WsHtoLSum(_cstr(arg)):
            raise RuntimeError(f"ws_htol_sum() on file {filename} failed.")

def ws_htol_last(filename: str, varlist):
    ws_htol(filename, varlist, HTOL_LAST)

def ws_htol_mean(filename: str, varlist):
    ws_htol(filename, varlist, HTOL_MEAN)

def ws_htol_sum(filename: str, varlist):
    ws_htol(filename, varlist, HTOL_SUM)


# Low to High
# -----------
def ws_ltoh(filename: str, varlist, series_type, method: Union[int, str]):
    #if type(varlist) == list:
    if isinstance(varlist, list):
        varlist = ' '.join(varlist)

    arg = f"{method} {filename} {varlist}"
    if series_type == LTOH_FLOW:
        if B_WsLtoHFlow(_cstr(arg)):
            raise RuntimeError(f"ws_ltoh_flow() on file {filename} failed.")
    else:
        if B_WsLtoHStock(_cstr(arg)):
            raise RuntimeError(f"ws_ltoh_stock() on file {filename} failed.")

def ws_ltoh_flow(filename: str, varlist, method: Union[int, str] = LTOH_CS):
    ws_ltoh(filename, varlist, LTOH_FLOW, method)

def ws_ltoh_stock(filename: str, varlist, method: Union[int, str] = LTOH_CS):
    ws_ltoh(filename, varlist, LTOH_STOCK, method)
