/**
 *  
 */

/*----------------------- WS ----------------------------*/

#define KC_WS   K_WS[K_CMT]
#define KE_WS   K_WS[K_EQS]
#define KI_WS   K_WS[K_IDT]
#define KL_WS   K_WS[K_LST]
#define KS_WS   K_WS[K_SCL]
#define KT_WS   K_WS[K_TBL]
#define KV_WS   K_WS[K_VAR]

#define KC_RWS   K_RWS[K_CMT][K_PWS[K_CMT]]
#define KE_RWS   K_RWS[K_EQS][K_PWS[K_EQS]]
#define KI_RWS   K_RWS[K_IDT][K_PWS[K_IDT]]
#define KL_RWS   K_RWS[K_LST][K_PWS[K_LST]]
#define KS_RWS   K_RWS[K_SCL][K_PWS[K_SCL]]
#define KT_RWS   K_RWS[K_TBL][K_PWS[K_TBL]]
#define KV_RWS   K_RWS[K_VAR][K_PWS[K_VAR]]


#define KARCH(kdb)   ((kdb)->k_arch)
#define KMAGIC(kdb)  ((kdb)->k_magic)
#define KTYPE(kdb)   ((kdb)->k_type)
#define KMODE(kdb)   ((kdb)->k_mode)
#define KNAMEPTR(kdb)((kdb)->k_nameptr) 
#define KDESC(kdb)   ((kdb)->k_desc)
#define KDATA(kdb)   ((kdb)->k_data)
#define KFREE(kdb)   ((kdb)->k_free)
#define KNB(kdb)     ((kdb)->k_nb)
#define KOBJS(kdb)   ((kdb)->k_objs)

/******************************* TYPEDEFS **********************************/
typedef char    OFNAME[OK_MAX_FILE];
typedef char    FNAME[K_MAX_FILE];
typedef char    OONAME[OK_MAX_NAME + 1];
typedef char    ONAME[K_MAX_NAME + 1];
typedef long    OSIZE;   /* JMP 23-05-00 */

typedef struct  _okobj_ {
    short       o_val;
    OONAME      o_name;
    char        o_pad[3];
} OKOBJ;

typedef struct  _kobj_ {
    SWHDL       o_val;      /* SWHDL=long */ /* IODE64K */
    ONAME       o_name;
    char        o_pad[3];
} KOBJ;

typedef struct _okdb_ {
    KOBJ* k_objs;
    long        k_nb;
    short       k_type;
    short       k_mode;
    char        k_arch[LMAGIC];
    char        k_magic[LMAGIC];
    OFNAME       k_name;
    char        k_desc[K_MAX_DESC];
    char        k_data[K_MAX_DESC];     /* Client Data */
} OKDB;

typedef struct _okdb643_ {
    KOBJ* k_objs;
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
} OKDB643;

typedef struct _kdb_ {
    KOBJ* k_objs;
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
    char* k_nameptr;             /* 6.44 */
} KDB;

typedef struct _period {
    long    p_y; /* PERIOD LONG */
    long    p_s; /* PERIOD LONG */
    char    p_p;
    char    p_pad[3];
} PERIOD;

typedef struct _sample {
    PERIOD  s_p1,
        s_p2;
    short   s_nb;
    char    s_pad[2];
} SAMPLE;

typedef struct _olname_ {
    OONAME   name;
    char    pad[3];
    short   pos;
} OLNAME;

typedef struct _lname_ {
    ONAME   name;
    char    pad[3];
    long    pos; /* SWHDL */ /* IODE64K */
} LNAME;

typedef struct _oclec_ {
    short   tot_lg,         /* SWHDL */
        exec_lg,
        nb_names;
    char    dupendo;
    char    pad;
    OLNAME  lnames[1];
} OCLEC;

typedef struct _clec_ {
    long    tot_lg,        /* JMP 20-05-00 */ /* IODE64K */
        exec_lg;       /* JMP 20-05-00 */ /* IODE64K */
    short   nb_names;
    char    dupendo;
    char    pad;
    LNAME   lnames[1];
} CLEC;

typedef IODE_REAL* VAR;

typedef struct _scl_ {
    IODE_REAL    val;
    IODE_REAL    relax;
    IODE_REAL    std;
} SCL;

typedef char* CMT;

typedef char* LIS;

typedef struct _idt_ {
    char* lec;
    CLEC* clec;
} IDT;

#define EQS_NBTESTS     20

typedef struct _eq_ {
    char* lec;
    CLEC* clec;
    char    solved;
    char    method;
    SAMPLE  smpl;
    char* cmt;
    char* blk;
    char* instr;
    long    date;
    float   tests[EQS_NBTESTS]; /* FLOAT 12-04-98 */
} EQ;

/* Table definition */
typedef struct _tcell_ {
    char* tc_val;
    char    tc_type;    /* TEXT, LEC */
    char    tc_attr;    /* LEFT, CENTER, RIGHT, BOLD, ITALIC, UNDERLINE, NORMAL */
    char    tc_pad[2];
} TCELL;

typedef struct _tline_ {
    char* tl_val;        /* if tl_type = title then val = ptr unsigned char else ptr to TCELL */
    char    tl_type;        /* FILES, MODE, TITLE, LINE, CELL */
    char    tl_graph;       /* G_LINE = 0, .... */
    U_ch    tl_axis : 1;      /* was tl_attr, Unused before? */
    U_ch    tl_pbyte : 7;     /* was tl_attr, Unused before? */
    char    tl_pad[1];
} TLINE;

typedef struct _tbl_ {
    short   t_lang;
    short   t_free;     /* = 0, first column is frozen */
    short   t_nc;
    short   t_nl;
    TLINE   t_div;     /* t_nc CELLS, each CELL contains a divider, attr */
    TLINE* t_line;   /* t_nl TLINE of t_nc CELLS */
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
} TBL;

/******************************* X64  : TYPEDEFS  particuliers 32 bits pour import/export  **********************************/
#define X64 (sizeof(char*) == 8)
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

/* Table definition */
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
    U_ch    tl_axis : 1;      /* was tl_attr, Unused before? */
    U_ch    tl_pbyte : 7;     /* was tl_attr, Unused before? */
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
