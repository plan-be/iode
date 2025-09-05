#ifndef _TT_H_
#define _TT_H_
#include "s_mtobjs.h"

#include <scr4.h>

// WARNING: using min and max as macro names makes conflicts with the GNU implementation 
//          of the C++ standard library
#ifndef _max_
    #define _max_(x,y)        ((x)<(y)?(y):(x))
#endif
#ifndef _min_
    #define _min_(x,y)        ((x)<(y)?(x):(y))
#endif

#define TT_ATTR     SCR_DEFAULT
//#define TT_ATTR     SCR_CYAN

#define     TT_LEN_REALLOC      1
#define     TT_L_R_LINE         500
#define     TT_LEN_TAB          4
#define     NB_LINE             current_tt->nb_line
#define     LINE                current_tt->line
#define     COL                 current_tt->col
#define     NB_LINE_V           current_tt->nb_line_v
#define     NB_COL_V            current_tt->nb_col_v
#define     CSG_Y               current_tt->csg_y
#define     CSG_X               current_tt->csg_x
#define     PE_Y                current_tt->pe_y
#define     PE_X                current_tt->pe_x
#define     TXT_X               current_tt->txt_x
#define     TXT                 current_tt->txt
#define     POS_Y               (PE_Y + CSG_Y)
#define     POS_X               (PE_X + CSG_X)
#define     FILENAME            current_tt->filename
#define     SAVE_ON             current_tt->save_on
#define     POS_SCROLL          current_tt->pos_scroll
#define     LEN_SCROLL          current_tt->len_scroll
#define     NB_LINE_M           current_tt->nb_line_mem
#define     SCROLLB             current_tt->sb
#define     TT_NO_OPERATION     0
#define     TT_COMPRESS_BLOCK   1
#define     NB_FOND             13
#define     NB_VAL_FOND         12

/* ERRORS */

#define TT_MEM_FULL                 0
#define TT_HELP_NOT_FOUND           1
#define TT_BAD_DIR                  2
#define TT_MARG_ERR                 3
#define TT_MARG_UNDEF               4
#define TT_NEW_FILE                 5
#define TT_RW_DENIED                6
#define TT_RD_ONLY                  7
#define TT_NOT_LOADED               8
#define TT_CANT_WRITE               9
#define TT_NO_PRINTER              10
#define TT_CANT_OPEN               11
#define TT_NO_REC_KEY              12
#define TT_END_RECORD              13
#define TT_LIMIT_RECORD            14
#define TT_PRESS_F3                15
#define TT_STRING_NOT_FOUND        16
#define TT_LINE_TRUNCATED          17   /* JMP_M 4.19 08-05-95 */
#define TT_LAST_MSG                18   /* JMP_M 4.19 08-05-95 */

/* STRUCTURES */

typedef struct _tt {
    unsigned char    **txt;     /* JMP MODIF 28 08 90 */
    char    *filename;
    int     nb_line;
    int     line;
    int     col;
    int     nb_line_v;
    int     nb_col_v;
    int     csg_y;
    int     csg_x;
    int     pe_y;
    int     pe_x;
    int     txt_x;
    IMAGE   *im;
    int     save_on;
    int     pos_scroll;
    int     len_scroll;
    int     nb_line_mem;
    S_BAR   sb;
    } TT;

typedef struct _tt_tt {
    TT      **tt;
    int     nb_tt;
} _TT_TT;

typedef struct _mark {
    unsigned char    **buf;
    int     nb_line;
    int     nb_col;
} MARK;

/* FUNCTION KEY */

typedef struct TT_key  {
    char    *name;
    short   def;
    short   nb_key;
    unsigned short   *val;
} TT_KEY;

/* PRINTER */

typedef struct _print {
    char    name[21];
    char    device[11];
    int     system;
    int     fond[NB_VAL_FOND * NB_FOND];
} PRINT;

typedef struct _printers {
    short   nb_prt;
    PRINT   *print;
} PRINTERS;

extern char     TT_BLANC[132], TT_SN_USER[], TT_SN_SN[], TT_SN_AK[];
extern char     TT_LINE[132];
extern _TT_TT   TT_TT;
extern int      TT_OPERATION, TT_SN_DEMO;

/* BLOCK */

extern TT       *TT_BLOCK;
extern TT       *TT_ERROR;
extern int      TT_MARK1[2];
extern int      TT_MARK2[2];
extern int      TT_TYPE_MARK;       /* 0 = BLOCK : 1 = LINE */
extern int      TT_TYPE_WRITE;
extern short    TT_INSERT;
extern int      TT_FLAG_ERROR;
extern int      TT_TYPE_MOVE;
extern int      TT_MESSAGE;
extern int      TT_mode_edit;
extern int      TT_current_key;
extern int      TT_key_exec;
extern int      TT_MODIFY_CONF;
extern int      current_file;
extern TT_KEY   TT_key[26];
extern int      TT_nb_key;
extern TT       *current_tt;
extern int      TT_MAX_FILE;
extern int      TT_DISPLAY;
extern MENU     *TT_MENU_OLD;
extern int      TT_LM;
extern int      TT_RM;
extern int      TT_ACT_MARGE;
extern int      mode_file, load_conf, mode_mono, mode_ansi; /* JMP 31-01-96 */
extern MARK     *TT_mem_block();
extern char     *TT_construct_filename();
extern char     *TT_LIMITW;
extern PRINTERS TT_prt;
extern short    TT_current_imp;
extern MENU     *ttm_chimp;
extern FILE     *TT_open_printer();
extern int      TT_get_key();
extern char     *TT_MEMORY_FULL;
extern int      TT_nb_fv;
extern unsigned char     TT_UPPER[256 * 2];
extern int TT_mode_edit;
extern int      current_key;
extern int      TT_nb_frappe;
extern int      TT_IBM_MODE[], TT_TOSH_MODE[], TT_HPD_MODE[];

extern short    TT_nb_lastf;
extern char     *TT_last_file[40];
extern short    TT_last_pos[30][4];

/* variable externe de SCR */           /* BP 16-11-1990 17:13 */
extern APPL     *SCR_APPL;
extern int      SCR_PAGE_SIZE[2];
extern IMAGE    *SCR_TMP_IMAGE, *SCR_SCREEN;
extern int      SCR_CURRENT_ATTR;
extern IMAGE    *SCR_save_image();

#endif
