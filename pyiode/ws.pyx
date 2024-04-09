#  IODE EXTENSION FOR PYTHON
#  =========================
#
#    @header4iode
#
#  IODE workspaces functions
#  -------------------------
#   ws_content(pattern: Union[str, List[str]] = '*', objtype) -> List[str]  | Return the names of all objects of a given type, satisfying pattern specification(s)  
#   ws_content_cmt(pattern: Union[str, List[str]] = '*') -> List[str]       | Returns the list of comment names corresponding to the given pattern(s)
#   ws_content_eqs(pattern: Union[str, List[str]] = '*') -> List[str]       | Returns the list of equation names corresponding to the given pattern(s)
#   ws_content_idt(pattern: Union[str, List[str]] = '*') -> List[str]       | Returns the list of identity names corresponding to the given pattern(s)
#   ws_content_lst(pattern: Union[str, List[str]] = '*') -> List[str]       | Returns the list of list names corresponding to the given pattern(s)
#   ws_content_scl(pattern: Union[str, List[str]] = '*') -> List[str]       | Returns the list of scalar names corresponding to the given pattern(s)
#   ws_content_tbl(pattern: Union[str, List[str]] = '*') -> List[str]       | Returns the list of table names corresponding to the given pattern(s)
#   ws_content_var(pattern: Union[str, List[str]] = '*') -> List[str]       | Returns the list of variable names corresponding to the given pattern(s)
#
#   ws_clear(filetype: int) | Clear WS of the given filetype (K_CMT..K_VAR)
#   ws_clear_cmt()          | Clear the comment WS
#   ws_clear_eqs()          | Clear the equation WS
#   ws_clear_idt()          | Clear the identity WS
#   ws_clear_lst()          | Clear the list WS
#   ws_clear_scl()          | Clear the scalar WS
#   ws_clear_tbl()          | Clear the table WS
#   ws_clear_var()          | Clear the variable WS
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

from collections.abc import Iterable
import warnings

from ws cimport (IodeLoad, IodeSave, IodeClearWs, IodeClearAll, IodeContents, 
                        B_WsHtoLLast, B_WsHtoLMean, B_WsHtoLSum, B_WsLtoHStock, B_WsLtoHFlow)
from iode_python cimport free_tbl


def _ws_content_from_str(pattern: str = '*', objtype: int = 6) -> List[str]:
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

def ws_content(pattern: Union[str, List[str]] = '*', objtype: int = K_VAR) -> List[str]:
    r"""Return the names of objects of a given type, satisfying pattern specification(s). 
    The resulting list is sorted in alphabetic order.

    Parameters
    ----------
    pattern: string or list of strings
        each string may contain wildcards characters like '*' or '?'.
        Default '*', meaning "all objects".

    objtype: int
        IODE object type : K_CMT, K_EQS, K_IDT, K_LST, K_SCL, K_TBL, K_VAR
        Default K_VAR

    Returns
    -------
    List[str]
        List of object names
    """
    if isinstance(pattern, str):
        return(_ws_content_from_str(pattern, objtype))

    elif isinstance(pattern, Iterable):
        res = set()
        for pattern1 in pattern:
            res = res| set(_ws_content_from_str(pattern1, objtype))
        res = list(res)
        res.sort()
        return res
    else:
        raise RuntimeError("ws_content() only accepts strings or list of strings")


def ws_content_cmt(pattern: Union[str, List[str]] = '*') -> List[str]:
    '''Returns the list of comment names corresponding to the given pattern'''
    warnings.warn("ws_content_cmt() is deprecated. " + 
        "Please use the new syntax: cmt_db.get_names(pattern)", DeprecationWarning)
    cmt_db = Comments()
    return cmt_db.get_names(pattern)

def ws_content_eqs(pattern: Union[str, List[str]] = '*') -> List[str]:
    '''Returns the list of equation names corresponding to the given pattern'''
    return ws_content(pattern, K_EQS)

def ws_content_idt(pattern: Union[str, List[str]] = '*') -> List[str]:
    '''Returns the list of identity names corresponding to the given pattern'''
    warnings.warn("ws_content_idt() is deprecated. " + 
        "Please use the new syntax: idt_db.get_names(pattern)", DeprecationWarning)
    idt_db = Identities()
    return idt_db.get_names(pattern)

def ws_content_lst(pattern: Union[str, List[str]] = '*') -> List[str]:
    return ws_content(pattern, K_LST)

def ws_content_scl(pattern: Union[str, List[str]] = '*') -> List[str]:
    return ws_content(pattern, K_SCL)

def ws_content_tbl(pattern: Union[str, List[str]] = '*') -> List[str]:
    return ws_content(pattern, K_TBL)

def ws_content_var(pattern: Union[str, List[str]] = '*') -> List[str]:
    warnings.warn("ws_content_var() is deprecated. " + 
        "Please use the new syntax: var_db.get_names(pattern)", DeprecationWarning)
    var_db = Variables()
    return var_db.get_names(pattern)


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
        "Please use the new syntax: cmt_db.clear()", DeprecationWarning)
    cmt_db = Comments()
    cmt_db.clear()

def ws_clear_eqs():
    ws_clear(K_EQS)

def ws_clear_idt():
    warnings.warn("ws_clear_idt() is deprecated. " + 
        "Please use the new syntax: idt_db.clear()", DeprecationWarning)
    idt_db = Identities()
    idt_db.clear()

def ws_clear_lst():
    ws_clear(K_LST)

def ws_clear_scl():
    ws_clear(K_SCL)

def ws_clear_tbl():
    ws_clear(K_TBL)

def ws_clear_var():
    warnings.warn("ws_clear_var() is deprecated. " + 
        "Please use the new syntax: var_db.clear()", DeprecationWarning)
    var_db = Variables()
    var_db.clear()


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
        "Please use the new syntax: cmt_db = Comments(filepath)", DeprecationWarning)
    cmt_db = Comments()
    cmt_db = Comments(filename)

def ws_load_eqs(filename: str) -> int:
    return ws_load(filename, K_EQS)

def ws_load_idt(filename: str) -> int:
    warnings.warn("ws_load_idt() is deprecated. " + 
        "Please use the new syntax: idt_db = Identities(filepath)", DeprecationWarning)
    idt_db = Identities()
    idt_db = Identities(filename)

def ws_load_lst(filename: str) -> int:
    return ws_load(filename, K_LST) 
                             
def ws_load_scl(filename: str) -> int:
    return ws_load(filename, K_SCL) 
                             
def ws_load_tbl(filename: str) -> int:
    return ws_load(filename, K_TBL) 
                             
def ws_load_var(filename: str) -> int:
    warnings.warn("ws_load_var() is deprecated. " + 
        "Please use the new syntax: var_db = Variables(filepath)", DeprecationWarning)
    var_db = Variables()
    var_db = Variables(filename)


# Save WS
# -------
def ws_save(filename: str, filetype: int):
    '''Save the current IODE workspace of a given type'''
    if IodeSave(_cstr(filename), filetype):
        raise RuntimeError(f"Workspace of type {filetype} cannot be saved in file {filename}.")

def ws_save_cmt(filename: str):
    warnings.warn("ws_save_cmt() is deprecated. " + 
        "Please use the new syntax: cmt_db.save(filepath)", DeprecationWarning)
    cmt_db = Comments()
    cmt_db.save(filename)

def ws_save_eqs(filename: str):
    '''Save the current equation workspace'''
    ws_save(filename, K_EQS)

def ws_save_idt(filename: str):
    '''Save the current identity workspace'''
    warnings.warn("ws_save_idt() is deprecated. " + 
        "Please use the new syntax: idt_db.save(filepath)", DeprecationWarning)
    idt_db = Identities()
    idt_db.save(filename)

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
    warnings.warn("ws_save_var() is deprecated. " + 
        "Please use the new syntax: var_db.save(filepath)", DeprecationWarning)
    var_db = Variables()
    var_db.save(filename)


# High to Low
# -----------
def ws_htol(filename: str, varlist, series_type: int):
    varlist = _arg_to_str(varlist, sep = ' ')
#    if isinstance(varlist, list):
#        varlist = ' '.join(varlist)

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
    varlist = _arg_to_str(varlist, sep = ' ')
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
