#include "scr4.h"

int     ERR_TYPE = 0;
FILE_TABLE SCR_READ_FILES = {0, 0} ;
int  SCR_PAGE_SIZE[2] = {25, 80};
int  SCR_CURRENT_COLOR = -1;
int  SCR_CURRENT_ATTR = -1;
int  SCR_CURRENT_FONT = -1;
int  SCR_CURSOR_STATUS = -1;
int  SCR_CURRENT_LINE = -1;
int  SCR_CURRENT_COLUMN = -1;
int  SCR_CURRENT_CSR_SIZE = -1;
char SCR_LANGUAGE[5] = "f";
char SCR_TERM[20] = "";
IMAGE *SCR_SCREEN = 0;
int  SCR_INS_TGL = 2; /* JMP 11-10-98 */
int SCR_USER_CATG = -123;
int SCR_CLOCK_POS[2] = {-1, 72};
int SCR_CLOCK_SECS = 1;
int SCR_DATE_POS[2] = {-1, 72};
int SCR_MEM_POS[2] = {-1, 72};
int SCR_KEY_POS[2] = {-1, 72};
int SCR_BREAK_ON = 0;
IMAGE *SCR_TMP_IMAGE = 0;
YYFILE *SCR_YY = 0;
int SCR_PG_ABORT = 0;
int SCR_FLD_SKIP    = 0;
int SCR_FLD_REEDIT  = 0;
int SCR_LAST_ATTR = SCR_DEFAULT;
int SCR_APP_ABORT = 0;
APPL *SCR_APPL;
int SCR_LKEY = 0;
int SCR_nb_cmt;
unsigned char   SCR_FILL_SCREEN = ' ';
int SCR_NB_DEC = -1;
int SCR_RIGHT_MENU = 0;
int SCR_MAP_EXEC = 1;
int SCR_OLD_ATTR = 0;
int SCR_REPLI_FORCE = 0;    /* BP_M 25-03-2008 */
int SCR_fdbsys;     /* BP_M 13-06-2008 */
int SCR_dft_dbsys;  /* BP_M 13-06-2008 */
char SCR_sLIST = ',';
/*
char    *SCR_CURRENT_NMS        = 0;
*/          /* BP_M 23-10-95 */
/* int     SCR_EDIT_KILL = 1; /* JMP 22-10-91 */
int     SCR_CHECK_DELETE = 1;
_SCR_CMT     SCR_CMT  = {-1, 0, 1, 80, SCR_YELLOW, 0};
char    *SCR_CMT_TXT = 0;
int     SCR_CONFIRME_ON = 1;
long    SCR_END_TIME_1, SCR_END_TIME_2;
long    SCR_LOCK_TIME = 0L;
char    *SCR_LOCK_TXT = 0;

int     SCR_NB_KEYS = 0;
YYKEYS  *SCR_SPECIAL_KEYS;
int     SCR_NB_CAPS;
YYKEYS  *SCR_CAPABILITIES;
YYKEYS  *SCR_MAP_OUT;
int     SCR_EXPORT_YN;
/* int     SCRE_first_table; */
int     SCR_level = 0;

char    SCR_db_name[2048];
char    SCR_OUT_SQL_FILE[SCR_MAX_FILE_LG_MAX];      /* BP_M 30-03-2007 */
char    SCR_CUR_DSN[SCR_MAX_FILE_LG_MAX];   /* BP_M 18-02-2008 */

int     IS_ERRNO = 0;
char    *SCR_transfile = 0;
char    *SCR_pgcmt_file = 0;
int     SCR_openmenu = 0;
U_ch    *SCR_username = 0; /* JMP 25-02-92 */
U_ch    *SCR_applname = 0; /* JMP 25-02-92 */
U_ch    *SCR_serialnb = 0; /* JMP 25-02-92 */
long    SCR_actkey  = 0;   /* JMP 25-02-92 */
long    SCR_limdate = 0;   /* JMP 25-02-92 */
long    SCR_crdate = 0;    /* JMP 25-02-92 */
long    SCR_crtime = 0;    /* JMP 25-02-92 */

/* =========== FILES ========== */
char    SCR_CMT_FILE[SCR_MAX_FILE_LG_MAX + 1] = "";
int     SCR_CMT_FILE_NB = 0;            /* JMP 16-04-95 */
long    SCR_CMT_FILE_POS = 0L;          /* JMP 16-04-95 */
char    SCR_NAME_SCR[SCR_MAX_FILE_LG_MAX + 1];
int     SCR_SCR_FILE_NB  = 0;           /* JMP 16-04-95 */
long    SCR_SCR_FILE_POS = 0L;          /* JMP 16-04-95 */
int     SCR_ERR_FILE_NB  = 0;           /* JMP 16-04-95 */
long    SCR_ERR_FILE_POS = 0L;          /* JMP 16-04-95 */

FILE    *fd_cmt = 0;

int     SCR_EXEC_BEG_IN_DISP = 0; /* BP_M 30-03-1995 */
int     SCR_DISP_MIN32 = 0; /* BP_M 30-03-1995 */

int     ACT_lscreen_fn = 0;  /* BP_M 31-10-95 */

int     MN_DISP_DOWN = 2;       /* BP_M 25-10-95 */

/* === MOUSE === */
int     SCR_MS_X;
int     SCR_MS_Y;
int     SCR_ms_button;
int     SCR_ms_event;

/* JMP 4.23 14-12-95 */
PR_DEF  *PR_CURRENT = 0;
char    *PR_TITLE = 0;
char    *PR_DOCNAME = 0;        /* JMP 23-08-98 */
int (*SCR_FN_KEY)() = 0;        /* JMP 4.23 14-12-95 */
int (*SCR_HIT_KEY_FN)() = 0;    /* JMP 4.23 14-12-95 */
MSSTAT   MS_stat;               /* JMP 4.23 14-12-95 */
int      MS_present = 0;        /* JMP 4.23 14-12-95 */
int     SCR_vtime;              /* JMP 4.23 14-12-95 */
char    *SCR_DOS_SCREEN = 0;    /* JMP 4.23 14-12-95 */
char    *SCR_OLD_SCREEN = 0;    /* JMP 4.23 14-12-95 */

unsigned char **SCR_CC_DEFS = 0; /* JMP 16-01-96 */
int     SCR_CC_NB_DEFS = 0;         /* JMP 16-01-96 */
int     IS_DEFAULT_TITLE = 0;       /* JMP 04-06-96 */
int     W_PRINTF_USED = 0;          /* JMP 05-06-96 */
int     W_PRINTF_BUF = 1;           /* JMP 30-09-98 */
int     SCR_COMP_RI = 0;            /* JMP 17-04-97 */
int     LIMIT_SQL;  /* BP_M 07-08-2008 */
/*NH*/
unsigned char SCR_MAP_BOX_12[] = {
    SCR_V_BAR2,     SCR_H_BAR2,
    SCR_T1_L2, SCR_T1_C2, SCR_T1_R2, SCR_M1_L2, SCR_M1_C2,
    SCR_M1_R2, SCR_B1_L2, SCR_B1_C2, SCR_B1_R2,
    SCR_T2_L1, SCR_T2_C1, SCR_T2_R1, SCR_M2_L1, SCR_M2_C1,
    SCR_M2_R1, SCR_B2_L1, SCR_B2_C1, SCR_B2_R1,
    SCR_T2_L2, SCR_T2_C2, SCR_T2_R2, SCR_M2_L2, SCR_M2_C2,
    SCR_M2_R2, SCR_B2_L2, SCR_B2_C2, SCR_B2_R2,
    0
};

unsigned char SCR_MAP_BOX_11[] = {
    SCR_V_BAR1,     SCR_H_BAR1,
    SCR_T1_L1, SCR_T1_C1, SCR_T1_R1, SCR_M1_L1, SCR_M1_C1,
    SCR_M1_R1, SCR_B1_L1, SCR_B1_C1, SCR_B1_R1,
    SCR_T1_L1, SCR_T1_C1, SCR_T1_R1, SCR_M1_L1, SCR_M1_C1,
    SCR_M1_R1, SCR_B1_L1, SCR_B1_C1, SCR_B1_R1,
    SCR_T1_L1, SCR_T1_C1, SCR_T1_R1, SCR_M1_L1, SCR_M1_C1,
    SCR_M1_R1, SCR_B1_L1, SCR_B1_C1, SCR_B1_R1,
    0
};


char *SCR_CHECK_BOX_TXT = 0;        /* BP_M 21-01-97 */
char *SCR_RADIO_TXT     = 0;        /* BP_M 21-01-97 */
char *SCR_CHECK_BOX_CHAR = "X";     /* BP_M 21-01-97 */
char *SCR_RADIO_CHAR     = 0;       /* BP_M 21-01-97 */

int  SCR_PROMPT_BA = 0;             /* BP_M 09-04-1997 */
int  PG_cur_nu;                     /* BP_M 10-04-1997 */
int  WSCR_ATTRS[20];                /* JMP 03-05-97 */
int  WSCR_NOCOLOR = 0; /* JMP 29-04-00 */

int  SCR_MODE_SQL = 0;  /* BP_M 01-10-97 */
char SCR_sDECIMAL = '.';  /* JMP 06-03-98 */
char SCR_sTHOUSAND = ',';       /* JMP 29-2-2012 */
char SCR_sDATE = '-';           /* JMP 29-2-2012 */
char SCR_sCURRENCY = 0;         /* JMP 29-2-2012 */
int  SCR_Y2K = 0; /* JMP 13-09-98 */
int  SCR_Y2K_PIVOT = 75; /* JMP 28-11-98 */
int  SCR_DFT_SET_TODAY = 1; /* JMP 01-10-98 */
int  FLD_mark[2] = {-1, -1};
int  SCR_SQL_CHECK_TODAY = 0; /* JMP 03-12-98 */
int  SCR_SELECT_COLOR = SCR_RED;    /* BP_M 08-12-1998 */
int  SCR_ISAM_NOORDER = 0; /* JMP 12-12-98 */
int  SCR_ISAM_NOBUF = 0; /* JMP 12-12-98 */
int  SCR_PG2IS_INPUT = 0;    /* BP_M 09-12-1998 */
int  SCR_ISAM_ANSI = 1; /* JMP 01-01-99 */
int  SCR_AUTO_MARK = 1; /* JMP 21-01-99 */
int  SCR_RESET_BEFORE_PGSEARCH = 1; /* BP_MODIF 22-12-98 */

int  SCR_IS2PG_PGSEARCH = 1; /* BP_M 26-05-1999 */
int  SCR_PGSEARCH_NOEDIT = 0;       /* BP_M 05-10-1999 */
int  WSOCK_DEBUG = 0; /* JMP 09-12-99 */
int  WSOCK_BUFSIZE = 4096; /* JMP 09-12-99 */
int  POP3_MAXBUFSIZE = 8192; /* JMP 10-10-06 */
int  POP3_MAXDELAY = 200;      /* JMP 10-10-06 */

unsigned char    SCR_NATS[]  = " 0123456789";
unsigned char    SCR_DATS[]  = "0123456789/-dDjJ";
unsigned char    SCR_TIMS[]  = "0123456789:tT";
unsigned char    SCR_INTS[]  = " +-0123456789";
unsigned char    SCR_REALS[] = " +-.0123456789Ee";
unsigned char    SCR_AL_CHECK_BOX[] = " Xx10";


int SCR_ISLD_QUIET = 0;     /* BP_M 06-01-2000 */
int SCR_HLP_NOPGNAME = 0; /* JMP 01-02-00 */

int SCR_MN_FREE_TITLE = 1;      /* BP_M 22-02-2000 */
int SCR_MN_FREE_FOOT = 1;       /* BP_M 22-02-2000 */
int SCR_PG_CHECK_NODUP = 1;     /* BP_M 22-02-2000 */

IMAGE   **PGIM_LIST = 0; /* JMP 05-05-00 */
int     NB_PGIM = 0;     /* JMP 05-05-00 */

FILE    *DW_FD_OUT;     /* BP_M 10-03-2001 15:44 */

int     IS_STRIP_STRING = 1; /* JMP 29-12-01 */
int     ScrReadFnWrite; /* BP_M 01-01-2002 19:07 */

int     SCR_GZIP_LEVEL = 8; /* JMP 26-10-02 */

int     SCR_SC_HIT_KEY = 0; /* JMP 15-01-03 : Change check_key en hit_key dans SC_hit_key() SILMM */

int     SCR_DEF_SILMM;  /* BP_M 07-02-2003 12:34 */

int     SCR_WIN_HELP_ACTIF = 1;         /* BP_M 20-02-2003 12:22 */

int     SCR_TYPE_DB = 0;    /* BP_M 01-03-2003 10:23 */

int GDEBUG = 0;         /* JMP 10-03-03 */
int GMSG_LEVEL = 0;     /* JMP 10-03-03 */
int GNCP = 8;           /* JMP 13-03-03 */
int GNLP = 24;          /* JMP 13-03-03 */
int GNLPLV = 16;        /* JMP 13-03-03 */
int GNLPFLD = 20;        /* JMP 13-03-03 */
int GNLPBUT = 32;        /* JMP 13-03-03 */
int GBUTTONACTIVATE = 1; /* JMP 15-03-03 */
char **GSCR_APPL_IMAGE;  /* JMP 15-03-03 */
int GSCR_QUIT_KEY = SCR_ESCAPE; /* JMP 15-03-03 */

int SCR_WGOTO_NEXT_FLD = 1;     /* BP_M 02-05-2003 11:34 */

char    SCR_REPLICA_INI[255];   /* BP_M 07-10-2009 15:20 */


long WSCR_PALETTE[6 * (1 + SCR_CHELP2) /* 1024 */];        /* BP_M 14-10-2009 19:20 + JMP 14-9-2015*/
char *SCR_PB_FILENAME;
unsigned char *SCR_PB_STRING;   /* JMP 16-01-11 */
int  SCR_USE_LOCALS;            /* BP_M 19-10-2009 14:56 */

char    SCR_EXPORT_TXT[SCR_MAX_FIELD_LENGTH + 1];   /* BP_M 12-12-2009 13:22 */
int     SCR_COMP_PG_ABORT = 0;
int     SCR_EXEC_DISP_BEFORE = 0;    /* BP_M 12-12-2009 13:43 */
int     SCR_flag_fork = -1;  /* BP_M 12-12-2009 13:49 */
int     PG_EXEC_DISP_FN = 1, PG_EXEC_DISP_FN_FLDS = 1;  /* BP_M 12-12-2009 13:59 */

/* BP_M 12-12-2009 15:50 */
#ifdef DOS
char    SCR_ISC_SEPS[2] = ":!";
#else
char    SCR_ISC_SEPS[2] = ":@";
#endif

long    ISC_SRV_TIME = 0L;
int     ISC_SRV_ALGO = 1;

int     ISC_NO_CHECK_LIVE;
long    last_check_live = 0;

U_ch    *ISC_BUFS[21];
int     ISC_BUFS_LG[21];
int     ISC_BUFS_POS[21];
int     ISC_BUFS_MODE[21];
int     ISC_BUFS_UNGETC[21];
int     ISC_BUFS_MAXLG[21];
int     ISC_BUFS_SOCKET[21];
int     ISC_BUFS_REEL[21];

int SCR_COPY_FILE_NB_TRY = 10;

int     SCR_APPLYCMP_IFEMPTY = 1;   /* BP_M 01-10-1998 */
int     SCR_NOT_APPLY_JOIN = 0; /* BP_M 29-02-2008 */

int SCR_hitkey_inuse = 0;
int     SCR_last_key = 0;
int     WSCR_DOSKEYS[WSCR_MAX_DOSKEYS];
int     WSCR_NB_DOSKEYS = 0;
/* MSGBOX */
int     SCR_msgbox_nc = -1, SCR_msgbox_line = -1;

WSOCK1  WSOCKS[WSOCK_MAX_SOCKS];
int     WSOCKS_CURRENT;

int         forceDebug = 0;  /* JMP 13-01-11 */
