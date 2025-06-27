cdef extern from "scr4/s_a2m.h":
    # Public (Modifiables par programme)
    cdef int    A2M_TFONTSIZE
    cdef int    A2M_FONTSIZE
    cdef int    A2M_FONTINCR
    cdef int    A2M_FONTFAMILY
    cdef int    A2M_TFONTFAMILY
    cdef int    A2M_TBREAK
    cdef int    A2M_TPAGE
    cdef int    A2M_GPAGE
    cdef unsigned char      A2M_CURTAG[41]
    cdef unsigned char*     A2M_PGHEAD
    cdef unsigned char*     A2M_PGFOOT
    cdef int    A2M_CMDCH 
    cdef int    A2M_ESCCH 
    cdef int    A2M_SEPCH
    cdef int    A2M_DEFCH
    cdef int    A2M_TSHADING_COL[2]
    cdef int    A2M_TSHADING_BRUSH[2]
    cdef int    A2M_NUMBERS
    cdef int    A2M_LFON 
    cdef int    A2M_BLON
    cdef int    A2M_NB_TOPICS 
    cdef int    A2M_TOPS_CASE 
    cdef int    A2M_TOPS_AUTO 
    cdef int    A2M_TOPS_PARTIAL 
    cdef int    A2M_TOPS_TITLE 
    cdef int    A2M_MAXPARLEVEL 
    cdef int    A2M_GR_MULTIBAR 
    
    # Public GDI specific
    cdef int    A2M_GDI_COLOR
    cdef int    A2M_GDI_TBORDER
    cdef int    A2M_GDI_LMARG
    cdef int    A2M_GDI_RMARG
    cdef int    A2M_GDI_TMARG
    cdef int    A2M_GDI_BMARG
    cdef int    A2M_GDI_TWIDTH_FIXED
    
    # Public GIF specific
    cdef int    A2M_GIF_BGCOLOR[3]
    cdef int    A2M_GIF_TRCOLOR[3]
    cdef int    A2M_GIF_INTER
    cdef int    A2M_GIF_TRANS
    cdef int    A2M_GIF_WIDTH
    cdef int    A2M_GIF_HEIGHT
    cdef int    A2M_GIF_KEEP
    cdef int    A2M_GIF_RESIZE
    cdef char*  A2M_GIF_FRAC
    cdef int    A2M_GIF_FONT
    cdef int    A2M_GIF_FILLED
    
    # Public RTF specific
    cdef int    A2M_RTF_TCOLOR, 
    cdef int    A2M_RTF_TBORDER
    cdef int    A2M_RTF_TOC
    cdef unsigned char*    A2M_RTF_TITLE
    cdef unsigned char*    A2M_RTF_COPYRIGHT
    cdef int    A2M_RTF_TCOL1
    cdef int    A2M_RTF_TCOLN
    cdef int    A2M_RTF_TSPLIT
    cdef int    A2M_RTF_TWIDTH
    cdef int    A2M_RTF_HLINES 
    cdef int    A2M_RTF_VLINES
    cdef int    A2M_RTF_COMPRESS
    cdef int    A2M_RTF_GIFTOBMP
    cdef int    A2M_RTF_GIFTOPNG
    cdef int    A2M_RTF_OEMTOANSI
    
    # Public MAKER specific
    cdef int    A2M_FRM_TCOLOR
    cdef int    A2M_FRM_TCOL1
    cdef int    A2M_FRM_TCOLN
    cdef int    A2M_FRM_TSPLIT
    cdef int    A2M_FRM_TWIDTH
    cdef int    A2M_FRM_TBORDER
    cdef int    A2M_FRM_REF
    cdef int    A2M_FRM_HLINES
    cdef int    A2M_FRM_VLINES
    cdef int    A2M_FRM_TTITLE
    
    # Public HTML specific
    cdef int    A2M_HTML_TCOLOR
    cdef int    A2M_HTML_TBORDER
    cdef int    A2M_HTML_TOC
    cdef char*              A2M_HTML_BODY
    cdef unsigned char*     A2M_HTML_TITLE
    cdef int    A2M_HTML_LTGT
    cdef int    A2M_HTML_STRIP
    cdef int    A2M_HTML_TTITLE
    cdef int    A2M_HTML_GIFTOBMP
    cdef int    A2M_HTML_GIFTOPNG
    cdef char*  A2M_HTML_STYLE
    cdef char*  A2M_HTML_RELSTYLE
    cdef char*  A2M_HTML_TARGET
    cdef int    A2M_HTML_AUTOLINK
    cdef int    A2M_HTML_IMGWIDTH
    cdef int    A2M_HTML_IMGBORDER
    cdef int    A2M_HTML_OEMTOANSI
    cdef int    A2M_HTML_NOINLINESTYLE
    cdef char*  A2M_HTML_TABLECLASS
    cdef char*  A2M_HTML_TRCLASS
    cdef char*  A2M_HTML_THCLASS
    cdef char*  A2M_HTML_TDCLASS

    # Public CSV specific 
    cdef int    A2M_CSV_SEP
    cdef int    A2M_CSV_QUOTE
    cdef int    A2M_CSV_NA

cdef extern from "api/all.h":
    cdef double     A2M_GWIDTH
    cdef double     A2M_GHEIGHT
    cdef int        A2M_BOXWIDTH
    cdef int        A2M_BACKBRUSH
    cdef int        A2M_BACKCOLOR
    cdef int        W_gdiask
    cdef int        W_gdiduplex
    cdef int        W_gdiorient
    cdef int        W_a2mapp
    cdef int        W_rtfhelp
    cdef void       B_A2mSetRtfTitle(unsigned char* title)
    cdef void       B_A2mSetRtfCopy(unsigned char* title)

cdef extern from "a2m_print.cpp":
    cdef void set_page_header(char*, size_t)
    cdef void set_page_footer(char*, size_t)
    cdef void set_curtag(char*)
    cdef void set_a2m_html_body(char*, size_t) 
    cdef void set_a2m_html_title(char*, size_t)
