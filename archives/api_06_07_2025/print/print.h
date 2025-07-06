#pragma once

#include "api/constants.h"
#include "api/utils/time.h"
#include "api/gsample/gsample.h"
#include "api/objs/tables.h"

#ifdef __cplusplus
extern "C" {
#endif

extern  int     G_CUR_CHART;
extern  int     G_CUR_DATA;
extern  int     G_CUR_WINDOW;
extern  int     G_CUR_TITLE;

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

enum TableGraphGrid
{
	TABLE_GRAPH_MAJOR,
	TABLE_GRAPH_NONE,
	TABLE_GRAPH_MINOR
};

const static int IODE_NB_AXIS_THICKS = 3;

enum IodeGraphAxis
{
    G_VALUES = 40,
    G_LOG = 41,
    G_SEMILOG = 42,
    G_PERCENT = 50
};

enum TableGraphAxis
{
	TABLE_GRAPH_VALUES,
	TABLE_GRAPH_LOG,
	TABLE_GRAPH_SEMILOG,
	TABLE_GRAPH_PERCENT
};

enum IodeGraphChartType
{
    G_LINE = 100,
    G_SCATTER,
    G_BAR,
    G_SCLINE,
    G_SBAR,
    G_PBAR,
    G_FILL
};

enum TableGraphType
{
	TABLE_GRAPH_LINE,
	TABLE_GRAPH_SCATTER,
	TABLE_GRAPH_BAR
};

const static int IODE_NB_CHART_TYPES = 3;

enum TableGraphAlign
{
	TABLE_GRAPH_LEFT,
	TABLE_GRAPH_CENTER,
	TABLE_GRAPH_RIGHT
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
extern int APIChartAlloc(int nl);
extern int APIChartAxis(int hdl, int i);
extern double *APIChartData(int hdl, int i);
extern int APIChartFree(int hdl);
extern int APIChartNc(int hdl);
extern int APIChartNl(int hdl);
extern char *APIChartTitle(int hdl, int i);
extern int APIChartType(int hdl, int i);
extern int APIGraphLegendTitle(int hdl, int axis, int type, char *txt, char *fileop);
extern int APIGraphLine(int hdl, TBL *tbl, int i, COLS *cls, SAMPLE *smpl, double *x, double *y, COLS *fcls);
extern int APIGraphLineTitle(int hdl, TLINE *line, COLS *fcls, int i);
extern int APIGraphTimeData(int hdl, SAMPLE *smpl, double *y);
extern int APIGraphTitle(int hdl, char *txt, double *x, int nb);
extern int APIPrepareChart(TBL *tbl, char *gsmpl);
extern int T_GraphEnd();
extern int T_GraphInit(double w, double h, int xgrid, int ygrid, double ymin, double ymax, double zmin, double zmax, int align, int box, int brush);
extern int T_GraphLegend(int axis, int type, char *txt, char *fileop);
extern int T_GraphLine(TBL *tbl, int i, COLS *cls, SAMPLE *smpl, double *x, double *y, COLS *fcls);
//extern int T_GraphLineData(int nb, double *x, double *y);
//extern int T_GraphLineTitle(TLINE *line, COLS *fcls, int i);
extern int T_GraphTest(TBL *tbl);
extern int T_GraphTimeData(SAMPLE *smpl, double *y);
extern int T_GraphTitle(char *txt);
extern int T_GraphXYData(int nb, double *x, double *y);
extern int T_GraphXYLegend(int axis, int type, char *txt, char *fileop);
extern int T_find_opf(COLS *fcls, COL *cl);
extern int T_graph_tbl_1(TBL *tbl, char *gsmpl, int mode);
extern int T_prep_smpl(COLS *cls, COLS **fcls, SAMPLE *smpl);
extern int V_graph(int ,int ,int ,int ,int ,int ,double ,double ,SAMPLE *,char **);

/* k_print.c */
extern int K_NBDEC;
extern int KT_CUR_TOPIC;
extern int KT_CUR_LEVEL;
extern int T_prep_cls(TBL *,char *,COLS **);
extern int T_print_tbl(TBL *,char *);
extern int T_print_line(TBL *,int ,COLS *);
extern void T_print_cell(TCELL *,COL *,int );
extern void T_fmt_val(char *,double ,int ,int );
extern void T_print_val(double );
extern void T_print_string(COL *,char *);
extern void T_open_cell(int ,int ,int );
extern void T_open_attr(int );
extern void T_close_attr(int );
extern char **T_find_files(COLS *);
extern void T_print_files(COLS *,int );
extern void T_print_mode(COLS *,int );
extern void T_print_date(int );
extern int T_begin_tbl(int ,COLS *);
extern void T_end_tbl(void);
extern unsigned char *T_get_title(TBL *);

#ifdef __cplusplus
}
#endif
