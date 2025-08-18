/**
 *  @header4iode
 *  
 *  Functions to generate IODE graphs in A2M format.
 *  The graphs are based on 
 *      - TBL structures and a GSAMPLE definition, or
 *      - VAR list(s) or combination(s) or VARS. 
 *  
 *  Includes some A2M helper functions. 
 *  
 *  List of functions 
 *  -----------------
 *      int T_GraphInit(double w, double h, int xgrid, int ygrid, double ymin, double ymax, double zmin, double zmax, int align, int box, int brush)    Initialises a graph by sending a2m commands to W_printf().
 *      int T_GraphTest(TBL *tbl)                                               Displays the table tbl as a graph (in level) on the full sample of the current WS.
 *      int T_GraphEnd()                                                        Ends a A2M graph definition by sending the a2m command ".ge" to W_printf().
 *      int T_graph_tbl_1(TBL *tbl, char *gsmpl, int mode)                      Generates one graph in A2M format from a TBL struct and a GSAMPLE.
 *      int T_GraphTitle(char *txt)                                             Defines the graph title by sending a2m command ".gtitle" to W_printf().
 *      int T_GraphLegend(int axis, int type, char *txt, char *fileop)          Adds (in A2M) graph *time* axis (.gty or .gtz, see a2m language) with its position, type and title.
 *      int T_GraphXYLegend(int axis, int type, char *txt, char *fileop)        Adds (in A2M) graph *xy* axis with its position, type and title.
 *      int T_GraphTimeData(SAMPLE *smpl, double *y)                         Adds numerical data on a *time* graph line or bar.
 *      int T_GraphXYData(int nb, double *x, double *y)                   Adds numerical data on a *xy* graph line or bar.
 *      int T_GraphLine(TBL *tbl, int i, COLS *cls, SAMPLE *smpl, double *x, double *y, COLS *fcls)   Adds graph curves from a table line definition and a calculated GSAMPLE. 
 *      int T_find_opf(COLS *fcls, COL *cl)                                     Tries to find the position in *fcls of the opf (operation on files) in cl.
 *      int T_prep_smpl(COLS *cls, COLS **fcls, SAMPLE *smpl)                   Given a compiled GSAMPLE, constructs a new COLS struct with unique file ops and the minimum SAMPLE smpl containing all periods present in cls.
 *      int V_graph(int view, int mode, int type, int xgrid, int ygrid, int axis, double ymin, double ymax, SAMPLE* smpl, char** names)  Prints or displays graph(s) from variable list(s) or combination(s) or variables.
 */
#include "api/constants.h"
#include "api/b_errors.h"
#include "api/k_lang.h"
#include "api/objs/kdb.h"
#include "api/objs/objs.h"
#include "api/objs/variables.h"
#include "api/print/print.h"
#include "api/write/write.h"


/**
 *  Displays the table tbl as a graph (in level) on the full sample of the current WS.
 *  
 *  @param [in] TBL*    tbl table to print
 *  @return     int         0 on success, -1 on error 
 */
int T_GraphTest(TBL *tbl)
{
    char    gsmpl[20];
    SAMPLE  *smpl = (SAMPLE *) KDATA(KV_WS);

    W_InitDisplay();
    sprintf(gsmpl, "%s:%d",
            PER_pertoa(&(smpl->s_p1), NULL), smpl->s_nb); /* JMP 28-11-93 */
    //KT_nb = 1; // JMP 11/05/2022
    if(T_graph_tbl_1(tbl, gsmpl, 1)) { // JMP 11-05-2022 : mode = 1 => display 
        std::string err_msg = "Testing failed ...";
        error_manager.append_error(err_msg);
        return(-1);
    }

    W_EndDisplay((char*) T_get_title(tbl), -1, -1, -1, -1);
    return(0);
}

static char *T_GRIDS[] = {"MM", "NN", "mm"}; // Grids (Horiz/Vert) M=Major, m = minor, N = no grid 

/**
 *  Initialises a graph by sending a2m commands to W_printf().
 *  
 *  @param [in] double  w       width of the graph (in cm)
 *  @param [in] double  h       height ofg the graph (in cm)
 *  @param [in] int     xgrid   type of x grids (0-2). 0=T, 1=N, 2=t (see a2m for more infos)
 *  @param [in] int     ygrid   type of y grids (0-2). 0=MM, 1=NN, 2=mm (see a2m for more infos)
 *  @param [in] double  ymin    IODE_NAN or min value of the y axis
 *  @param [in] double  ymax    IODE_NAN or max value of the y axis
 *  @param [in] double  zmin    IODE_NAN or min value of the z axis
 *  @param [in] double  zmax    IODE_NAN or max value of the z axis
 *  @param [in] int     align   type of alignment of bars (0-2). 0=L, 1=M, 2=R (see a2m for more infos)
 *  @param [in] int     box     size of the box around the graph (0 for no box)
 *  @param [in] int     brush   id of the background brush (see a2m manual)
 *  @return     int             0 always
 */
int T_GraphInit(double w, double h, 
            int xgrid, int ygrid, 
            double ymin, double ymax, double zmin, 
            double zmax, int align, int box, int brush)
{
    W_printf(".gb %.2lf %.2lf\n", w, h);
    W_printf(".ggrid %c%s\n", "TNt"[xgrid], T_GRIDS[ygrid]); /* JMP 26-11-2003 */

    W_printf(".galign %c\n", "LMR"[align]);
    W_printf(".gbox %d %c %d\n", box, A2M_BACKCOLOR, brush);


    if(IODE_IS_A_NUMBER(ymin)) W_printf(".gyMin %lg\n", ymin);
    if(IODE_IS_A_NUMBER(ymax)) W_printf(".gyMax %lg\n", ymax);
    if(IODE_IS_A_NUMBER(zmin)) W_printf(".gzMin %lg\n", zmin);
    if(IODE_IS_A_NUMBER(zmax)) W_printf(".gzMax %lg\n", zmax);
    return(0);
}


/**
 *  Ends a A2M graph definition by sending the a2m command ".ge" to W_printf().
 *
 *  @return     int  0 always  
 */
int T_GraphEnd()
{
    W_printf(".ge\n");
    W_flush();

    return(0);
}


/**
 *  Generates one graph in A2M format from a TBL struct and a GSAMPLE.
 *  
 *  @param [in] TBL*    tbl     source TBL
 *  @param [in] char*   gsmpl   GSAMPLE definition
 *  @param [in] int     mode    0 for view mode, not null for print mode (generates A2M RTF topic)
 *  @return     int             0 on success, -1 on error (more than 2 cols in tbl or one of the ref files is not in K_RWS)
 */
int T_graph_tbl_1(TBL *tbl, char *gsmpl, int mode)
{
    int     i, dim, begin = 1, w;
    char    **files;
    COLS    *cls, *fcls;
    TLINE   *line;
    SAMPLE  smpl;
    double    step, *x, *y;
    TCELL   *cell;   

//    KT_attr = 4;
    if(tbl->t_nc != 2) {
        std::string err_msg = "Only dimension 2 tables can be graphed";
        error_manager.append_error(err_msg);
        return(-1);
    }

    dim = T_prep_cls(tbl, gsmpl, &cls);
    if(dim < 0) return(-1);

    T_prep_smpl(cls, &fcls, &smpl);
    files = T_find_files(cls);
    if(files == 0) return(-1); /* JMP 11-06-99 */

    x = (double *) SW_nalloc(sizeof(double) * smpl.s_nb);
    step = 1.0 / PER_nbper(&(smpl.s_p1));
    x[0] = PER_per2real(&(smpl.s_p1), 0);
    if(step < 1) x[0] += step/2; /* GB 23-11-2010, center Q en M  */
    for(i = 1; i < smpl.s_nb; i++) x[i] = x[i - 1] + step;

    y = (double *) SW_nalloc(sizeof(double) * smpl.s_nb);

    //if(B_viewmode != 0) B_PrintRtfTopic(T_get_title(tbl)); /* JMP 06-01-02 */
    //if(mode != 0) B_PrintRtfTopic(T_get_title(tbl)); // JMP 11-05-2022
    if(mode != 0) W_print_rtf_topic((char*) T_get_title(tbl)); // JMP 01-07-2022
    
    w = T_GraphInit(A2M_GWIDTH, A2M_GHEIGHT,  /* JMP 19-02-98 */
                    tbl->t_gridx, tbl->t_gridy,
                    (double)tbl->t_ymin, (double)tbl->t_ymax,
                    (double)tbl->t_zmin, (double)tbl->t_zmax,
                    tbl->t_align, tbl->t_box, 50 * tbl->t_shadow); /* JMP 23-02-98 */

    for(i = 0; i < T_NL(tbl) && w >= 0; i++) {
        line = T_L(tbl) + i;
        cell = (TCELL *) line->tl_val;

        switch(line->tl_type) {
            case TABLE_LINE_CELL  :
                if(cell[1].tc_type != TABLE_CELL_LEC) break;
                begin = 0;
                if(T_GraphLine(tbl, i, cls, &smpl, x, y, /*c, t,*/ fcls)) w = -1;
                break;

            case TABLE_LINE_TITLE :
                T_GraphTitle(T_cell_cont(cell, 0));
                break;

            case TABLE_LINE_FILES :
            /*
                for(j = 0 ; files[j] ; j++) T_add_title(f, files[j], 'L');*/

            default :
                break;
        }
    }
    T_GraphEnd();

    SCR_free_tbl((unsigned char**) files);
    SW_nfree(x);
    SW_nfree(y);
    COL_free_cols(cls);
    COL_free_cols(fcls);
    return(w);
}


/**
 *  Defines the graph title by sending a2m command ".gtitle" to W_printf().
 *
 *  @return     int  0 always  
 */
int T_GraphTitle(char *txt)
{
    W_printf(".gtitle %s\n", txt);
    return(0);
}


/**
 *  Adds (in A2M) graph *time* axis (.gty or .gtz, see a2m language) 
 *  with its position, type and title.
 *  
 *  @param [in] int     axis    0 for left (y) axis, 1 for right (z) axis
 *  @param [in] int     type    type of chart ('L' or 'B'): see a2m manual for possible choices)
 *  @param [in] char*   txt     axis title
 *  @param [in] char*   fileop  operation on files ("[1%2]" for ex.)
 *  @return     int             0 always
 */
 
int T_GraphLegend(int axis, int type, char *txt, char *fileop)
{
    if(axis == 0) W_printf(".gty %c \"%s", type, txt);
    else W_printf(".gtz %c \"%s", type, txt);

    if(fileop != NULL && fileop[0] != 0) W_printf(" %s", fileop);

    W_printf("\" "); /* GB 25/11/98 */
    return(0);
}


/**
 *  Adds (in A2M) the graph *time* axis corresponding to a TLINE and a specific COL.
 *  
 *  @param [in] TLINE* line     pointer to a TBL line
 *  @param [in] COLS*  fcls     compiled GSAMPLE (combination of files, op on files, periods, op on periods)
 *  @param [in] int    i        nb of the GSAMPLE column to use for generating the legend
 *  @return 
 */
static int T_GraphLineTitle(TLINE *line, COLS *fcls, int i) 
{
    char    *fileop = NULL;
    COL     *cl = fcls->cl_cols + i;
    TCELL   *cell = (TCELL *) line->tl_val;

    if(fcls->cl_nb > 1 || cl->cl_opf != COL_NOP) fileop = COL_ctoa(cl, 'f', 0, 2);
    T_GraphLegend(line->tl_axis, "LLBL"[line->tl_graph], T_cell_cont(cell, 0), fileop);
    return(0);
}


/**
 *  Adds (in A2M) graph *xy* axis with its position, type and title.
 *  
 *  @param [in] int     axis    0 for left (y) axis, 1 for right (z) axis
 *  @param [in] int     type    type of chart ('L' or 'B'): see a2m manual for possible choices)
 *  @param [in] char*   txt     axis title
 *  @param [in] char*   fileop  operation on files ("[1%2]" for ex.)
 *  @return     int             0 always
 */
int T_GraphXYLegend(int axis, int type, char *txt, char *fileop)
{
    if(axis == 0) W_printf(".gxy %c \"%s", type, txt);
    else W_printf(".gxz %c \"%s", type, txt);

    if(fileop != NULL && fileop[0] != 0) W_printf(" %s", fileop);

    W_printf("\" ");
    return(0);
}


/**
 *  Adds numerical data on a *time* graph line or bar.
 *  Example of result: "2000Y1 2.3 2.5 ..."
 *  
 *  @param [in] SAMPLE*     smpl    sample of the data in y
 *  @param [in] double*  y       y-values
 *  @return     int                 0 always
 */
int T_GraphTimeData(SAMPLE *smpl, double *y)
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

/**
 *  Adds numerical data on a *xy* graph line or bar.
 *  Example of result: "1.3 3000.2     1.7 2000.3     1.1 1500.0"
 *  
 *  @param [in] int         nb  number of observations in x and y
 *  @param [in] double*  x   x-values
 *  @param [in] double*  y   y-values
 *  @return     int             0 always
 */
int T_GraphXYData(int nb, double *x, double *y)
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


/**
 *  Adds graph curves from a table line definition and a calculated GSAMPLE. 
 *  The number of generated curves depends on the GSAMPLE definition. 
 *  For example, if the GSAMPLE is 
 *      2000Y1:20[1;2;1%2]
 *  3 curves will be generated: [1],[2] and [1%2].
 *  
 *  @param [in]  TBL*        tbl    Source table
 *  @param [in]  int         i      line number
 *  @param [in]  COLS*       cls    compiled GSAMPLE containing the calculated values 
 *                                  for each operation on files and periods
 *  @param [in]  SAMPLE*     smpl   SAMPLE of the data
 *  @param [in]  double*  x      unused  
 *  @param [out] double*  y      data of the 
 *  @param [in]  COLS*       fcls   
 *  @return 
 *  
 */
int T_GraphLine(TBL *tbl, int i, COLS *cls, SAMPLE *smpl, double *x, double *y, COLS *fcls)
{
    int     j, dt, k;
    TLINE   *line = T_L(tbl) + i;
    COL     *cl;

    COL_clear(cls);
    if(COL_exec(tbl, i, cls) < 0) return(-1);

    for(k = 0 ; k < fcls->cl_nb ; k++) {
        T_GraphLineTitle(line, fcls, k);

        for(j = 0 ; j < smpl->s_nb ; j++) y[j] = IODE_NAN;
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


/**
 *  Tries to find the position in *fcls of the opf (operation on files) in cl.
 *  
 *  @param [in] COLS*   fcls    already found opf's 
 *  @param [in] COL*    cl      opf to search
 *  @return     int             -1 of cl not yet present in fcls, pos if present    
 */
int T_find_opf(COLS *fcls, COL *cl)
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


/**
 *  Given a COLS struct (compiled GSAMPLE), constructs:
 *      - a new COLS struct containing unique files and operation on files
 *      - the minimum SAMPLE smpl containing all periods present in cls.
 *  
 *  @param [in]  COLS*   cls    compiled GSAMPLE
 *  @param [out] COLS**  fcls   unique files / op on files
 *  @param [out] SAMPLE* smpl   minimum SAMPLE enclosing cls periods
 *  @return 
 */
int T_prep_smpl(COLS *cls, COLS **fcls, SAMPLE *smpl)
{
    int     i, pos;
    COL     *cl;

    *fcls = 0;
    cl = cls->cl_cols;  // First col in cls
    memcpy(&(smpl->s_p1), cl->cl_per, sizeof(PERIOD));
    memcpy(&(smpl->s_p2), cl->cl_per, sizeof(PERIOD));

    for(i = 0; i < cls->cl_nb; i++) {
        cl = cls->cl_cols + i;
        pos = T_find_opf(*fcls, cl);
        if(pos < 0) {
            *fcls = COL_add_col(*fcls); // JMP 19/04/2022
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


// =====================================================================================================
// Generation of graphs based on a list of variables.
// -----------------------------------------------------------------------------------------------------

/**
 *  Sub function of V_graph_vars().
 *  
 *  @param [in] int     gnb         graphic number (unused)
 *  @param [in] int     type        type of chart (0, 2 or 3 for line chart, 1 for Bar chart) TODO: check this 
 *  @param [in] int     xgrid       type of x grids (0-2). 0=T, 1=N, 2=t (see a2m for more infos)
 *  @param [in] int     ygrid       type of y grids (0-2). 0=MM, 1=NN, 2=mm (see a2m for more infos)
 *  @param [in] int     axis        unused
 *  @param [in] double  ymin        IODE_NAN or min value of the y axis
 *  @param [in] double  ymax        IODE_NAN or max value of the y axis
 *  @param [in] SAMPLE* smpl        printing sample
 *  @param [in] int     dt          shift from the sample beginning of the first observation to keep for the graph
 *  @param [in] int     nt          nb of obs to print
 *  @param [in] KDB*    kdb         KDB containing the variables to graph
 *  @param [in] char**  names       List of variable names or groups of names separated by + or -.
 *                                  "A+B" means a graph with the 2 vars A and B
 *  @param [in] int     mode        transformation of the variables (KLG_E_LEVEL, KLG_E_DIFF, KLG_E_GRT,  KLG_E_DIFFY or KLG_E_GRTY) 
 *  @return     int                 0 on success, -1 on error    
 *  
 */
static int V_graph_vars_1(int gnb, int type, int xgrid, int ygrid, int axis, 
           double ymin, double ymax, SAMPLE* smpl, int dt, int nt, KDB* kdb, char* names, int mode)
{
    char    *buf, **vars;
    int     i, t, ng, var_nb, rc = 0;
    double    *y;

    vars = (char**) SCR_vtoms((unsigned char*) names, (unsigned char*) "+-");
    ng = SCR_tbl_size((unsigned char**) vars);
    if(ng == 0) {
        std::string err_msg = "No variable defined";
        error_manager.append_error(err_msg);
        return(-1);
    }

    y = (double *) SW_nalloc(sizeof(double) * nt);

    T_GraphInit(A2M_GWIDTH, A2M_GHEIGHT, xgrid, ygrid, ymin, ymax, IODE_NAN, IODE_NAN, 0, A2M_BOXWIDTH, A2M_BACKBRUSH);
    /* GB 10/08/98 */
    buf = SCR_malloc((int)strlen(names) + 20);
    //sprintf(buf, "%s (%s)", names, KLG_MODES[global_VM][0]);
    sprintf(buf, "%s (%s)", names, KLG_MODES[mode][0]);
    T_GraphTitle(buf);
    SCR_free(buf);

    for(i = 0 ; i < ng ; i++) {
        var_nb = K_find(kdb, vars[i]);
        if(var_nb < 0) {
            std::string err_msg = "DataDisplayGraph : '";
            err_msg += std::string(vars[i]);
            err_msg += "' undefined variable";
            error_manager.append_error(err_msg);
            rc = -1;
            goto fin;
        }

        for(t = 0; t < nt; t++) {
            //y[t] = (double ) KV_get(kdb, var_nb, dt + t, (int)global_VM);
            y[t] = (double ) KV_get(kdb, var_nb, dt + t, mode);
        }
        T_GraphLegend(0, "LLBL"[type], vars[i], NULL);
        /*        T_GraphLegend(0, "LBLL"[type], vars[i], NULL); */
        T_GraphTimeData(smpl, y);
    }

fin:
    T_GraphEnd();

    SW_nfree(y);
    SCR_free_tbl((unsigned char**) vars);
    return(rc);
}


/**
 *  Sub function of V_graph() but applied on any KDB of variables.
 *  
 *  @param [in] KDB*    kdb     KDB containing the variables in names
 *  @return     int             0 on success, -1 on error    
 *  
 *  See V_graph() for the other parameter definitions.
 */
 
static int V_graph_vars(int view, int type, int xgrid, int ygrid, int axis, double ymin, double ymax, SAMPLE* smpl, KDB* kdb, char** names, int mode)
{
    int     i, dt, nt, ng;

    ng = SCR_tbl_size((unsigned char**) names);
    if(ng == 0) {
        std::string err_msg = "DataDisplayGraph : empty variable list";
        error_manager.append_error(err_msg);
        return(-1);
    }

    nt = smpl->s_nb;
    dt = PER_diff_per(&(smpl->s_p1), &(KSMPL(kdb)->s_p1));
    if(dt < 0 || dt + nt > KSMPL(kdb)->s_nb) {
        std::string err_msg = "DataDisplayGraph : Sample out of the Variables sample boundaries";
        error_manager.append_error(err_msg);
        return(-1);
    }

    for(i = 0; i < ng; i++) {
        if(view) W_InitDisplay();
        V_graph_vars_1(i, type, xgrid, ygrid, axis, ymin, ymax, smpl, dt, nt, kdb, names[i], mode);
        if(view) W_EndDisplay(names[i], -ng, -i, -1, -1);
    }
    W_close();
    return(0); /* JMP38 01-10-92 */
}


/**
 *  Prints or displays graph(s) from variable list(s) or combination(s) or variables.
 *  
 *  Example
 *  -------
 *      varlist = (char**) SCR_vtoms((U_ch*)"ACAF,ACAG,ACAF+ACAG", ",;");
 *      smpl = PER_atosmpl("1990Y1", "2010Y1");
 *      rc = V_graph(0, 0, 0, 1, 1, 0, IODE_NAN, IODE_NAN, smpl, varlist);
 *  
 *  @param [in] int     view    0 for printing, 1 for displaying the graphs
 *  @param [in] int     mode    transformation of the variables (KLG_E_LEVEL, KLG_E_DIFF, KLG_E_GRT,  KLG_E_DIFFY or KLG_E_GRTY) 
 *  @param [in] int     type    type of graph (see a2m ?)
 *  @param [in] int     xgrid   type of x grids (0-2). 0=T, 1=N, 2=t (see a2m for more infos)
 *  @param [in] int     ygrid   type of y grids (0-2). 0=MM, 1=NN, 2=mm (see a2m for more infos)
 *  @param [in] int     axis    unused
 *  @param [in] double  ymin    IODE_NAN or min value of the y axis
 *  @param [in] double  ymax    IODE_NAN or max value of the y axis
 *  @param [in] SAMPLE* smpl    printing sample
 *  @param [in] char**  names   list of VARs to print
 *  @return     int             0 on success, -1 on error
 */
int V_graph(int view, int mode, int type, int xgrid, int ygrid, int axis, double ymin, double ymax, SAMPLE* smpl, char** names)
{
    int     rc;
    //int old_mode = global_VM, rc;

    //global_VM = mode;
    rc = V_graph_vars(view, type, xgrid, ygrid, axis, ymin, ymax, smpl, KV_WS, names, mode); // JMP 8/8//2022

    //global_VM = old_mode;
    return(rc);
}



// =============================================================================================================================
// API CHART: used in b_api.c but for what purpose ???
// TODO: answer this question...
// -----------------------------------------------------------------------------------------------------------------------------

// Struct declarations
struct APICHRT {
    char    **ChrtTitle;
    char    *ChrtType;
    int     *ChrtAxis;
    double    **ChrtData;
    int     ChrtI;
    int     ChrtNb;
};

// Global variables
APICHRT** API_CHARTS = NULL;
int       API_NBCHARTS = 0;

// Functions declarations 
int APIGraphLine(int hdl, TBL *tbl, int i, COLS *cls, SAMPLE *smpl, double *x, double *y, COLS *fcls);
int APIGraphTimeData(int hdl, SAMPLE *smpl, double *y);
int APIGraphTitle(int hdl, char *txt, double *x, int nb);
int APIGraphLegendTitle(int hdl, int axis, int type, char *txt, char *fileop);
int APIGraphLineTitle(int hdl, TLINE *line, COLS *fcls, int i);
APICHRT *APIChartInit(int nl);
int APIChartEnd(APICHRT *Chrt);
int APIChartAlloc(int nl);
int APIChartFree(int hdl);
int APIChartNl(int hdl);
int APIChartNc(int hdl);
int APIChartType(int hdl, int i);
int APIChartAxis(int hdl, int i);
char *APIChartTitle(int hdl, int i);
double *APIChartData(int hdl, int i);
int APIPrepareChart(TBL *tbl, char *gsmpl);


int APIGraphLine(int hdl, TBL *tbl, int i, COLS *cls, SAMPLE *smpl, double *x, double *y, COLS *fcls)
{
    int     j, dt, k;
    TLINE   *line = T_L(tbl) + i;
    COL     *cl;
    APICHRT    *Chrt = API_CHARTS[hdl];

    COL_clear(cls);
    if(COL_exec(tbl, i, cls) < 0) return(-1);
    for(k = 0 ; k < fcls->cl_nb ; k++) {
        APIGraphLineTitle(hdl, line, fcls, k);
        for(j = 0 ; j < smpl->s_nb ; j++) y[j] = IODE_NAN;
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


/**
 *  
 *  @param [in] int         hdl  
 *  @param [in] SAMPLE*     smpl 
 *  @param [in] double*  y    
 *  @return 
 */
int APIGraphTimeData(int hdl, SAMPLE *smpl, double *y)
{
    int         nb = smpl->s_nb;
    APICHRT    *Chrt = API_CHARTS[hdl];
    Chrt->ChrtData[Chrt->ChrtI] = (double *)SW_nalloc(sizeof(double) * nb);
    memcpy(Chrt->ChrtData[Chrt->ChrtI], y, sizeof(double) * nb);
    return(0);
}


/**
 *  
 *  @param [in] int         hdl 
 *  @param [in] char*       txt 
 *  @param [in] double*  x   
 *  @param [in] int         nb  
 *  @return 
 */
int APIGraphTitle(int hdl, char *txt, double *x, int nb)
{
    int         i = 0;

    APICHRT    *Chrt = API_CHARTS[hdl];
    Chrt->ChrtType[Chrt->ChrtI] = 'T';
    Chrt->ChrtTitle[Chrt->ChrtI] = (char*) SCR_stracpy((unsigned char*) txt);
    SCR_OemToAnsi((unsigned char*) Chrt->ChrtTitle[Chrt->ChrtI],
                  (unsigned char*) Chrt->ChrtTitle[Chrt->ChrtI]);
    Chrt->ChrtData[Chrt->ChrtI] = (double *) SW_nalloc(sizeof(double) * nb);
    memcpy(Chrt->ChrtData[Chrt->ChrtI], x, sizeof(double) * nb);
    Chrt->ChrtI++;
    return(0);
}


/**
 *  
 *  
 *  @param [in] int     hdl    
 *  @param [in] int     axis   
 *  @param [in] int     type   
 *  @param [in] char*   txt    
 *  @param [in] char*   fileop 
 *  @return 
 */
int APIGraphLegendTitle(int hdl, int axis, int type, char *txt, char *fileop)
{
    APICHRT    *Chrt = API_CHARTS[hdl];
    Chrt->ChrtType[Chrt->ChrtI] = type;
    Chrt->ChrtAxis[Chrt->ChrtI] = axis;
    Chrt->ChrtTitle[Chrt->ChrtI] = (char*) SCR_stracpy((unsigned char*) txt);
    if(fileop != NULL && fileop[0] != 0)
        Chrt->ChrtTitle[Chrt->ChrtI] = (char*) SCR_strafcat((unsigned char*) Chrt->ChrtTitle[Chrt->ChrtI], (unsigned char*) fileop);
    SCR_OemToAnsi((unsigned char*) Chrt->ChrtTitle[Chrt->ChrtI],
                  (unsigned char*) Chrt->ChrtTitle[Chrt->ChrtI]);
    return(0);
}


/**
 *  
 *  
 *  @param [in] int     hdl  
 *  @param [in] TLINE*  line 
 *  @param [in] COLS*   fcls 
 *  @param [in] int     i    
 *  @return 
 *  
 */
int APIGraphLineTitle(int hdl, TLINE *line, COLS *fcls, int i) 
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


/**
 *  
 *  
 *  @param [in] int nl 
 *  @return 
 *  
 */
APICHRT *APIChartInit(int nl)
{
    APICHRT *Chrt;
    Chrt = (APICHRT *) SW_nalloc(sizeof(APICHRT));
    Chrt->ChrtTitle = (char **) SW_nalloc(sizeof(char *) * nl);
    Chrt->ChrtType = (char *) SW_nalloc(sizeof(char) * nl);
    Chrt->ChrtAxis = (int *) SW_nalloc(sizeof(int) * nl);
    Chrt->ChrtData = (double **) SW_nalloc(sizeof(double *) * nl);
    Chrt->ChrtI = 0;
    Chrt->ChrtNb = 0;
    return(Chrt);
}


/**
 *  
 *  
 *  @param [in] APICHRT*    Chrt 
 *  @return 
 */
int APIChartEnd(APICHRT *Chrt)
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
    return(0);
    
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


int APIChartFree(int hdl)
{
    if(API_NBCHARTS == 0) return(-1);
    if(API_CHARTS[hdl] == 0) return(-1);
    APIChartEnd(API_CHARTS[hdl]);
    API_CHARTS[hdl] = 0;
    return(0);
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


int APIChartType(int hdl, int i)
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


char *APIChartTitle(int hdl, int i)
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


int APIPrepareChart(TBL *tbl, char *gsmpl)
{
    int     i, dim, begin = 1, w, hdl;
    char    **files;
    COLS    *cls, *fcls;
    TLINE   *line;
    SAMPLE  smpl;
    double    step, *x, *y;
    TCELL   *cell;

    if(tbl->t_nc != 2) {
        std::string err_msg = "Only dimension 2 tables can be graphed";
        error_manager.append_error(err_msg);
        return(-1);
    }
    dim = T_prep_cls(tbl, gsmpl, &cls);
    if(dim < 0) return(-1);
    T_prep_smpl(cls, &fcls, &smpl);
    files = T_find_files(cls);
    if(files == 0) return(-1); /* JMP 11-06-99 */
    x = (double *) SW_nalloc(sizeof(double) * smpl.s_nb);
    step = 1.0 / PER_nbper(&(smpl.s_p1));
    x[0] = PER_per2real(&(smpl.s_p1), 0);
    for(i = 1; i < smpl.s_nb; i++) x[i] = x[i - 1] + step;
    y = (double *) SW_nalloc(sizeof(double) * smpl.s_nb);
    hdl = APIChartAlloc(T_NL(tbl));
    w = 1;
    for(i = 0; i < T_NL(tbl) && w > 0; i++) {
        line = T_L(tbl) + i;
        cell = (TCELL *) line->tl_val;
        switch(line->tl_type) {
            case TABLE_LINE_CELL  :
                if(cell[1].tc_type != TABLE_CELL_LEC) break;
                if(APIGraphLine(hdl, tbl, i, cls, &smpl, x, y, fcls)) w = -1;
                break;
            case TABLE_LINE_TITLE :
                APIGraphTitle(hdl, T_cell_cont(cell, 0), x, smpl.s_nb);
                break;
            case TABLE_LINE_FILES :
            default :
                break;
        }
    }
    API_CHARTS[hdl]->ChrtNb = smpl.s_nb;
    SCR_free_tbl((unsigned char**) files);
    SW_nfree(x);
    SW_nfree(y);
    COL_free_cols(cls);
    COL_free_cols(fcls);
    return(hdl);
}

