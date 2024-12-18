from pyiode.tests.tests cimport kerror
from pyiode.tests.tests cimport kwarning
from pyiode.tests.tests cimport kpause
from pyiode.tests.tests cimport kmsg
# from pyiode.tests.tests cimport kpanic
from pyiode.tests.tests cimport kconfirm


def iode_c_error(level: int, message: str):
    c_error_super(level, message.encode('utf-8'))

def iode_c_warning(message: str):
    c_warning_super(message.encode('utf-8'))

def iode_c_msg(message: str):
    c_msg_super(message.encode('utf-8'))

def iode_c_confirm(message: str):
    return c_confirm_super(message.encode('utf-8'))

def iode_c_panic():
    c_panic_super()


def iode_error(level: int, message: str):
    kerror(level, message.encode('utf-8'))

def iode_warning(message: str):
    kwarning(message.encode('utf-8'))

def iode_msg(message: str):
    kmsg(message.encode('utf-8'))

def iode_confirm(message: str):
    return kconfirm(message.encode('utf-8'))

def iode_panic():
    # kpanic()
    pass
