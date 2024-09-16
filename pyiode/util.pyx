#  IODE EXTENSION FOR PYTHON
#  =========================
#
#    @header4iode
# 
#  Utilities
#  =========
#       version() -> str                        | Return the Iode version.    
#       _cstr(pystr) -> bytes                   | Convert a python string (UTF8) to a C null terminated string (ANSI cp850).
#       _pystr(cstr) -> str                     | Convert a C null terminated string (ANSI cp850) into a python string (UTF8).
#       _arg_to_str(arg, sep: str = ' ') -> str | Convert None, a list of str or an str to a str
#       suppress_msgs()                         | Suppress the output during an IODE session.'''
#       reset_msgs()                            | Reset the normal output mechanism during an IODE session.'''
#       
#       add_error_msg(msg: str = '')            | Add an error message to the stack of error messages.
#       display_error_msgs()                    | Display all messages accumulated on to the stack of error messages.
#       clear_error_msgs()                      | Empty the stack of error messages
# ------------------------------------------------------------------------------------------------------------------

from collections.abc import Iterable
from util cimport (IODE_IS_A_NUMBER, IodeVersion, B_GetIodeMsgPath, B_msg, IodeSuppressMsgs, IodeResetMsgs,  
                   IodeAddErrorMsg, IodeDisplayErrorMsgs, IodeClearErrorMsgs)


def is_NA(value: float) -> bool:
    """
    Check whether a float value represents a valid IODE number or an IODE *Not Available* :math:`NA` value.

    Parameters
    ----------
    value: float

    Returns
    -------
    bool
        True if the float value represents an IODE *Not Available* :math:`NA` value.

    Examples
    --------
    >>> from iode import NA, is_NA
    >>> is_NA(1.0)
    False
    >>> is_NA(NA)
    True
    """
    return not IODE_IS_A_NUMBER(value)

def _iode_number_to_str(value: float) -> str:
    return "na" if is_NA(value) else f"{value:g}"


# Miscellaneous functions
# -----------------------
def version() -> str:
    "Return the Iode version."
    return _pystr(IodeVersion())


# Conversions python-C strings
# ----------------------------
def _cstr(py_str) -> bytes:
    '''Convert a python string (UTF8) to a C null terminated string (ANSI cp850).'''
    return py_str.encode("cp850") if py_str is not None else None

def _pystr(c_str) -> str:
    '''Convert a C null terminated string (ANSI cp850) into a python string (UTF8).'''
    return c_str.decode("cp850") if c_str is not None else None

# Convert None, a string or an Iterable of strings to a string
def _arg_to_str(arg, sep: str = ' ') -> str:
    '''Convert a string, a list of strings or None to a string.'''
    if isinstance(arg, str):
        return arg
    elif isinstance(arg, Iterable) and all(isinstance(item, str) for item in arg):
        return sep.join(arg)
    elif arg is None:
        return ""
    else:
        raise RuntimeError(f"arg_to_str(): Bad argument type: {type(arg).__name__}")
    


# Messages
# --------

def suppress_msgs():
    '''Suppress the output during an IODE session.'''
    IodeSuppressMsgs()


def reset_msgs():
    '''Reset the normal output mechanism during an IODE session.'''
    IodeResetMsgs()


def _iode_msg_path() -> str:
    '''Return the path to the iode.msg file.
    
    Returns
    -------
    str

    Examples
    --------
    >>> from pathlib import Path
    >>> from iode.iode_python import _iode_msg_path
    >>> iode_msg_path = Path(_iode_msg_path())
    >>> iode_msg_path.exists()
    True
    >>> iode_msg_path.name
    'iode.msg'
    '''
    iode_msg_path = B_GetIodeMsgPath()
    if iode_msg_path is None:
        raise RuntimeError("iode.msg file not found.")
    return iode_msg_path.decode("cp850")


def _print_error_msg(error_code: int) -> str:
    '''Print an error message corresponding to an error code.
       See error codes and error messages in the iode.msg file.

    Parameters
    ----------
    error_code: int
       error code

    Returns
    -------
    str

    Examples
    --------
    >>> from iode.iode_python import _print_error_msg
    >>> _print_error_msg(16)
    ' Sample modified'
    '''
    return B_msg(error_code).decode("cp850")


def add_error_msg(msg: str = ''):
    r'''
    Add an error message to the stack of error messages.
   
    Parameters
    ----------
    msg: str
        string containing the text of the error
        
    Examples
    --------
    >>> from iode import add_error_msg, display_error_msgs
    >>> add_error_msg("Missing variables: QIG, PIG")
    >>> display_error_msgs() # doctest: +SKIP
    '''
    IodeAddErrorMsg(_cstr(msg))
 
def display_error_msgs():
    r'''
    Display all messages accumulated on to the stack of error messages.
    Calls clear_error_msgs().
        
    Examples
    --------
    >>> from iode import add_error_msg, display_error_msgs
    >>> add_error_msg("Missing variables: QIG, PIG")
    >>> display_error_msgs()     # doctest: +SKIP
    '''
    IodeDisplayErrorMsgs()
 
def clear_error_msgs():
    r'''
    Empty the stack of error messages.
    clear_error_msgs() is called by display_error_msgs()
   
    Examples
    --------
    >>> from iode import clear_error_msgs, add_error_msg, display_error_msgs
    >>> # reset the stack of messages
    >>> clear_error_msgs() 
    >>> add_error_msg("Missing variables: QIG, PIG")
    >>> display_error_msgs()    # doctest: +SKIP
    '''
    IodeClearErrorMsgs()
