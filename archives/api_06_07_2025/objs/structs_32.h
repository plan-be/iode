#pragma once

#include "api/constants.h"
#include "api/objs/equations.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************* TYPEDEFS  particuliers 32 bits pour import/export  **********************************/

typedef long PTR32; // pour garantir char  *

typedef struct _okdb32_ {
    PTR32       k_objs;
    long        k_nb;
    short       k_type;
    short       k_mode;
    char        k_arch[LMAGIC];
    char        k_magic[LMAGIC];
    OFNAME       k_name;
    char        k_desc[K_MAX_DESC];
    char        k_data[K_MAX_DESC];     /* Client Data */
} OKDB32;

typedef struct _okdb643_32_ {
    PTR32       k_objs;
    long        k_nb;
    short       k_type;
    short       k_mode;
    char        k_arch[LMAGIC];
	char        k_magic[LMAGIC];
    OFNAME       k_name;
	char        k_desc[K_MAX_DESC];
    char        k_data[K_MAX_DESC];     /* Client Data */
    char        k_compressed;           /* IODE64K */
    char        k_reserved[63];         /* IODE64K */
} OKDB643_32;

typedef struct _kdb32_ {
    PTR32       k_objs;
    long        k_nb;
    short       k_type;
    short       k_mode;
    char        k_arch[LMAGIC];
    char        k_magic[LMAGIC];
    //OFNAME       k_name; // 6.44
    OFNAME       k_oname;  // 6.44 (compat size but not used)
    char        k_desc[K_MAX_DESC];
    char        k_data[K_MAX_DESC];     /* Client Data */
    char        k_compressed;           /* IODE64K */
    char        k_reserved[59];         /* 6.44 : decreased by 4 bytes for k_nameptr */
    PTR32       k_nameptr;             /* 6.44 */
} KDB32;

typedef struct _idt32_ {
    PTR32     lec;
    PTR32     clec;
} IDT32;

typedef struct _eq32_ {
    PTR32   lec;
    PTR32   clec;
    char    solved;
    char    method;
    SAMPLE  smpl;
    PTR32   cmt;
    PTR32   blk;
    PTR32   instr;
	long    date;
    float   tests[EQS_NBTESTS]; /* FLOAT 12-04-98 */
} EQ32;

// Table definition internal format (for 32 and 64 bits)
typedef struct _tcell32_ {
    PTR32   tc_val;
    char    tc_type;    /* TEXT, LEC */
    char    tc_attr;    /* LEFT, CENTER, RIGHT, BOLD, ITALIC, UNDERLINE, NORMAL */
    char    tc_pad[2];
} TCELL32;

typedef struct _tline32_ {
    PTR32   tl_val;        /* if tl_type = title then val = ptr unsigned char else ptr to TCELL */
    char    tl_type;        /* FILES, MODE, TITLE, LINE, CELL */
    char    tl_graph;       /* G_LINE = 0, .... */
    U_ch    tl_axis:1;      /* was tl_attr, Unused before? */
    U_ch    tl_pbyte:7;     /* was tl_attr, Unused before? */
    char    tl_pad[1];
} TLINE32;

typedef struct _tbl32_ {
    short   t_lang;
    short   t_free;     /* = 0, first column is frozen */
    short   t_nc;
    short   t_nl;
    TLINE32 t_div;     /* t_nc CELLS, each CELL contains a divider, attr */
    PTR32   t_line;    /* t_nl TLINE of t_nc CELLS */
    float   t_zmin;    /* FLOAT 12-04-98 */
    float   t_zmax;    /* FLOAT 12-04-98 */
    float   t_ymin;    /* FLOAT 12-04-98 */
    float   t_ymax;    /* FLOAT 12-04-98 */
    char    t_attr;    /* 0, ... 9 */
    char    t_box;     /* 0, ... 9 */
    char    t_shadow;  /* 0, ... 9 */
    char    t_gridx;   /* G_NONE, G_MINOR, G_MAJOR */
    char    t_gridy;   /* G_NONE, G_MINOR, G_MAJOR */
    char    t_axis;    /* G_VALUES, G_LOG, G_SEMILOG, G_PERCENT */
	char    t_align;   /* G_LEFT, G_MIDDLE, G_RIGHT */
    char    t_pad[13];
} TBL32;

#ifdef __cplusplus
}
#endif
