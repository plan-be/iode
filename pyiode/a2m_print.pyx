from libc.stdlib cimport malloc, free
from libc.string cimport strcpy, strlen

# printer_preferences
from pyiode.a2m_print cimport A2M_FONTSIZE, A2M_FONTFAMILY, A2M_TFONTFAMILY, A2M_FONTINCR
from pyiode.a2m_print cimport A2M_TFONTSIZE, A2M_NUMBERS
from pyiode.a2m_print cimport A2M_GDI_LMARG, A2M_GDI_RMARG, A2M_GDI_TMARG, A2M_GDI_BMARG 
from pyiode.a2m_print cimport A2M_GDI_COLOR, A2M_GDI_TBORDER
from pyiode.a2m_print cimport A2M_TBREAK, A2M_TPAGE, A2M_GPAGE, A2M_TSHADING_BRUSH
from pyiode.a2m_print cimport W_gdiask, set_page_header, set_page_footer

# A2M_preferences
from pyiode.a2m_print cimport A2M_ESCCH, A2M_SEPCH, A2M_DEFCH, A2M_CMDCH
from pyiode.a2m_print cimport A2M_BLON, A2M_LFON, A2M_CURTAG
from pyiode.a2m_print cimport A2M_GWIDTH, A2M_GHEIGHT, A2M_BACKCOLOR, A2M_BACKBRUSH, A2M_BOXWIDTH
from pyiode.a2m_print cimport W_a2mapp, set_curtag

# MIF_preferences
from pyiode.a2m_print cimport A2M_FRM_TCOLOR, A2M_FRM_TCOL1, A2M_FRM_TCOLN, A2M_FRM_TSPLIT
from pyiode.a2m_print cimport A2M_FRM_TWIDTH, A2M_FRM_TBORDER, A2M_FRM_HLINES, A2M_FRM_VLINES
from pyiode.a2m_print cimport A2M_FRM_REF

# RTF_preferences
from pyiode.a2m_print cimport A2M_RTF_TCOLOR, A2M_RTF_TCOL1, A2M_RTF_TCOLN, A2M_RTF_TSPLIT
from pyiode.a2m_print cimport A2M_RTF_TWIDTH, A2M_RTF_TBORDER, A2M_RTF_HLINES, A2M_RTF_VLINES
from pyiode.a2m_print cimport A2M_RTF_COMPRESS, A2M_RTF_TOC
from pyiode.a2m_print cimport W_rtfhelp, B_A2mSetRtfTitle, B_A2mSetRtfCopy

# HTML_preferences
from pyiode.a2m_print cimport A2M_HTML_TBORDER, A2M_HTML_TCOLOR, A2M_HTML_TOC
from pyiode.a2m_print cimport set_a2m_html_body, set_a2m_html_title

import warnings


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

    if not font_family:
        raise ValueError("font_family is empty")
    font_family = font_family.upper()[0]
    if font_family not in "HTCBP":
        raise ValueError("font_family must be one of 'HTCBP'")

    if not table_font_family:
        raise ValueError("table_font_family is empty")
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
    # W_gdiduplex = wPrinter_DUPLEX    // JMP 22/8/2016
    # W_gdiorient = wPrinter_ORIENT    // JMP 22/8/2016


# see B_A2mGetGnlParms() from api/b_a2mini.c 
def set_A2M_preferences(escape_char: str='\\', cell_separator: str='@', define_char: str ='&', 
                        command_char: str='.', append: bool=False, preserve_spaces: bool=False, 
                        preserve_linefeed: bool=False, default_paragraph: str='', 
                        graph_width: int=160, graph_height: int=100, graph_background_color: str='b',
                        graph_background_brush: int=0, graph_box: int=0):
    
    cdef bytes b_default_paragraph = b''

    # control characters
    if len(escape_char) != 1:
        raise ValueError("escape_char must be a single character")
    if len(cell_separator) != 1:
        raise ValueError("cell_separator must be a single character")
    if len(define_char) != 1:
        raise ValueError("define_char must be a single character")
    if len(command_char) != 1:
        raise ValueError("command_char must be a single character")

    A2M_ESCCH = ord(escape_char)
    A2M_SEPCH = ord(cell_separator)
    A2M_DEFCH = ord(define_char)
    A2M_CMDCH = ord(command_char)

    # Startup Options
    if len(default_paragraph) > 40:
        warnings.warn("default_paragraph is too long, truncating to 40 characters")
        default_paragraph = default_paragraph[:40]
    b_default_paragraph = default_paragraph.encode('utf-8')

    W_a2mapp  = int(append)
    A2M_BLON  = int(preserve_spaces)
    A2M_LFON  = int(preserve_linefeed)
    set_curtag(b_default_paragraph)

    # Graphs
    if not graph_background_color:
        raise ValueError("graph_background_color is empty")
    graph_background_color = graph_background_color.lower()
    if graph_background_color == "black":
        graph_background_color = "B"
    graph_background_color = graph_background_color[0]
    if graph_background_color not in "byBwrgcm":
        raise ValueError("graph_background_color must be one of 'byBwrgcm'")
    
    A2M_GWIDTH    = 0.1 * graph_width
    A2M_GHEIGHT   = 0.1 * graph_height
    A2M_BACKCOLOR = ord(graph_background_color)
    A2M_BACKBRUSH = graph_background_brush
    A2M_BOXWIDTH  = graph_box


# see B_A2mGetMifParms() from api/b_a2mini.c 
def set_MIF_preferences(font_size: int=10, font_incr: int=2, font_family: str='H', 
                        table_font_family: str='H', table_font_size: int=10, 
                        table_use_color: bool=False, table_first_col_width: int=60, 
                        table_other_col_width: int=15, table_split: bool=False, 
                        table_width: int=165, table_outside_borders: bool=False, 
                        table_horizontal_lines: bool=False, table_vertical_lines: bool=False, 
                        image_ref_in_text: bool=False):

    if not font_family:
        raise ValueError("font_family is empty")
    font_family = font_family.upper()[0]
    if font_family not in "HTCBP":
        raise ValueError("font_family must be one of 'HTCBP'")

    if not table_font_family:
        raise ValueError("table_font_family is empty")
    table_font_family = table_font_family.upper()[0]
    if table_font_family not in "HTCBP":
        raise ValueError("table_font_family must be one of 'HTCBP'")

    A2M_FONTSIZE     = font_size
    A2M_FONTINCR     = font_incr
    A2M_FONTFAMILY   = ord(font_family)
    A2M_TFONTFAMILY  = ord(table_font_family)
    A2M_TFONTSIZE    = table_font_size
    A2M_FRM_TCOLOR   = int(table_use_color)
    A2M_FRM_TCOL1    = table_first_col_width
    A2M_FRM_TCOLN    = table_other_col_width
    A2M_FRM_TSPLIT   = int(table_split)
    A2M_FRM_TWIDTH   = table_width
    A2M_FRM_TBORDER  = int(table_outside_borders)
    A2M_FRM_HLINES   = int(table_horizontal_lines)
    A2M_FRM_VLINES   = int(table_vertical_lines)
    A2M_FRM_REF      = int(image_ref_in_text)


# see B_A2mGetRtfParms() from api/b_a2mini.c 
def set_RTF_preferences(font_size: int=10, font_incr: int=2, font_family: str='H', 
                        table_font_family: str='H', table_font_size: int=10, 
                        paragraph_numbers: bool=False, table_use_color: bool=False, 
                        table_first_col_width: int=60, table_other_col_width: int=15, 
                        table_width: int=165, table_outside_borders: bool=False, 
                        table_horizontal_lines: bool=False, table_vertical_lines: bool=False, 
                        table_split_tables: bool=False, prepare_for_hcw: bool=False, 
                        compress_help: bool=False, generate_toc: bool=False, 
                        help_title: str="", copyright: str=""):

    cdef bytes b_help_title = help_title.encode('utf-8') 
    cdef bytes b_copyright = copyright.encode('utf-8')

    if not font_family:
        raise ValueError("font_family is empty")
    font_family = font_family.upper()[0]
    if font_family not in "HTCBP":
        raise ValueError("font_family must be one of 'HTCBP'")

    if not table_font_family:
        raise ValueError("table_font_family is empty")
    table_font_family = table_font_family.upper()[0]
    if table_font_family not in "HTCBP":
        raise ValueError("table_font_family must be one of 'HTCBP'")
    
    A2M_FONTSIZE     = font_size
    A2M_FONTFAMILY   = ord(font_family)
    A2M_TFONTFAMILY  = ord(table_font_family)
    A2M_FONTINCR     = font_incr
    A2M_TFONTSIZE    = table_font_size
    A2M_RTF_TCOLOR   = int(table_use_color)
    A2M_RTF_TCOL1    = table_first_col_width
    A2M_RTF_TCOLN    = table_other_col_width
    A2M_RTF_TSPLIT   = int(table_split_tables)
    A2M_RTF_TWIDTH   = table_width
    A2M_RTF_TBORDER  = int(table_outside_borders)
    A2M_RTF_HLINES   = int(table_horizontal_lines)
    A2M_RTF_VLINES   = int(table_vertical_lines)
    W_rtfhelp        = int(prepare_for_hcw)
    A2M_RTF_COMPRESS = int(compress_help)
    A2M_NUMBERS      = int(paragraph_numbers)
    A2M_RTF_TOC      = int(generate_toc)

    B_A2mSetRtfTitle(b_help_title)
    B_A2mSetRtfCopy(b_copyright)


# see B_A2mGetHtmlParms() from api/b_a2mini.c 
def set_HTML_preferences(font_size: int=10, font_family: str='H', table_font_family: str='H', 
                         font_incr: int=2, table_font_size: int=10, paragraph_numbers: bool=False,
                         table_border_width: bool=False, table_use_color: bool=False, 
                         generate_toc: bool=False, body_tag: str="", title: str=""):
    
    cdef bytes b_body_tag = body_tag.encode('utf-8')
    cdef size_t body_tag_len = len(b_body_tag)
    cdef bytes b_title = title.encode('utf-8')
    cdef size_t title_len = len(b_title)

    if not font_family:
        raise ValueError("font_family is empty")
    font_family = font_family.upper()[0]
    if font_family not in "HTCBP":
        raise ValueError("font_family must be one of 'HTCBP'")

    if not table_font_family:
        raise ValueError("table_font_family is empty")
    table_font_family = table_font_family.upper()[0]
    if table_font_family not in "HTCBP":
        raise ValueError("table_font_family must be one of 'HTCBP'")

    A2M_FONTSIZE     = font_size
    A2M_FONTINCR     = font_incr
    A2M_FONTFAMILY   = ord(font_family)
    A2M_TFONTFAMILY  = ord(table_font_family)
    A2M_TFONTSIZE    = table_font_size
    A2M_NUMBERS      = int(paragraph_numbers)
    A2M_HTML_TBORDER = int(table_border_width)
    A2M_HTML_TCOLOR  = int(table_use_color)
    A2M_HTML_TOC     = int(generate_toc)

    set_a2m_html_body(b_body_tag, body_tag_len)
    set_a2m_html_title(b_title, title_len)
