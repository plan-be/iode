# distutils: language = c++

from pyiode.wrt cimport W_dest, W_close, W_flush, W_printf, W_print_enum
from pyiode.wrt cimport W_print_cmd, W_print_par, W_print_tit
from pyiode.wrt cimport W_print_pg_header, W_print_pg_footer


def cython_write_destination(filename: str, file_type: int):
    W_dest(filename.encode(), file_type)

def cython_write_flush() -> bool:
    res: int = W_flush()
    return res == 0

def cython_write_close():
    W_close()

def cython_write(txt: str) -> bool:
    res: int = W_printf("%s".encode(), txt.encode())
    return res == 0
    
def cython_write_enum(level: int):
    W_print_enum(level)
    
def cython_write_code_block(level: int):
    W_print_cmd(level)
    
def cython_write_paragraph(level: int):
    W_print_par(level)
    
def cython_write_title(level: int):
    W_print_tit(level)

def cython_write_page_header(arg: str):
    W_print_pg_header(arg.encode())
    
def cython_write_page_footer(arg: str):
    W_print_pg_footer(arg.encode())
