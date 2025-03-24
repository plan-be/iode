from pyiode.tests.tests cimport cpp_kerror
from pyiode.tests.tests cimport kwarning
from pyiode.tests.tests cimport kpause
from pyiode.tests.tests cimport kmsg
# from pyiode.tests.tests cimport kpanic
from pyiode.tests.tests cimport kconfirm
from pyiode.tests.tests cimport kpause
from pyiode.tests.tests cimport kmsgbox


def iode_error(message: str):
    cpp_kerror(message.encode('utf-8'))

def iode_warning(message: str):
    kwarning(message.encode('utf-8'))

def iode_msg(message: str):
    kmsg(message.encode('utf-8'))

def iode_confirm(message: str):
    return kconfirm(message.encode('utf-8'))

def iode_panic():
    # kpanic()
    pass

def iode_pause():
    kpause()

def iode_msgbox(title: str, message: str) -> int:
    return kmsgbox(title.encode('utf-8'), message.encode('utf-8'), NULL)
