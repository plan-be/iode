from libc.stdlib cimport malloc, free
from libc.string cimport strcpy, strlen

from pyiode.a2m_print cimport A2M_FONTSIZE, A2M_FONTFAMILY, A2M_TFONTFAMILY, A2M_FONTINCR
from pyiode.a2m_print cimport A2M_TFONTSIZE, A2M_NUMBERS
from pyiode.a2m_print cimport A2M_GDI_LMARG, A2M_GDI_RMARG, A2M_GDI_TMARG, A2M_GDI_BMARG 
from pyiode.a2m_print cimport A2M_GDI_COLOR, A2M_GDI_TBORDER
from pyiode.a2m_print cimport A2M_TBREAK, A2M_TPAGE, A2M_GPAGE, A2M_TSHADING_BRUSH
from pyiode.a2m_print cimport set_page_header, set_page_footer
from pyiode.a2m_print cimport W_gdiask


# see B_A2mGetGdiParms() from api/b_a2mini.c
def set_printer_preferences(font_size: int=10, font_family: str='H', table_font_family: str='H', 
                            font_incr: int=2, table_font_size: int=10, paragraph_numbers: bool=False, 
                            left_margin: int=10, right_margin: int=10, top_margin: int=10, 
                            bottom_margin: int=10, black_and_white: bool=True, table_border_width: int=0, 
                            table_break: bool=False, table_new_page: bool=False, graph_new_page: bool=False, 
                            table_shading: bool=False, page_header: str="", page_footer: str="", 
                            ask_prompt: bool=True):
    
    cdef bytes b_page_header = page_header.strip().encode('utf-8')
    cdef size_t length_page_header = len(b_page_header) + 1
    cdef bytes b_page_footer = page_footer.strip().encode('utf-8')
    cdef size_t length_page_footer = len(b_page_footer) + 1

    font_family = font_family.upper()[0]
    if font_family not in "HTCBP":
        raise ValueError("font_family must be one of 'HTCBP'")

    table_font_family = table_font_family.upper()[0]
    if table_font_family not in "HTCBP":
        raise ValueError("table_font_family must be one of 'HTCBP'")

    A2M_FONTSIZE    = font_size
    A2M_FONTFAMILY  = ord(font_family)
    A2M_TFONTFAMILY = ord(table_font_family)
    A2M_FONTINCR    = font_incr
    A2M_TFONTSIZE   = table_font_size
    A2M_NUMBERS     = int(paragraph_numbers)

    A2M_GDI_LMARG   = left_margin
    A2M_GDI_RMARG   = right_margin
    A2M_GDI_TMARG   = top_margin
    A2M_GDI_BMARG   = bottom_margin
    A2M_GDI_COLOR   = int(not black_and_white)
    A2M_GDI_TBORDER = table_border_width

    A2M_TBREAK      = int(table_break)
    A2M_TPAGE       = int(table_new_page)
    A2M_GPAGE       = int(graph_new_page)

    if table_shading: 
        A2M_TSHADING_BRUSH[0] = 7
        A2M_TSHADING_BRUSH[1] = 5
    else:
        A2M_TSHADING_BRUSH[0] = 0
        A2M_TSHADING_BRUSH[1] = 0

    set_page_header(b_page_header, length_page_header)
    set_page_footer(b_page_footer, length_page_footer)

    W_gdiask = ask_prompt
    # W_gdiduplex = wPrinter_DUPLEX;    // JMP 22/8/2016
    # W_gdiorient = wPrinter_ORIENT;    // JMP 22/8/2016
