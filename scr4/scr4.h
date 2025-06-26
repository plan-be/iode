/* =+======== SCR.H =================================================== */

#ifndef _SCR4_H_
#define _SCR4_H_

#define SCR_VERSION     "SCR/AL1 Version 4.68 [11/2013]"
#define SCR_COPYRIGHT   "Copyright (c) 1987-2013 JM & B PAUL"
#define SCR_VERSION_DRV "SCR/AL1 Version 4.0 - Screen Driver\032";
#define SCR_VERSION_HLP "SCR/AL1 Version 4.56 - Help file\032";
#define SCR_ISC_VERSION  406

/*#define PG_CMT*/

#ifdef __cplusplus
#define SCRCPP
#endif

#ifdef SCRCPP        /* JMP 12-01-98 */
    #ifndef SCRPROTO
    #define SCRPROTO
    #endif
#endif

#ifdef SCRW16        /* JMP 12-02-97 */
#ifndef DOS
#define DOS
#endif
#ifndef WINDOWS
#define WINDOWS
#endif
/*#define PRWINDOWS  /* JMP 10-03-97 */
#define SCRWIN       /* JMP 12-02-97 */
#endif

#ifdef SCRW32        /* JMP 12-02-97 */
#ifndef DOS
#define DOS
#endif
#ifndef WINDOWS
#define WINDOWS
#endif
#ifndef WIN32
#define WIN32
#endif
#define PRWINDOWS
#define SCRWIN       /* JMP 12-02-97 */
#endif

#ifdef DOS
#define MS_MOUSE
#endif

#ifdef HP
#define MS_MOUSE
#endif

#ifdef X11
#define MS_MOUSE
#endif

#ifdef SCRGNOME
#undef MS_MOUSE
#endif

#ifdef WATCOM
#define HUGEMEM
#endif

#ifdef DOSW32
#ifndef DOSWIN
#define DOSWIN
#endif
#define PRWINDOWS
#endif

#ifdef DOSWIN
#define HUGEMEM
#ifndef WIN32
#define WIN32       /* JMP 28-05-00 */
#endif
/*#define PRWINDOWS /* JMP 10-03-97 */
#endif

#ifdef UNIX
#define HUGEMEM
#endif

#ifdef WINDOWS
#define HUGEMEM
#define PRWINDOWS
#undef MS_MOUSE
#endif

#if !defined(UNIX) && !defined(DOSW32) && !defined(SCRW32)
#define DOS16
#endif

#define SCR_NULL        ((char *)0)
#define SCR_NOT_AVAIL   -2.0e+37
#define SCR_NAN         SCR_NOT_AVAIL
#define SCR_ISAN(x)     ((x) >= -1.0e37)
#define SCR_PG_MAX_ALLOC        1024
#define SCR_MAXUSH              65535       /* BP_M 09-04-1997 */

#include <stdio.h>
#include <stdlib.h>
//#include <ctype.h>

//#include <s_alldoc.h>
#include <s_yy.h>
#include <s_date.h>
#include <s_strs.h>
#include <s_tar.h>
#include <s_dir.h>
#include <s_ini.h>
#include <s_isc.h>

#include <scr4_key.h>
#include <scr4_box.h>
#include <scr4_str.h>
#include <scr4_rd.h>

/* ======== ERROR ======================================= */

#define     ERR_FATAL                   2
#define     ERR_ERROR                   1
#define     ERR_WARNING                 0
#define     ERR_END                     -1

#define     ERR_ACCESS_FILE             0
#define     ERR_KEYWORD_UNKNOWN         1
#define     ERR_FIELD_UNKNOWN           2
#define     ERR_NO_ISAM                 3
#define     ERR_ISAM_LIMIT              4
#define     ERR_LIMIT_CMP               5
#define     ERR_WAIT_STRING             6
#define     ERR_WAIT_INT                7
#define     ERR_DIFFERENT_TYPES         8
#define     ERR_FIELD_EMPTY             9
#define     ERR_NO_NAME                10
#define     ERR_FIELD_UNDEFINED        11
#define     ERR_INCORRECT_ISAM         12
#define     ERR_SYNTAX                 13
#define     ERR_MEMORY                 14
#define     ERR_NOT_OPEN               15
#define     ERR_NO_PAGE                16
#define     ERR_NO_INDEX               17
#define     ERR_INCORRECT_COND         18
#define     ERR_UNDEFINED              19
#define     ERR_TOO_LONG               20
#define     ERR_LENGTH                 21
#define     ERR_DUPLICATE              22
#define     ERR_NO_ACTION              23
#define     ERR_NO_OPTION              24
#define     ERR_OBJ_EMPTY              25
#define     ERR_EOF                    26
#define     ERR_NOT_LOCK               27
#define     ERR_NOT_UNLOCK             28
#define     ERR_NAME_NOT_EXIST         29
#define     ERR_PAGE_SIZE              30
#define     ERR_IS_FLD_0               31 /* JMP 15-02-92 */
#define     ERR_FIELD_NOT_MENU         32 /* JMP_M 4.19 03-06-95 */
#define     ERR_FIELD_NOT_NATURAL      33 /* JMP_M 4.19 03-06-95 */
#define     ERR_INDEX_DUP              34 /* JMP_M 4.19 03-06-95 */
#define     ERR_VIEW_ISAM              35 /* JMP_M 4.20 16-06-95 */

/* =+====== KEYWORDS DEFINITION ====================== */

#define SCR_ATTRIBUTES      6001
#define SCR_BEGIN_FN        6002
#define SCR_CASE            6003
#define SCR_COLOR           6004
#define SCR_COLUMN          6005
#define SCR_CSR_COLUMN      6006
#define SCR_CSR_FLD         6007
#define SCR_CSR_LINE        6008
#define SCR_DATE            6009
#define SCR_END_FN          6010
#define SCR_END_KEYS        6011
#define SCR_FIELD           6012
#define SCR_FONT            6014
#define SCR_LINE            6016
#define SCR_MODIFIED        6017
#define SCR_NB_COLUMN       6018
#define SCR_NB_FLDS         6019
#define SCR_NB_LINE         6020
#define SCR_NB_OPTS         6021
#define SCR_TEXT            6024
#define SCR_TYPE            6025
#define SCR_WRAP            6026
#define SCR_BOX             6028
#define SCR_BACKGROUND      6030
#define SCR_CLIENT1         6031
#define SCR_CLIENT2         6032
#define SCR_CLIENT3         6033
#define SCR_CHOICE          6034
#define SCR_LETTER          6035
#define SCR_ENABLE          6036
#define SCR_FIRST_OPTION    6037
#define SCR_MENU_BAR        6038
#define SCR_MENU_COLUMN     6039
#define SCR_PAGE            SCR_TYPE_PAGE
#define SCR_LENGTH          6042
#define SCR_INDEX           6043
#define SCR_ZSTRING         6047
#define SCR_FILENAME        6049
#define SCR_ISAM            SCR_TYPE_ISAM
#define SCR_ADD_FLD         6051
#define SCR_NB_CMPS         6052
#define SCR_CODEISAM        6053
#define SCR_IO              6054
#define SCR_ALLOWED         6055
#define SCR_CLS             6056
#define SCR_MOVE            6057
#define SCR_CSR_ON          6058
#define SCR_CSR_OFF         6059
#define SCR_CSR_BOLD        6060
#define SCR_CSR_NORMAL      6061
#define SCR_INIT            6062
#define SCR_FONT0           6063
#define SCR_FONT1           6064
#define SCR_FONT2           6065
#define SCR_FONT3           6066
#define SCR_FONT4           6067
#define SCR_FONT5           6068
#define SCR_ISAM_NB         6069
#define SCR_NB_LNKS         6070

#define SCR_WRITE           6071
#define SCR_READ            6072
#define SCR_UPDATE          6073

#define SCR_LOCK            6074
#define SCR_NO_LOCK         6075

#define SCR_EQ              6076
#define SCR_LE              6077
#define SCR_GE              6078
#define SCR_GT              6079
#define SCR_LT              6080
#define SCR_FIRST           6081
#define SCR_LAST            6082
#define SCR_CIRC            6083
#define SCR_DB              6084
#define SCR_ASC             6085
#define SCR_DES             6086

#define SCR_DRAW            6087
#define SCR_FNKEY           SCR_TYPE_FNKEYS
#define SCR_VLINE           6089
#define SCR_HLINE           6090
#define SCR_TITLE           6091
#define SCR_FOOTNOTE        6092
#define SCR_REDRAW          6093

#define SCR_NB_PAGE         6094

#define SCR_LPG             6095
#define SCR_BPG             6096
#define SCR_BUF_SIZE        6097
#define SCR_USER            6098
#define SCR_CRITERION       6099
#define SCR_NE              6100
#define SCR_APG             6101
#define SCR_BC_ISAM         6102
#define SCR_LBC_ISAM        6103
#define SCR_ACTION          SCR_TYPE_ACT
#define SCR_FILL            6106
#define SCR_LINK_FN         6107
#define SCR_REWRITE_FN      6108
#define SCR_WRITE_FN        6109
#define SCR_READ_FN         6110
#define SCR_NO_MODIFY       6111
#define SCR_NO_CREATE       6112
#define SCR_NO_DELETE       6113
#define SCR_NO_SEARCH       6114
#define SCR_TODAY           6115
#define SCR_TIME            6116
#define SCR_TVA             6117
#define SCR_NOW             6118
#define SCR_BANK            6119

#define SCR_DUP             6121
#define SCR_NODUP           6122
#define SCR_DIR             6123

#define SCR_VAR             6130
#define SCR_NAME            6131
#define SCR_PROMPT          6132
#define SCR_FORMAT          6133
#define SCR_PLUS            6134
#define SCR_MINUS           6135
#define SCR_DFT             6136
#define SCR_NODFT           6137
#define SCR_ABS             6138
#define SCR_ISF             6139
#define SCR_CMP             6140
#define SCR_NO_ISAM         6141

#define SCR_COLON           6142
#define SCR_SEMI_COLON      6143
#define SCR_DOT             6144
#define SCR_COMMA           6145
#define SCR_UNDISPLAY       6146
#define SCR_DISPLAY_FN      6147
#define SCR_NO_FORMAT       6148
#define SCR_RETURN          6149
#define SCR_SEARCH_FN       6150  /* JMP 16-02-96 */

#define SCR_BPG_VIEW        6155    /* BP_M 20-02-2012 09:41 */
#define SCR_BPG_MODIFY      6156    /* BP_M 20-02-2012 09:42 */

/* =+============= SCR_REPORT ================================== */

#define     SCR_TOP_MARGIN      6200
#define     SCR_BOTTOM_MARGIN   6201
#define     SCR_LEFT_MARGIN     6202
#define     SCR_RIGHT_MARGIN    6203
#define     SCR_TO              6204
#define     SCR_FLD             6205

/* =+============= SCR_DRV.C =================================== */
#define     SCR_CAPS            6150
#define     SCR_MAPPING_IN      6151
#define     SCR_MAPPING_OUT     6152
#define     SCR_SP_KEYS         6153

/* #define SCR_END              901 */
#define SCR_MODIFY_FN        902
#define SCR_DELETE_FN        903
#define SCR_CREATE_FN        904
#define SCR_VIEW_FN          911
#define SCR_ISAM_FN          913


/* =+============= ATTRIBUTES ================================== */

#define SCR_INVISIBLE   0  /* BLACK BLACK */
#define SCR_DEFAULT     1  /* BLACK CYAN */
#define SCR_REVERSE     2  /* LIGHT GREY BLACK */
#define SCR_UNDERLINE   3  /* LIGHT GREY BLUE  */
#define SCR_BOLD        4  /* BLACK RED */
#define SCR_BLINKING    5  /* BLACK LIGHT RED */
#define SCR_DIM         6  /* BLACK RED  */

/* =+============= COLORS ======================================== */

#define SCR_BLACK       7  /* BLACK LIGHT GREY */
#define SCR_BLUE        8  /* BLUE  WHITE      */
#define SCR_GREEN       9  /* GREEN LIGHT GREY */
#define SCR_CYAN       10  /* CYAN  YELLOW     */
#define SCR_RED        11  /* RED   LIGHT GREY */
#define SCR_MAGENTA    12  /* MAGENTA white */
#define SCR_WHITE      13  /* LIGHT GREY , RED */
#define SCR_YELLOW     14  /* BROWN YELLOW */
#define SCR_SHADOW     15  /* Couleur des ombres */
#define SCR_CHELP1     16  /* Couleur des helps */
#define SCR_CHELP2     17  /* Couleur des helps */

/* ========== OTHERS ============= */
#define SCR_CONDENSED  20  /* BLACK RED  */
#define SCR_ENLARGED   21  /* BLACK RED  */

/* =+============= IO ============================================ */

#define SCR_INPUT       7001
#define SCR_OUTPUT      7002
#define SCR_HIDDEN      7003
#define SCR_ACCESSIBLE  7004

/* =+============= TYPES ========================================= */

#define SCR_STRING      7019
#define SCR_FLOAT       7005
#define SCR_REAL        SCR_FLOAT
#define SCR_INTEGER     7006
#define SCR_MENU        SCR_TYPE_MENU
#define SCR_MENUI       SCR_TYPE_MENUI
#define SCR_OPTION      7009
#define SCR_NATURAL     7010
#define SCR_AUTO        7011
#define SCR_LINK        7012
#define SCR_REF         7013
#define SCR_SHORT       7014
#define SCR_CHAR        7015
#define SCR_LONG        7016
#define SCR_DOUBLE      7017
#define SCR_EDITOR      7018
#define SCR_PASSWD      7020

/* =+============= CASES ========================================= */

#define SCR_UPPERCASE   8001
#define SCR_LOWERCASE   8002

/* =+============= CENTER ======================================== */

#define SCR_ASIS        8003
#define SCR_LJUST       8004
#define SCR_RJUST       8005
#define SCR_CENTER      8006
#define SCR_SQZ         8007

/* =+============= COMPULSORY ==================================== */

#define SCR_NOT_COMPULSORY 8009
#define SCR_COMPULSORY     8010
#define SCR_REQUIRED       8011
#define SCR_IMP_REQ        8012 /* JMP_M 4.19 09-05-95 */
#define SCR_IMP_TODAY      8013 /* JMP_M 4.19 09-05-95 */

/* =+============= WRAP ========================================== */

#define SCR_OFF         8020
#define SCR_ON          8021

/* =+============= ACTIONS ======================================= */

#define ACT_PG_DISPLAY       9000
#define ACT_MN_EDIT          9001
#define ACT_PG_EDIT_ISAM     9002
#define ACT_BC_EDIT_ISAM     9003
#define ACT_ACTION           9004
#define ACT_SYSTEM           9005
#define ACT_PG_EDIT          9006
#define ACT_PG_EDIT_FIELD    9007
#define ACT_PG_SET_CSR_FIELD 9008
#define ACT_MP_EDIT_ISAM     9009
#define ACT_CATG             9011
#define ACT_USER             9012
#define ACT_REC_KEY          9013
#define ACT_INFO             9014
#define ACT_PG_SLIDE         9015
#define ACT_PR_PAGE          9016
#define ACT_PR_ISAM          9017
#define ACT_PR_STRING        9018
#define ACT_PR_TAB           9019
#define ACT_PR_PG_TAB        9020
#define ACT_PR_SEND          9021
#define ACT_PR_SELECT        9022
#define ACT_PR_FORMFEED      9023
#define ACT_PR_LINEFEED      9024
#define ACT_PR_BOLD          9026
#define ACT_PR_ENLARGED      9027
#define ACT_PR_CONDENSED     9028
#define ACT_PR_UNDERLINE     9029
#define ACT_PR_NORMAL        9030
#define ACT_PR_VTAB          9031
#define ACT_C_FN             9032
#define ACT_FLD_SKIP         9033
#define ACT_FLD_REEDIT       9034
#define ACT_PG_ABORT         9035
#define ACT_AB_DISPLAY       9036
#define ACT_AB_UNDISPLAY     9037
#define ACT_AB_EDIT          9038
#define ACT_PG_REEDIT        9039
#define ACT_APPL_RUN         9040
#define ACT_APPL_QUIT        9041
#define ACT_APPL_MAIN        9042
#define ACT_ASK              9043
#define ACT_APPLY_CMP        9044
#define ACT_HELP             9045
#define ACT_HELPFILE         9046
#define ACT_PG_SCAN_ISAM     9047
#define ACT_MI_EDIT          9048
#define ACT_CREATE_REC       9049
#define ACT_SCAN_ISAM        9050
#define ACT_FORK             9051
#define ACT_AB_EDIT_ISAM     9052
#define ACT_BCAB_EDIT_ISAM   9053
#define ACT_PG_DISPLAY_FLDS  9054
#define ACT_PG_UNDISPLAY     9055
#define ACT_NEXT_REC         9056
#define ACT_PREV_REC         9057
#define ACT_SEARCH_REC       9058
#define ACT_OPEN_ISAM        9059
#define ACT_CLOSE_ISAM       9060
#define ACT_PG_CREATE_REC    9061
#define ACT_PG_RESET         9062
#define ACT_DELETE_REC       9063
#define ACT_MODIFY_REC       9064
#define ACT_BC_RESET         9065
#define ACT_BC_DISPLAY       9066
#define ACT_BC_UNDISPLAY     9067
#define ACT_BC_CREATE        9068
#define ACT_BC_OPEN          9069
#define ACT_BC_CLOSE         9070
#define ACT_BC_SEARCH        9071
#define ACT_BC_NEXT          9072
#define ACT_BC_PREV          9073
#define ACT_BC_MODIFY        9074
#define ACT_BC_DELETE        9075
#define ACT_BC_DISPLAY_FLDS  9076
#define SCR_EXPORT           9077
#define SCR_PG_IMPORT        9078
#define SCR_MP_IMPORT        9079
#define SCR_AP_IMPORT        9080
#define SCR_SHADOW0          9081
#define SCR_SHADOW1          9082
#define SCR_SHADOW2          9083
#define SCR_SHADOW3          9084
#define SCR_SHADOW4          9085
#define SCR_NO_VIEW_ORDER    9086
#define SCR_MP_SET_CBUF      9087
#define SCR_DRW_MEM          9088
#define SCR_FLD_MEM          9089
#define SCR_OPT_MEM          9090
#define SCR_IS_SEARCH        9091
#define SCR_IS_NEXT          9092
#define SCR_IS_PREV          9093
#define SCR_IS_DELETE        9094
#define SCR_IS_REWRITE       9095
#define SCR_IS_CLOSE         9096
#define ACT_PR_UP            9097
#define ACT_PR_DOWN          9098
#define ACT_PR_LEFT          9099
#define ACT_PR_RIGHT         9100
#define ACT_PR_GOTO          9101
#define SCR_DB_NAME          9102
#define SCR_SQL              9103
#define SCR_SQL1             9104
#define SCR_PAGE_TO_ISAM     9105
#define SCR_ISAM_TO_PAGE     9106
#define SCR_MP_ADD           9107
#define SCR_MP_DELETE        9108
#define SCR_MP_MODIFY        9109
#define SCR_MP_VIEW          9110
#define SCR_MP_CREATE        9111
#define SCR_LOCK_SCREEN      9112
#define SCR_SET_ALARM        9113
#define SCR_STOP_ALARM       9114
#define SCR_RESTART_ALARM    9115
#define SCR_ALIAS_ISAM       9122
#define SCR_IF_BC_SELECT     9123
#define SCR_IF_PG_SELECT     9124
#define SCR_IF_ALFA          9125
#define SCR_PG_CMT           9126
#define SCR_DATE_FORMAT      9127
#define SCR_TIME_FORMAT      9128
#define SCR_GLOBAL           9129
#define ACT_ASKI             9130   /* JMP 24-01-96 */
#define ACT_ASKS             9131   /* JMP 24-01-96 */

#define SCR_PROPERTIES       9132   /* BP_M 26-01-97 */
#define SCR_WIZARD           9133   /* BP_M 26-01-97 */
#define ACT_EDIT_TPAGES      9134   /* BP_M 26-01-97 */
#define ACT_TP_EDIT_ISAM     9135   /* JMP 20-08-97 */
#define SCR_SQL2             9136   /* BP_M 27-02-1998 */
#define SCR_GE2EQ            9137   /* JMP 15-08-99 */
#define SCR_AP_EDIT_ISAM     9138   /* BP_M 04-10-1999 */
#define SCR_IS_WRITE         9139   /* BP_M 15-07-1999 */
#define SCR_SQL3             9140   /* BP_M 27-02-1998 */

/****** DB_SYS *********************/
#define SCR_IS_DBSYS         9179
#define SCR_IS_CTREE         9180    /* JMP 06-02-03 */
#define SCR_IS_ORACLE        9181    /* JMP 06-02-03 */
#define SCR_IS_INFORMIX      9182    /* JMP 06-02-03 */
#define SCR_IS_ODBC          9183    /* JMP 06-02-03 */
#define SCR_IS_CISAM         9184    /* JMP 06-02-03 */
#define SCR_IS_SQLSERVER     9185
#define SCR_IS_MYSQL         9186
#define SCR_IS_POSTGRES      9187

#define SCR_IS_INGRES        9188    /* JMP 06-02-03 */
#define SCR_IS_SYBASE        9189
#define SCR_IS_CTREE6        9190

#define CTREE4    0
#define ODBC      (SCR_IS_ODBC       - SCR_IS_CTREE)
#define ACCESS    ODBC
#define CTREE6    (SCR_IS_CTREE6     - SCR_IS_CTREE)
#define CISAM     (SCR_IS_CISAM      - SCR_IS_CTREE)
#define INFORMIX_DB  (SCR_IS_INFORMIX   - SCR_IS_CTREE)
#define ORACLE    (SCR_IS_ORACLE     - SCR_IS_CTREE)
#define INGRES    (SCR_IS_INGRES     - SCR_IS_CTREE)
#define SYBASE    (SCR_IS_SYBASE     - SCR_IS_CTREE)
#define SQLSERVER (SCR_IS_SQLSERVER  - SCR_IS_CTREE)
#define IS_MYSQL  (SCR_IS_MYSQL      - SCR_IS_CTREE)
#define POSTGRES  (SCR_IS_POSTGRES   - SCR_IS_CTREE)

#define SCR_IS_DB_STRING     9199       /* BP_M 14-06-1999 */

#define SCR_NLV              9200
#define SCR_NCV              9201
#define SCR_DIAS             9202
#define SCR_NAME_MEM         9203
#define SCR_ONE_OBJ          9204
#define SCR_NO_DEF           9205
#define SCR_PAGE_CENTER      9206
#define SCR_MENU_AUTO        9207
#define SCR_PAGE_AUTO        9208
#define SCR_SYNC             9209
#define SCR_CLOCK_POSI       9210
#define SCR_MEM_POSI         9211
#define SCR_DATE_POSI        9212
#define SCR_KEY_POSI         9213
#define SCR_PAGE_COLOR       9214
#define SCR_MENU_COLOR       9215
#define SCR_FIELD_COLOR      9216
#define SCR_PAGE_BOX         9217
#define SCR_PAGE_SHADOW      9218
#define SCR_CONF_CREATE      9219
#define SCR_CONF_DELETE      9220
#define SCR_SMP_FILE         9221
#define SCR_RESET_FN         9222
#define SCR_APPLY_TO         9223
#define SCR_NEXT             9224
#define SCR_TRANS_FILE       9225
#define SCR_INIT_HISTORY     9226
#define SCR_END_HISTORY      9227
#define SCR_NO_DEL_EOF       9228
#define SCR_IF_ERROR         9229
#define SCR_MESSAGE          9230
#define SCR_HSI_FIX          9231
#define SCR_PGCMT_FILE       9232
#define SCR_INSERT_LBC       9238
#define SCR_WRITE_LBC_FN     9239
#define SCR_OPEN_MENU        9240
#define SCR_BOX_1            9241
#define SCR_PROFILE          9242
#define SCR_CLOCK_DISP_SECS  9243
#define SCR_APPL_NAME        9244
#define SCR_USER_NAME        9245
#define SCR_SERIAL_NB        9246
#define SCR_LIM_DATE         9247
#define SCR_SBAR             9248
#define SCR_STBAR            9249
#define SCR_SMP_BPG          9250
#define SCR_PGALIGN          9251
#define SCR_ISALIGN          9252
#define SCR_CRYPT            9253       /* JMP38 14-09-92 */
#define SCR_CLOSEBOX         9254       /* JMP41 19-01-93 */
#define SCR_NOCLOSEBOX       9255       /* JMP41 19-01-93 */
#define SCR_NOSHADOW         9256       /* 4.03 */
#define SCR_ANYK             9257       /* BP_MODIF 16-11-94 */
#define SCR_HSI_VAR          9258       /* BP_M 29-10-96 */
#define SCR_ISI_MEM          9259       /* BP_M 15-01-97 */
#define SCR_NO_ISI_MEM       9260       /* BP_M 15-01-97 */
#define SCR_CHECK_BOX        9261       /* BP_M 15-01-97 */
#define SCR_BEGIN_RADIO      9262       /* BP_M 21-01-97 */
#define SCR_END_RADIO        9263       /* BP_M 21-01-97 */
#define SCR_GROUP            9264       /* BP_M 21-01-97 */
#define SCR_TYPE_RADIO       9266       /* BP_M 21-01-97 */
#define SCR_CHECK_BOX_TXT_   9267       /* BP_M 21-01-97 */
#define SCR_RADIO_TXT_       9268       /* BP_M 21-01-97 */
#define SCR_CHECK_BOX_CHAR_  9269       /* BP_M 21-01-97 */
#define SCR_RADIO_CHAR_      9150       /* JMP 25-01-97 */
#define SCR_TITLE_COLUMN     9151       /* JMP 25-01-97 */
#define SCR_FIELDS           9152       /* JMP 25-01-97 */
#define SCR_WX               9153       /* JMP 25-01-97 */
#define SCR_WY               9154       /* JMP 25-01-97 */
#define SCR_WW               9155       /* JMP 25-01-97 */
#define SCR_WH               9156       /* JMP 25-01-97 */


/* BP_M 06-04-1993 13:56 */

#define SCR_FLD_IDX          9270       /* BP_M 06-04-1993 13:55 */
#define SCR_EQ1              9271
#define SCR_EQ2              9272
#define SCR_EQ3              9273
#define SCR_EQ4              9274
#define SCR_EQ5              9275
#define SCR_EQ6              9276       /* BP_M 06-04-1993 13:55 */
#define SCR_FLD_COND         9277       /* JMP_M 4.19 31-05-95 */
#define SCR_FLD_COUNT        9278       /* JMP_M 4.19 31-05-95 */
#define SCR_SPG              9279       /* JMP_M 4.19 31-05-95 */
#define SCR_LPG_MEM          9280       /* JMP_M 4.19 10-06-95 */
#define SCR_VIEW             9281       /* JMP_M 4.20 14-06-95 */
#define SCR_VIEW_CREATE_FN   9282       /* JMP_M 4.20 14-06-95 */
#define SCR_AUTO_CREATE      9283       /* JMP_M 4.20 14-06-95 */
#define SCR_BC_AUTO_CREATE   9284       /* JMP_M 4.20 14-06-95 */
#define SCR_MP_AUTO_CREATE   9285       /* JMP_M 4.20 14-06-95 */
#define SCR_BC_LPG_MEM       9286       /* JMP_M 4.20 14-06-95 */
#define SCR_MP_LPG_MEM       9287       /* JMP_M 4.20 14-06-95 */
#define SCR_MP_USE_REF       9288       /* JMP_M 4.20 14-06-95 */
#define SCR_NO_MARK          9289       /* JMP_M 4.21 01-09-95 */
#define SCR_WIN_VLINE        9290       /* JMP 11-10-96 */
#define SCR_WIN_HLINE        9291       /* JMP 11-10-96 */
#define SCR_WIN_FIXED        9292       /* JMP 11-10-96 */
#define SCR_WIN_NCP          9293       /* JMP 19-04-97 */
#define SCR_WIN_TITLE        9294       /* JMP 23-04-97 */
#define SCR_WIN_NLPFLD       9295       /* JMP 26-04-97 */
#define SCR_WIN_NLPTXT       9296       /* JMP 26-04-97 */
#define SCR_WIN_NLPVLINE     9297       /* JMP 26-04-97 */
#define SCR_WIN_NLPHLINE     9298       /* JMP 26-04-97 */
#define SCR_WIN_NLPNULL      9299       /* JMP 26-04-97 */

#define SCR_CREATE           9300       /* BP_M 24-01-97 */
//#define SCR_NEXT             9301     /* JMP 24-01-97 */
#define SCR_PREV             9302       /* BP_M 24-01-97 */
#define SCR_MODIFY           9303       /* BP_M 24-01-97 */
#define SCR_BUTTON           9305       /* BP_M 24-01-97 */
#define SCR_BITMAPFILE       9306       /* BP_M 24-01-97 */
#define SCR_BUTTON_NL        9307       /* BP_M 24-01-97 */
#define SCR_BUTTON_NC        9308       /* BP_M 24-01-97 */
#define SCR_ICON_NL          9309       /* BP_M 24-01-97 */
#define SCR_ICON_NC          9310       /* BP_M 24-01-97 */
#define SCR_SPACE            9311       /* BP_M 24-01-97 */
#define SCR_CHECK_BUTTON     9312       /* JMP 24-01-97 */
#define SCR_GR_NAME          9313       /* BP_M 09-04-1997 */
#define SCR_PROMPT_AFTER     9314       /* BP_M 09-04-1997 */
#define SCR_ALT_KEY          9315       /* BP_M 09-04-1997 */
#define SCR_FORCE_NULL       9316       /* BP_M 09-04-1997 */

#define SCR_WIN_ENABLE       9317       /* JMP 03-05-97 */
#define SCR_WIN_DISABLE      9318       /* JMP 03-05-97 */
#define SCR_ISF_CHECK        9319       /* JMP 23-05-97 */
#define SCR_ISF_CHECKALWAYS  9320       /* JMP 23-05-97 */
#define SCR_ISF_CHECKNEVER   9321       /* JMP 23-05-97 */
#define SCR_RATTR            9325       /* JMP 23-05-97 */

/* =+============= WINDOWS ATTRIBUTES ================================== */

#define SCR_WIN_ATTR     9326  /* JMP 17-12-98 */
#define SCR_ITALIC       9327  /* JMP 17-12-98 */
#define SCR_BOLDITALIC   9328  /* JMP 17-12-98 */
#define SCR_WIN_NCPFLD   9329  /* JMP 14-12-99 */
#define SCR_WIN_NOCOLOR  9330  /* JMP 29-04-00 */

/* ================ TOOBARS BUTTONS ==================== */

#define SCRTB_FIRST        8500
#define SCRTB_NEW          8500
#define SCRTB_OPEN         8501
#define SCRTB_SAVE         8502
#define SCRTB_CUT          8503
#define SCRTB_COPY         8504
#define SCRTB_PASTE        8505
#define SCRTB_PRINT        8506
#define SCRTB_HELP         8507
#define SCRTB_CONTEXT      8508



/* =========== WIN_ATTRS ============================ */

#define WIN_CLOSEBOX    1L
#define WIN_MENUBOX     2L
#define WIN_ROTATE      4L
#define WIN_PREV        8L
#define WIN_NEXT        16L
/* #define WIN_         326L
#define WIN_            64L
#define WIN_            128L
*/
#define WIN_UPDOWN      256L
#define WIN_DOWN        512L
#define WIN_UP          1024L
#define WIN_HELP        2048L

#define WIN_HSBAR       8192L
#define WIN_VSBAR       16384L
#define WIN_MOVE        32768L
#define WIN_RESIZE      65536L

/* =+============= NEW DEFINE ==================================== */

#define SCR_RESTORE         12000

/* =+============= DIVERS ======================================== */

#define SCR_TOGGLE           10000
#define SCR_COMMENT          10001
#define SCR_PG_COMMENT       10002
#define SCR_SHORT1           10003
#define SCR_NB_DECI          10004
#define SCR_EQS              10005
#define SCR_FIX_DEC          10006
#define SCR_DEL_EOF          10007
#define SCR_NO_OBJ           10008
#define SCR_DEF_OBJ          10009
#define SCR_SCREEN_PG        10010
#define SCR_VIEW_ORDER       10011
#define SCR_NO_HIST          10012 /* BP_M 09-03-1995 */
#define SCR_VARLEN           10013 /* BP_M 11-07-95 */
#define SCR_VFIELD           10014 /* BP_M 09-03-1995 */
#define SCR_L_KEY            10015 /* BP_M 26-10-95 */
#define SCR_GET_KEY          10016 /* BP_M 26-10-95 */
#define SCR_LSCREEN_FN       10017 /* BP_M 31-10-95 */
#define SCR_VAL_LOCK_TXT     10019 /* BP_M 31-10-95 */
#define SCR_VAL_LOCK_DELAY   10020 /* BP_M 31-10-95 */
#define SCR_RAW              10021 /* BP_M 11-09-1997 */
#define SCR_DEF_MAX          10022 /* BP_M 02-10-97 */
#define SCR_BUF_SEARCH       10023 /* BP_M 08-10-97 */
#define SCR_EXT_SIZE         10024 /* BP_M 08-12-1998 */
#define SCR_MINMAX           10025 /* BP_M 03-10-97 */
#define SCR_BUF_SYNC         10026
#define SCR_PG_TABENTER      10027 /* BP_M 25-10-1999 */
#define SCR_QUERY            10028 /* JMP 28-10-99 */
#define SCR_FOREIGN_KEY      10029
#define SCR_SHRINK           10030
#define SCR_READ_AHEAD       10031
#define SCR_PROT             10032
#define SCR_MP_IMPORT_FLD    10033      /* BP_M 17-06-2001 17:21 */
#define SCR_LIMIT            10034
#define SCR_CDRAW            10035      /* BP_M 06-02-2003 16:51 */
#define SCR_S4SRV            10036      /* BP_M 14-01-2008 */
#define SCR_S4SRV_PORT       10037      /* BP_M 14-01-2008 */
#define SCR_JOIN             10038  /* BP_M 28-02-2008 */
#define SCR_REQ_JOIN         10039  /* BP_M 28-02-2008 */

#define SCR_A_GRAVE         133
#define SCR_A_CIRC          131
#define SCR_A_TREMA         132

#define SCR_C_CEDILLE       135

#define SCR_E_AIGU          130
#define SCR_E_GRAVE         138
#define SCR_E_CIRC          136
#define SCR_E_TREMA         137

#define SCR_I_CIRC          140
#define SCR_I_TREMA         139

#define SCR_O_CIRC          147
#define SCR_O_TREMA         148

#define SCR_U_GRAVE         151
#define SCR_U_CIRC          150
#define SCR_U_TREMA         129

#define SCR_SPEC_1          176  /* PREMIER CARACTERE ASCENSEUR */
#define SCR_SPEC_2          177  /* DEUXIEME CARACTERE ASCENSEUR */
#define SCR_SPEC_3          178  /* CARATERE POUR < 32 */


/* PRINTER DEFINES */

#define     PR_FILE             20001
#define     PR_BOLD             20002
#define     PR_E_BOLD           20003
#define     PR_UNDERL           20004
#define     PR_E_UNDERL         20005
#define     PR_REVERSE          20006
#define     PR_E_REVERSE        20007
#define     PR_CONDENSED        20008
#define     PR_E_CONDENSED      20009
#define     PR_ENLARGED         20010
#define     PR_E_ENLARGED       20011
#define     PR_BEGIN            20012
#define     PR_END              20013
#define     PR_NL               20014
#define     PR_NC               20015
#define     PR_TABS             20016
#define     PR_PG_TABS          20017
#define     PR_MAP              20018

#define     PR_TYPE             20019
#define     PR_PRINTER          20020
#define     PR_VTABS            20021
#define     PR_FORMFEED         20022
#define     PR_LINEFEED         20023
#define     PR_NORMAL           20024
#define     PR_DIRECT           20025       /* JMP 04-05-97 */

#define     ISC_EOF             -1000

/* ==== EXTERN VARS DECLARATIONS ================================= */

//#ifdef SCRCPP
//extern "C" {
//#endif /* SCRCPP*/

extern int          SCR_PAGE_SIZE[];
extern int          SCR_CURRENT_COLOR;
extern int          SCR_CURRENT_ATTR;
extern int          SCR_CURRENT_FONT;
extern int          SCR_CURSOR_STATUS;
extern int          SCR_CURRENT_LINE;
extern int          SCR_CURRENT_COLUMN;
extern int          SCR_CURRENT_CSR_SIZE;
extern char         SCR_LANGUAGE[] ;
extern char         SCR_DIRECTORY[] ;
extern char         SCR_TERM[] ;
extern IMAGE        *SCR_SCREEN;
extern int          SCR_INS_TGL;
extern char         SCR_BUFFER[];

extern FILE_TABLE   SCR_READ_FILES;
extern int          SCR_BC_OPERATION;
extern int          SCR_PG_OPERATION;
extern int          SCR_NB_KEYS;
extern YYKEYS       *SCR_SPECIAL_KEYS;
extern int          SCR_NB_CAPS;
extern YYKEYS       *SCR_CAPABILITIES;
extern YYKEYS       *SCR_MAP_OUT;
extern int          SCR_USER_CATG;
extern int          SCR_PG_ABORT;
extern char         **ARG_VALUES;
extern int          ARG_COUNT;
extern int          SCR_CLOCK_POS[];
extern int          SCR_CLOCK_SECS;
extern int          SCR_DATE_POS[];
extern int          SCR_MEM_POS[];
extern int          SCR_KEY_POS[];
extern int          SCR_BREAK_ON;
extern int          CNF_EXEC_MENU;
extern int          CNF_NOTEPAD;
extern int          CNF_REVERSE_TITLE;
extern IMAGE        *SCR_TMP_IMAGE;
extern int          TXT_nb_tt;
extern int          ERR_TYPE;
extern YYFILE       *SCR_YY;
extern YYKEYS       YY_TBL[];
extern int          YY_NO_COMMENT2; /* JMP 03-03-2004 */
extern char         *ERR_MSG_TEXT[];
extern char         *ERR_LEVEL[];
extern char         **SCR_CFNS;
extern int          SCR_NB_CFNS;
extern int          SCR_FLD_SKIP;
extern int          SCR_FLD_REEDIT;
extern int          SCR_LAST_ATTR;
extern int          SCR_APP_ABORT;
extern APPL         *SCR_APPL;
extern int          SCR_LKEY;
extern int          SCR_nb_cmt;
extern unsigned char SCR_FILL_SCREEN;
extern int          SCR_NB_DEC;
extern int          ERR_NB_ERRS;
extern int          SCR_RIGHT_MENU;
extern char         *SCR_INCLUDE_FILE;
extern int          SCR_E_STATUS;
extern PR_DEF       *PR_CURRENT;
extern int          SCR_MAP_EXEC;
extern FILE         *HLP_FD;
extern char         HLP_NAME[SCR_MAX_FILE_LG + 1];
extern HELPS        HLPS;
extern int          SCR_OLD_ATTR;
extern int          IS_DEF_SQZ;
extern int          SCR_CHECK_DELETE;
extern int          SCR_EDIT_KILL;
extern char         *PR_TITLE, *PR_DOCNAME; /* JMP 23-08-98 */
extern char         SCR_CMT_FILE[SCR_MAX_FILE_LG_MAX + 1];
extern int          SCR_CMT_FILE_NB;            /* JMP 16-04-95 */
extern long         SCR_CMT_FILE_POS;           /* JMP 16-04-95 */
extern char         SCR_NAME_SCR[SCR_MAX_FILE_LG_MAX + 1];
extern int          SCR_SCR_FILE_NB;            /* JMP 16-04-95 */
extern long         SCR_SCR_FILE_POS;           /* JMP 16-04-95 */
extern char         SCR_NAME_ERR[SCR_MAX_FILE_LG_MAX + 1];
extern int          SCR_ERR_FILE_NB;            /* JMP 16-04-95 */
extern long         SCR_ERR_FILE_POS;           /* JMP 16-04-95 */
extern _SCR_CMT     SCR_CMT;
extern char         *SCR_CMT_TXT;
extern int          SCR_CONFIRME_ON;
extern long         SCR_END_TIME_1, SCR_END_TIME_2;
extern int          CT_SYNC;
//extern char         SCR_EXPORT_TXT[];
extern char         SCR_EXPORT_TXT[SCR_MAX_FIELD_LENGTH + 1];   /* BP_M 12-12-2009 13:22 */
extern int          SCR_EXPORT_YN;
extern int          SCRE_first_table;
extern int          SCR_level;
extern int          SCR_openmenu;
extern U_ch         *SCR_username;
extern U_ch         *SCR_applname;
extern U_ch         *SCR_serialnb;
extern long         SCR_actkey;
extern long         SCR_limdate;
extern long         SCR_crdate;
extern long         SCR_crtime;
extern U_ch         SCR_magic[];
extern long         SCR_LOCK_TIME;
extern char         *SCR_LOCK_TXT;
extern int          SCR_vtime;
extern char         *SCR_CHECK_BOX_TXT;  /* BP_M 21-01-97 */
extern char         *SCR_RADIO_TXT;      /* BP_M 21-01-97 */
extern char         *SCR_CHECK_BOX_CHAR; /* BP_M 21-01-97 */
extern char         *SCR_RADIO_CHAR;     /* BP_M 21-01-97 */
extern int          SCR_PROMPT_BA;       /* BP_M 09-04-1997 */
extern int          PG_cur_nu;           /* BP_M 10-04-1997 */

extern int          SCR_MODE_SQL;   /* BP_M 01-10-97 */

extern int          SCR_EXEC_BEG_IN_DISP;   /* BP_M 30-03-1995 */
extern int          SCR_DISP_MIN32;         /* BP_M 30-03-1995 */
extern int          PG_EXEC_DISP_FN;        /* BP_M 30-03-1995 */
extern int          PG_EXEC_DISP_FN_FLDS;   /* JMP_M 25-05-95*/
extern int          SCR_ERR_LANG;           /* JMP_M 4.19 07-05-95 */
extern int          SCR_msgbox_line;        /* JMP_M 4.19 12-05-95 */
extern int          SCR_msgbox_nc;          /* JMP_M 4.19 12-05-95 */

extern int          SCR_MP_EXEC_LPG_DISP_ACT;    /* BP_M 01-11-1999 */
extern int          SCR_BC_EXEC_LPG_DISP_ACT;    /* BP_M 01-11-1999 */

extern long         SCR_STEP_BCCOUNT;       /* BP_M 20-10-95 */
extern int          W_PRINTF_USED;          /* JMP 05-06-96 */
extern int          IS_ERRNO;               /* JMP 14-10-96 */

extern int          WSCR_ATTRS[20];         /* JMP 03-05-97 */
extern char         SCR_sDECIMAL;           /* JMP 06-03-98 */
extern char         SCR_sLIST;              /* JMP 06-03-98 */
extern char         SCR_sTHOUSAND;          /* JMP 29-02-12 */
extern char         SCR_sDATE;              /* JMP 29-02-12 */
extern char         SCR_sCURRENCY;          /* JMP 29-02-12 */
extern int          SCR_Y2K;                /* JMP 13-09-98 */

extern int          SCR_BC_EMPTY_REC;       /* BP_MODIF 29-10-98 */
extern int          SCR_BC_RESET_APG;       /* BP_MODIF 29-10-98 */
extern int          SCR_NO_REREAD;          /* JMP 01-11-98 */
extern int          SCR_SELECT_COLOR;       /* BP_M 08-12-1998 */
extern int          SCR_ISAM_NOORDER;       /* JMP 12-12-98 */
extern int          SCR_ISAM_NOBUF;         /* JMP 13-12-98 */
extern int          SCR_ISAM_ANSI;          /* JMP 01-01-99 */
extern int          SCR_AUTO_MARK;          /* JMP 21-01-99 */

extern char         SCR_db_name[2048];  /* BP_M 08-12-2010 20:02 */
extern char         SCR_OUT_SQL_FILE[SCR_MAX_FILE_LG_MAX];     /* BP_M 30-03-2007 */
extern char         SCR_CUR_DSN[SCR_MAX_FILE_LG_MAX];   /* BP_M 18-02-2008 */

extern int          SCR_MPRETRIEVE_REC;     /* BP_M 16-10-1999 */
extern int          ScrSqlGE2EQ;            /* JMP 16-12-99 */
extern int          DebugActif;             /* JMP 16-12-99 */
extern int          ODebugDetail;           /* JMP 16-12-99 */
extern char         *DebugFilename;         /* JMP 16-12-99 */
extern int          SCR_SQL_CHECK_TODAY;    /* BP_M 22-12-1999 */
extern int          SCR_SEARCH_LIKE    ;    /* BP_M 22-12-1999 */

extern int          SCR_MN_FREE_TITLE;      /* BP_M 22-02-2000 */
extern int          SCR_MN_FREE_FOOT;       /* BP_M 22-02-2000 */
extern int          SCR_PG_CHECK_NODUP;     /* BP_M 22-02-2000 */
extern int          IS_STRIP_STRING;        /* JMP 29-12-01 */
extern int          ScrReadFnWrite;         /* BP_M 01-01-2002 19:07 */
extern int          SCR_REPOS_AFTER_WRITE;  /* BP_M 19-06-2003 */
extern int          SCR_SQL_REPOSIT;        /* BP_M 20-05-2003 */

extern int          SCR_USE_LOCALS;         /* JMP 26-10-2004 */
extern char         *SCR_PB_FILENAME;       /* JMP 28-05-05 */
extern unsigned char *SCR_PB_STRING;         /* JMP 16-01-11 */

/* BP_M 02-11-1999 */
extern unsigned char    SCR_NATS[];
extern unsigned char    SCR_DATS[];
extern unsigned char    SCR_TIMS[];
extern unsigned char    SCR_INTS[];
extern unsigned char    SCR_REALS[];
extern unsigned char    SCR_AL_CHECK_BOX[];

extern int          WSOCK_DEBUG;            /* JMP 05-12-99 */
extern int          SCR_ISC_PORT;           /* JMP 06-12-99 */
extern char         *SCR_ISC_SERVER;        /* JMP 06-12-99 */
extern char         *SCR_ISC_ALIAS; /* BP_M 31-01-2008 */
extern int          SCR_HLP_NOPGNAME;       /* JMP 01-02-00 */
extern int          WSCR_NOCOLOR;           /* JMP 01-05-00 */
extern int          ISC_COMPRESS;           /* JMP 03-06-00 */
extern int          ISC_NB_NEXTS;           /* JMP 08-12-00 */
extern int          ISC_SERVER_VERS;        /* JMP 06-02-05 */
extern int          SCR_GZIP_LEVEL;         /* JMP 04-06-00 */
extern int          SCR_CHECK_PROT;         /* BP_M 06-06-2001 19:46 */
extern int          WSOCK_BUFSIZE;          /* JMP 18-10-02 */
extern long         WSOCK_CRYPTKEY;         /* JMP 22-10-02 */
extern int          POP3_MAXBUFSIZE;        /* JMP 10-10-06 */
extern int          POP3_MAXDELAY;          /* JMP 10-10-06 */

extern int          SCR_MODE_CLIENT;        /* JMP 10-06-02 */

/* ======== INTERFACE WEB ================================== */
extern long     SCR_WEB_SESSION;
extern char    *SCR_WEB_IP;
extern U_ch    **SCR_WEB_PARMS;
extern U_ch    **SCR_WEB_ARGS;
extern U_ch    *SCR_WEB_CGI;
extern int     SCR_WEB_NB_ARGS;
extern int     SCR_WEB_DEBUG_LEVEL;
extern char    *SCR_WEB_MSG;
extern int     SCR_WEB_NOHEADER;
extern char    SCR_WEB_PATH[255];
extern char    *SCR_WEB_LANGUAGE;
extern char    SCR_WEB_BCMD[];
extern char    SCR_WEB_ECMD[];
extern char    SCR_WEB_TRDFILE[255];                   /* BP_M 09-11-2002 16:16 */
extern int     SCR_WEB_TRDLANG;                        /* BP_M 09-11-2002 16:17 */

/*======== GNOME ====================================================== */
extern int      GDEBUG;                 /* JMP 10-03-03 */
extern int      GNCP, GNLP, GNLPFLD, GNLPBUT, GNLPLV ;  /* JMP 15-03-03 */
extern int      GBUTTONACTIVATE;                        /* JMP 15-03-03 */
extern char     **GSCR_APPL_IMAGE;                      /* JMP 15-03-03 */
extern int      GSCR_QUIT_KEY;
extern int      GSCR_KEYB_LATENCY;
extern int      GSCR_KEYB_INTERVAL;

/*=========== DOSW32 et X11 =========================================== */
extern char    *WSCR_FONTNAME;
extern int     WSCR_SPACE_LEFT;
extern int     WSCR_SPACE_RIGHT;
extern int     WSCR_SPACE_TOP;
extern int     WSCR_SPACE_BOTTOM;
extern int     WSCR_ROUND_CORNERS;
extern int     WSCR_SEMIGRAPHIC_CHARS;
extern int     WSCR_SET_CADRES;
extern long    WSCR_PALETTE[6 * (SCR_CHELP2 + 1) /* 1024*/];  /* JMP 14-09-2015 */

#define WSCR_MAX_DOSKEYS    20

/* ========== COMPATIBILITY BETWEEN VERSIONS ============ */
extern int  SCR_COMP_RI; /* MPIS et EDIS : edite index méme si champ Hidden ou Output */
extern int  SCR_COMP_PG_ABORT; /* Reset SCR_PG_ABORT aprés action PG_EDIT */
extern int  SCR_SC_HIT_KEY; /* JMP 15-01-03 : Change check_key en hit_key dans SC_hit_key() SILMM */

extern int  SCR_MP_REDISPLAY;   /* BP_M 25-01-2013 15:16 */
extern int  SCR_MP_FORCE_ADD;   /* BP_M 15-02-2013 10:29 */

#ifdef DOSWIN
extern int  WSCR_ICON;          /* JMP 4.23 06-01-96 */
extern int  WSCR_FONTSIZE;      /* JMP 13-09-2015 : suppression de WSCR_FONTNB */
extern int  WSCR_AUTO_QUIT;     /* JMP 4.23 06-01-96 */
extern int  WSCR_NOWINDOW;      /* JMP 02-07-99 */
extern int  (*WSCR_PROPSFN)(void); /* JMP 06-02-98 */
extern int  WSCR_CURSORCOLOR;    /* JMP 12-05-11 */
#endif
    /* BP_M 01-10-97 */
/*
#ifdef IN_SQL
extern char         SCR_db_name[2048];
#endif
*/
#ifdef DEBUG
extern FILE         *fd_debug;
#endif

/* ============== MMT COLORS =============================== */
extern int  MMT_BACK,
	    MMT_REVERSE,
	    MMT_COMMENT,
	    MMT_BOXES;

//#ifdef SCRCPP
//}
//#endif /* SCRCPP*/

/* ==== DEFINE FOR SCR_TBL ================================= */

#define  SCR_EOF        -1
#define  SCR_OPEN_BR    520
#define  SCR_CLOSE_BR   521

#define  SCR_write(a, b, c, d) (SCR_TMP_IMAGE != 0) ? SCR_write_im(a, b, c, d) : SCR_write_text(a, b, c, d) /* JMP 24-10-91 */
#define  SCR_write_xattrs(a, b, c, d) (SCR_TMP_IMAGE != 0) ? SCR_write_xattrs_im(a, b, c, d) : SCR_write_xattrs_text(a, b, c, d) /* JMP 12-01-2004 */
#define  SCR_clear_xattrs(a, b, c, d) (SCR_TMP_IMAGE != 0) ? SCR_clear_xattrs_im(a, b, c, d) : SCR_clear_xattrs_text(a, b, c, d) /* JMP 12-01-2004 */

#define PAGE_READ      SCR_read_flt_nu(SCR_TYPE_PAGE)
#define MENU_READ      SCR_read_flt_nu(SCR_TYPE_MENU)
#define ISAM_READ      SCR_read_flt_nu(SCR_TYPE_ISAM)
#define MPAGE_READ     SCR_read_flt_nu(SCR_TYPE_MPAGE)
#define BC_READ        SCR_read_flt_nu(SCR_TYPE_BC)
#define ACT_READ       SCR_read_flt_nu(SCR_TYPE_ACT)
#define PRT_READ       SCR_read_flt_nu(SCR_TYPE_PRINTER)
#define FNKEYS_READ    SCR_read_flt_nu(SCR_TYPE_FNKEYS)
#define AB_READ        SCR_read_flt_nu(SCR_TYPE_ABAR)
#define AP_READ        SCR_read_flt_nu(SCR_TYPE_APPL)
#define MI_READ        SCR_read_flt_nu(SCR_TYPE_MENUI)
#define TP_READ        SCR_read_flt_nu(SCR_TYPE_TPAGES)
#define TB_READ        SCR_read_flt_nu(SCR_TYPE_WTOOLB) /* BP_M 18-04-1997 */

#define TYPE5(type)    (type - SCR_TYPE_PAGE)
#define TYPE16(type)   (type + SCR_TYPE_PAGE)

#define  MN_text(mn, i)     (((MENU *)mn)->mn_opts[i].opt_text)

#define  CONFIRME           (SCR_confirme((char *)0) != 0)
#ifdef YYTEXT           /* JMP38 20-09-92 */
#undef YYTEXT
#undef YYLONG
#undef YYREAD
#endif
#define YYTEXT  (SCR_YY->yy_text)
#define YYLONG  (SCR_YY->yy_long)
#define YYREAD  (SCR_YY->yy_type)

#define IS_search_sql(is, sql)      IS_search(is, 0, SCR_SQL1, sql)

/* =============== MOUSE DEFINITIONS ======================== */

#define MS_MOVE     1
#define MS_PRESS    2
#define MS_DEPRESS  4
#define MS_DRAG     3

#define MS_C        (MS_stat.col)
#define MS_L        (MS_stat.line)
#define MS_E        (MS_stat.event)
#define MS_B        (MS_stat.but1)
#define MS_B2       (MS_stat.but2)
#define MS_B3       (MS_stat.but3)

typedef struct _msstatus_ {
    int     line,
	    col,
	    but1, but2, but3,
	    event;
} MSSTAT;

extern MSSTAT   MS_stat;
extern int      MS_present;

/* ==== FUNCTIONS TYPES ================================= */

/* === 1. PSEUDO FUNCTIONS (MACROS) =====*/

#define PG_fld_name(pg, fld_nb)     _SCR_find_buf(pg->pg_fld_name, fld_nb)
#define PG_fld_help(pg, fld_nb)     _SCR_find_buf(pg->pg_fld_help, fld_nb)
#define PG_fld_fmt(pg, fld_nb)      _SCR_find_buf(pg->pg_fld_fmt, fld_nb)
#define PG_fld_allowed(pg, fld_nb)  _SCR_find_buf(pg->pg_fld_allowed, fld_nb)
#define PG_fld_cmt(pg, fld_nb)      _SCR_find_buf(pg->pg_fld_cmt, fld_nb)

#define SCR_get_field_date(pg, fld_nb)      SCR_get_field_long(pg, fld_nb)
#define SCR_get_field_time(pg, fld_nb)      SCR_get_field_long(pg, fld_nb)
#define SCR_set_field_date(pg, fld_nb, l)   SCR_set_field_long(pg, fld_nb, l)
#define SCR_set_field_time(pg, fld_nb, l)   SCR_set_field_long(pg, fld_nb, l)

#define SCR_set_attr(attr)          (SCR_CURRENT_ATTR = attr)
#define SCR_PTR(i)                  (SCR_READ_FILES.tbl_fl[(i)]->fl_ptr)
#define SCR_OBJ_TYPE(i)             (TYPE16(SCR_READ_FILES.tbl_fl[i]->fl_type))
#define SCR_NU(ptr)                 (SCR_find_ptr_nu((char *)(ptr)))  /* JMP 14-01-98 */
#define SCR_PTR_TYPE(i)             (SCR_OBJ_TYPE(SCR_NU(i)))   /* JMP 20-08-97 */

#define PAGE_PTR(i)                 ((PAGE *)SCR_PTR(i))  /* JMP 14-01-98 */
#define ISAM_PTR(i)                 ((ISAM *)SCR_PTR(i))  /* JMP 14-01-98 */
#define MENU_PTR(i)                 ((MENU *)SCR_PTR(i))  /* JMP 14-01-98 */
#define BC_PTR(i)                   ((BC *)SCR_PTR(i))    /* JMP 14-01-98 */
#define MPAGE_PTR(i)                ((MPAGE *)SCR_PTR(i)) /* JMP 14-01-98 */
#define AB_PTR(i)                   ((A_BAR *)SCR_PTR(i)) /* JMP 14-01-98 */
#define AP_PTR(i)                   ((APPL *)SCR_PTR(i)) /* JMP 14-01-98 */
#define FNKEYS_PTR(i)               ((FNKEYS *)SCR_PTR(i)) /* JMP 14-01-98 */
#define TOOLBAR_PTR(i)              ((WTOOLBAR *)SCR_PTR(i)) /* JMP 14-01-98 */

#define SCR_set_char(obj, val)      SCR_set_vchar  ((obj), 0, (val))
#define SCR_set_double(obj, val)    SCR_set_vdouble((obj), 0, (val))
#define SCR_set_float(obj, val)     SCR_set_vfloat ((obj), 0, (val))
#define SCR_set_long(obj, val)      SCR_set_vlong  ((obj), 0, (val))
#define SCR_set_short(obj, val)     SCR_set_vshort ((obj), 0, (val))
#define SCR_set_text(obj, val)      SCR_set_vtext  ((obj), 0, (val))

#define SCR_get_char(obj)           SCR_get_vchar  ((obj), 0)
#define SCR_get_double(obj)         SCR_get_vdouble((obj), 0)
#define SCR_get_float(obj)          SCR_get_vfloat ((obj), 0)
#define SCR_get_long(obj)           SCR_get_vlong  ((obj), 0)
#define SCR_get_short(obj)          SCR_get_vshort ((obj), 0)
#define SCR_get_text(obj, val)      SCR_get_vtext  ((obj), 0, (val))

#define SCR_rget_char(obj)          SCR_rget_vchar  ((obj), 0)
#define SCR_rget_double(obj)        SCR_rget_vdouble((obj), 0)
#define SCR_rget_float(obj)         SCR_rget_vfloat ((obj), 0)
#define SCR_rget_long(obj)          SCR_rget_vlong  ((obj), 0)
#define SCR_rget_short(obj)         SCR_rget_vshort ((obj), 0)
#define SCR_rget_text(obj, val)     SCR_rget_vtext  ((obj), 0, (val))

#define PG_text(obj)                PG_vtext((obj), 0)
#define PG_set_io(obj, type)        PG_set_vio((obj), 0, (type))
#define PG_reset_obj(obj)           PG_reset_vobj((obj), 0)             /* BP_M 16-01-2003 12:11 */
#define PG_set_field(obj, text)     PG_set_vfield((obj), 0, (text))
#define PG_get_field(obj, text)     PG_get_vfield((obj), 0, (text))
#define PG_set_attr(obj, attr)      PG_set_vattr((obj), 0, (attr))
#define PG_display_obj(obj)         PG_display_vobj((obj), 0)
#define PG_edit_obj(obj)            PG_edit_vobj((obj), 0)
#define PG_set_dfld(obj, text)      PG_set_vdfld((obj), 0, (text))
#define PG_empty_obj(obj)           PG_empty_vobj((obj), 0)
#define PG_goto_obj(obj)            PG_goto_vobj((obj), 0)
#define OBJ_set_fld_isam(obj1, obj2)  FLD_set_isam((obj1), 0, (obj2))
#define PG_get_text(pg, fld_nb)     (((PAGE *)(pg))->pg_rec + ((PAGE *)(pg))->pg_fld_pos[fld_nb])

#define COUCOU  printf("File : %s, Line %d\n", __FILE__, __LINE__);

#define SCR_is_graph(ch)             ((ch) > 178 && (ch) < 219) /* BP_M 11-06-1999 */


/* === 2. FUNCTIONS PROTOTYPES =====*/

#ifndef SCRPROTO  /* JMP 28-12-97 */

extern char *U_control_to_string(/* unsigned char */) ;
extern struct page *PG_create(/* void*/) ;
extern TPAGES *TP_create(/* void*/) ;  /* BP_M 26-01-97 */
extern struct image *PG_save(/* struct page **/) ;
extern struct bcom *BC_create(/* void*/) ;
extern char SCR_graphic_char(/* int ,int ,int ,int */) ;
extern struct menu *MN_create(/* void*/) ;
extern struct menui *MI_create(/* void*/) ;
extern struct option *MN_add_option(/* struct menu **/) ;
extern struct isam *IS_create(/* void*/) ;
WTOOLBAR *TB_create(/* void*/) ;  /* BP_M 18-04-1997 */
extern struct isfield *IS_add_fld(/* struct isam **/) ;
extern struct composite *IS_add_cmp(/* struct isam **/) ;
extern struct link *IS_add_lnk(/* struct isam **/) ;
extern char *SCR_construct_filename(/* char **/) ;
extern char *SCR_add_ext(/* char **/) ;
extern long SCR_date_to_long(/* char **/) ;
extern long SCR_current_auto_num(/* char **/) ;
extern long SCR_current_auto_num_isam(/* char **/) ;
extern long SCR_auto_num(/* char **/) ;
extern long ScrGetAutoNum(/* char **/) ;
extern long ScrIncrAutoNum(/* char **/) ;
extern long IS_get_auto_num(/* char **/) ;
extern long IS_incr_auto_num(/* char **/) ;
extern long SCR_get_field_long(/* struct page *,int */) ;
extern double SCR_get_field_double(/* struct page *,int */) ;
extern char *SCR_get_field_text(/* struct page *,int, char * */) ;
extern char *SCR_find_filename(/* void **/) ;
extern char *SCR_message(/* int */) ;
extern struct image *SCR_save_image(/* int ,int ,int ,int */) ;
extern struct image *SCR_extract_image(/* int ,int ,int ,int */) ;
extern struct image *SCR_copy_image(/* IMAGE * */) ;
extern struct image *SCR_alloc_image(/* int ,int */) ;
extern char *SCR_find_cap(/* int */) ;
extern struct status *SCR_save_status(/* void*/) ;
extern struct draw *PG_add_drw(/* struct page **/) ;
extern struct image *MP_save(/* struct mpage **/) ;
extern struct mpage *MP_create(/* void*/) ;
extern struct bclist *BCL_alloc(/* void*/) ;
extern struct bclist *BCL_locate(/* struct bclist *,int */) ;
extern struct bclist *BC_add_bcl(/* struct bcom *,int */) ;
extern long BC_get_count(/* BC, int line */);   /* JMP_M 4.19 09-06-95 */
extern char *BC_lpg_rec(/* BC *,int */) ;       /* JMP_M 4.19 08-06-95 */
extern char *BC_lbc_rec(/* BC *,int */) ;       /* JMP_M 4.19 08-06-95 */
extern char *IS_error_text(/* void*/);
extern char *IS_error(/* void*/);
extern long IS_get_count_nois(/* char **/);
extern long IS_get_max_nois(/* char **/);
extern double IS_query_double_nois(/* char **/);
extern long IS_get_count_where_nois(/* char *, char **/);
extern U_ch **IS_list_tables_nois(/*void */);
extern unsigned char **SCR_read_file(/*char **/);
extern FILE *IV_open(/* char *filename */);
extern long *IV_write(/* FILE *, char *, long */);
extern long *IV_rewrite(/* FILE *, long, char *, long */);
extern long *IV_read_len(/* FILE *, long */);
extern char *TXT_strip_str(/* char * */);
extern char *SCR_read_object(/* char *, int, FILE *, function */);
extern char  *SCR_yylex_str(/* FILE *, char ** */);
extern PR_DEF   *PR_create();
extern PRTDEV   **SCR_ReadPrts();
extern int      PR_string();
extern ACTION   *ACT_create();
extern long     PG_get_long();
extern double   PG_get_double();
extern char     *SCR_get_vtext();
extern long     SCR_get_vlong();
extern short    SCR_get_vshort();
extern double   SCR_get_vdouble();
extern float    SCR_get_vfloat();
extern char     *SCR_rget_vtext();
extern long     SCR_rget_vlong();
extern short    SCR_rget_vshort();
extern double   SCR_rget_vdouble();
extern float    SCR_rget_vfloat();
extern U_ch     *SCR_get_field_editor();
extern U_ch     *SCR_get_editor();
extern FIELD    *PG_add_field();
extern int      PG_read_def();
extern int      IS_read_def();
extern int      MN_read_def();
extern int      MP_read_def();
extern int      BC_read_def();
extern int      ACT_read_def();
extern int      PR_read_def();
extern int      AB_read_def();
extern int      AP_read_def();
extern FILE_TYPE *SCR_find_flt();
extern FILE_TYPE *SCR_add_flt();
extern FILE_TYPE *SCR_add_flt_ptr();
extern char     *SCR_create();
extern unsigned char     *SCR_yylex_alloc_str();
extern char     *SCR_yylex_alloc_cfn();
extern unsigned char     *SCR_yylex_alloc_name();
extern ACTION   *ACT_read_fn();
// extern char     *PG_get_text();/* JMP 4.23 13-12-95 */
extern char     *_SCR_find_buf();
extern char     *PG_get_rec_txt();
extern char     *PG_aget_rec_txt();

extern REPORT   *RP_create();
extern APPL     *AP_create();
extern A_BAR    *AB_create();
extern A_OPT    *AB_add_option();
extern DRAW     *DRW_add();
extern IMAGE    *MN_save();
/*extern long     SCR_get_field_date(); */
/*extern long     SCR_get_field_time(); */

extern FILE_TYPE *SCR_flt_exist();
extern long     SC_scan_page_gn();
extern long     SC_scan_isam_gn_chk();
extern long     SC_scan_isam_gn();
extern long     SC_scan_page();
extern long     SC_scan_isam();
extern long     SC_set_isam_gn();
extern long     SC_set_page_gn();
extern unsigned char  *PG_vtext();

extern ISAM     *PG_find_isam();
extern ISAM     *IS_dup();
extern MENU     *MN_auto_create();
extern char     *IS_text();
extern MENU     *MN_create_default();
extern unsigned char *OBJ_search_ptr();
extern char     *ACT_find_str();
extern long     ACT_find_lg();
extern double   ACT_find_db();
extern long     IS_copy(), IS_copy_fn(), IS_dup_dbase(); /* JMP 08-11-98 */
extern IMAGE    *PG_cmt_display();
extern char     *SCR_construct_cmt_fnk();
extern long     KEY_calc();
extern long     SCR_ak();

extern char     *SCR_find_load_obj();
extern char     *PG_search_fld_name();
extern char     *SCR_find_ptr_name();

extern char     *IS_get_tblname();
extern CTINFO   *IS_get_doda();       /* JMP 11-09-97 */

extern char     *OBJ_read();
extern char     *SCR_fnk_txt();

/* MMT */
extern MMT  *MMT_create();
extern int  MMT_edit_window();
extern int  MMT_edit();
extern unsigned char *MMT_get_vec();
extern unsigned char *MMT_edit_line();
extern unsigned char *MMT_edit_field();
extern LST  *MMT_get_lst();
extern char *SCR_fnk_txt();

extern double SCR_atof();   /* BP_M 24-07-95 */

extern short SCR_str_add(); /* BP_M 23-01-97 */
extern short SCR_str_find_str(); /* BP_M 23-01-97 */
extern char *SCR_str_get(); /* BP_M 23-01-97 */

extern long WscrGetMS(); /* JMP 01-04-99 */

extern int  (*SCR_FN_KEY)(); /* JMP 10-06-94 */
extern int  (*SCR_HIT_KEY_FN)();

extern char *IS_get_field();        /* BP_M 22-02-2000 */
extern GZIPFILE *GzipCreate();      /* JMP 19-10-00 */

extern char *SCR_HttpGetFile(); 

#ifndef WATCOM
extern double  atof();
#endif

#else /* SCRPROTO */

#ifndef SCRSUPPROTO


#ifdef SCRCPP
extern "C" {
#endif /* SCRCPP*/

/* s_wprint.c */
    int Wprintf(char *, ...);
    void Werror(char *,...);
/* s_err.c */
    void SCR_yyerror(int, int, ...);
    void SCR_yyplant(int, int, ...);
/* s_pgcr.c */
    int PG_display_error(char *,...);
/* s_pgask.c */
    int SCR_ask(int ,char *, ...);
/* s_pgcro.c */
    int PG_v3_display_error(char *, ...);
/* s_pgcnfo.c */
    int SCR_v3_confirme(char *, ...);
/* s_pgcnf.c */
    int SCR_confirme(char *, ...);
/* s_stprf.c */
    int SCR_printf_file(char *, ...);
/* s_prtstr.c */
    int PR_string(char *, ...);
/* s_menu.c */
    int MN_set_title(MENU *,char *, ...);
    int MN_set_footnote(MENU *,char *, ...);
    int MN_add_set_option_text(MENU *,char *, ...);
    int MN_set_option_text(MENU *,int ,char *, ...);
/* s_mnch.c */
    OPTION *MN_insert_option(MENU *,int ,char *, ...);
/* s_isvarg.c */
    int IS_open_files(ISAM *,...);
    int IS_close_files(ISAM *,...);
    int IS_check_open(ISAM *,...);
/* other files */
    int SCR_init(void);
    int ACT_functions(int);

    extern char *SCR_HttpGetFile(char *, int , char *); 
#ifdef SCRCPP
}
#endif /* SCRCPP */

#endif /* SCRSUPPROTO */

#include <s_probas.h>
#include <s_probis.h>

extern int  (*SCR_FN_KEY)(int, int);
extern int  (*SCR_HIT_KEY_FN)(void);

#endif /* SCRPROTO */

/*********** MACROS DE DEBUGGING *********************/

#ifdef LINUX

#define GMSGL {                         \
    extern  int GMSG_LEVEL;             \
    char    GMSG_BUF[80];               \
    memset(GMSG_BUF, ' ', sizeof(GMSG_BUF)); \
    GMSG_LEVEL = max(0, GMSG_LEVEL);    \
    GMSG_LEVEL = min(40, GMSG_LEVEL);   \
    GMSG_BUF[GMSG_LEVEL] = 0;                \
    Gdbg(GMSG_BUF);                     \
    }

#define GMSGC {    \
    GMSGL \
    Gdbg("%s() [%s:%d]\n", __FUNCTION__, __FILE__, __LINE__);\
    }

#define GMSGB {             \
    extern int GMSG_LEVEL;  \
    GMSG_LEVEL++;           \
    }

#define GMSGE {             \
    extern int GMSG_LEVEL;  \
    GMSG_LEVEL--;           \
    }

#define GMSG(...) {    \
    GMSGL \
    Gdbg("%s() [%s:%d] ", __FUNCTION__, __FILE__, __LINE__);\
    Gdbg(__VA_ARGS__); \
    }

#define GMSGBC {    \
    GMSGB \
    GMSGC \
    }

#define GMSGEC {    \
    GMSGC \
    GMSGE \
    }

#define GMSGBM(...) {   \
    GMSGB               \
    GMSG(__VA_ARGS__)  \
    }

#define GMSGEM(...) {   \
    GMSG(__VA_ARGS__);  \
    GMSGE               \
    }

#define GMSGR(val) {        \
    GMSGEM(" return(%d)\n", val)            \
    return(val);            \
    }

#else // Windows ...

#define GMSGL {                         \
    extern  int GMSG_LEVEL;             \
    char    GMSG_BUF[80];               \
    memset(GMSG_BUF, ' ', sizeof(GMSG_BUF)); \
    GMSG_LEVEL = max(0, GMSG_LEVEL);    \
    GMSG_LEVEL = min(40, GMSG_LEVEL);   \
    GMSG_BUF[GMSG_LEVEL] = 0;                \
    Gdbg(GMSG_BUF);                     \
    }

#define GMSGC {    \
    GMSGL \
    Gdbg("%s:%d\n", __FILE__, __LINE__);\
    }

#define GMSGB {             \
    extern int GMSG_LEVEL;  \
    GMSG_LEVEL++;           \
    }

#define GMSGE {             \
    extern int GMSG_LEVEL;  \
    GMSG_LEVEL--;           \
    }

#define GMSG(x, y) {    \
    GMSGL \
    Gdbg("%s:%d ", __FILE__, __LINE__);\
    Gdbg(x, y); \
    }

#define GMSGBC {    \
    GMSGB \
    GMSGC \
    }

#define GMSGEC {    \
    GMSGC \
    GMSGE \
    }

#define GMSGBM(x, y) {   \
    GMSGB               \
    GMSG(x, y)  \
    }

#define GMSGEM(x, y) {   \
    GMSG(x, y);  \
    GMSGE               \
    }

#define GMSGR(val) {        \
    GMSGEM("return(%d)\n", val) \
    return(val);            \
    }


#endif // Linux


#endif /* _SCR4_H_ */













