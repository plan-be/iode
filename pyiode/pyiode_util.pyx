#  IODE EXTENSION FOR PYTHON
#  =========================
#
#    @header4iode
# 
#  Utilities
#  =========
#       version() -> str      | Return the Iode version.    
#       cstr(pystr) -> bytes  | Convert a python string (UTF8) to a C null terminated string (ANSI cp850).
#       pystr(cstr) -> str    | Convert a C null terminated string (ANSI cp850) into a python string (UTF8).
#  
#       suppress_msgs()       | Suppress the output during an IODE session.'''
#       reset_msgs()          | Reset the normal output mechanism during an IODE session.'''
# ------------------------------------------------------------------------------------------------------------------

from pyiode_util cimport IodeVersion, IodeSuppressMsgs, IodeResetMsgs


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



# Messages
# --------

def suppress_msgs():
    '''Suppress the output during an IODE session.'''
    IodeSuppressMsgs()
    
def reset_msgs():
    '''Reset the normal output mechanism during an IODE session.'''
    IodeResetMsgs()
    
 
 
