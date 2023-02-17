# IODE EXTENSION FOR PYTHON
# =========================
#
#    @header4iode
#
# IODE print functions
# --------------------
#
#   print_obj_def : imprime une définition d'objet cmt | eqs | idt | lst | scl | tbl | var
#   print_obj_title : indique si les titres seuls sont à imprimer
#   print_obj_lec : indique le type d'équation à imprimer
#   print_obj_infos : indique les informations à imprimer
#
#   print_tbl
#   print_var


def print_obj_def_cmt(cmt_list):
    '''

    '''




# IODE writing functions
# ----------------------

#   w_dest(char *filename, int type)
#   w_close()
#   w_flush()
#   w_print(str)
#   # w_InitDisplay()
#   # w_EndDisplay(char *title, int x, int y, int w, int h)
#
#   W_print_enum(int n)
#   W_print_cmd(int n)
#   W_print_par(int n)
#   W_print_tit(int n)
#   W_print_pg_header(char* arg)
#   W_print_pg_footer(char* arg)


def w_dest(filename:str="", dest:int=W_DUMMY):
    '''
    Initialise a new output session
    '''
    return W_dest(cstr(filename), dest)

def w_flush():
    '''
    Flush the output session buffer.
    '''
    return W_flush()

def w_close():
    '''
    End an output session.
    '''
    return W_close()

def w_print(txt:str=""):
    '''
    Send a string into the output session buffer.
    '''
    return W_printf(cstr("%s"), cstr(txt))
    
def w_print_enum(level:int=1, text:str=""):
    W_print_enum(level)
    if len(text) > 0:
        w_print(text + "\n\n")
    
def w_print_cmd(level:int=1, text:str=""):
    W_print_cmd(level)
    if len(text) > 0:
        w_print(text + "\n\n")
    
def w_print_par(level:int=1, text:str=""):
    W_print_par(level)
    if len(text) > 0:
        w_print(text + "\n\n")
    
def w_print_tit(level:int=1, title:str=""):
    W_print_tit(level)
    if len(title) > 0:
        w_print(title + "\n\n")

def w_print_pg_header(arg:str=""):
    W_print_pg_header(cstr(arg))
    
def w_print_pg_footer(arg:str=""):
    W_print_pg_footer(cstr(arg))

    

