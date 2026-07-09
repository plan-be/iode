from pyiode.tests.tests cimport cpp_kerror
from pyiode.tests.tests cimport kwarning
from pyiode.tests.tests cimport kpause
from pyiode.tests.tests cimport kmsg
# from pyiode.tests.tests cimport kpanic
from pyiode.tests.tests cimport kconfirm
from pyiode.tests.tests cimport kpause
from pyiode.tests.tests cimport kinformation


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

def iode_information(message: str):
    kinformation(message.encode('utf-8'))
