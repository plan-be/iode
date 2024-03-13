#  IODE EXTENSION FOR PYTHON
#  =========================
#
#    @header4iode
# 
#  Utilities
#  =========
#       version() -> str      | Return the Iode version.    
#       _cstr(pystr) -> bytes  | Convert a python string (UTF8) to a C null terminated string (ANSI cp850).
#       _pystr(cstr) -> str    | Convert a C null terminated string (ANSI cp850) into a python string (UTF8).
#  
#       suppress_msgs()       | Suppress the output during an IODE session.'''
#       reset_msgs()          | Reset the normal output mechanism during an IODE session.'''
# ------------------------------------------------------------------------------------------------------------------

from pyiode_util cimport IodeVersion, IodeSuppressMsgs, IodeResetMsgs


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


# Messages
# --------

def suppress_msgs():
    '''Suppress the output during an IODE session.'''
    IodeSuppressMsgs()
    
def reset_msgs():
    '''Reset the normal output mechanism during an IODE session.'''
    IodeResetMsgs()
    
 
 
