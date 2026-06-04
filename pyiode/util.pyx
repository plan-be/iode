# distutils: language = c++

from libcpp.string cimport string
from util cimport IODE_IS_A_NUMBER, IodeSuppressMsgs, IodeResetMsgs
from util cimport RP_define, RP_macro_exists, RP_get_macro


def cython_is_NA(value: float) -> bool:
    return not IODE_IS_A_NUMBER(value)

def cython_suppress_msgs():
    IodeSuppressMsgs()

def cython_enable_msgs():
    IodeResetMsgs()

def cython_define(name: str, macro: str) -> bool:
    arg: str = name + " " + macro
    cdef bytes b_arg = arg.encode()
    cdef char* c_arg = b_arg
    res: int = RP_define(c_arg)
    return res == 0

def cython_macro_exists(name: str) -> bool:
    cdef string c_name = name.encode()
    return RP_macro_exists(c_name)

def cython_macro(name: str) -> str:
    cdef string c_name = name.encode()
    cdef string c_macro = RP_get_macro(c_name)
    b_macro: bytes = c_macro
    return b_macro.decode()
 