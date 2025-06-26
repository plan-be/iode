#ifdef S5WEB

typedef struct _s5page {
    short           pg_csr_fld;
    U_sh            pg_nb_flds;
    U_ch            pg_modify       : 1; /* records may be modified */
    U_ch            pg_create       : 1; /* records may be created */
    U_ch            pg_delete       : 1; /* records may be deleted */
    U_ch            pg_search       : 1; /* records may be searched */
    U_ch            pg_conf_create  : 1;
    U_ch            pg_conf_delete  : 1;
    U_ch            pg_operation    : 4;   /* opÔÇÜration en cours sur cette page */
    U_ch            pg_abort        : 1;   /* Page quittÔÇÜe par abort */ /* JMP_M 4.19 11-05-95 */
    U_ch            pg_impreq       : 1;   /* Importation de required de l'ISAM */ /* JMP_M 4.19 11-05-95 */
    U_ch            pg_imptoday     : 1;   /* Importation de today de l'ISAM */ /* JMP_M 4.19 11-05-95 */
    U_ch            pg_prof         : 1;   /* page ÔÇª sauver dans le profile *//* JMP 21-02-92 */
    U_sh            pg_begin_act /*: MAXBF14*/;  /* function called when entering */
    U_sh            pg_end_act   /*: MAXBF14*/;  /*       "    "     "   exiting  */
    U_sh            pg_disp_act  /*: MAXBF14*/;  /*                      display */
    U_sh            pg_reset_act /*: MAXBF14*/;
    U_sh            pg_fnks      /*: MAXBF14*/;  /* pointer to the first fnkey */
    U_sh            pg_isam      /*: MAXBF14*/;
    unsigned char   *pg_title;          /* title of the page */
    unsigned char   *pg_footnote;       /* footnote of the page */
    S5FIELD         *pg_flds;           /* pointer to the first field */
    U_sh            pg_rec_len; /* JMP 11-04-00 */
    char            *pg_rec;
    long            pg_fld_posit;
    short           pg_nb_tb;           /* BP_M 24-01-97 */
    U_sh            *pg_toolb;          /* BP_M 24-01-97 */
    U_sh            pg_abort_act;       /* function called by SC_CLOSE */
    U_sh            pg_nu;              /* JMP : numÔÇÜro de la page */
} S5PAGE;

typedef struct _s5isfld {
    U_sh            if_isam     /*: MAXBF14*/;  /* pointer to the isam def */
    U_sh            if_isam_nb  /*: MAXBF12*/;  /* nb of the isam field */
    U_ch            if_cmp_nb   /*: MAXBF6*/;  /* ISAM composite nb */
    short           if_cmp_cond ;      /* search cond for codes */
} S5ISFLD;

typedef struct _s5field {
    U_ch            fld_nb_line   ;         /* nbr of lines */
    U_ch            fld_nb_column ;         /* nbr of columns */
    U_ch            fld_nlv       ;         /* nbr of visible lines */
    U_ch            fld_ncv       ;         /* nbr of visible columns */
    U_ch            fld_attributes;         /* color or REVERSE, ... */
    U_ch            fld_case      : 2 ;         /* case */
    U_ch            fld_modified  : 1 ;         /* modified or not during last access*/
    U_ch            fld_wrap      : 1 ;         /* wrap */
    U_ch            *fld_allowed;
    U_ch            *fld_help;
    U_ch            *fld_name;
    U_ch            *fld_fmt;
    U_ch            *fld_dtext;                 /* pointer to the default txt */

    U_ch            fld_date;         /* if type date : TODAY or not TODAY */
    U_ch            fld_box       : 1;          /* Box YES/NO MMT */
    U_ch            fld_io        : 3 ;         /* input or output or hidden or access */

    U_ch            fld_center    : 3 ;         /* center */
    U_ch            fld_compulsory: 2 ;         /* field must be completed */
    U_ch            fld_nb_isam   : 3;
    U_ch            fld_nb_dec    : 4;
    U_ch            fld_del_eof   : 1;
    U_ch            fld_fix_dec   : 1;
    U_ch            fld_check     : 2;       /* JMP 23-05-97 */
    U_sh            fld_begin_act /*: MAXBF14*/;         /* function called when entering */
    U_sh            fld_end_act   /*: MAXBF14*/;         /*       "    "     "   exiting  */
    U_sh            fld_disp_act  /*: MAXBF14*/;         /*       "    "     "   display */
    U_sh            fld_fnks      /*: MAXBF14*/;         /* FNKEY pointer */
    U_sh            fld_menu      /*: MAXBF14*/;
    U_sh            fld_if_alfa   /*: MAXBF14*/;
    S5ISFLD         *fld_isam;
    short           *fld_acmp;                  /* numÔÇÜro des champs oÔÇö appliquer le CODEISAM */
    short           fld_type;                   /* type (DATE, INTEGER, REAL, NATURAL) */

} S5FIELD;

#endif
