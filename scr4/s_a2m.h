#ifndef _S_A2M_H_
#define _S_A2M_H_

#include <scr4.h>

#ifdef PRWINDOWS
#include <scr4w.h>
#endif

#include <s_cpp.h>
#include <math.h>

/******** DEFINES **************/

#define A2M_DESTGDIPRT  1
#define A2M_DESTGDIEMF  2
#define A2M_DESTGDIWMF  3
#define A2M_DESTRTF     4
#define A2M_DESTHTML    5
#define A2M_DESTFRM     6
#define A2M_DESTA2M     7
#define A2M_DESTCSV     8
#define A2M_DESTDUMMY   9
#define A2M_DESTTCHRT  10

#define A2M_PAGE    1
#define A2M_PAR     2
#define A2M_TBL     3
#define A2M_GRF     4
#define A2M_LINE1   5
#define A2M_LINE2   6
#define A2M_TOPIC   7
#define A2M_TOPICKW 8
#define A2M_LINE    9
#define A2M_ETOPIC  10
#define A2M_TOPICALIAS 11
#define A2M_MINIDOC 12
#define A2M_GOTO    13
#define A2M_MARGINS 14

#define A2M_TEXT    0
#define A2M_IMAGE   1
#define A2M_NEWLINE 2
#define A2M_TAB     3
#define A2M_SEP     4
#define A2M_IMAGEF  5
#define A2M_IMAGER  6
#define A2M_IMAGEFR 11
#define A2M_BHREF   12  /* JMP 29-04-99 */
#define A2M_EHREF   13  /* JMP 29-04-99 */
#define A2M_ANCHOR  14
#define A2M_FNOTE   15

/********* short cuts **********/

#define GR_X_scale (A2MGRFSCALE[0])
#define GR_Y_scale (A2MGRFSCALE[1])
#define GR_Z_scale (A2MGRFSCALE[2])

#define GR_X_origin (A2MGRFORIG[0])
#define GR_Y_origin (A2MGRFORIG[1])
#define GR_Z_origin (A2MGRFORIG[2])

#define GR_X_0      (A2MGRF0[0])
#define GR_Y_0      (A2MGRF0[1])
#define GR_Z_0      (A2MGRF0[2])

#define GR_WIDTH    (A2MGRFDIM[0])
#define GR_HEIGHT   (A2MGRFDIM[1])

#define GR_obj_id   A2MGRFOBJID
#define GR_graph_id A2MGRFID

/* defines */
#define A2MPCM  28.3
#define GR_WHITE        -2
#define GR_BLACK        -1

/* Device specifics */
#ifdef __cplusplus
extern int (**A2M_GRF_FNS)(...);
#else
extern int (**A2M_GRF_FNS)();
#endif

#define A2mDEVGrfInit       (A2M_GRF_FNS[0])
#define A2mDEVGrfEnd        (A2M_GRF_FNS[1])
#define A2mDEVGrfPen        (A2M_GRF_FNS[2])
#define A2mDEVGrfBrush      (A2M_GRF_FNS[3])
#define A2mDEVGrfLine       (A2M_GRF_FNS[4])
#define A2mDEVGrfText       (A2M_GRF_FNS[5])
#define A2mDEVGrfLabel      (A2M_GRF_FNS[6])
#define A2mDEVGrfPar        (A2M_GRF_FNS[7])
#define A2mDEVGrfGroupObj   (A2M_GRF_FNS[8])
#define A2mDEVGrfGroup      (A2M_GRF_FNS[9])
#define A2mDEVGrfBox        (A2M_GRF_FNS[10])
#define A2mDEVGrfPolyLine   (A2M_GRF_FNS[11])
#define A2mDEVGrfPolyBar    (A2M_GRF_FNS[12])
#define A2mDEVGrfPrepare    (A2M_GRF_FNS[13])
#define A2mDEVGrfGetPen     (A2M_GRF_FNS[14])
#define A2mDEVGrfGetBrush   (A2M_GRF_FNS[15])

/****** STRUCTURES ************/

typedef struct _a2mfile_ {
    CPPFILE *af_cpp;            /* CPPFILE ptr */
    char    *af_outfile;        /* Output File */
    FILE    *af_fda2m;          /* File descriptor A2M */
    short   af_dest;            /* Destination */
} A2MFILE;

typedef struct _a2mobj_ {
    short   ao_type;
    void    *ao_ptr;
    short   ao_spaceb;
    short   ao_spacea;
    short   ao_w;
    short   ao_h;
    short   ao_h1;
    short   ao_kwn;
    short   ao_newpage;   /* JMP 08-09-97 */
} A2MOBJ;

typedef struct _a2mtop_ {
    short   atp_nb;
    short   atp_level;
    short   atp_linkauto;
    short   atp_linkcase;
    short   atp_linkpart;
    short   atp_lg;
    short   atp_linktitle;
    U_ch    *atp_keyws;
    U_ch    *atp_keywsu;
} A2MTOP;

typedef struct _a2mtkw_ {
    U_ch    *atk_keyws;
} A2MTKW;

typedef struct _a2mline_ {
    short   al_type;
} A2MLINE;

typedef struct _a2mpage_ {
    short   apg_type;
} A2MPAGE;

typedef struct _a2mfnt_ { /* Font Definition */
    short   af_size;
    short   af_asize;       /* Added point size */
    short   af_color;
    short   af_pos;         /* -1=subscript, 0=normal, 1=superscript */
    U_ch    af_family;      /* 0='T'=Times, 'A'rial, 'C'ourier */
    U_ch    af_italic   :2;
    U_ch    af_bold     :2;
    U_ch    af_underline:2;
    U_ch    af_caps     :2;
    U_ch    af_strike   :2;
} A2MFNT;

typedef struct _a2mppr_ { /* Para PRops */
    char    pp_tag[32];     /* Par tag */
    A2MFNT  pp_fnt;
    short   pp_just;        /* 0=left, 1=center, 2=right, 3=dec, 4=leftright */
    short   pp_lmarg;       /* left margin */
    short   pp_spaceb;      /* space before */
    short   pp_spacea;      /* space after */
    U_ch    pp_keepwn;      /* keep with next */
    U_ch    pp_keepwp;      /* keep with prev */
    U_ch    pp_number;      /* numbering level */
    U_ch    pp_newpage;     /* start top of page */
    U_ch    pp_bullet;      /* bulleted */
    U_ch    pp_html;        /* HTML Header level */
    U_ch    pp_nowrap;      /* HTML NOWRAP */
} A2MPPR;

typedef struct _a2mstr_ {
    U_ch    *as_txt;        /* Paragraph text or filename */
    U_ch    *as_ctag;       /* Character Tag, if any (dft 0) */
    A2MFNT  as_fnt;         /* Font properties */
    U_ch    as_type;        /* A2M_TEXT, A2M_IMAGE, A2M_NEWLINE, A2M_TAB, A2M_SEP, ...*/
    long    as_w, as_h;     /* Width and Height of the string */
} A2MSTR;

typedef struct _a2mpar_ {
    A2MSTR  **ap_strs;      /* Paragraph sub strings NULL Term. list */
    A2MPPR  ap_ppr;         /* Paragraph props */
    A2MSTR  **ap_as2;       /* Para Splitted in words */
    U_ch    *ap_tag;        /* Paragraph tag  */
    U_ch    ap_lfon;        /* Linefeed ON */
    short   ap_lmarg;       /* Left margin */
    short   ap_spaceb;      /* Space before */
    short   ap_spacea;      /* Space after */
    short   ap_w;           /* Para width */
    short   ap_wmin;        /* Min Para width (SEP's) */
    short   ap_wmax;        /* Max Para width (NEWLINEs) */
    short   ap_h;           /* Para height */
    short   ap_h1;          /* First line height */
} A2MPAR;

typedef struct a2mtcell_ {
    A2MPAR  *atc_par;
    short   atc_ncells,
	    atc_center; /* L=0, C=1, R=2, D=3, J=4, <tag>=5 */
    short   atc_w;
} A2MTC;

typedef struct a2mtl_ {
    A2MTC   *atl_tcs;
    short   atl_type;
    short   atl_hbf; /* 0=Header, 1=Boodi, 2=Fouter */
    short   atl_h;
} A2MTL;

typedef struct _a2mtbl_ {
    U_ch    *at_tag;        /* Table tag, if defined */
    A2MPAR  *at_title;      /* Title */
    A2MPAR  *at_footnote;   /* Footnote */
    A2MTL   *at_tls;        /* Body lines */
    short   at_nl,
	    at_nc;
    double  *at_widths;     /* User defined col widths (cm) */
    short   *at_ws;         /* Col widths  */
    short   at_spaceb;      /* Space before */
    short   at_spacea;      /* Space after */
    short   at_h;           /* Table height */
    short   at_h1;          /* First block height */
    short   at_hhead;       /* Header height */
    short   at_hfoot;       /* Footer height */
    short   at_hbody;       /* Body   height */
    short   at_hbody1;      /* First body line height */
    short   at_shadcol[2];  /* Shading colors*/
    short   at_shadbrush[2];/* Shading brushs*/
    short   at_border;      /* Border Width*/
    short   at_break;       /* Break */           /* JMP 08-09-97 */
    short   at_newpage;     /* New Page */        /* JMP 08-09-97 */
    short   at_tspaceb;     /* Space before .tspaceb */     /* JMP 15-03-2004 */
    short   at_tspacea;     /* Space after .tspacea */      /* JMP 15-03-2004 */
} A2MTBL;

typedef struct _a2mpen_ {
    double  p_width;                /* valeur en pts */
    char    p_color[9];
    short   p_nbsegments;
    double  p_segment[8];           /* valeur en pts */
} A2MPEN;

typedef struct _a2mbrush_ {
    short   b_fill;
    char    b_color[9];
} A2MBRUSH;

typedef struct _a2mgrd_ {
    double  ag_val;
    short   ag_type;
} A2MGRID;

typedef struct _a2mlab_ {
    A2MPAR  *al_par;
    double  al_val;
} A2MLAB;

typedef struct _a2mgxy_ {
    A2MPAR  *xy_title;
    double  *xy_vals;
    short   xy_nobs;
    short   xy_type;   /* 0 = L ; 1 = B ; 2 = S ; 3 = F */
    short   xy_axis;   /* 0 = y ; 1 = z */
} A2MGXY;

typedef struct _a2mgrf_ {
    A2MPAR  *ag_title;
    A2MPAR  *ag_footnote;
    A2MPAR  *ag_header;
    A2MPAR  *ag_footer;
    A2MPAR  *ag_legends[3];  /* Axes titles x, y, z */

    double  ag_size[2];      /* Graph size in cm */
    double  ag_mins[3];
    double  ag_maxs[3];

    short   ag_grid[3];         /* N = none, M = Major, m = Minor , T, t = time */
    A2MGRID **ag_grids[3];
    A2MLAB  **ag_labs[3];

    A2MGXY  **ag_gxy;
    short   ag_spaceb;
    short   ag_spacea;
    short   ag_h;
    short   ag_w;
    short   ag_newpage;   /* JMP 08-09-97 */
    short   ag_fillbrush; /* JMP 20-02-98 */
    short   ag_fillcolor; /* JMP 20-02-98 */
    short   ag_box;       /* JMP 20-02-98 */
} A2MGRF;

typedef struct _a2manchor_ {
    U_ch    aa_name[20];
    short   aa_nb_aos;
    A2MOBJ  **aa_aos;
} A2MANCHOR;

typedef struct _a2mgoto_ {
    double ag_x;
    double ag_y;
} A2MGOTO;

typedef struct _a2mmargins_ {
    double am_left, am_right;
    double am_top, am_bottom;
} A2MMARGINS;



/************** VARIABLES ********************/
#ifdef __cplusplus 
extern "C" {
#endif    
    /* Public (Modifiables par programme) */
    extern int  A2M_TFONTSIZE, A2M_FONTSIZE, A2M_FONTINCR, A2M_FONTFAMILY,
            A2M_TFONTFAMILY; /* JMP 26-02-98 */
    extern int  A2M_TBREAK, A2M_TPAGE, A2M_GPAGE; /* JMP 08-09-97 */
    extern U_ch A2M_CURTAG[41], *A2M_PGHEAD, *A2M_PGFOOT;
    extern int  A2M_CMDCH, A2M_ESCCH, A2M_SEPCH,
            A2M_DEFCH, A2M_TSHADING_COL[2], A2M_TSHADING_BRUSH[2];
    extern int  A2M_NUMBERS, A2M_LFON, A2M_BLON;
    extern A2MTOP **A2M_TOPICS;  /* JMP 17-05-99 */
    extern int  A2M_NB_TOPICS; /* JMP 17-05-99 */
    extern int  A2M_TOPS_CASE; /* JMP 21-05-99 */
    extern int  A2M_TOPS_AUTO; /* JMP 21-05-99 */
    extern int  A2M_TOPS_PARTIAL; /* JMP 21-05-99 */
    extern int  A2M_TOPS_TITLE; /* JMP 24-05-99 */
    extern int  A2M_MAXPARLEVEL;  /* JMP 03-09-11 */
    extern int  A2M_GR_MULTIBAR;  /* JMP 21-11-2011 */
    
    /* Public GDI specific */
    extern int A2M_GDI_COLOR;
    extern int A2M_GDI_TBORDER;
    extern int A2M_GDI_LMARG;
    extern int A2M_GDI_RMARG;
    extern int A2M_GDI_TMARG;
    extern int A2M_GDI_BMARG;
    extern int A2M_GDI_TWIDTH_FIXED; /* JMP 21-03-05  : Si 0 (dft), élargi les colonnes au min du paragraphe, si 1, reste fixe*/
    
    /* Public GIF specific */
    extern int A2M_GIF_BGCOLOR[3];
    extern int A2M_GIF_TRCOLOR[3];
    extern int A2M_GIF_INTER;
    extern int A2M_GIF_TRANS;
    extern int A2M_GIF_WIDTH;
    extern int A2M_GIF_HEIGHT;
    extern int A2M_GIF_KEEP;
    extern int A2M_GIF_RESIZE;
    extern char *A2M_GIF_FRAC;
    extern int A2M_GIF_FONT;
    extern int A2M_GIF_FILLED;
    
    /* Public RTF specific */
    extern int     A2M_RTF_TCOLOR, A2M_RTF_TBORDER, A2M_RTF_TOC;
    extern U_ch    *A2M_RTF_TITLE, *A2M_RTF_COPYRIGHT;
    extern int     A2M_RTF_TCOL1; /* mm */
    extern int     A2M_RTF_TCOLN; /* mm */
    extern int     A2M_RTF_TSPLIT, A2M_RTF_TWIDTH;
    extern int     A2M_RTF_HLINES, A2M_RTF_VLINES;
    extern int     A2M_RTF_COMPRESS;
    extern int     A2M_RTF_GIFTOBMP; /* JMP 16-12-97 */
    extern int     A2M_RTF_GIFTOPNG; /* JMP 23-08-15 */
    extern int     A2M_RTF_OEMTOANSI; /* JMP 12-04-07 */
    
    /* Public MAKER specific */
    extern int    A2M_FRM_TCOLOR;
    extern int    A2M_FRM_TCOL1; /* mm */
    extern int    A2M_FRM_TCOLN; /* mm */
    extern int    A2M_FRM_TSPLIT, A2M_FRM_TWIDTH, A2M_FRM_TBORDER;
    extern int    A2M_FRM_REF, A2M_FRM_HLINES, A2M_FRM_VLINES, A2M_FRM_TTITLE;
    
    /* Public HTML specific */
    extern int    A2M_HTML_TCOLOR, A2M_HTML_TBORDER, A2M_HTML_TOC;
    extern char   *A2M_HTML_BODY;
    extern U_ch   *A2M_HTML_TITLE;
    extern int    A2M_HTML_LTGT;
    extern int    A2M_HTML_STRIP;
    extern int    A2M_HTML_TTITLE; /* JMP 12-02-99 */
    extern int    A2M_HTML_GIFTOBMP; /* JMP 16-12-97 */
    extern int    A2M_HTML_GIFTOPNG; /* JMP 23-08-15 */
    extern char   *A2M_HTML_STYLE; /* JMP 09-05-99 */
    extern char   *A2M_HTML_RELSTYLE; /* JMP 29-01-2004 */
    extern char   *A2M_HTML_TARGET ; /* JMP 28-02-2004 */
    extern int    A2M_HTML_AUTOLINK; /* JMP 20-10-01 */
    extern int    A2M_HTML_IMGWIDTH;    /* JMP 29-06-02 */
    extern int    A2M_HTML_IMGBORDER;   /* JMP 29-06-02 */
    extern int    A2M_HTML_OEMTOANSI; /* JMP 16-08-10 */
    extern int    A2M_HTML_NOINLINESTYLE; /* JMP 25-09-10 */
    extern char   *A2M_HTML_TABLECLASS; // JMP 4/10/2013
    extern char   *A2M_HTML_TRCLASS; // JMP 4/10/2013
    extern char   *A2M_HTML_THCLASS; // JMP 4/10/2013
    extern char   *A2M_HTML_TDCLASS; // JMP 4/10/2013
    
    
    
    /* Public CSV specific */
    extern int    A2M_CSV_SEP, A2M_CSV_QUOTE, A2M_CSV_NA;
    
    /* Private */
    extern A2MPPR   *A2M_PPRS;
    extern int      A2M_NB_PPRS;
    extern A2MANCHOR **A2M_ANCHORS;
    extern int      A2M_NB_ANCHORS, A2M_NB_FNOTES;
    extern char     **A2M_FNOTES;
    
    extern int     A2M_TBLID, A2M_FRAMEID, A2MGRFID, A2M_FONTSIZE, A2MGRFOBJID;
    extern double  A2MGRFDIM[3], A2MGRF0[3], A2MGRFSCALE[3], A2MGRFORIG[3];
    extern FILE    *A2M_fdfrm, *A2M_fdtbl, *A2M_fdfrw;
#ifdef __cplusplus     
}
#endif

/************ FUNCTIONS PROTOTYPES *******************/

#ifndef SCRPROTO

A2MFILE *A2mOpen();
A2MFILE *A2mMemBegin();
A2MOBJ  *A2mRead();
A2MSTR  **A2mAddParLine();
A2MPAR  *A2mNewPar();
U_ch    *A2mError();
A2MPAR  *A2mTxt2Par();
void A2mMessage(char *msg);
void (*A2mMessage_super)(char* msg); 
void A2mMessage_toggle(int IsOn);

#else
#include <s_proa2m.h>
#endif

#endif

