from collections.abc import Iterable
import warnings

from ws cimport (IodeLoad, IodeSave, IodeClearWs, IodeContents, 
                B_WsHtoLLast, B_WsHtoLMean, B_WsHtoLSum, B_WsLtoHStock, B_WsLtoHFlow)
from iode_python cimport free_tbl

# TODO: rewrite IodeContents with regex or a better algorithm because K_grep() is VERY slow for large databases


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

def ws_content(pattern: Union[str, List[str]] = '*', obj_type: int = K_VAR) -> List[str]:
    if obj_type == 0:
        ws_content_cmt(pattern)
    elif obj_type == 1:
        ws_content_eqs(pattern)
    elif obj_type == 2:
        ws_content_idt(pattern)
    elif obj_type == 3:
        ws_content_lst(pattern)
    elif obj_type == 4:
        ws_content_scl(pattern)
    elif obj_type == 5:
        ws_content_tbl(pattern)
    elif obj_type == 6:
        ws_content_var(pattern)
    else:
        raise ValueError(f"IODE type {obj_type} is invalid")

def ws_content_cmt(pattern: Union[str, List[str]] = '*') -> List[str]:
    '''Returns the list of comment names corresponding to the given pattern'''
    warnings.warn("ws_content_cmt() is deprecated. " + 
        "Please use the new syntax:\ncmt_db = Comments()\ncmt_db.get_names(pattern)", DeprecationWarning)
    cmt_db = Comments()
    return cmt_db.get_names(pattern)

def ws_content_eqs(pattern: Union[str, List[str]] = '*') -> List[str]:
    '''Returns the list of equation names corresponding to the given pattern'''
    warnings.warn("ws_content_eqs() is deprecated. " + 
        "Please use the new syntax:\neqs_db = Equations()\neqs_db.get_names(pattern)", DeprecationWarning)
    eqs_db = Equations()
    return eqs_db.get_names(pattern)

def ws_content_idt(pattern: Union[str, List[str]] = '*') -> List[str]:
    '''Returns the list of identity names corresponding to the given pattern'''
    warnings.warn("ws_content_idt() is deprecated. " + 
        "Please use the new syntax:\nidt_db = Identities()\nidt_db.get_names(pattern)", DeprecationWarning)
    idt_db = Identities()
    return idt_db.get_names(pattern)

def ws_content_lst(pattern: Union[str, List[str]] = '*') -> List[str]:
    warnings.warn("ws_content_lst() is deprecated. " + 
        "Please use the new syntax:\nlst_db = Lists()\nlst_db.get_names(pattern)", DeprecationWarning)
    lst_db = Lists()
    return lst_db.get_names(pattern)

def ws_content_scl(pattern: Union[str, List[str]] = '*') -> List[str]:
    warnings.warn("ws_content_scl() is deprecated. " + 
        "Please use the new syntax:\nscl_db = Scalars()\nscl_db.get_names(pattern)", DeprecationWarning)
    scl_db = Scalars()
    return scl_db.get_names(pattern)

def ws_content_tbl(pattern: Union[str, List[str]] = '*') -> List[str]:
    if isinstance(pattern, str):
        return(_ws_content_from_str(pattern, K_TBL))

    elif isinstance(pattern, Iterable):
        res = set()
        for pattern_ in pattern:
            res = res| set(_ws_content_from_str(pattern_, K_TBL))
        res = list(res)
        res.sort()
        return res
    else:
        raise RuntimeError("ws_content() only accepts strings or list of strings")

def ws_content_var(pattern: Union[str, List[str]] = '*') -> List[str]:
    warnings.warn("ws_content_var() is deprecated. " + 
        "Please use the new syntax:\nvar_db = Variables()\nvar_db.get_names(pattern)", DeprecationWarning)
    var_db = Variables()
    return var_db.get_names(pattern)

# Clear WS
# --------

def ws_clear_all():
    '''Clear all WS'''
    ws_clear_cmt()
    ws_clear_eqs()
    ws_clear_idt()
    ws_clear_lst()
    ws_clear_scl()
    ws_clear_tbl()
    ws_clear_var()

def ws_clear(filetype: int):
    '''Clear WS of the given filetype (K_CMT..K_VAR)'''
    if filetype == 0:
        ws_clear_cmt()
    elif filetype == 1:
        ws_clear_eqs()
    elif filetype == 2:
        ws_clear_idt()
    elif filetype == 3:
        ws_clear_lst()
    elif filetype == 4:
        ws_clear_scl()
    elif filetype == 5:
        ws_clear_tbl()
    elif filetype == 6:
        ws_clear_var()
    else:
        raise ValueError(f"IODE type {filetype} is invalid")

def ws_clear_cmt():
    warnings.warn("ws_clear_cmt() is deprecated. " + 
        "Please use the new syntax:\ncmt_db = Comments()\ncmt_db.clear()", DeprecationWarning)
    cmt_db = Comments()
    cmt_db.clear()

def ws_clear_eqs():
    warnings.warn("ws_clear_eqs() is deprecated. " + 
        "Please use the new syntax:\neqs_db = Equations()\neqs_db.clear()", DeprecationWarning)
    eqs_db = Equations()
    eqs_db.clear()

def ws_clear_idt():
    warnings.warn("ws_clear_idt() is deprecated. " + 
        "Please use the new syntax:\nidt_db = Identities()\nidt_db.clear()", DeprecationWarning)
    idt_db = Identities()
    idt_db.clear()

def ws_clear_lst():
    warnings.warn("ws_clear_lst() is deprecated. " + 
        "Please use the new syntax:\nidt_db = Identities()\nidt_db.clear()", DeprecationWarning)
    lst_db = Lists()
    lst_db.clear()

def ws_clear_scl():
    warnings.warn("ws_clear_scl() is deprecated. " + 
        "Please use the new syntax:\nscl_db = Scalars()\nscl_db.clear()", DeprecationWarning)
    scl_db = Scalars()
    scl_db.clear()

def ws_clear_tbl():
    if IodeClearWs(K_TBL):
        raise RuntimeError(f"Workspace of type {K_TBL} cannot be cleared")

def ws_clear_var():
    warnings.warn("ws_clear_var() is deprecated. " + 
        "Please use the new syntax:\nvar_db = Variables()\nvar_db.clear()", DeprecationWarning)
    var_db = Variables()
    var_db.clear()

# Load WS
# -------

def ws_load(filename: str, filetype: int) -> int:
    '''Load an IODE file and return the number of read objects'''
    if filetype == 0:
        ws_load_cmt(filename)
    elif filetype == 1:
        ws_load_eqs(filename)
    elif filetype == 2:
        ws_load_idt(filename)
    elif filetype == 3:
        ws_load_lst(filename)
    elif filetype == 4:
        ws_load_scl(filename)
    elif filetype == 5:
        ws_load_tbl(filename)
    elif filetype == 6:
        ws_load_var(filename)
    else:
        raise ValueError(f"IODE type {filetype} is invalid") 

def ws_load_cmt(filename: str):
    warnings.warn("ws_load_cmt() is deprecated. " + 
        "Please use the new syntax:\ncmt_db = Comments(filepath)", DeprecationWarning)
    return Comments(filename)

def ws_load_eqs(filename: str) -> int:
    warnings.warn("ws_load_eqs() is deprecated. " + 
        "Please use the new syntax:\neqs_db = Equations(filepath)", DeprecationWarning)
    return Equations(filename)

def ws_load_idt(filename: str) -> int:
    warnings.warn("ws_load_idt() is deprecated. " + 
        "Please use the new syntax:\nidt_db = Identities(filepath)", DeprecationWarning)
    return Identities(filename)

def ws_load_lst(filename: str) -> int:
    warnings.warn("ws_load_lst() is deprecated. " + 
        "Please use the new syntax:\nlst_db = Lists(filepath)", DeprecationWarning)
    return Identities(filename)

def ws_load_scl(filename: str) -> int:
    warnings.warn("ws_load_scl() is deprecated. " + 
        "Please use the new syntax:\nscl_db = Scalars(filepath)", DeprecationWarning)
    return Scalars(filename) 
                
def ws_load_tbl(filename: str) -> int:
    nb = IodeLoad(_cstr(filename), K_TBL)
    if nb < 0:
        raise RuntimeError(f"File {filename} of type {K_TBL} cannot be loaded")
    return nb 
                             
def ws_load_var(filename: str) -> int:
    warnings.warn("ws_load_var() is deprecated. " + 
        "Please use the new syntax:\nvar_db = Variables(filepath)", DeprecationWarning)
    var_db = Variables()
    var_db = Variables(filename)

# Save WS
# -------

def ws_save(filename: str, filetype: int):
    '''Save the current IODE workspace of a given type'''
    if filetype == 0:
        ws_save_cmt(filename)
    elif filetype == 1:
        ws_save_eqs(filename)
    elif filetype == 2:
        ws_save_idt(filename)
    elif filetype == 3:
        ws_save_lst(filename)
    elif filetype == 4:
        ws_save_scl(filename)
    elif filetype == 5:
        ws_save_tbl(filename)
    elif filetype == 6:
        ws_save_var(filename)
    else:
        raise ValueError(f"IODE type {filetype} is invalid") 

def ws_save_cmt(filename: str):
    '''Save the current comment workspace'''
    warnings.warn("ws_save_cmt() is deprecated. " + 
        "Please use the new syntax:\ncmt_db = Comments()\ncmt_db.save(filepath)", DeprecationWarning)
    cmt_db = Comments()
    cmt_db.save(filename)

def ws_save_eqs(filename: str):
    '''Save the current equation workspace'''
    warnings.warn("ws_save_eqs() is deprecated. " + 
        "Please use the new syntax:\neqs_db = Equations()\neqs_db.save(filepath)", DeprecationWarning)
    eqs_db = Equations()
    eqs_db.save(filename)

def ws_save_idt(filename: str):
    '''Save the current identity workspace'''
    warnings.warn("ws_save_idt() is deprecated. " + 
        "Please use the new syntax:\nidt_db = Identities()\nidt_db.save(filepath)", DeprecationWarning)
    idt_db = Identities()
    idt_db.save(filename)

def ws_save_lst(filename: str):
    '''Save the current list workspace'''
    warnings.warn("ws_save_lst() is deprecated. " + 
        "Please use the new syntax:\nlst_db = Lists()\nlst_db.save(filepath)", DeprecationWarning)
    lst_db = Lists()
    lst_db.save(filename)

def ws_save_scl(filename: str):
    '''Save the current scalar workspace'''
    warnings.warn("ws_save_scl() is deprecated. " + 
        "Please use the new syntax:\nscl_db = Scalars()\nscl_db.save(filepath)", DeprecationWarning)
    scl_db = Scalars()
    scl_db.save(filename)

def ws_save_tbl(filename: str):
    '''Save the current table workspace'''
    if IodeSave(_cstr(filename), K_TBL):
        raise RuntimeError(f"Workspace of type {K_TBL} cannot be saved in file {filename}.")

def ws_save_var(filename: str):
    warnings.warn("ws_save_var() is deprecated. " + 
        "Please use the new syntax:\nvar_db = Variables()\nvar_db.save(filepath)", DeprecationWarning)
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
