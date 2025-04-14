# distutils: language = c++

from util cimport IODE_IS_A_NUMBER, IodeSuppressMsgs, IodeResetMsgs


def cython_is_NA(value: float) -> bool:
    return not IODE_IS_A_NUMBER(value)

def cython_suppress_msgs():
    IodeSuppressMsgs()

def cython_enable_msgs():
    IodeResetMsgs()
