import sys
import warnings

from pyiode.super cimport K_end_ws
from pyiode.super cimport kerror_super
from pyiode.super cimport kwarning_super
from pyiode.super cimport kmsg_super
from pyiode.super cimport kconfirm_super
from pyiode.super cimport kpanic_super

# https://cython.readthedocs.io/en/stable/src/userguide/language_basics.html#function-pointers


cdef void c_error_super(const int level, const char* msg):
    cdef bytes b_msg = msg
    if level > 0:
        sys.exit("Error: An unexpected error occurred!\nWill exit the program now")
    else:
        raise RuntimeError(b_msg.decode())

kerror_super   = c_error_super

cdef void c_warning_super(const char* msg):
    cdef bytes b_msg = msg
    warnings.warn(b_msg.decode())

kwarning_super = c_warning_super

cdef void c_msg_super(const char* msg):
    cdef bytes b_msg = msg
    print(b_msg.decode())

kmsg_super     = c_msg_super

cdef int c_confirm_super(const char* msg):
    cdef bytes b_msg = msg
    answer = input(b_msg.decode())
    if not len(answer):
        return 1
    answer = answer[0]
    return 0 if answer in "OoYyJj1" else 1

kconfirm_super = c_confirm_super

cdef void c_panic_super():
    msg = "SWAP PANIC!\n"
    msg += "Attempting to save Data saved in ws.*files\n"
    msg += "To restart iode with the last data, type :\n"
    msg += "    iode -ws"
    K_end_ws(1)
    sys.exit(msg)

kpanic_super   = c_panic_super
