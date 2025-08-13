#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"       // KDB

/*----------------------- ENUMS ----------------------------*/

enum TableCellFont
{
    TABLE_CELL_NORMAL = 0,
    TABLE_CELL_BOLD = 1,
    TABLE_CELL_ITALIC = 2,
    TABLE_CELL_UNDERLINE = 4
};

enum TableCellAlign
{
	TABLE_CELL_CENTER = 8,
	TABLE_CELL_LEFT = 16,
	TABLE_CELL_RIGHT = 32,
	TABLE_CELL_DECIMAL = 64
};

enum TableAscii
{
    TABLE_ASCII_ENGLISH = 100,
    TABLE_ASCII_DUTCH,
    TABLE_ASCII_FRENCH,
    TABLE_ASCII_DIM,
    TABLE_ASCII_DIVIDER,
	TABLE_ASCII_LINE_FILES,
	TABLE_ASCII_LINE_MODE,
    TABLE_ASCII_OPEN,
    TABLE_ASCII_CLOSE,
    TABLE_ASCII_BREAK,
	TABLE_ASCII_LINE_CELL,
	TABLE_ASCII_LINE_SEP,
    TABLE_ASCII_BOLD_LINE,
	TABLE_ASCII_LINE_TITLE,
	TABLE_ASCII_CELL_LEC,
	TABLE_ASCII_CELL_STRING,
	TABLE_ASCII_LINE_DATE,
    TABLE_ASCII_YMIN,
    TABLE_ASCII_YMAX,            
    TABLE_ASCII_ZMIN,            
    TABLE_ASCII_ZMAX,            
    TABLE_ASCII_XGRID,           
    TABLE_ASCII_YGRID,           
    TABLE_ASCII_BOX,             
    TABLE_ASCII_AXIS,            
    TABLE_ASCII_ALIGN,               
    TABLE_ASCII_LEFT_AXIS,           
    TABLE_ASCII_RIGHT_AXIS,          
    TABLE_ASCII_GRAPH_LINE,          
    TABLE_ASCII_GRAPH_BAR,           
    TABLE_ASCII_GRAPH_SCATTER      
};

enum TableLang
{
    TABLE_ENGLISH = TABLE_ASCII_ENGLISH,
    TABLE_DUTCH = TABLE_ASCII_DUTCH,
    TABLE_FRENCH = TABLE_ASCII_FRENCH
};

const static int IODE_NB_LANGS = 3;

enum TableLineType
{
	TABLE_LINE_FILES = TABLE_ASCII_LINE_FILES,
	TABLE_LINE_MODE  = TABLE_ASCII_LINE_MODE,
	TABLE_LINE_CELL  = TABLE_ASCII_LINE_CELL,
	TABLE_LINE_SEP   = TABLE_ASCII_LINE_SEP,
	TABLE_LINE_TITLE = TABLE_ASCII_LINE_TITLE,
	TABLE_LINE_DATE  = TABLE_ASCII_LINE_DATE
};

enum TableCellType
{
	TABLE_CELL_LEC    = TABLE_ASCII_CELL_LEC,
	TABLE_CELL_STRING = TABLE_ASCII_CELL_STRING
};

#define  KT_CHUNCK   5

/*----------------------- GLOBALS ----------------------------*/

inline int T_GRAPHDEFAULT = 0;

/*----------------------- STRUCTS ----------------------------*/

struct TCELL {
    char    *tc_val;    // NULL or
                        // char* if type == TABLE_CELL_STRING or
                        // packed IDT (i.e. char*) if type == TABLE_CELL_LEC
    char    tc_type;    // TABLE_CELL_STRING or TABLE_CELL_LEC
    char    tc_attr;    // TABLE_CELL_LEFT, TABLE_CELL_CENTER, TABLE_CELL_RIGHT, TABLE_CELL_BOLD, TABLE_CELL_ITALIC, TABLE_CELL_UNDERLINE, TABLE_CELL_NORMAL
    char    tc_pad[2];  // Padding for struct alignment
};

struct TLINE {
    char    *tl_val;    // if tl_type == TABLE_LINE_CELL  : tl_val is TCELL*
                        // if tl_type == TABLE_LINE_TITLE : tl_val is TCELL*
                        // if tl_type == TABLE_LINE  : tl_val is NULL
                        // if tl_type == TABLE_LINE_MODE  : tl_val is NULL
                        // if tl_type == TABLE_LINE_DATE  : tl_val is NULL
                        // if tl_type == TABLE_LINE_FILES : tl_val is NULL
    char    tl_type;    // TABLE_LINE_FILES, TABLE_LINE_MODE, TABLE_LINE_TITLE, TABLE_LINE or TABLE_LINE_CELL
    char    tl_graph;   // 0=Line, 1=scatter, 2=bar (non implemented in all IODE flavours)
    U_ch    tl_axis:1;  // 0 if values are relative to the left axis, 1 to the right axis
    U_ch    tl_pbyte:7; // available free space
    char    tl_pad[1];  // Padding for struct alignment
};

struct TBL {
    short   t_lang;     // Output language : TABLE_ENGLISH, TABLE_FRENCH, TABLE_DUTCH
    short   t_free;     // if 0, first column is frozen, otherwise, col 1 is repeated as other columns
    short   t_nc;       // Number of columns (of text and lec, not calculated values)
    short   t_nl;       // Number of lines
    TLINE   t_div;      // t_nc TCELL's, each TCELL contains a divider
    TLINE   *t_line;    // t_nl TLINE's of t_nc TCELL's
    float   t_zmin;     // Min on the right axis
    float   t_zmax;     // Max on the right axis
    float   t_ymin;     // Min on left axis
    float   t_ymax;     // Max on left axis
    char    t_attr;     // Combination (logical &) of attributes: TABLE_CELL_BOLD, TABLE_CELL_ITALIC, TABLE_CELL_UNDERLINE, TABLE_CELL_CENTER,
                        // TABLE_CELL_DECIMAL, TABLE_CELL_LEFT and TABLE_CELL_RIGHT
    char    t_box;      // 1 to surround the chart by a box
    char    t_shadow;   // 1 to place a shadow behind the chart
    char    t_gridx;    // 0 = major grids, 1 = no grids, 2 = minor + major grids
    char    t_gridy;    // idem
    char    t_axis;     // 0=normal axis, 1=log, 2=semi-log, 3=percents (TODO: to be tested)
    char    t_align;    // Text alignment: 0=left, 1=centered, 2 = right
    char    t_pad[13];  // Padding for struct alignment
};

/*----------------------- FUNCS ----------------------------*/

TBL* K_tptr(KDB* kdb, char* name);

/* k_tbl.c */
TBL *T_create(int );
void T_free(TBL *);
void T_free_line(TLINE *,int );
void T_free_cell(TCELL *);
int T_add_line(TBL *);
TCELL *T_create_cell(TBL *,TLINE *);
TCELL *T_create_title(TBL *,TLINE *);
char *T_cell_cont(TCELL *,int );
char *T_cell_cont_tbl(TBL *,int, int, int );
char *T_div_cont_tbl(TBL *, int, int );
int T_insert_line(TBL *,int ,int ,int );
int T_set_lec_cell(TCELL *,unsigned char *);
int T_set_lec_cell_tbl(TBL *, int, int, unsigned char *);
void T_set_string_cell(TCELL *,unsigned char *);
void T_set_string_cell_tbl(TBL *, int, int, unsigned char *);
void T_set_cell_attr(TBL *,int ,int ,int );
int T_default(TBL *,char *,char **,char **,int ,int ,int );
void T_auto(TBL *,char *,char **,int ,int ,int );

/*----------------------- MACROS ----------------------------*/

#define T_NC(tbl)           (tbl->t_nc)
#define T_NL(tbl)           (tbl->t_nl)
#define T_LANG(tbl)         (tbl->t_lang)
#define T_L(tbl)            (tbl->t_line)
#define T_C(tbl, i, j)      ((tbl->t_line[i]).tl_val[j])

#define TABLE_CELL_ALIGN(attr, align)   (((attr) & 7) | (align))
#define TABLE_CELL_FONT(attr, font)     ((attr) | (font))
#define TABLE_CELL_SETFONT(attr, font)  (((attr) & 120) | (font))

#define KTVAL(kdb, pos)     (K_tunpack(SW_getptr(kdb->k_objs[pos].o_val)) )
#define KTPTR(name)         K_tptr(KT_WS, name)      // returns an allocated TBL
