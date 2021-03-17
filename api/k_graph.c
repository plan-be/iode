#include "iode.h"

int      KT_nb, KT_attr;
extern int B_viewmode;

/* Graph tables */
int T_GraphTest(tbl)
TBL     *tbl;
{
    int     dest;
    char    gsmpl[20];
    SAMPLE  *smpl = (SAMPLE *) KDATA(KV_WS);

    W_InitDisplay();
    sprintf(gsmpl, "%s:%d",
	    PER_pertoa(&(smpl->s_p1), NULL), smpl->s_nb); /* JMP 28-11-93 */
    KT_nb = 1;
    if(T_graph_tbl_1(tbl, gsmpl)) {
	B_seterrn(72);
	return(-1);
    }

    W_EndDisplay(T_get_title(tbl), -1, -1, -1, -1);
    return(0);
}

//static char *T_GRIDS[] = {"Major", "No", "Minor"}; /* JMP 04-05-01 */
static char *T_GRIDS[] = {"MM", "NN", "mm"}; /* JMP 21-04-2004 */

T_GraphInit(w, h, xgrid, ygrid, ymin, ymax, zmin, zmax, align, box, brush)
double  w, h, ymin, ymax, zmin, zmax;
int     align, box, brush;
{
    W_printf(".gb %.2lf %.2lf\n", w, h);
    W_printf(".ggrid %c%s\n", "TNt"[xgrid], T_GRIDS[ygrid]); /* JMP 26-11-2003 */

    W_printf(".galign %c\n", "LMR"[align]);
    W_printf(".gbox %d %c %d\n", box, A2M_BACKCOLOR, brush);


    if(L_ISAN(ymin)) W_printf(".gyMin %lg\n", ymin);
    if(L_ISAN(ymax)) W_printf(".gyMax %lg\n", ymax);
    if(L_ISAN(zmin)) W_printf(".gzMin %lg\n", zmin);
    if(L_ISAN(zmax)) W_printf(".gzMax %lg\n", zmax);
    return(0);
}

T_GraphEnd()
{
    W_printf(".ge\n");
    W_flush();

    return(0);
}

int T_graph_tbl_1(tbl, gsmpl)
TBL     *tbl;
char    *gsmpl;
{
    int     i, j, dim, begin = 1, fop,
	    w, t, f, c, l;
    char    **files;
    COLS    *cls, *fcls;
    TLINE   *line;
    SAMPLE  smpl;
    IODE_REAL    step,
	    *x, *y;
    TCELL   *cell;
    extern char *KLG_OPERS_TEXTS[][3];

    KT_attr = 4;
    if(tbl->t_nc != 2) {
	B_seterrn(71);
	return(-1);
    }

    dim = T_prep_cls(tbl, gsmpl, &cls);
    if(dim < 0) return(-1);

    T_prep_smpl(cls, &fcls, &smpl);
    files = T_find_files(cls);
    if(files == 0) return(-1); /* JMP 11-06-99 */

    x = (IODE_REAL *) SW_nalloc(sizeof(IODE_REAL) * smpl.s_nb);
    step = 1.0 / PER_nbper(&(smpl.s_p1));
	x[0] = PER_per2real(&(smpl.s_p1), 0);
	if(step < 1) x[0] += step/2; /* GB 23-11-2010, center Q en M  */
    for(i = 1; i < smpl.s_nb; i++) x[i] = x[i - 1] + step;

    y = (IODE_REAL *) SW_nalloc(sizeof(IODE_REAL) * smpl.s_nb);

    if(B_viewmode != 0) B_PrintRtfTopic(T_get_title(tbl)); /* JMP 06-01-02 */

    w = T_GraphInit(A2M_GWIDTH, A2M_GHEIGHT,  /* JMP 19-02-98 */
	    tbl->t_gridx, tbl->t_gridy,
	    (double)tbl->t_ymin, (double)tbl->t_ymax,
	    (double)tbl->t_zmin, (double)tbl->t_zmax,
	    tbl->t_align, tbl->t_box, 50 * tbl->t_shadow); /* JMP 23-02-98 */

   for(i = 0; i < T_NL(tbl) && w >= 0; i++) {
	line = T_L(tbl) + i;
	cell = (TCELL *) line->tl_val;

	switch(line->tl_type) {
	case KT_CELL  :
	    if(cell[1].tc_type != KT_LEC) break;
	    begin = 0;
	    if(T_GraphLine(tbl, i, cls, &smpl, x, y, /*c, t,*/ fcls)) w = -1;
	    break;

	case KT_TITLE :
	    T_GraphTitle(T_cell_cont(cell, 0));
	    break;

	case KT_FILES :
	/*
	    for(j = 0 ; files[j] ; j++) T_add_title(f, files[j], 'L');*/

	default :
		break;
	}
    }
    T_GraphEnd();

    SCR_free_tbl(files);
    SW_nfree(x);
    SW_nfree(y);
    COL_free(cls);
    COL_free(fcls);
    return(w);
}

T_GraphTitle(txt)
char    *txt;
{
    W_printf(".gtitle %s\n", txt);
    return(0);
}

T_GraphLineTitle(line, fcls, i) /* GB 08-97 */
TLINE   *line;
COLS    *fcls;
int     i;
{
    char    *fileop = NULL;
    COL     *cl = fcls->cl_cols + i;
    TCELL   *cell = (TCELL *) line->tl_val;

    if(fcls->cl_nb > 1 || cl->cl_opf != COL_NOP) fileop = COL_ctoa(cl, 'f', 0, 2);
    T_GraphLegend(line->tl_axis, "LLBL"[line->tl_graph], T_cell_cont(cell, 0), fileop);
    return(0);
}

T_GraphLegend(axis, type, txt, fileop)
char    axis, type, *txt, *fileop;
{
    if(axis == 0) W_printf(".gty %c \"%s", type, txt);
    else W_printf(".gtz %c \"%s", type, txt);

    if(fileop != NULL && fileop[0] != 0) W_printf(" %s", fileop);

    W_printf("\" "); /* GB 25/11/98 */
    return(0);
}

T_GraphXYLegend(axis, type, txt, fileop)
char    axis, type, *txt, *fileop;
{
    if(axis == 0) W_printf(".gxy %c \"%s", type, txt);
    else W_printf(".gxz %c \"%s", type, txt);

    if(fileop != NULL && fileop[0] != 0) W_printf(" %s", fileop);

    W_printf("\" "); /* GB 25/11/98 */
    return(0);
}

T_GraphLineData(nb, x, y)
int     nb; IODE_REAL    *x, *y;
{
    int     i = 0;
    char    buf[21];

    for(i = 0; i < nb; i++) {
	T_fmt_val(buf, x[i], 16, -1);
	W_printf(" %s ", buf);
	T_fmt_val(buf, y[i], 16, -1);
	W_printf(" %s ", buf);
    }
    W_printf("\n");
    return(0);
}

T_GraphTimeData(smpl, y)
SAMPLE  *smpl; IODE_REAL    *y;
{
    int     i = 0;
    char    buf[21];

    PER_pertoa(&(smpl->s_p1), buf);
    W_printf("%s ", buf);

    for(i = 0; i < smpl->s_nb; i++) {
	T_fmt_val(buf, y[i], 16, -1);
	W_printf(" %s ", buf);
    }
    W_printf("\n");
    return(0);
}

T_GraphXYData(nb, x, y)
int     nb; IODE_REAL    *x, *y;
{
    int     i = 0;
    char    bufx[21], bufy[21];

    for(i = 0; i < nb; i++) {
	T_fmt_val(bufx, x[i], 16, -1);
	T_fmt_val(bufy, y[i], 16, -1);
	W_printf(" %s %s ", bufx, bufy);
    }
    W_printf("\n");
    return(0);
}

T_GraphLine(tbl, i, cls, smpl, x, y, /*c, t,*/ fcls) /* GB 08-97 */
TBL     *tbl;
int     i; /* c, t*/
COLS    *cls, *fcls;
SAMPLE  *smpl; IODE_REAL    *x, *y;
{
    int     j, dt, k, hy;
    TLINE   *line = T_L(tbl) + i;
    COL     *cl;
    char    buf[120];

    COL_clear(cls);
    if(COL_exec(tbl, i, cls) < 0) return(-1);

    for(k = 0 ; k < fcls->cl_nb ; k++) {
	T_GraphLineTitle(line, fcls, k);

	for(j = 0 ; j < smpl->s_nb ; j++) y[j] = L_NAN;
	for(j = 1; j < cls->cl_nb; j += 2) {
	    cl = cls->cl_cols + j;
	    if(T_find_opf(fcls, cl) != k) continue;
	    dt = PER_diff_per(cl->cl_per, &(smpl->s_p1));
	    y[dt] = cl->cl_res;
	}

	T_GraphTimeData(smpl, y);

    }
    return(0);
}

T_prep_smpl(cls, fcls, smpl)
COLS    *cls, **fcls;
SAMPLE  *smpl;
{
    int     i, pos;
    COL     *cl;

    *fcls = 0;
    cl = cls->cl_cols;
    memcpy(&(smpl->s_p1), cl->cl_per, sizeof(PERIOD));
    memcpy(&(smpl->s_p2), cl->cl_per, sizeof(PERIOD));

    for(i = 0; i < cls->cl_nb; i++) {
	cl = cls->cl_cols + i;
	pos = T_find_opf(*fcls, cl);
	if(pos < 0) {
	    *fcls = COL_add(*fcls, 1, sizeof(COL));
	    memcpy((*fcls)->cl_cols + (*fcls)->cl_nb - 1, cl, sizeof(COL));
	    }

	if(PER_diff_per(cl->cl_per, &(smpl->s_p1)) < 0)
	    memcpy(&(smpl->s_p1), cl->cl_per, sizeof(PERIOD));
	if(PER_diff_per(cl->cl_per, &(smpl->s_p2)) > 0)
	    memcpy(&(smpl->s_p2), cl->cl_per, sizeof(PERIOD));
	}
    smpl->s_nb = 1 + PER_diff_per(&(smpl->s_p2), &(smpl->s_p1));
    return(0);
}

T_find_opf(fcls, cl)
COLS    *fcls;
COL     *cl;
{
    int     i;
    COL     *fcl;

    if(fcls == 0) return(-1);
    for(i = 0; i < fcls->cl_nb; i++) {
	fcl = fcls->cl_cols + i;
	if(fcl->cl_opf    == cl->cl_opf &&
	   fcl->cl_fnb[0] == cl->cl_fnb[0] &&
	   fcl->cl_fnb[1] == cl->cl_fnb[1])
		return(i);
	}
    return(-1);
}



/* API CHART */
typedef struct _apichrt {
    char    **ChrtTitle;
    char    *ChrtType;
    int     *ChrtAxis;
    IODE_REAL    **ChrtData;
    int     ChrtI;
    int     ChrtNb;
} APICHRT;
APICHRT **API_CHARTS = 0;
int     API_NBCHARTS = 0;
APIGraphLine(hdl, tbl, i, cls, smpl, x, y, fcls)
TBL     *tbl;
int     i, hdl;
COLS    *cls, *fcls;
SAMPLE  *smpl; IODE_REAL    *x, *y;
{
    int     j, dt, k, hy;
    TLINE   *line = T_L(tbl) + i;
    COL     *cl;
    char    buf[120];
    APICHRT    *Chrt = API_CHARTS[hdl];
    COL_clear(cls);
    if(COL_exec(tbl, i, cls) < 0) return(-1);
    for(k = 0 ; k < fcls->cl_nb ; k++) {
	APIGraphLineTitle(hdl, line, fcls, k);
	for(j = 0 ; j < smpl->s_nb ; j++) y[j] = L_NAN;
	for(j = 1; j < cls->cl_nb; j += 2) {
	    cl = cls->cl_cols + j;
	    if(T_find_opf(fcls, cl) != k) continue;
	    dt = PER_diff_per(cl->cl_per, &(smpl->s_p1));
	    y[dt] = cl->cl_res;
	}
	APIGraphTimeData(hdl, smpl, y);
	Chrt->ChrtI++;
    }
    return(0);
}
APIGraphTimeData(hdl, smpl, y)
int     hdl;
SAMPLE  *smpl; IODE_REAL    *y;
{
    int         nb = smpl->s_nb;
    APICHRT    *Chrt = API_CHARTS[hdl];
    Chrt->ChrtData[Chrt->ChrtI] = SW_nalloc(sizeof(IODE_REAL) * nb);
    memcpy(Chrt->ChrtData[Chrt->ChrtI], y, sizeof(IODE_REAL) * nb);
    return(0);
}
APIGraphTitle(hdl, txt, x, nb)
int     hdl;
char    *txt; IODE_REAL    *x;
int     nb;
{
    int         i = 0;
    char        buf[21];
    APICHRT    *Chrt = API_CHARTS[hdl];
    Chrt->ChrtType[Chrt->ChrtI] = 'T';
    Chrt->ChrtTitle[Chrt->ChrtI] = SCR_stracpy(txt);
    SCR_OemToAnsi(Chrt->ChrtTitle[Chrt->ChrtI],
	Chrt->ChrtTitle[Chrt->ChrtI]);
    Chrt->ChrtData[Chrt->ChrtI] = SW_nalloc(sizeof(IODE_REAL) * nb);
    memcpy(Chrt->ChrtData[Chrt->ChrtI], x, sizeof(IODE_REAL) * nb);
    Chrt->ChrtI++;
    return(0);
}
APIGraphLegendTitle(hdl, axis, type, txt, fileop)
int     hdl, axis, type;
char    *txt, *fileop;
{
    APICHRT    *Chrt = API_CHARTS[hdl];
    Chrt->ChrtType[Chrt->ChrtI] = type;
    Chrt->ChrtAxis[Chrt->ChrtI] = axis;
    Chrt->ChrtTitle[Chrt->ChrtI] = SCR_stracpy(txt);
    if(fileop != NULL && fileop[0] != 0)
	Chrt->ChrtTitle[Chrt->ChrtI] = SCR_strafcat(Chrt->ChrtTitle[Chrt->ChrtI], fileop);
    SCR_OemToAnsi(Chrt->ChrtTitle[Chrt->ChrtI],
	Chrt->ChrtTitle[Chrt->ChrtI]);
    return(0);
}
APIGraphLineTitle(hdl, line, fcls, i) /* GB 08-97 */
int     hdl;
TLINE   *line;
COLS    *fcls;
int     i;
{
    char    *fileop = NULL;
    COL     *cl = fcls->cl_cols + i;
    TCELL   *cell = (TCELL *) line->tl_val;
    if(fcls->cl_nb > 1 || cl->cl_opf != COL_NOP)
	fileop = COL_ctoa(cl, 'f', 0, 2);
    APIGraphLegendTitle(hdl, line->tl_axis,
	"LLBL"[line->tl_graph], T_cell_cont(cell, 0), fileop);
    return(0);
}
APICHRT *APIChartInit(nl)
int nl;
{
    APICHRT *Chrt;
    Chrt = (APICHRT *) SW_nalloc(sizeof(APICHRT));
    Chrt->ChrtTitle = (char **) SW_nalloc(sizeof(char *) * nl);
    Chrt->ChrtType = (char *) SW_nalloc(sizeof(char) * nl);
    Chrt->ChrtAxis = (int *) SW_nalloc(sizeof(int) * nl);
    Chrt->ChrtData = (IODE_REAL **) SW_nalloc(sizeof(IODE_REAL *) * nl);
    Chrt->ChrtI = 0;
    Chrt->ChrtNb = 0;
    return(Chrt);
}
APIChartEnd(APICHRT *Chrt)
{
    int i;
    for(i = 0; Chrt->ChrtTitle[i]; i ++)
	SW_nfree(Chrt->ChrtTitle[i]);
    SW_nfree(Chrt->ChrtTitle);
    for(i = 0; Chrt->ChrtData[i]; i ++)
	SW_nfree(Chrt->ChrtData[i]);
    SW_nfree(Chrt->ChrtData);
    SW_nfree(Chrt->ChrtType);
    SW_nfree(Chrt->ChrtAxis);
    SW_nfree(Chrt);
}
int APIChartAlloc(int nl)
{
    int     i;
    for(i = 0; i < API_NBCHARTS; i++)
	if(API_CHARTS[i] == 0) break;
    if(i == API_NBCHARTS) {
	API_CHARTS = (APICHRT **) SCR_realloc(API_CHARTS, sizeof(APICHRT *), API_NBCHARTS, API_NBCHARTS + 10);
	API_NBCHARTS += 10;
    }
    API_CHARTS[i] = APIChartInit(nl);
    return(i);
}
APIChartFree(int hdl)
{
    if(API_NBCHARTS == 0) return(-1);
    if(API_CHARTS[hdl] == 0) return(-1);
    APIChartEnd(API_CHARTS[hdl]);
    API_CHARTS[hdl] = 0;
}
int APIChartNl(int hdl)
{
    APICHRT    *Chrt;
    if(hdl > API_NBCHARTS || API_CHARTS[hdl] == 0) return(0);
    Chrt = API_CHARTS[hdl];
    return(Chrt->ChrtI);
}
int APIChartNc(int hdl)
{
    APICHRT    *Chrt;
    if(hdl > API_NBCHARTS || API_CHARTS[hdl] == 0) return(0);
    Chrt = API_CHARTS[hdl];
    return(Chrt->ChrtNb);
}
char    APIChartType(int hdl, int i)
{
    APICHRT    *Chrt;
    if(hdl > API_NBCHARTS || API_CHARTS[hdl] == 0) return(0);
    Chrt = API_CHARTS[hdl];
    return(Chrt->ChrtType[i]);
}
int APIChartAxis(int hdl, int i)
{
    APICHRT    *Chrt;
    if(hdl > API_NBCHARTS || API_CHARTS[hdl] == 0) return(0);
    Chrt = API_CHARTS[hdl];
    return(Chrt->ChrtAxis[i]);
}
char    *APIChartTitle(int hdl, int i)
{
    APICHRT    *Chrt;
    if(hdl > API_NBCHARTS || API_CHARTS[hdl] == 0) return(0);
    Chrt = API_CHARTS[hdl];
    return(Chrt->ChrtTitle[i]);
}
double  *APIChartData(int hdl, int i)
{
    APICHRT    *Chrt;
    if(hdl > API_NBCHARTS || API_CHARTS[hdl] == 0) return(0);
    Chrt = API_CHARTS[hdl];
    return(Chrt->ChrtData[i]);
}
int APIPrepareChart(tbl, gsmpl)
TBL     *tbl;
char    *gsmpl;
{
    int     i, j, dim, begin = 1, fop,
	    w, t, f, c, l, hdl;
    char    **files;
    COLS    *cls, *fcls;
    TLINE   *line;
    SAMPLE  smpl;
    IODE_REAL    step,
	    *x, *y;
    TCELL   *cell;
    extern char *KLG_OPERS_TEXTS[][3];
    if(tbl->t_nc != 2) {
	B_seterrn(71);
	return(-1);
    }
    dim = T_prep_cls(tbl, gsmpl, &cls);
    if(dim < 0) return(-1);
    T_prep_smpl(cls, &fcls, &smpl);
    files = T_find_files(cls);
    if(files == 0) return(-1); /* JMP 11-06-99 */
    x = (IODE_REAL *) SW_nalloc(sizeof(IODE_REAL) * smpl.s_nb);
    step = 1.0 / PER_nbper(&(smpl.s_p1));
    x[0] = PER_per2real(&(smpl.s_p1), 0);
    for(i = 1; i < smpl.s_nb; i++) x[i] = x[i - 1] + step;
    y = (IODE_REAL *) SW_nalloc(sizeof(IODE_REAL) * smpl.s_nb);
    hdl = APIChartAlloc(T_NL(tbl));
    w = 1;
    for(i = 0; i < T_NL(tbl) && w > 0; i++) {
	line = T_L(tbl) + i;
	cell = (TCELL *) line->tl_val;
	switch(line->tl_type) {
	case KT_CELL  :
	    if(cell[1].tc_type != KT_LEC) break;
	    if(APIGraphLine(hdl, tbl, i, cls, &smpl, x, y, fcls)) w = -1;
	    break;
	case KT_TITLE :
	    APIGraphTitle(hdl, T_cell_cont(cell, 0), x, smpl.s_nb);
	    break;
	case KT_FILES :
	default :
	    break;
	}
    }
    API_CHARTS[hdl]->ChrtNb = smpl.s_nb;
    SCR_free_tbl(files);
    SW_nfree(x);
    SW_nfree(y);
    COL_free(cls);
    COL_free(fcls);
    return(hdl);
}



















































































