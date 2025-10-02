#pragma once

#include "api/constants.h"
#include "api/objs/equations.h"

/******************************* TYPEDEFS  particuliers 32 bits pour import/export  **********************************/

typedef long PTR32;         // pour garantir char  *

struct OKDB32 
{
    PTR32       k_objs;
    long        k_nb;
    short       k_type;
    short       k_mode;
    char        k_arch[LMAGIC];
    char        k_magic[LMAGIC];
    OFNAME      k_name;
    char        k_desc[K_MAX_DESC];
    char        k_data[K_MAX_DESC];     /* Client Data */
};

struct OKDB643_32 
{
    PTR32       k_objs;
    long        k_nb;
    short       k_type;
    short       k_mode;
    char        k_arch[LMAGIC];
    char        k_magic[LMAGIC];
    OFNAME      k_name;
    char        k_desc[K_MAX_DESC];
    char        k_data[K_MAX_DESC];     /* Client Data */
    char        k_compressed;           /* IODE64K */
    char        k_reserved[63];         /* IODE64K */
};

struct KDB32 
{
    PTR32       k_objs;
    long        k_nb;
    short       k_type;
    short       k_mode;
    char        k_arch[LMAGIC];
    char        k_magic[LMAGIC];
    //OFNAME       k_name; // 6.44
    OFNAME      k_oname;  // 6.44 (compat size but not used)
    char        k_desc[K_MAX_DESC];
    char        k_data[K_MAX_DESC];     /* Client Data */
    char        k_compressed;           /* IODE64K */
    char        k_reserved[59];         /* 6.44 : decreased by 4 bytes for filepath */
    PTR32       filepath;              /* 6.44 */
};

struct IDT32 
{
    PTR32     lec;
    PTR32     clec;
};

struct EQ32 
{
    PTR32   lec;
    PTR32   clec;
    char    solved;
    char    method;
    Sample  smpl;
    PTR32   cmt;
    PTR32   blk;
    PTR32   instr;
    long    date;
    float   tests[EQS_NBTESTS]; /* FLOAT 12-04-98 */
};

// Table definition internal format (for 32 and 64 bits)
struct TableCell32 
{
    PTR32   content;
    char    type;    /* TEXT, LEC */
    char    attribute;    /* LEFT, CENTER, RIGHT, BOLD, ITALIC, UNDERLINE, NORMAL */
    char    pad[2];
};

struct TableLine32 
{
    PTR32   cells;        /* if type = title then val = ptr unsigned char else ptr to TableCell */
    char    type;        /* FILES, MODE, TITLE, LINE, CELL */
    char    graph_type;       /* G_LINE = 0, .... */
    U_ch    right_axis:1;      /* was tl_attr, Unused before? */
    U_ch    unused:7;     /* was tl_attr, Unused before? */
    char    pad[1];
};

struct Table32 
{
    short   language;
    short   repeat_columns;     /* = 0, first column is frozen */
    short   nb_columns;
    short   nb_lines;
    TableLine32 divider_line;     /* nb_columns CELLS, each CELL contains a divider, attr */
    PTR32   lines;    /* nb_lines TableLine of nb_columns CELLS */
    float   z_min;    /* FLOAT 12-04-98 */
    float   z_max;    /* FLOAT 12-04-98 */
    float   y_min;    /* FLOAT 12-04-98 */
    float   y_max;    /* FLOAT 12-04-98 */
    char    attribute;    /* 0, ... 9 */
    char    chart_box;     /* 0, ... 9 */
    char    chart_shadow;  /* 0, ... 9 */
    char    chart_gridx;   /* G_NONE, G_MINOR, G_MAJOR */
    char    chart_gridy;   /* G_NONE, G_MINOR, G_MAJOR */
    char    chart_axis_type;    /* G_VALUES, G_LOG, G_SEMILOG, G_PERCENT */
    char    text_alignment;   /* G_LEFT, G_MIDDLE, G_RIGHT */
    char    pad[13];
};
