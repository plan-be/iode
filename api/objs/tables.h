#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"           // KDB
#include "api/objs/identities.h"    // Identity

#include <string>
#include <vector>

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

struct TCELL 
{
    std::string content;    // if type == TABLE_CELL_STRING
    Identity*   idt;        // if type == TABLE_CELL_LEC
    char        type;       // TABLE_CELL_STRING or TABLE_CELL_LEC
    char        attribute;  // TABLE_CELL_LEFT, TABLE_CELL_CENTER, TABLE_CELL_RIGHT, TABLE_CELL_BOLD,
                            // TABLE_CELL_ITALIC, TABLE_CELL_UNDERLINE, TABLE_CELL_NORMAL

public:
    TCELL() : content(""), idt(nullptr), type(TABLE_CELL_STRING), attribute(TABLE_CELL_NORMAL) {}

     ~TCELL()
    {
        content.clear();
        if(idt)
            delete idt;
        idt = nullptr;
    }

    bool is_null() const
    {
        if (type == TABLE_CELL_LEC && idt == NULL)
            return true;
        if (type == TABLE_CELL_STRING && content.empty())
            return true;
        return false;
    }
};

struct TLINE 
{
    char*   cells;          // if type == TABLE_LINE_CELL  : cells is TCELL*
                            // if type == TABLE_LINE_TITLE : cells is TCELL*
                            // if type == TABLE_LINE  : cells is NULL
                            // if type == TABLE_LINE_MODE  : cells is NULL
                            // if type == TABLE_LINE_DATE  : cells is NULL
                            // if type == TABLE_LINE_FILES : cells is NULL
    char    type;           // TABLE_LINE_FILES, TABLE_LINE_MODE, TABLE_LINE_TITLE, TABLE_LINE or TABLE_LINE_CELL
    char    graph_type;     // 0=Line, 1=scatter, 2=bar (non implemented in all IODE flavours)
    bool    right_axis;     // false if values are relative to the left axis, true to the right axis
};

struct TBL 
{
    short   language;           // Output language : TABLE_ENGLISH, TABLE_FRENCH, TABLE_DUTCH
    short   repeat_columns;     // if 0, first column is frozen, otherwise, col 1 is repeated as other columns
    short   nb_columns;         // Number of columns (of text and lec, not calculated values)
    short   nb_lines;           // Number of lines
    TLINE   divider_line;       // nb_columns TCELL's, each TCELL contains a divider
    TLINE*  lines;              // nb_lines TLINE's of nb_columns TCELL's
    float   z_min;              // Min on the right axis
    float   z_max;              // Max on the right axis
    float   y_min;              // Min on left axis
    float   y_max;              // Max on left axis
    char    attribute;          // Combination (logical &) of attributes: TABLE_CELL_BOLD, TABLE_CELL_ITALIC, TABLE_CELL_UNDERLINE, TABLE_CELL_CENTER,
                                // TABLE_CELL_DECIMAL, TABLE_CELL_LEFT and TABLE_CELL_RIGHT
    char    chart_box;          // 1 to surround the chart by a box
    char    chart_shadow;       // 1 to place a shadow behind the chart
    char    chart_gridx;        // 0 = major grids, 1 = no grids, 2 = minor + major grids
    char    chart_gridy;        // idem
    char    chart_axis_type;    // 0=normal axis, 1=log, 2=semi-log, 3=percents (TODO: to be tested)
    char    text_alignment;     // Text alignment: 0=left, 1=centered, 2 = right
};

/*----------------------- FUNCS ----------------------------*/

TBL* K_tptr(KDB* kdb, char* name);

/* k_tbl.c */
TBL *T_create(int );
void T_free(TBL *);
void T_free_line(TLINE *,int );
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

#define T_NC(tbl)           (tbl->nb_columns)
#define T_NL(tbl)           (tbl->nb_lines)
#define T_LANG(tbl)         (tbl->language)
#define T_L(tbl)            (tbl->lines)
#define T_C(tbl, i, j)      ((tbl->lines[i]).cells[j])

#define TABLE_CELL_ALIGN(attr, align)   (((attr) & 7) | (align))
#define TABLE_CELL_FONT(attr, font)     ((attr) | (font))
#define TABLE_CELL_SETFONT(attr, font)  (((attr) & 120) | (font))

#define KTVAL(kdb, pos)     (K_tunpack(SW_getptr(kdb->k_objs[pos].o_val)) )
#define KTPTR(name)         K_tptr(KT_WS, name)      // returns an allocated TBL
