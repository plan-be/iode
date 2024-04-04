# IODE EXTENSION FOR PYTHON
# =========================
#
#    @header4iode
#
# IODE writing functions
# ----------------------

#   w_dest(filename: str, dest: type = W_DUMMY)                 | Initialise a new output session
#   w_close()                                                   | End an output session.
#   w_flush()                                                   | Flush the output session buffer.
#   w_print(txt: str = '')                                      |  Send a string into the output session buffer.
#
#   w_print_enum(level: int = 1, text: str = "")    | Print a bulleted paragraph of the given level
#   w_print_cmd(level: int = 1, text: str = "")     | Start a code block of the given level or end the block if level < 0
#   w_print_par(level: int = 1, text: str = "")     | Print a paragraph of the given level
#   w_print_tit(level: int = 1, title: str = "")    | Print a title of the given level
#   w_print_pg_header(arg: str = "")                | Define the page header as from the current page
#   w_print_pg_footer(arg: str = "")                | Define the page footer as from the current page

from wrt cimport (W_dest, W_close, W_flush, W_printf, W_print_enum, W_print_cmd, W_print_par, 
                         W_print_tit, W_print_pg_header, W_print_pg_footer)


def w_dest(filename: str = "", dest: int = W_DUMMY):
    '''Initialise a new output session'''
    W_dest(_cstr(filename), dest)

def w_flush():
    '''Flush the output session buffer.'''
    return W_flush()

def w_close():
    '''End an output session.'''
    return W_close()

def w_print(txt: str = ""):
    '''Send a string into the output session buffer.'''
    return W_printf(_cstr("%s"), _cstr(txt))
    
def w_print_enum(level: int = 1, text: str = ""):
    '''Print a bulleted paragraph of the given level''' 
    W_print_enum(level)
    if len(text) > 0:
        w_print(text + "\n\n")
    
def w_print_cmd(level: int = 1, text: str = ""):
    '''Start a code block of the given level or end the block if level < 0''' 
    W_print_cmd(level)
    if len(text) > 0:
        w_print(text + "\n\n")
    
def w_print_par(level: int = 1, text: str = ""):
    '''Print a paragraph of the given level''' 
    W_print_par(level)
    if len(text) > 0:
        w_print(text + "\n\n")
    
def w_print_tit(level: int = 1, title: str = ""):
    '''Print a title of the given level''' 
    W_print_tit(level)
    if len(title) > 0:
        w_print(title + "\n\n")

def w_print_pg_header(arg: str = ""):
    '''Define the page header as from the current page''' 
    W_print_pg_header(_cstr(arg))
    
def w_print_pg_footer(arg: str = ""):
    '''Define the page footer as from the current page''' 
    W_print_pg_footer(_cstr(arg))

    

