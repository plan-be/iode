#ifndef _SCR4_STR_H_
#define _SCR4_STR_H_

#include <stdio.h>
#include <s_swap.h>

/* =+============= SCR4_STR.H ================================== */

#define MAX_BFIELD14    16383
#define MAX_BFIELD12    4095
#define MAX_BFIELD6     63

#define MAXBF14         14
#define MAXBF12         12
#define MAXBF6          6

#define U_int unsigned int
#define U_sh  unsigned short
#define U_ch  unsigned char
// #define INT short /* JMP 14-10-96 */

//#include "scr4web.h"

typedef char GZIPFILE; /* JMP 19-10-00 */

typedef struct status {
    short  sta_csr_on  ;
    short  sta_csr_line;
    short  sta_csr_col ;
    short  sta_csr_size;
    short  sta_attr    ;
    short  sta_color   ;
    short  sta_font    ;
    short  sta_insert  ;
} STATUS;

typedef struct a_action {
    U_ch   aa_nb_lg   : 3;
    U_ch   aa_nb_db   : 3;
    U_ch   aa_nb_str  : 2;
    short  aa_len;
    short  aa_fn;
} A_ACTION;

typedef struct action {
    U_ch      act_nb   : 8;
    U_ch      act_catg : 8;
    U_sh      act_len;
    A_ACTION  *act_aa;
} ACTION;

typedef struct file_type {
    char            *fl_name;
    char            *fl_ptr;
    U_ch            fl_type : 5;
    U_ch            fl_def : 1;
    U_ch            fl_mem : 1;
    long            fl_pos;
} FILE_TYPE;

typedef struct file_table {
    short           tbl_nb_fl;
    FILE_TYPE       **tbl_fl;
    } FILE_TABLE;

/*
typedef struct image {
    U_ch           im_line   : 8;
    U_ch           im_column : 8;
    U_ch           im_nb_line: 8;
    U_ch           im_nb_column : 8;
    SWHDL          im_sw;
    unsigned char  *im_cells;
    unsigned char  *im_xattrs; // JMP 04-01-04
} IMAGE;
*/
/* JMP 12-01-11 */

typedef struct image {
    U_sh           im_line   ;      /* JMP 12-01-11 */
    U_sh           im_column ;      /* JMP 12-01-11 */
    U_sh           im_nb_line;      /* JMP 12-01-11 */
    U_sh           im_nb_column;    /* JMP 12-01-11 */
    SWHDL          im_sw;
    unsigned char  *im_cells;
    unsigned char  *im_xattrs;
} IMAGE;


typedef struct _sbar {
    short   sb_line,        /* Ligne   */
	    sb_col,         /* Colonne */
	    sb_orient,      /* 0 = vertical, 1 = horizontal */
	    sb_nlv,         /* nbre lignes (ou colonnes) du scrollbar */
	    sb_nbi,         /* Nombre d'infos dans une page */
	    sb_l1v,         /* Première ligne marquée du scroll */
	    sb_nls,         /* nombre de lignes marquées de scroll */
	    sb_c1,          /* caractère de remplissage 1 */
	    sb_c2,          /* caractère de remplissage 2 */
	    sb_attr;        /* attribut du scroll (couleur) */
} S_BAR;


#define  SCR_MAX_FIELD_LENGTH 512
#define  SCR_MAX_FILE_LG       63
#define  SCR_MAX_FILE_LG_MAX   255
#define  SCR_MAX_LFILE_LG     127   /* JMP 28-09-96 */
#define  SCR_MAX_REC_KEYS     200
#define  SCR_MAX_ISFLNKS      3

#define SCR_MAX_CMP_FIELDS 5

typedef struct isflnk {
    short   isl_isam;
    short   isl_fld_nb;
    U_ch    *isl_fldname; /* BP_M 02-06-01 */
} ISFLNK;

typedef struct isfield {
    U_sh    isf_length; /* JMP 11-04-00 */
    short   isf_type;
    U_sh    isf_start; /* JMP 11-04-00 */
    short   __b1;
    U_ch    isf_required : 1;
    U_ch    isf_var      : 1;
    char    __b2:6;
    U_sh    isf_hsi_fix;
} ISFIELD;

typedef struct _isfinfo {
    unsigned char   *isi_fmt;
    unsigned char   *isi_name;
    unsigned char   *isi_title;
    unsigned char   *isi_prompt;
    unsigned char   *isi_dtext;
    unsigned char   *isi_cmt;
    U_sh            isi_is  /*: MAXBF14*/;
    short           isi_cmp_nb;
    short           isi_fld_nb;
    short           isi_date;
    short           isi_nb_dec;
    short           isi_fix_dec;
    short           isi_del_eof;
    short           isi_io;
    short           isi_menu;
    short           isi_case;
    short           isi_center;
    short           isi_compulsory;
    short           isi_fnks;
    short           isi_nl;
    short           isi_nc;
    short           isi_nlv;
    short           isi_ncv;
    short           __b1;
    unsigned char   *isi_allowed;
    unsigned char   *isi_help;
    short           isi_def_obj;
    short           isi_nb_lnks;  /* JMP 29-10-99 */
    ISFLNK          isi_lnks[SCR_MAX_ISFLNKS]; /* JMP 29-10-99 */
} ISFINFO;

typedef struct composite {
    unsigned char    *cmp_name;
    short   cmp_nb_flds ;
    short   cmp_flds[SCR_MAX_CMP_FIELDS];
    U_sh    cmp_pos[SCR_MAX_CMP_FIELDS]; /* JMP 12-04-00 */
    short   cmp_lgs[SCR_MAX_CMP_FIELDS];
    short   cmp_sqz[SCR_MAX_CMP_FIELDS];
    short   cmp_orderk[SCR_MAX_CMP_FIELDS];
    short   cmp_ref;
    U_ch    cmp_order : 1;
    U_ch    cmp_dup   : 1;
    U_ch    cmp_def_obj:1;
    U_ch    cmp_ge2eq:1;    /* JMP 05-08-99 */
    char    __b1:4;
} COMPOSITE;

typedef struct _isfref_ {
    U_ch    *isr_refidxname;   // index name
    U_sh    isr_refisam;    // referenced isam
    U_sh    isr_refidx;     // referenced index
    U_sh    isr_isf_nb;     // field or index number
    U_ch    isr_type : 1;   // field or index
    U_ch    isr_delete : 1; // delete record if delete ref
    U_ch    isr_update: 1;  // update record if rewrite ref
    char    __b1 : 5;
} ISREF;

typedef struct link {           /* LINK description */
    U_ch    lnk_nb_flds : 4;    /* fields count */
    U_sh    lnk_isam  /*  : MAXBF14*/;  /* reference isam */
    U_ch    *lnk_name;
    short   lnk_flds[SCR_MAX_CMP_FIELDS]; /* fields numbers */
    short   lnk_cmp;            /* reference index in lnk_isam */
} LINK;

typedef struct _vfield {
    long        vf_lg;           /* length of the field */
    char        *vf_ptr;         /* ptr to the value */
    long        vf_rlg;          /* length of the field */
    char        *vf_rptr;        /* ptr to the value */
} VFIELD;

typedef struct _ctinfo {            /* BP_M 17-01-97 */
    unsigned char   *cti_name;          /* nom du champ */
    short           cti_length;         /* longueur total */
    short           cti_cttype;         /* type de ctree */
    short           cti_type;           /* type de SCR */
} CTINFO;

typedef struct _isamsrch_ {         /* JMP 11-12-98 */
    short   isb_cmp;        /* COMPOSITE Nb */
    short   isb_cond;       /* Search condition */
    long    isb_ms;         /* Search Time */
    char    isb_rec[1];     /* Record and Index values */
    char    __b1[3];
} ISAMBUF;

typedef struct _isnexts_ {
    short   isn_nba;    /* Nombre de records alloués */
    short   isn_nb;     /* Nombre de records corrects */
    short   isn_cur;    /* Prochain record */
    short   isn_rcur;   /* Current réel dans isam */
    short   isn_len;    /* Longeur total du record, y compris les tables jointes */
    short   _b1;
    char    isn_recs[1];  /* isn_nba * is->is_rec_len caractères */
    char    _b3[3];

} ISNEXTS;

typedef struct _isjoin {
    U_sh    is;
    short   __b1;
    char    *req;
} ISJOIN;

typedef struct isam {                             /* ISAM description */
    unsigned char  *is_filename;         /* filename */
    short          is_nb_flds;           /* fields count */
    short          is_nb_cmps ;          /* index count */
    short          is_nb_isrefs;         // ISREFS count
    short          is_nb_isrels;         // ISRELS count
    short          is_nb_lnks;           /* links count */
    short          __b10;

    ISFIELD        *is_flds;             /* pointer to the first ISFIELD */
    COMPOSITE      *is_cmps;             /* pointer to the first index */
    ISREF          *is_isrefs;           // pointer to the first isfref
    ISREF          *is_isrels;           // pointer to the first isfrel

    LINK           *is_lnks;             /* pointer to the first link */
    char           *is_isam;             /* structure depending on the isam system */
    char           *is_rec;              /* work record */
    char           *is_rrec;             /* last read record */
    char           *is_rrrec;            /* reread record */

    /* BP_M 02-10-97 */
    char           *is_maxrec;           /* Record contenant les valeurs maximum à trouver */

    U_sh           is_rec_len;           /* len of the record */
    short          is_open_type;         /* type of opening */
    U_ch           is_def_obj     : 1;
    U_ch           is_crypt       : 1;      /* JMP38 14-09-92 */
    U_ch           is_hist        : 1;
    U_ch           is_varlen      : 1;      /* BP_M 13-07-95 */
    U_ch           is_supfile     : 1;      /* BP_M 11-07-95 */
    U_ch           is_type_dbd    : 3;
    U_ch           is_nb_open     : 8;   /* number of open call */
    U_ch           is_nb_vfld     : 8;      /* BP_M 13-07-95 */
    U_ch           is_def_max     : 1;  /* BP_M 02-10-97 */
    U_ch           is_buf_search  : 1;  /* BP_M 08-10-97 */
    U_ch           is_query       : 1;  /* JMP 28-10-99 */
//    U_ch           is_odbc        : 1;   // Source ODBC ou non /* JMP 07-02-11 */
    short          __b1           : 5;   /* JMP 07-02-11 */
    U_sh           is_read_act    /*: MAXBF14*/;  /* IS_fn  executed before reading a record */
    U_sh           is_write_act   /*: MAXBF14*/;  /* IS_fn  executed before writing a record */
    U_sh           is_rewrite_act /*: MAXBF14*/;  /* IS_fn  executed before rewriting a record */
    U_sh           is_delete_act  /*: MAXBF14*/;  /* IS_fn  executed before deleting a record */
    U_sh           is_search_act  /*: MAXBF14*/;  /* IS_fn  executed before search */ /* JMP 16-02-96 */
    short          is_modify_catg;       /* records may be modified if user_catg < */
    short          is_create_catg;       /* records may be created if user_catg < */
    short          is_delete_catg;       /* records may be deleted if user_catg < */
    short          is_search_catg;       /* records may be searched if user_catg < */
    short          __b11;
    ISFINFO        *is_info;
    long           is_recno;             /* current record "number" */
    VFIELD         *is_vfld;             /* BP_M 12-09-1997 */
    U_ch           is_isimem;            /* is_info en mémoire */ /* BP_M 15-01-97 */
    short          __b2[3];
    long           is_ext_size;          /* pour INFORMIX : taille minimum de réallocation sur disque */   /* BP_M 08-12-1998 */

    short          is_cmp_len;           /* Max COMPOSITE Length */
    short          is_nb_isbs;           /* Number of ISAM Buffers */
    ISAMBUF        **is_isbs;            /* Table of ISAMBUF */
    short          is_cur_search;        /* Nb of Current search in ISAMBUFs or -1 */
    short          is_isb_search;        /* Current buffer search in ISAMBUFs or -1 */
    U_sh           is_isb_sync;          /* Max seconds while ISAMBUF remain correct */
    short          __b4;
    char           *is_db_string;        /* String de connection à la base de données */ /* BP_M 14-06-1999 */
    U_sh           is_objnb;             /* Numéro de l'objet dans la FILE_TABLE */ /* JMP 02-12-99 */
    U_sh           is_nb_nexts;          /* Nbre de records à lire en mode client */
    ISNEXTS        *is_nexts;            /* Records lus à l'avance en mode client */
    U_sh           is_limit;             /* BP_M 26-9-02 */
    short          __b6;
    char           reserved[2];          /* JMP 02-12-99 */
    short          __b7;
    char           *is_lim_sql;          /* BP_M 16-11-2004 14:33 */
    char           *is_s4srv;            /* BP_M 14-01-2008 */
    long           is_s4srv_port;        /* BP_M 14-01-2008 */
    long           is_socket;            /* BP_M 14-01-2008 */
    long           is_nb_join;           /* BP_M 29-02-2008 */
    ISJOIN         *is_join;             /* BP_M 29-02-2008 */
} ISAM;

typedef struct draw {
    U_ch   *drw_text;
    U_sh   drw_type;
    short  drw_column;
    short  drw_nb_column;
    short  drw_line;
    short  drw_nb_line;
    U_ch   drw_size;
    U_ch   drw_attr;
    short  drw_fld;     /* BP_M 08-04-1997 */ /* numéro du champs de référence */
    U_ch   drw_grp;     /* BP_M 10-04-1997 */ /* numéro du groupe auquel il appartient */
} DRAW;

typedef struct _draws {
    short   dw_nb_drws;
    DRAW    *dw_drws;
} DRAWS;

typedef struct fnkey {
    short   fnk_key;
    U_sh    fnk_act;
    char    *fnk_cmt;
    unsigned char   *fnk_prot;  /* BP_M 05-03-2010 12:06 */
} FNKEY;

typedef struct _fnkeys {
    short   fk_nb_fnks;
    FNKEY   *fk_fnks;
} FNKEYS;

typedef struct _isfld {
    U_sh            if_isam     /*: MAXBF14*/;  /* pointer to the isam def */
    U_sh            if_isam_nb  /*: MAXBF12*/;  /* nb of the isam field */
    U_ch            if_cmp_nb   /*: MAXBF6*/;  /* ISAM composite nb */
    short           if_cmp_cond ;      /* search cond for codes */
} ISFLD;

typedef struct field {
    U_ch            fld_line      ;         /* first line in the page */
    U_ch            fld_column    ;         /* first column in the page */
    U_ch            fld_nb_line   ;         /* nbr of lines */
    U_ch            fld_nb_column ;         /* nbr of columns */
    U_ch            fld_nlv       ;         /* nbr of visible lines */
    U_ch            fld_ncv       ;         /* nbr of visible columns */
    U_sh            fld_pv        /*: MAXBF12*/;         /* current first visible pos */
    U_ch            fld_attributes;         /* color or REVERSE, ... */
    U_ch            fld_case      : 2 ;         /* case */
    U_ch            fld_modified  : 1 ;         /* modified or not during last access*/
    U_ch            fld_wrap      : 1 ;         /* wrap */
    U_ch            fld_allowed   : 1 ;         /* allowed 0:1 */
    U_ch            fld_help      : 1 ;         /* help    0:1 */
    U_ch            fld_name      : 1 ;         /* name    0:1 */
    U_ch            fld_fmt       : 1 ;         /* fmt     0:1 */

    U_ch            fld_date      : 1 ;         /* if type date : TODAY or not TODAY */
    U_ch            fld_pg_cmt    : 1;
    U_ch            fld_box       : 1;          /* Box YES/NO MMT */
    U_ch            fld_sbar      : 1;          /* scroll bar YES/NO MMT */
    U_ch            fld_stbar     : 1;          /* status bar YES/NO MMT */
    U_ch            fld_io        : 3 ;         /* input or output or hidden or access */

    U_ch            fld_center    : 3 ;         /* center */
    U_ch            fld_compulsory: 2 ;         /* field must be completed */
    U_ch            fld_nb_isam   : 3;

    U_ch            fld_nb_dec    : 4;
    U_ch            fld_del_eof   : 1;
    U_ch            fld_fix_dec   : 1;
    U_ch            fld_def_obj   : 1;
    U_ch            fld_check     : 2;       /* JMP 23-05-97 */
    U_ch            fld_radio     ;          /* BP_M 21-01-97 */
					     /* 0 si ne fait pas partir d'un groupe radio
						> 0 < 127 si 1er d'un groupe
						> 100 < 255 si membre d'un groupe
					     */
    U_ch            fld_grp       ;         /* donne le numéro du groupe */ /* BP_M 08-04-1997 */
    U_ch            fld_qkey      ;          /* 4.01 */
    U_sh            fld_begin_act /*: MAXBF14*/;         /* function called when entering */
    U_sh            fld_end_act   /*: MAXBF14*/;         /*       "    "     "   exiting  */
    U_sh            fld_disp_act  /*: MAXBF14*/;         /*       "    "     "   display */
    U_sh            fld_fnks      /*: MAXBF14*/;         /* FNKEY pointer */
    U_sh            fld_menu      /*: MAXBF14*/;
    U_sh            fld_if_alfa   /*: MAXBF14*/;
    U_sh            fld_next      /*: 9*/;          /* 4.01 */
    unsigned char   *fld_dtext;                 /* pointer to the default txt */
    ISFLD           *fld_isam;
    short           *fld_acmp;                  /* numéro des champs où appliquer le CODEISAM */
    short           fld_type;                   /* type (DATE, INTEGER, REAL, NATURAL) */
    short           fld_cmt;                    /* POSITION OF COMMENT */
    short           fld_dir_type;               /* BP_M 23-01-97 */

    U_sh            fld_pg_nu;                  /* JMP : Num de la page */
    U_sh            fld_nb;                     /* JMP : Num du champ */
    U_sh            fld_wx;                     /* JMP : Pos en x */
    U_sh            fld_wy;                     /* JMP : Pos en y */
    U_sh            fld_ww;                     /* JMP : Largeur  */
    U_sh            fld_wh;                     /* JMP : Hauteur  */
    char            *fld_wdg;                   /* JMP : widget du champ */
    char            reserved[10];
} FIELD;

typedef struct _dft {
    FIELD   dft_field;
    char    *dft_fmt;
    char    *dft_allowed;
    char    *dft_text;
    short   dft_act_line;
    short   dft_act_col;
} DFT;

typedef struct _namefld {
    char    name[20];
    int     line;
    int     col;
    int     type;
    int     size;
    int     use;
    int     lag;
} NFLD;                 /* JMP 06-01-98 */

typedef struct _ract {
    int     nb_str;
    int     nb_db;
    int     nb_lg;
    int     fn;
    long    lg[4];
    double  db[4];
    char    *str[4];
} RACT;                 /* JMP 06-01-98 */


typedef struct _scani {
    short   cmp_nb;
    char    *from;
    char    *to;
    char    *from_def;
    char    *to_def;
} SCANI;

typedef struct _grpbox_ {      /* BP_M 16-04-1997 */
    U_ch        *gr_name;
    U_ch        *gr_title;
    U_ch        gr_box : 2;
    short       gr_back;
    U_ch        gr_f_line : 1;
    U_ch        gr_f_col  : 1;
    U_ch        gr_f_nbl  : 1;
    U_ch        gr_f_nbc  : 1;
    U_ch        gr_line;
    U_ch        gr_col;
    U_ch        gr_nbl;
    U_ch        gr_nbc;
    short       gr_qkey;
} GRPBOX;

typedef struct _grinfo_ {            /* BP_M 09-04-1997 */
    U_ch    *gri_name;
    U_ch    gri_line;
    U_ch    gri_col;
    U_ch    gri_nl;
    U_ch    gri_nc;
} GRINFO;

typedef struct _wbutton_ {      /* BP_M 24-01-97 */
    short   wb_iconnb;      /* Icon nb       */
    short   wb_attr;        /* Attr          */
    U_sh    wb_act;         /* Action        */
    U_ch    wb_toggle;
    char    *wb_help;       /* Tooltip text    */
    U_ch    wb_enable;      /* Tooltip enabled */
} WBUTTON;

typedef struct _wtoolbar_ {     /* BP_M 24-01-97 */
    U_ch    *wt_name;       /* nom du groupe        */
    short   wt_nb;          /* Nb de boutons        */
    WBUTTON *wt_buttons;    /* Attr                 */
    U_sh    wt_butsize[2];  /* buttons size         */
    U_sh    wt_iconsize[2]; /* icons size           */
    short   wt_bitmaprc;    /* Bitmap resource Nb   */
    short   wt_iconsnb;     /* Nb icons in bitmap   */
    U_ch    *wt_tbb;        /* Espace pour Windows  */
    U_ch    *wt_bitmapfile; /* fichier des icones */
    short   wt_visible;     /* le toolbar est-il visible O/N */
    char    *wt_gmenu;      /* JMP : structure GNOMEUIInfo en Gnome */
} WTOOLBAR;


typedef struct page {

    U_ch            pg_column          ; /* first column of the page */
    U_ch            pg_line            ; /* first line of the page */
    U_ch            pg_nb_line         ; /* nbr of lines */
    U_ch            pg_nb_column       ; /* nbr of columns */
    short           pg_csr_fld         ; /* BP_M 23-01-97 */
    U_ch            pg_csr_line        ; /* line of the cursor in that field */
    U_ch            pg_csr_column      ; /* col of the cursor in that field */
    U_sh            pg_nb_flds  /*: 9*/; /* nbr of fields */ /* 4.01*/

    U_ch            pg_modify       : 1; /* records may be modified */
    U_ch            pg_create       : 1; /* records may be created */
    U_ch            pg_delete       : 1; /* records may be deleted */
    U_ch            pg_search       : 1; /* records may be searched */
    U_ch            pg_auto         : 1; /* Page auto position */
    U_ch            pg_def_obj      : 1;
    U_ch            pg_cmt          : 1; /* comment YES or NO */
    U_ch            pg_drw_mem      : 1;

    U_ch            pg_order        : 2; /* edit order by definition or not */ /* 4.01*/
    U_ch            pg_fld_mem      : 1;
    U_ch            pg_fld_in_mem   : 1;
    U_ch            pg_conf_create  : 1;
    U_ch            pg_conf_delete  : 1;
    U_ch            pg_box          : 2;   /* type of box (0, 1 or 2) */

    U_ch            pg_operation    : 4;   /* opération en cours sur cette page */
    U_ch            pg_shadow       : 3;   /* ombre à la page 1 2 3 ou 4 */
    U_ch            pg_closebox     : 1;   /* Close Box */ /* JMP41 18-01-93 */

    U_ch            pg_abort        : 1;   /* Page quittée par abort */ /* JMP_M 4.19 11-05-95 */
    U_ch            pg_impreq       : 1;   /* Importation de required de l'ISAM */ /* JMP_M 4.19 11-05-95 */
    U_ch            pg_imptoday     : 1;   /* Importation de today de l'ISAM */ /* JMP_M 4.19 11-05-95 */
    U_ch            pg_name_mem     : 1;   /* garde les noms en mémoire */
    U_ch            pg_prof         : 1;   /* page à sauver dans le profile *//* JMP 21-02-92 */
    U_ch            pg_ncp          : 4;   /* WIN_NCP */
    U_sh            pg_begin_act /*: MAXBF14*/;  /* function called when entering */
    U_sh            pg_end_act   /*: MAXBF14*/;  /*       "    "     "   exiting  */
    U_sh            pg_disp_act  /*: MAXBF14*/;  /*                      display */
    U_sh            pg_reset_act /*: MAXBF14*/;
    U_sh            pg_fnks      /*: MAXBF14*/;  /* pointer to the first fnkey */
    U_sh            pg_menu      /*: MAXBF14*/;
    U_sh            pg_isam      /*: MAXBF14*/;
    U_sh            pg_fld_idx   /*: MAXBF12*/;  /* numéro du champ contenant le numéro de l'index de recherche */
    U_sh            pg_fld_cond  /*: MAXBF12*/;  /* numéro du champ contenant la cond de recherche (SCR_GE, SCR_EQ, SCR_LE) */ /* JMP_M 4.19 31-05-95 */
    U_sh            pg_fld_count /*: MAXBF12*/;  /* numéro du champ contenant un compteur de lecture */ /* JMP_M 4.19 31-05-95 */
    short           pg_last_key;        /* last key used */
    short           pg_back;            /* background of the page (0=no) */
    short           pg_fill;            /* must the page be filled (char) */
    unsigned char   *pg_title;          /* title of the page */
    unsigned char   *pg_footnote;       /* footnote of the page */
    short           pg_wrap;            /* page wrap */
    FIELD           *pg_flds;           /* pointer to the first field */
    DRAWS           pg_drws;            /* draws */
    DFT             *pg_dft;            /* DEFAULT value */
    short           pg_next_fld;        /* Next field to be edited (or -1) */
    unsigned char   *pg_help;
    U_sh            pg_rec_len; /* JMP 11-04-00 */
    char            *pg_rec;
    U_sh            *pg_fld_pos;   /* JMP 12-04-00 */
    unsigned char   *pg_fld_name;
    unsigned char   *pg_fld_help;
    unsigned char   *pg_fld_allowed;
    unsigned char   *pg_fld_fmt;
    unsigned char   *pg_fld_cmt;
    long            pg_cmt_pos;
    long            pg_scr_pos;
    long            pg_name_pos;
    IMAGE           *pg_im;
    long            pg_drw_pos;
    long            pg_fld_posit;
    SCANI           *pg_scan;
    short           pg_nb_grpbox;       /* BP_M 22-01-97 */
    GRPBOX          *pg_grpbox;         /* BP_M 22-01-97 */
    short           pg_index;           /* BP_M 23-01-97 */
    short           pg_nb_tb;           /* BP_M 24-01-97 */
    U_sh            *pg_toolb;          /* BP_M 24-01-97 */
    U_sh            pg_abort_act;       /* function called by SC_CLOSE */
    U_ch            pg_hline : 1;       /* WIN_HLINE */
    U_ch            pg_vline : 1;       /* WIN_VLINE */
    U_ch            pg_tabenter     : 1;   /* gérer le enter comme un tab en mode Windows */ /* BP_M 25-10-1999 */
    U_ch            pg_nlp_f ;          /* WIN_NLPFLD */
    U_ch            pg_nlp_t ;          /* WIN_NLPTXT */
    U_ch            pg_nlp_v ;          /* WIN_NLPVLINE */
    U_ch            pg_nlp_h ;          /* WIN_NLPHLINE */
    U_ch            pg_nlp_n ;          /* WIN_NLPNULL */
    U_ch            pg_w_title;         /* WIN_TITLE */
    short           pg_w_x;             /* Position Windows en X */
    short           pg_w_y;             /* Position Windows en Y */
    short           pg_w_w;
    short           pg_w_h;

    U_sh            pg_nu;              /* JMP : numéro de la page */
    char            *pg_wdg;            /* JMP : widget de la page */
    char            *pg_swdg;           /* JMP : listview pour MPAGE et BC*/
    U_sh            pg_status;          /* JMP : status dans MPAGE et BC */
    char            *pg_gstatusbar;     /* JMP : comment en Gnome */
    char            *pg_guiinfos;       /* JMP : menus de la page en Gnome */
    U_ch            pg_center : 1;      /* JMP : CENTER dans PAGE */

/*
    U_sh            pg_wparent;      // Parent Window for position
    short           pg_wvert;        // Vertical position (Windows)
    short           pg_whoriz;       // Horizontal position (Windows)
*/
/* JMP 14-05-00 */
} PAGE;

typedef struct _view_ {
    U_sh    v_lpg  ;    /* Little Page current View     */  /* JMP_M 4.20 14-06-95 */
    U_sh    v_bpg  ;    /* Big Page or BC current View  */  /* JMP_M 4.20 14-06-95 */
    U_sh    v_isam ;    /* ISAM current View            */  /* JMP_M 4.20 14-06-95 */
    U_ch    v_type : 1; /* Type bpg : 0 = page, 1 = BC  */  /* JMP_M 4.20 14-06-95 */
} VIEW;

typedef struct _mptcol_ {       /* BP_M 22-01-97 */
    U_ch    *tc_title;
    short   tc_idx;
} MPTCOL;

typedef struct mpage {
    U_ch   mp_nb_recs: 8;          /* nb of pages displayed simultan. on the screen */
    U_ch   mp_line   : 7;          /* line of the first page on the screen */
    U_ch   mp_modify : 1;          /* records may be modified */
    U_ch   mp_lpg_nl : 7;          /* number of line of a little page */
    U_ch   mp_create : 1;          /* records may be created */
    U_ch   mp_buf_size :7;         /* number of records simult. in mp_buf */
    U_ch   mp_delete : 1;          /* records may be deleted */
    U_ch   mp_operation:4;         /* opération en cours dans le MPAGE */
    U_ch   mp_search : 1;          /* records may be searched */
    U_ch   mp_cmt    : 1;          /* comment YES or NO */ /* JMP 29-09-91 */
    U_ch   mp_in_search : 1;       /* search operation ? */ /* JMP_M 4.19 13-06-95 */
    U_ch   mp_lpg_mem   : 1;       /* little page in mem ? */ /* JMP_M 4.20 15-06-95 */
    U_ch   mp_use_ref    : 1;      /* initialise with ref_rec */ /* JMP_M 4.20 15-06-95 */
    U_ch   mp_auto_create: 1;      /* create if no record ? */ /* JMP_M 4.20 15-06-95 */
    U_ch   mp_type_win : 1;        /* Type d'édition en mode Windows */ /* BP_M 05-10-1999 */
    U_sh   mp_apg       /*: MAXBF14*/;    /* pointer to the "around  page" */
    U_sh   mp_lpg       /*: MAXBF14*/;    /* pointer to the "little page" */
    U_sh   mp_bpg       /*: MAXBF14*/;    /* page to modify or view one rec "big page" */
    U_sh   mp_spg       /*: MAXBF14*/;    /* scan page */ /* JMP_M 4.19 31-05-95 */
    U_sh   mp_isam      /*: MAXBF14*/;    /* pointer to the edited isam */
    U_sh   mp_fnks      /*: MAXBF14*/;    /* allowed fn keys */
    U_sh   mp_crit      /*: MAXBF14*/;    /* pointer to the selection criterion */
    U_sh   mp_view_fn   /*: MAXBF14*/;   /* JMP_M 4.20 14-06-95 */
    U_sh   mp_view_crfn /*: MAXBF14*/; /* JMP_M 4.20 14-06-95 */
    U_sh   mp_lg_lpg;              /* total length of the fields of lpg */ /* JMP 12-04-00 */
    char    *mp_lpgs;               /* buffer for little page content */
    short   mp_index;               /* nb of the index of research */
    short   mp_cond;                /* search condition */
    char    *mp_ref_rec;            /* reference record for search */
    char    *mp_buf_def;            /* vector with 'Y' or 'N' : record exist or no */
    char    *mp_buf;                /* buffer of recs in memory */
    short   mp_c_line;              /* current page on the screen */
    short   mp_line0;               /* pos in buffer of the rec 0 of the screen */
    short   mp_c_rec;               /* current rec in the ISAM file (pos in buf) */
    long    mp_cmt_pos;             /* comment position in .cmt file */ /* JMP 29-09-91 */
    long    mp_count;               /* nb of read records */           /* JMP_M 4.19 06-06-95 */
    U_ch    *mp_help;               /* JMP_M 4.19 11-06-95 */
    short   mp_nb_views;            /* JMP_M 4.20 14-06-95 */
    VIEW    *mp_views;              /* JMP_M 4.20 14-06-95 */
    U_sh    mp_nb_tcol;             /* Number of title column *//* BP_M 22-01-97 */
    MPTCOL  *mp_tcol;               /* Title of column */       /* BP_M 22-01-97 */
    U_sh    mp_nu;                  /* JMP : numéro de l'objet */
    char    *mp_wdg;                /* JMP : widget de l'objet */
    int     mp_tot_len_rec;         /* BP_M 29-02-2008 : Longueur total du record ISAM y compris les tables join */
    U_sh    mp_bpg_view  /*: MAXBF14*/;    /* page to view one rec "big page" */
    U_sh    mp_bpg_mod   /*: MAXBF14*/;    /* page to modify one rec "big page" */
    short   mp_save_rec;
    U_sh    mp_link_fn;             /* Action exécutée après chaque création/modification/Destruction d'une ligne */
} MPAGE;

typedef struct _sfld_ {
    char    *sfld_text;
    int     sfld_fld;
    int     sfld_lg;
    char    *sfld_exp;
    ISAM    *sfld_isam;
} SFLD;

typedef struct option {
    U_ch           opt_letter    : 8;
    U_ch           opt_enable    : 1;
    U_ch           opt_undisplay : 2;
    U_ch           opt_sel       : 2;
    U_sh           opt_act       /*: MAXBF14*/;
    short           opt_key;
    unsigned char   *opt_help;
    unsigned char   *opt_text;
    unsigned char   *opt_pg_cmt;
    unsigned char   *opt_prot; /* JMP 04-06-01 */
    short           opt_cmt;
} OPTION;

typedef struct menu {

    U_ch                mn_line      : 8 ;
    U_ch                mn_column    : 8 ;
    U_ch                mn_box       : 8 ;
    U_ch                mn_nb_line   : 8 ;
    U_ch                mn_nb_column : 8 ;
    U_sh                mn_begin_act /*: MAXBF14*/;
    U_sh                mn_end_act   /*: MAXBF14*/;
    U_sh                mn_fnks      /*: MAXBF14*/;
    U_ch                mn_auto      : 1;
    U_ch                mn_cmt       : 1;
    U_ch                mn_opt_mem   : 1;
    U_ch                mn_in_mem    : 1;   /* BP 16-12-1991 12:35 */
    U_ch                mn_prof      : 1;   /* JMP 23-02-92 */
    U_ch                mn_sbar      : 1;   /* JMP_M 4.19 12-05-95 */
    U_ch                mn_shadow    : 1;   /* BP_M 25-10-95 */
    U_ch                mn_open      : 1;   /* BP_M 24-01-97 */
    short               mn_first_opt;
    short               mn_nb_opts;
    short               mn_choice;
    short               mn_back;
    unsigned  char      *mn_title;
    unsigned  char      *mn_foot;
    OPTION              *mn_opts;
    IMAGE               *mn_im;
    unsigned  char      *mn_help;
    long                mn_cmt_pos;
    long                mn_opt_pos;
    short               mn_w_x;
    short               mn_w_y;
    short               mn_w_w;
    short               mn_w_h;
    char                mn_reserved[10];
} MENU;

typedef struct menui {
    unsigned char *mi_title;
    U_ch      mi_line      : 8 ;
    U_ch      mi_column    : 8 ;
    U_ch      mi_box       : 8 ;
    U_ch      mi_nc        : 8 ;
    U_ch      mi_nl        : 8 ;
    U_sh      mi_begin_act /*: MAXBF14*/;
    U_sh      mi_end_act   /*: MAXBF14*/;
    U_sh      mi_isam      /*: MAXBF14*/;
    short     mi_first_opt;
    short     mi_nb_opts;
    unsigned char **mi_opts;
    short     mi_ropt;
    short     mi_choice;
    short     mi_back;
    IMAGE     *mi_im;
    short     mi_auto;
    unsigned char      *mi_help;
    short     mi_cmp;
    short     mi_nb_flds;
    short     *mi_flds;      /*!!!!!!*/
    unsigned char **mi_fmt;
} MENUI;

typedef struct bclist {
    struct bclist   *bcl_next;
    struct bclist   *bcl_prev;
    char            *bcl_rec;
    char            *bcl_pgrec; /* JMP_M 4.19 07-06-95 */
    } BCLIST;

typedef struct bcom {
    U_sh    bc_apg     /*: MAXBF14*/;   /* pointer to the BC page */
    U_sh    bc_bpg     /*: MAXBF14*/;   /* page to modify or view one rec "big page" */
    U_sh    bc_lpg     /*: MAXBF14*/;   /* pointer to the "little page" of lbc */
    U_sh    bc_is_bc   /*: MAXBF14*/;   /* pointer to the edited isam of BCom */
    U_sh    bc_is_lbc  /*: MAXBF14*/;   /* pointer to the edited isam of LBCom */
    U_sh    bc_fnks    /*: MAXBF14*/;   /* allowed fn keys */
    U_sh    bc_link_act/*: MAXBF14*/;   /* link via BC_functions */
    U_sh    bc_beg_act /*: MAXBF14*/;   /* begin fn */      /* BP 23-MAXBF14-1991 07:45 */
    U_sh    bc_end_act /*: MAXBF14*/;   /* End fn */        /* BP 23-12-1991 07:45 */
    U_sh    bc_write_lbc_act /*: MAXBF14*/;   /* End fn */        /* BP 23-12-1991 07:45 */
    U_sh    bc_act_anyk      /*: MAXBF14*/;  /* Action when any key pressed in LBC */
    U_sh    bc_view_fn       /*: MAXBF14*/;   /* Action to determine view on current Line */ /*JMP_M 4.20 14-06-95 */
    U_sh    bc_view_crfn     /*: MAXBF14*/; /* Action to determine view on new Line */     /* JMP_M 4.20 14-06-95 */
    U_ch    bc_modify   : 1;    /* records may be modified */
    U_ch    bc_create   : 1;    /* records may be created */
    U_ch    bc_delete   : 1;    /* records may be deleted */
    U_ch    bc_search   : 1;    /* records may be searched */
    U_ch    bc_operation: 2;    /* operation en  cours sur ce BC */
    U_ch    bc_lpg_op   : 2;    /* operation en  cours sur les lignes de ce BC */ /* BP 17-12-1991 19:17 */
    U_ch    bc_cmt      : 1;    /* comment YES or NO */ /* JMP 29-09-91 */
    U_ch    bc_insert_lbc : 1;  /* possibilité de inserer des lignes de bon */    /* BP 23-12-1991 09:34 */
    U_ch    bc_lpg_mem : 1;     /* lpg en mémoire */   /* JMP_M 4.19 10-06-95 */
    U_ch    bc_is_sbar : 1;     /* scrollbar */   /* JMP_M 4.19 10-06-95 */
    U_ch    bc_auto_create : 1;  /* scrollbar */   /* JMP_M 4.19 10-06-95 */
    U_ch    bc_no_mark : 1;     /* cannot mark lbcs, copy, ... */  /* JMP_M 4.21 01-09-95 */
    short   bc_is_lbc_cmp;      /* nb of the index of research in LBC */
    short   bc_is_bc_fld[SCR_MAX_CMP_FIELDS - 1];    /* field of bc corresponding to index of lbc */
    BCLIST  *bc_list;           /* buffer for little pages content */
    short   bc_nb_lbc;          /* number of lbc currently existing */
    short   bc_nb_recs;         /* nb of pages displayed simultan. on the screen */
    short   bc_line;            /* line of the first page on the screen */
    short   bc_lpg_nl;          /* number of line of a little page */
    short   bc_c_line;          /* current page on the screen */
    short   bc_line0;           /* pos in buffer of the rec 0 of the screen */
    short   bc_last_key;        /* last key pressed */
    long    bc_cmt_pos;         /* comment position in .cmt file */ /* JMP 29-09-91 */
    S_BAR   *bc_sbar;           /* Scroll bar for LBC */ /* JMP_M 4.19 10-06-95 */
    U_ch    *bc_help;           /* Help topic */ /* JMP_M 4.19 10-06-95 */
    short   bc_mark_1;          /* Début des marques */ /* JMP_M 4.19 10-06-95 */
    short   bc_mark_2;          /* Fin des marques   */ /* JMP_M 4.19 10-06-95 */
    short   bc_nb_views;          /* JMP_M 4.20 14-06-95 */
    VIEW    *bc_views;            /* JMP_M 4.20 14-06-95 */
    U_sh    bc_nb_tcol;             /* BP_M 23-01-97 */
    MPTCOL  *bc_tcol;               /* BP_M 22-01-97 */
    short   bc_index;               /* BP_M 23-01-97 */
    U_sh    bc_nu;                  /* JMP : numéro de l'objet */
    char    *bc_wdg;                /* JMP : widget de l'objet */
} BC;

/* ==== PRINTER STRUCT ===== */

#define PR_MAX_TABS     20      /* JMP 4.23 03-01-96 */
#define PR_MAX_PG_TABS  15      /* JMP 4.23 03-01-96 */

typedef struct _pr_def {
    unsigned char    *pr_name;
    unsigned char    *pr_normal;
    unsigned char    *pr_bold;
    unsigned char    *pr_e_bold;
    unsigned char    *pr_underl;
    unsigned char    *pr_e_underl;
    unsigned char    *pr_reverse;
    unsigned char    *pr_e_reverse;
    unsigned char    *pr_condensed;
    unsigned char    *pr_e_condensed;
    unsigned char    *pr_enlarged;
    unsigned char    *pr_e_enlarged;
    unsigned char    *pr_begin;
    unsigned char    *pr_end;
    FILE    *pr_fd;
    short   pr_nl;
    short   pr_nc;
    short   pr_nb_tabs;
    short   pr_tabs[PR_MAX_TABS];
    unsigned char    *pr_maps[256];
    IMAGE   *pr_im;
    char    pr_type;                    /* 'F' for file, 'P' for printer */
    short   pr_pgnb;                    /* Page nbr */
    short   pr_cline;
    short   pr_ccol;
    short   pr_cpg_tab;
    short   pr_attr;
    short   pr_nb_vtabs;
    short   pr_vtabs[PR_MAX_PG_TABS];
    unsigned char    *pr_formfeed;
    unsigned char    *pr_linefeed;
    } PR_DEF;

typedef struct _PRTDEF_             /* JMP 30-08-98 */
{
    U_ch    *prd_name;     /* Nom système de l'imprimante */
    U_ch    *prd_descr;    /* Description */
    U_ch    *prd_command;  /* Commande pour lancer le Job */
    U_ch    *prd_eol;      /* Valeur du caractère End Of Line */
    U_ch    *prd_eop;      /* Valeur du caractère End Of Page */
    U_ch    *prd_eof;      /* Valeur du caractère End Of File */
    char    prd_graph;     /* Caractères semi-graphiques */
    char    prd_charset;   /* Caractères (ANSI ou ASCII) supportés */
    char    prd_mode;      /* Mode (RAW, GDI ou FILE) */
} PRTDEV;

typedef struct _obj {
    char    *obj_ptr;       /* Pointer to objet (ISAM or PAGE) */
    short   obj_fld;        /* Field or Index number */
    short   obj_type;       /* Objet type (ISAM, PAGE, INDEX) */
} OBJ;

typedef struct _a_opt {
    unsigned char    *ao_text;
    short            ao_key;
    short            ao_type;
    U_sh             ao_ptr;
    unsigned char    *ao_help;
    U_sh             ao_act /*: MAXBF14*/;
    short            ao_cmt;
    U_ch             ao_enable;     /* BP_M 18-04-1997 */
    unsigned char    *ao_prot;      /* JMP 04-06-01 */
} A_OPT;

typedef struct _a_bar {
    short   ab_line;
    short   ab_col;
    short   ab_nb_col;
    IMAGE   *ab_im;
    short   ab_nb_opts;
    A_OPT   *ab_opts;
    U_sh    ab_begin_act /*: MAXBF14*/,
	    ab_end_act   /*: MAXBF14*/;
    U_ch    ab_choice    : 8;
    U_ch    ab_cmt       : 1;
    U_ch    ab_prof      : 1; /* JMP 23-02-92 */
    long    ab_cmt_pos;

    unsigned char    *ab_help;
} A_BAR;

typedef struct _appl {
    short   ap_line,
	    ap_col,
	    ap_nl,
	    ap_nc,
	    ap_box,
	    ap_back;
    char    ap_fill;
    unsigned char    *ap_title,
		     *ap_foot;
    DRAWS   ap_drws;
    U_ch    ap_cmt        : 1;    /* comment YES or NO */ /* JMP 29-09-91 */
    U_sh    ap_ab        /* : MAXBF14 */;
    U_sh    ap_fnks      /* : MAXBF14 */;
    U_sh    ap_begin_act /* : MAXBF14 */;
    U_sh    ap_end_act   /* : MAXBF14 */;
    U_sh    ap_act       /* : MAXBF14 */;
    U_sh    ap_ptr       /* : MAXBF14 */;
    short   ap_type;
    short   ap_status;
    short   ap_lkey;
    IMAGE   *ap_im;
    U_sh    ap_prev             /*: MAXBF14*/;
    long    ap_cmt_pos;         /* comment position in .cmt file */ /* JMP 29-09-91 */
    short   ap_nb_tb;           /* BP_M 18-04-1997 */
    U_sh    *ap_toolb;          /* BP_M 18-04-1997 */
    U_ch    ap_enable;          /* JMP 03-05-97 */
    char    *ap_wdg;            /* JMP window de l'APPL 14-03-03 */
    char    *ap_gstatusbar;     /* JMP : comment en Gnome */
    char    *ap_guiinfos;       /* JMP : menus de la page en Gnome */

/*
    U_ch    ap_wshrink;      // Shrink appl
    U_sh    ap_wparent;      // Parent window for position
    short   ap_wvert;        // Vertical position (Windows)
    short   ap_whoriz;       // Horizontal position (Windows)
    char    reserved[6];
*/
    /* JMP 14-05-00 */

} APPL;

/* =+=================================================
			 REPORT
   =================================================== */

typedef struct _rifld {
    short   rf_type;                /* type SCR_TYPE_PAGE | SCR_TYPE_ISAM */
    U_sh    rf_ptr     /* : MAXBF14*/;       /* pointeur sur l'élément */
    U_sh    rf_num     /* : MAXBF12*/;       /* numero du field */
    U_sh    rf_nbapp   /* : MAXBF14*/;       /* nombre d'application */
    U_sh    rf_cond         ;       /* condition de recherche */
    short   *rf_apply;     /* appliquer pour les champs numéro */
} RIFLD;

typedef struct _rfld {
    unsigned char       *rf_name;           /* nom du field */
    U_sh                rf_nbval     :MAXBF14;   /* nombre de valeur */
    unsigned char       *rf_val1;           /* pointeur sur les valeurs */
    unsigned char       *rf_val2;           /* pointeur sur les valeurs */
    short               rf_nbi;             /* nombre d'isam ou page */
    RIFLD               *rf_fld;            /* pointeur sur les éléments */
    U_sh                rf_disp_act  : MAXBF14;  /* action executée avant le display */
    U_sh                rf_nl        : 8 ;  /* nbr of lines */
    U_sh                rf_nc        : 8 ;  /* nbr of columns */
    U_sh                rf_attributes: 8 ;  /* color or REVERSE, ... */
    U_sh                rf_case      : 2 ;  /* case */
    U_sh                rf_center    : 3 ;  /* center */
    U_sh                rf_nb_dec    : 4;   /* nombre de décimal */
    unsigned char       *rf_txt;            /* text du champ */
    unsigned char       *rf_fmt;            /* format  */
} RFLD;

typedef struct _report {
    short   rp_tm;              /* top margin */
    short   rp_bm;              /* bottom margin */
    short   rp_lm;              /* left margin */
    short   rp_rm;              /* rigth margin */
    short   rp_nbfld;           /* nombre de champs */
    RFLD    *rp_rfld;           /* pointeur sur les champs */
    unsigned char    *rp_filetxt;
    struct  {
	short   rd_type;
	U_sh    rd_ptr;
    } rp_def;                   /* valeurs par defaut */
} REPORT;


/* ============== HELPS ===================== */

#define     H_MAX_KEYW      255      /* JMP 15-09-93 */
//#define     H_MAX_LINE      255  /* JMP 08-09-99 */
#define     H_MAX_STRING    255
#define     H_OPENB         254
#define     H_CLOSEB        255
#define     H_TITLE_LG      67
#define     H_LABEL_LG      67

typedef struct _hkeyw {
    short   hk_line,            /*4.04*/
	    hk_col,
	    hk_lg,
	    hk_num,
	    hk_type;            /* JMP 15-05-99 */
} HKEYW;

typedef struct _help  {
    char    **h_text;
    HKEYW   *h_keyws;
    char    *h_ptr;
    long    h_pos;
    short   h_nl;                       /*4.04*/
    short   h_nb_keyws;                 /*4.04*/
    long    h_lg;                       /* 4.56 JMP 13-01-02 */
    char    h_keyw[H_MAX_KEYW + 1];
    char    h_alias[H_MAX_KEYW + 1];    /* JMP 08-09-99 */
    char    h_cpath[SCR_MAX_FILE_LG_MAX + 1];  /* JMP 24-09-10 images dans les sous-directories */
} HELP;

typedef struct _helps {
    HELP    **hs_helps;
    IMAGE   *hs_im;
    short   hs_nb_helps,                /*4.04*/
	    hs_line,                    /*4.04*/
	    hs_col,
	    hs_nl,
	    hs_nc,
	    hs_box,
	    hs_attr,
	    hs_c_line,
	    hs_c_keyw;
    char    hs_title[H_TITLE_LG + 1];
    char    hs_footn[H_TITLE_LG + 1];
    char    hs_reserve[2];            /* JMP 15-09-93 : alignement */
} HELPS;

typedef struct _cmt {
    short     c_line;
    short     c_col;
    short     c_nl;
    short     c_nc;
    short     c_attr;
    short     c_keyattr; /* JMP41 18-01-93 */
    short     c_center;
} _SCR_CMT;

typedef struct _ecmt {
    char      name[21];
    short     box;
    long      pos;
    short     len;
    short     line;
    short     col;
    short     nb_line;
    short     nb_col;
    short     back;
} ECMT;

#include <s_lst.h>
#define SWAP

#define MMT_TITLEN      40

typedef struct _mmtfn_ {
    short   key;        /* touche fonction */
#ifdef SCRPROTO
    int     (*fn)(void); /* fonction à exécuter ou pointeur nul */
#else
    int     (*fn)();    /* fonction à exécuter ou pointeur nul */
#endif
    char    *txt;       /* libelle de la touche */
} MMTFN;

typedef struct _mmt_ {
    short   csr[2], mark[2][2], markt, csg[2],
	    size[2], osize[2], pos[2], opos[2], lim[2],
	    attr, cp[2], rattr, statbar, box, mod, issbar, output, zoom,
	    chng, focus;
    LST     *lst;
    IMAGE   *im, *tmpim;
    S_BAR   sbar;
    unsigned char title[MMT_TITLEN + 1];
    MMTFN   *mmtfns;
    char    *client;
#ifdef SCRPROTO
    int     (*fnkeys)(int);
#else
    int     (*fnkeys)();
#endif
} MMT;

typedef struct _tpgs_ {
    short   tp_type;           /* 0 = Properties, 1 = Wizard */
    short   tp_nb;             /* NOMBRE DE PAGES */
    short   *tp_pgsnbs;        /* PAGES */
    U_ch    *tp_title;         /* TITRE GENERAL */
    U_ch    **tp_pgstitles;    /* TITRE DES PAGES */
    U_sh    tp_begin_act;      /* AVANT AFFICHAGE */
    U_sh    tp_end_act;        /* OK ou FINISH  */
    U_sh    tp_back;           /* Background */
    U_sh    tp_rattr;          /* RATTR */
    U_sh    tp_box;            /* BOX */

    short   tp_applied;        /* APPLY Action done */
    short   tp_last_key;       /* last key used */ /* JMP 21-08-97 */
    IMAGE   *tp_im;            /* background image */ /* JMP 21-08-97 */
    U_sh    tp_nu;             /* JMP : numéro de l'objet */
    char    *tp_wdg;           /* JMP : widget de l'objet */
    char    reserved[4];       /* JMP 21-08-97 */
} TPAGES;


typedef struct _smpage_ {
    char    smp_name[65];
    SFLD    *smp_aflds;     /* pointer to the "around  page" fields */
    SFLD    *smp_lflds;     /* pointer to the "little  page" fields */
    int     smp_nb_aflds;
    int     smp_nb_lflds;
    ISAM    *smp_isam;      /* pointer to the edited isam */
    PAGE    *smp_bpg;       /* pointer to the Big Page */
    BC      *smp_bc;        /* pointer to the BC */         /* BP_M 01-11-2001 12:05 */
    TPAGES  *smp_tpg;       /* pointer to the TPage */
    int     smp_index;      /* index nb */
    int     smp_cond;       /* search condition */ /* JMP_M 4.19 25-05-95 */
    int     smp_nb_recs;    /* nb of pages displayed simultan. on the screen */
    int     smp_line;
    int     smp_col;
    int     smp_nc;
    int     smp_nl;
    U_ch    *smp_footn;
    U_ch    *smp_title;
    U_ch    *smp_exp;
    U_int   smp_menu;
    U_sh    smp_nb_tcol;     /* Number of title column *//* JMP 25-01-97 */
    MPTCOL  *smp_tcol;       /* Title of column */       /* JMP 25-01-97 */
    U_ch    *smp_apg_exp;    /* Free infos for APG */ /* JMP 25-10-99 */
    U_ch    *smp_lpg_exp;    /* Free infos for LPG */ /* JMP 25-10-99 */
} SMPAGE;

typedef struct _tblstr_ {       /* BP_M 23-01-97 */
    short   ts_len;
    char    *ts_ptr;
    long    *ts_pos;
} TBLSTR;


typedef struct _psdescr_ {
    long    pid;
    long    ppid;
    long    prior;
    long    nbthreads;
    char    *exefile;
} PSDESCR;

/* SOCKETS */
typedef struct _wsockclt_ {
    int     cltsock;
    int     srvsock;
    int     (*fn)(int);
} WSOCKCLT;

/****** WSOCK1 pour gérer plusieurs sockets en // dans la même session ****/
typedef struct _wsock1_ {
    int     cltsock;
    int     srvsock;
    int     wsock_debug;
    int     wsock_bufsize;
    long    wsock_totalr;
    long    wsock_totalw;
    char    *wsock_buf;
    int     wsock_buflen;
} WSOCK1;

#define WSOCK_MAX_SOCKS 10
#define WSOCK_BUF       (WSOCKS[WSOCKS_CURRENT].wsock_buf)
#define WSOCK_BUFLEN    (WSOCKS[WSOCKS_CURRENT].wsock_buflen)
#define WSOCK_TOTALR    (WSOCKS[WSOCKS_CURRENT].wsock_totalr)
#define WSOCK_TOTALW    (WSOCKS[WSOCKS_CURRENT].wsock_totalw)
extern WSOCK1  WSOCKS[WSOCK_MAX_SOCKS];
extern int     WSOCKS_CURRENT;

/******************** POP3 *********************************/

typedef struct _pop3msg_ {
    int     size;           /* Taille du message */
    char    *from;          /* Origine du message */
    char    *to;            /* Destinataire du message */
    char    *reply;         /* Adresse de retour du message */
    char    *date;          /* Date d'envoi du message */
    char    *subject;       /* Champ sujet du message */
    char    *type;          /* Content-type */
    char    *encoding;      /* Transfer-Encoding */
    char    *disposition;   /* Disposition (attachment) */
    char    *description;   /* Description (attachment) */
    char    *filename;      /* Nom de l'attachment */
    char    *boundary;      /* Boundary */
    char    **txt;          /* Texte du message */
    char    **body;         /* Corps du message */
    char    **headers;      /* Header du Message */
    int     nbattachs;      /* Nbre d'attach */
    struct _pop3msg_ **attachs;   /* Attachments */
} POP3MSG;

extern char    POP3_ERROR[];
extern POP3MSG **Pop3GetMsgs();
extern POP3MSG **Pop3GetHeaders();
extern POP3MSG **Pop3Session();

typedef struct _replica {
    char    *file;
    char    *section;
    int     socket;
    int     type;
    int     type_to;
    int     mode;
    int     port;
    int     direct;
    int     s4base;
    U_ch    **dir;
    U_ch    *serv;
    U_ch    **tables;
    U_ch    **dsn;
    U_ch    **f_dsn;
    U_ch    *f_serv;
    int     fs_port;
    int     in_beg;
    char    ext_tmp[255];
    void    **info;
} REPLICA;

typedef struct _last_req {
    int     type;
    char    *dsn;
    char    *tblname;
    char    *sql;
} LASTREQ;

typedef struct _s4c {
    int     socket;
    int     init;
    char    *srv;
    char    *dsn;
    int     port;
    char    *outbuf;
    long    outlg;
    long    outmaxlg;
    long    outpos;
    char    *inbuf;
    char    *inbufc;
    long    inlg;
    long    inmaxlg;
    long    incmaxlg;
    long    inpos;
    char    cwd[512];
} S4C;


#endif /* _SCR4_STR_H_ */
