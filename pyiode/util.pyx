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
#       suppress_msgs()                         | Suppress the output during an IODE session.
#       enable_msgs()                            | Reset the normal output mechanism during an IODE session.
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


# Messages
# --------

def suppress_msgs():
    '''Suppress the output during an IODE session.'''
    IodeSuppressMsgs()


def enable_msgs():
    '''Reset the normal output mechanism during an IODE session.'''
    IodeResetMsgs()
 