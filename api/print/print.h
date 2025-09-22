#pragma once

#include "api/constants.h"
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/gsample/gsample.h"
#include "api/objs/tables.h"


//inline char     KT_sep = '&';          // Table cell separator => replaced by A2M_SEPCH
inline int      K_NBDEC = -1;          // Default nb of decimals

inline char     **KT_names = NULL;     // Names of the files used in a GSample
inline int      KT_nbnames = 0;        // Number of names in KT_names
inline int      KT_mode[MAX_MODE];     // Modes used in a GSample
inline int      KT_CUR_TOPIC = 0;      // Used in A2M file generation
inline int      KT_CUR_LEVEL = 0;      // Used in A2M file generation

/*----------------------------- DEFINE -----------------------------*/

#define G_FSCALEMAX       2.0
#define G_NORM_WIDTH      1
#define G_THICK_WIDTH     3
#define G_NO              20
#define G_YES             21
#define G_HORIZ           200
#define G_VERT            201
#define G_F               1001
#define G_MAXOBS          1024

#define G_ABS             150
#define G_U               151
#define G_D               152

#define G_L               160
#define G_R               161
#define G_C               162

/*----------------------------- ENUMS -----------------------------*/
/*  COLOR LINE WIDTH FILL : warning: keep the defines values equal the
    enum values from graphics.h */

enum IodeGraphColor
{
    G_BLACK,
    G_BLUE,
    G_GREEN,
    G_CYAN,
    G_RED,
    G_MAGENTA,
    G_BROWN,
    G_LIGHTGRAY,
    G_DARKGRAY,
    G_LIGHTBLUE,
    G_LIGHTGREEN,
    G_LIGHTCYAN,
    G_LIGHTRED,
    G_LIGHTMAGENTA,
    G_YELLOW,
    G_WHITE
};

enum IodeGraphLineType
{
    G_SOLID_LINE,
    G_DOTTED_LINE,
    G_CENTER_LINE,
    G_DASHED_LINE
};

enum IodeGraphFillType
{
    G_EMPTY_FILL,
    G_SOLID_FILL,
    G_LINE_FILL,
    G_LTSLASH_FILL,
    G_SLASH_FILL,
    G_BKSLASH_FILL,
    G_LTBKSLASH_FILL,
    G_HATCH_FILL,
    G_XHATCH_FILL,
    G_INTERLEAVE_FILL,
    G_WIDE_DOT_FILL,
    G_CLOSE_DOT_FILL
};

enum IodeGraphElement
{
    G_WINDOW,
    G_TITLE,
    G_CHART,
    G_XYDATA,       /* JMP 05-07-92 */
    G_TIMEDATA,     /* JMP 05-07-92 */
    G_LABELDATA,    /* JMP 05-07-92 */
    G_XYCHART,      /* JMP 05-07-92 */
    G_TIMECHART,    /* JMP 05-07-92 */
    G_LABELCHART,   /* JMP 05-07-92 */
    G_DATA          /* HC 13/7/92 */
};

enum IodeGraphType
{
    G_XY = 10,
    G_TIME,
    G_PIE,
    G_LBAR
};

enum IodeGraphGrid
{
    G_MAJOR = 29,
    G_NONE,
    G_MINOR
};

enum IodeGraphAxis
{
    G_VALUES = 40,
    G_LOG = 41,
    G_SEMILOG = 42,
    G_PERCENT = 50
};

enum IodeGraphChartType
{
    G_LINE = 100,
    G_SCATTER,
    G_BAR,
    G_ScalarINE,
    G_SBAR,
    G_PBAR,
    G_FILL
};

#define G_SCATTER_SHAPE 250

enum IodeGraphScatterShape
{
    G_DOT = G_SCATTER_SHAPE,
    G_BOX,
    G_CROSS,
    G_X
};

enum IodeGraphError
{
    G_SYNTAX_ERR = -1,
    G_UNREC_ERR  = -2,
    G_DUP_ERR    = -3,
    G_MEM_ERR    = -4,
    G_BGI_ERR    = -5,
    G_OUT_ERR    = -6,
    G_DAT_ERR    = -7
};

/*----------------------------- FUNCS -----------------------------*/

/* k_graph.c */
/* New auto extraction 1/10/2021 */
int APIChartAlloc(int nl);
int APIChartAxis(int hdl, int i);
double *APIChartData(int hdl, int i);
int APIChartFree(int hdl);
int APIChartNc(int hdl);
int APIChartNl(int hdl);
char *APIChartTitle(int hdl, int i);
int APIChartType(int hdl, int i);
int APIGraphLegendTitle(int hdl, int axis, int type, char *txt, char *fileop);
int APIGraphLine(int hdl, TBL *tbl, int i, COLS *cls, Sample *smpl, double *x, double *y, COLS *fcls);
int APIGraphLineTitle(int hdl, TLINE *line, COLS *fcls, int i);
int APIGraphTimeData(int hdl, Sample *smpl, double *y);
int APIGraphTitle(int hdl, char *txt, double *x, int nb);
int APIPrepareChart(TBL *tbl, char *gsmpl);
int T_GraphEnd();
int T_GraphInit(double w, double h, int xgrid, int ygrid, double ymin, double ymax, double zmin, double zmax, int align, int box, int brush);
int T_GraphLegend(int axis, int type, char *txt, char *fileop);
int T_GraphLine(TBL *tbl, int i, COLS *cls, Sample *smpl, double *x, double *y, COLS *fcls);
// int T_GraphLineData(int nb, double *x, double *y);
// int T_GraphLineTitle(TLINE *line, COLS *fcls, int i);
int T_GraphTest(TBL *tbl);
int T_GraphTimeData(Sample *smpl, double *y);
int T_GraphTitle(char *txt);
int T_GraphXYData(int nb, double *x, double *y);
int T_GraphXYLegend(int axis, int type, char *txt, char *fileop);
int T_find_opf(COLS *fcls, COL *cl);
int T_graph_tbl_1(TBL *tbl, char *gsmpl, int mode);
int T_prep_smpl(COLS *cls, COLS **fcls, Sample *smpl);
int V_graph(int ,int ,int ,int ,int ,int ,double ,double ,Sample *,char **);

/* int.c */
int T_prep_cls(TBL *,char *,COLS **);
int T_print_tbl(TBL *,char *);
int T_print_line(TBL *,int ,COLS *);
void T_print_cell(TCELL *,COL *,int );
void T_fmt_val(char *,double ,int ,int );
void T_print_val(double );
void T_print_string(COL *,char *);
void T_open_cell(int ,int ,int );
void T_open_attr(int );
void T_close_attr(int );
char **T_find_files(COLS *);
void T_print_files(COLS *,int );
void T_print_mode(COLS *,int );
void T_print_date(int );
int T_begin_tbl(int ,COLS *);
void T_end_tbl(void);
unsigned char *T_get_title(TBL *);
