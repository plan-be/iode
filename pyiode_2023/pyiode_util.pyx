#  IODE EXTENSION FOR PYTHON
#  =========================
#
#    @header4iode
# 
#  Utilities
#  =========
#       version() -> str                        | Return the Iode version.    
#       cstr(pystr) -> bytes                    | Convert a python string (UTF8) to a C null terminated string (ANSI cp850).
#       pystr(cstr) -> str                      | Convert a C null terminated string (ANSI cp850) into a python string (UTF8).
#       arg_to_str(arg, sep: str = ' ') -> str  | Convert None, a list of str or an str to a str
#       suppress_msgs()                         | Suppress the output during an IODE session.'''
#       reset_msgs()                            | Reset the normal output mechanism during an IODE session.'''
#       
#       add_error_msg(msg: str = '')            | Add an error message to the stack of error messages.
#       display_error_msgs()                    | Display all messages accumulated on to the stack of error messages.
#       clear_error_msgs()                      | Empty the stack of error messages
# ------------------------------------------------------------------------------------------------------------------


# Miscellaneous functions
# -----------------------
def version() -> str:
    "Return the Iode version."
    return pystr(IodeVersion())

# Conversions python-C strings
# ----------------------------
def cstr(pystr) -> bytes:
    '''Convert a python string (UTF8) to a C null terminated string (ANSI cp850).'''
    if pystr is None: return None
    return pystr.encode("cp850")

def pystr(cstr) -> str:
    '''Convert a C null terminated string (ANSI cp850) into a python string (UTF8).'''
    if cstr is None: return None
    return cstr.decode("cp850")

# Convert None, a list or str or an str to a str
def arg_to_str(arg, sep: str = ' ') -> str:
    '''Convert a string, a list of strings or None to a string.'''
    if isinstance(arg, list):
        return sep.join(arg)
    elif isinstance(arg, str):
        return arg
#    elif callable(arg):
#        res = arg()
#        if callable(res): 
#            raise RuntimeError(f"arg_to_str(): Bad argument type: {type(arg)}")
#        else:
#            return arg_to_str(res, sep)
    elif arg is None:
        return ""
    else:
        raise RuntimeError(f"arg_to_str(): Bad argument type: {type(arg)}")
    


# Messages
# --------

def suppress_msgs():
    '''Suppress the output during an IODE session.'''
    IodeSuppressMsgs()
    
def reset_msgs():
    '''Reset the normal output mechanism during an IODE session.'''
    IodeResetMsgs()


def add_error_msg(msg: str = ''):

    r'''
    Add an error message to the stack of error messages.
   
    Parameters
    ----------
    msg: str
        string containing the text of the error
        
    Examples
    --------
    >>> iode.add_error_msg("Missing variables: QIG, PIG")
    >>> iode.display_error_msgs()
    '''

    IodeAddErrorMsg(cstr(msg))
 
 
def display_error_msgs():

    r'''
    Display all messages accumulated on to the stack of error messages.
    Calls clear_error_msgs().
        
    Examples
    --------
    >>> iode.add_error_msg("Missing variables: QIG, PIG")
    >>> iode.display_error_msgs()
    '''

    IodeDisplayErrorMsgs()
 
def clear_error_msgs():

    r'''
    Empty the stack of error messages.
    clear_error_msgs() is called by display_error_msgs()
   
    Examples
    --------
    >>> iode.clear_error_msgs() # reset the stack of messages
    >>> iode.add_error_msg("Missing variables: QIG, PIG")
    >>> iode.display_error_msgs()
    '''

    IodeClearErrorMsgs()
