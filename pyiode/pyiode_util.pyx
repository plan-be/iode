#  IODE EXTENSION FOR PYTHON
#  =========================
#
#    @header4iode
# 
#  Utilities
#  =========
#       version()                   Return the Iode version.    
#       cstr(pystr)                 Convert a python string (UTF8) to a C null terminated string (ANSI cp1252).
#       pystr(cstr)                 Convert a C null terminated string (ANSI cp1252) into a python string (UTF8).
#  
#       cpu_init(txt = None)        Reinitialize a cpu session
#       cpu_end_msg(t0, txt = None) Return a string containing a free text + the elapsed time since the last call to cpu_init()
#       cpu_end(t0, txt = None)     Print a free text and the elapsed time since the last call to cpu_init()
#  
#       suppress_msgs()             Suppress the output during an IODE session.'''
#       reset_msgs()                Reset the normal output mechanism during an IODE session.'''
# ------------------------------------------------------------------------------------------------------------------


# Miscellaneous functions
# -----------------------
def version():
    "Return the Iode version."
    return pystr(IodeVersion())

# Conversions python-C strings
# ----------------------------
def cstr(pystr):
    '''Convert a python string (UTF8) to a C null terminated string (ANSI cp1252).'''
    if pystr is None: return None
    return pystr.encode("cp1252")

def pystr(cstr):
    '''Convert a C null terminated string (ANSI cp1252) into a python string (UTF8).'''
    if cstr is None: return None
    return cstr.decode("cp1252")


# Elapsed time 
# ------------
import timeit

cpu_start_value = 0

def cpu_init(txt = None):
    '''Reinitialize a cpu session by recording the current time'''
    global cpu_start_value
    if txt != None:
        print(f"{txt}...")
    cpu_start_value = timeit.default_timer()
    return cpu_start_value

def cpu_end_msg(txt=None, t0=None):
    '''Return a string containing a free text + the elapsed time since the last call to cpu_init()'''
    if t0 is None:
        t0 = cpu_start_value
    cpu = round(1000 * (timeit.default_timer() - t0), 1)
        
    if txt != None:
        res = f"{txt} [{cpu} msecs]"
    else:
        res = f"[{cpu} msecs]"
        
    return res    
        
def cpu_end(txt = None, t0=None):
    '''Print a free text and the elapsed time since the last call to cpu_init()'''
    print(cpu_end_msg(txt, t0))


# Messages
# --------

def suppress_msgs():
    '''Suppress the output during an IODE session.'''
    IodeSuppressMsgs()
    
def reset_msgs():
    '''Reset the normal output mechanism during an IODE session.'''
    IodeResetMsgs()
    
 
 
