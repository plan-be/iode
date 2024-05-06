import warnings
from typing import Union, List, Any

from iode import (COMMENTS, EQUATIONS, IDENTITIES, LISTS, SCALARS, TABLES, VARIABLES, 
                  LTOH_FLOW, LTOH_STOCK, LTOH_CS, HTOL_LAST, HTOL_MEAN, HTOL_SUM, 
                  comments, equations, identities, lists, scalars, tables, variables, 
                  low_to_high, high_to_low)


def ws_content(pattern: Union[str, List[str]] = '*', obj_type: int = VARIABLES) -> List[str]:
    if obj_type == COMMENTS:
        return ws_content_cmt(pattern)
    elif obj_type == EQUATIONS:
        return ws_content_eqs(pattern)
    elif obj_type == IDENTITIES:
        return ws_content_idt(pattern)
    elif obj_type == LISTS:
        return ws_content_lst(pattern)
    elif obj_type == SCALARS:
        return ws_content_scl(pattern)
    elif obj_type == TABLES:
        return ws_content_tbl(pattern)
    elif obj_type == VARIABLES:
        return ws_content_var(pattern)
    else:
        raise ValueError(f"IODE type {obj_type} is invalid")

def ws_content_cmt(pattern: Union[str, List[str]] = '*') -> List[str]:
    '''Returns the list of comment names corresponding to the given pattern'''
    warnings.warn("ws_content_cmt() is deprecated. " + 
        "Please use the new syntax:\ncomments.get_names(pattern)",DeprecationWarning, stacklevel=2)
    return comments.get_names(pattern)

def ws_content_eqs(pattern: Union[str, List[str]] = '*') -> List[str]:
    '''Returns the list of equation names corresponding to the given pattern'''
    warnings.warn("ws_content_eqs() is deprecated. " + 
        "Please use the new syntax: equations.get_names(pattern)",DeprecationWarning, stacklevel=2)
    return equations.get_names(pattern)

def ws_content_idt(pattern: Union[str, List[str]] = '*') -> List[str]:
    '''Returns the list of identity names corresponding to the given pattern'''
    warnings.warn("ws_content_idt() is deprecated. " + 
        "Please use the new syntax: identities.get_names(pattern)",DeprecationWarning, stacklevel=2)
    return identities.get_names(pattern)

def ws_content_lst(pattern: Union[str, List[str]] = '*') -> List[str]:
    warnings.warn("ws_content_lst() is deprecated. " + 
        "Please use the new syntax: lists.get_names(pattern)",DeprecationWarning, stacklevel=2)
    return lists.get_names(pattern)

def ws_content_scl(pattern: Union[str, List[str]] = '*') -> List[str]:
    warnings.warn("ws_content_scl() is deprecated. " + 
        "Please use the new syntax: scalars.get_names(pattern)",DeprecationWarning, stacklevel=2)
    return scalars.get_names(pattern)

def ws_content_tbl(pattern: Union[str, List[str]] = '*') -> List[str]:
    warnings.warn("ws_content_tbl() is deprecated. " + 
        "Please use the new syntax: tables.get_names(pattern)",DeprecationWarning, stacklevel=2)
    return tables.get_names(pattern)

def ws_content_var(pattern: Union[str, List[str]] = '*') -> List[str]:
    warnings.warn("ws_content_var() is deprecated. " + 
        "Please use the new syntax: variables.get_names(pattern)",DeprecationWarning, stacklevel=2)
    return variables.get_names(pattern)

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
    if filetype == COMMENTS:
        ws_clear_cmt()
    elif filetype == EQUATIONS:
        ws_clear_eqs()
    elif filetype == IDENTITIES:
        ws_clear_idt()
    elif filetype == LISTS:
        ws_clear_lst()
    elif filetype == SCALARS:
        ws_clear_scl()
    elif filetype == TABLES:
        ws_clear_tbl()
    elif filetype == VARIABLES:
        ws_clear_var()
    else:
        raise ValueError(f"IODE type {filetype} is invalid")

def ws_clear_cmt():
    warnings.warn("ws_clear_cmt() is deprecated. " + 
        "Please use the new syntax:\ncomments.clear()",DeprecationWarning, stacklevel=2)
    comments.clear()

def ws_clear_eqs():
    warnings.warn("ws_clear_eqs() is deprecated. " + 
        "Please use the new syntax: equations.clear()",DeprecationWarning, stacklevel=2)
    equations.clear()

def ws_clear_idt():
    warnings.warn("ws_clear_idt() is deprecated. " + 
        "Please use the new syntax: identities.clear()",DeprecationWarning, stacklevel=2)
    identities.clear()

def ws_clear_lst():
    warnings.warn("ws_clear_lst() is deprecated. " + 
        "Please use the new syntax: lists.clear()",DeprecationWarning, stacklevel=2)
    lists.clear()

def ws_clear_scl():
    warnings.warn("ws_clear_scl() is deprecated. " + 
        "Please use the new syntax: scalars.clear()",DeprecationWarning, stacklevel=2)
    scalars.clear()

def ws_clear_tbl():
    warnings.warn("ws_clear_tbl() is deprecated. " + 
        "Please use the new syntax: tables.clear()",DeprecationWarning, stacklevel=2)
    tables.clear()

def ws_clear_var():
    warnings.warn("ws_clear_var() is deprecated. " + 
        "Please use the new syntax: variables.clear()",DeprecationWarning, stacklevel=2)
    variables.clear()

# Load WS
# -------

def ws_load(filename: str, filetype: int) -> Any:
    '''Load an IODE file and return the number of read objects'''
    if filetype == COMMENTS:
        ws_load_cmt(filename)
    elif filetype == EQUATIONS:
        ws_load_eqs(filename)
    elif filetype == IDENTITIES:
        ws_load_idt(filename)
    elif filetype == LISTS:
        ws_load_lst(filename)
    elif filetype == SCALARS:
        ws_load_scl(filename)
    elif filetype == TABLES:
        ws_load_tbl(filename)
    elif filetype == VARIABLES:
        ws_load_var(filename)
    else:
        raise ValueError(f"IODE type {filetype} is invalid") 

def ws_load_cmt(filename: str):
    warnings.warn("ws_load_cmt() is deprecated. " + 
        "Please use the new syntax: comments.load(filepath)",DeprecationWarning, stacklevel=2)
    comments.load(filename)

def ws_load_eqs(filename: str) -> int:
    warnings.warn("ws_load_eqs() is deprecated. " + 
        "Please use the new syntax: equations.load(filepath)",DeprecationWarning, stacklevel=2)
    equations.load(filename)

def ws_load_idt(filename: str) -> int:
    warnings.warn("ws_load_idt() is deprecated. " + 
        "Please use the new syntax: identities.load(filepath)",DeprecationWarning, stacklevel=2)
    identities.load(filename)

def ws_load_lst(filename: str) -> int:
    warnings.warn("ws_load_lst() is deprecated. " + 
        "Please use the new syntax: lists.load(filepath)",DeprecationWarning, stacklevel=2)
    lists.load(filename)

def ws_load_scl(filename: str) -> int:
    warnings.warn("ws_load_scl() is deprecated. " + 
        "Please use the new syntax: scalars.load(filepath)",DeprecationWarning, stacklevel=2)
    scalars.load(filename) 
                
def ws_load_tbl(filename: str) -> int:
    warnings.warn("ws_load_tbl() is deprecated. " + 
        "Please use the new syntax: tables.load(filepath)",DeprecationWarning, stacklevel=2)
    tables.load(filename) 
                             
def ws_load_var(filename: str) -> int:
    warnings.warn("ws_load_var() is deprecated. " + 
        "Please use the new syntax: variables.load(filepath)",DeprecationWarning, stacklevel=2)
    variables.load(filename)

# Save WS
# -------

def ws_save(filename: str, filetype: int):
    '''Save the current IODE workspace of a given type'''
    if filetype == COMMENTS:
        ws_save_cmt(filename)
    elif filetype == EQUATIONS:
        ws_save_eqs(filename)
    elif filetype == IDENTITIES:
        ws_save_idt(filename)
    elif filetype == LISTS:
        ws_save_lst(filename)
    elif filetype == SCALARS:
        ws_save_scl(filename)
    elif filetype == TABLES:
        ws_save_tbl(filename)
    elif filetype == VARIABLES:
        ws_save_var(filename)
    else:
        raise ValueError(f"IODE type {filetype} is invalid") 

def ws_save_cmt(filename: str):
    '''Save the current comment workspace'''
    warnings.warn("ws_save_cmt() is deprecated. " + 
        "Please use the new syntax: comments.save(filepath)",DeprecationWarning, stacklevel=2)
    comments.save(filename)

def ws_save_eqs(filename: str):
    '''Save the current equation workspace'''
    warnings.warn("ws_save_eqs() is deprecated. " + 
        "Please use the new syntax: equations.save(filepath)",DeprecationWarning, stacklevel=2)
    equations.save(filename)

def ws_save_idt(filename: str):
    '''Save the current identity workspace'''
    warnings.warn("ws_save_idt() is deprecated. " + 
        "Please use the new syntax: identities.save(filepath)",DeprecationWarning, stacklevel=2)
    identities.save(filename)

def ws_save_lst(filename: str):
    '''Save the current list workspace'''
    warnings.warn("ws_save_lst() is deprecated. " + 
        "Please use the new syntax: lists.save(filepath)",DeprecationWarning, stacklevel=2)
    lists.save(filename)

def ws_save_scl(filename: str):
    '''Save the current scalar workspace'''
    warnings.warn("ws_save_scl() is deprecated. " + 
        "Please use the new syntax: scalars.save(filepath)",DeprecationWarning, stacklevel=2)
    scalars.save(filename)

def ws_save_tbl(filename: str):
    '''Save the current table workspace'''
    warnings.warn("ws_save_tbl() is deprecated. " + 
        "Please use the new syntax: tables.save(filepath)",DeprecationWarning, stacklevel=2)
    tables.save(filename)

def ws_save_var(filename: str):
    warnings.warn("ws_save_var() is deprecated. " + 
        "Please use the new syntax: variables.save(filepath)",DeprecationWarning, stacklevel=2)
    variables.save(filename)


# High to Low
# -----------

def ws_htol(filename: str, varlist, series_type: int):
    if series_type == HTOL_LAST:
        ws_htol_last(filename, varlist)
    elif series_type == HTOL_MEAN:
        ws_htol_mean(filename, varlist)
    elif series_type == HTOL_SUM:
        ws_htol_sum(filename, varlist)
    else:
        raise ValueError("Wrong value for 'series_type'")

def ws_htol_last(filename: str, varlist):
    warnings.warn("ws_ltoh_stock() is deprecated. " + 
        "Please use the new syntax:\nhigh_to_low(HTOL_LAST, filepath, var_list)",DeprecationWarning, stacklevel=2)
    high_to_low(HTOL_LAST, filename, varlist)

def ws_htol_mean(filename: str, varlist):
    warnings.warn("ws_htol_mean() is deprecated. " + 
        "Please use the new syntax:\nhigh_to_low(HTOL_MEAN, filepath, var_list)",DeprecationWarning, stacklevel=2)
    high_to_low(HTOL_MEAN, filename, varlist)

def ws_htol_sum(filename: str, varlist):
    warnings.warn("ws_htol_sum() is deprecated. " + 
        "Please use the new syntax:\nhigh_to_low(HTOL_SUM, filepath, var_list)",DeprecationWarning, stacklevel=2)
    high_to_low(HTOL_SUM, filename, varlist)


# Low to High
# -----------

def ws_ltoh(filename: str, varlist, series_type, method: Union[int, str]):
    if isinstance(method, int):
        method = chr(method)
    if series_type == LTOH_FLOW:
        ws_ltoh_flow(filename, varlist, method)
    elif series_type == LTOH_STOCK:
        ws_ltoh_stock(filename, varlist, method)
    else:
        raise ValueError("Wrong value for 'series_type'")

def ws_ltoh_flow(filename: str, varlist, method: Union[int, str] = LTOH_CS):
    warnings.warn("ws_ltoh_flow() is deprecated. " + 
        "Please use the new syntax:\nlow_to_high(LTOH_FLOW, method, filepath, var_list)",DeprecationWarning, stacklevel=2)
    low_to_high(LTOH_FLOW, method, filename, varlist)

def ws_ltoh_stock(filename: str, varlist, method: Union[int, str] = LTOH_CS):
    warnings.warn("ws_ltoh_stock() is deprecated. " + 
        "Please use the new syntax:\nlow_to_high(LTOH_STOCK, method, filepath, var_list)",DeprecationWarning, stacklevel=2)
    low_to_high(LTOH_STOCK, method, filename, varlist)
