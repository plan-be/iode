#include "iodedos.h"

o_calc_lhs(name, res, rhs)
char    *name, *res, *rhs;
{
    int     i, pos;
    char    *lec;

    memset(res, 0, 71);
    memset(rhs, 0, 71);
    pos = K_find(K_WS[K_EQS], name);
    if(pos < 0) return(-1);
    lec = KELEC(K_WS[K_EQS], pos);
    i = L_split_eq(lec);
    if(i < 0) return(-1);
    i = min(i, 70);
    memcpy(res, lec, i);
    SCR_strlcpy(rhs, lec + i + 2, 70);
    SCR_replace(rhs, "\n", " ");  /* JMP 07-10-98 */
    SCR_replace(res, "\n", " ");  /* JMP 07-10-98 */
    return(0);
}

o_estgr(titles, smpl, mlhs, mrhs, view, res)
char    **titles;
SAMPLE  *smpl;
MAT     *mlhs, *mrhs;
int     view, res;
{
    char    buf[256], lhs[80], rhs[80];
    int     i, t, nt, ng;
    double    *y;
    extern char *KLG_OBS[], *KLG_CALC[], *KLG_RES[];

    ng = M_NL(mlhs);
    nt = M_NC(mlhs);

    y = (double *)SW_nalloc(sizeof(double) * nt);

    for(i = 0; i < ng && res == 0; i ++) {
        if(view) W_InitDisplay();

        T_GraphInit(A2M_GWIDTH, A2M_GHEIGHT, 0, 0, L_NAN, L_NAN, L_NAN, L_NAN, 0, A2M_BOXWIDTH, A2M_BACKBRUSH); /* JMP 19-02-98 */
        sprintf(buf, "Equation %s : observed and fitted values", titles[i]);    /* JMP 26-02-98 */
        T_GraphTitle(buf);                             /* JMP 26-02-98 */

        for(t = 0; t < nt; t ++) y[t] = MATE(mlhs, i, t);
        o_calc_lhs(titles[i], lhs, rhs);
        sprintf(buf, "%s : observed", lhs);
        T_GraphLegend(0, 'L', buf, NULL);

        T_GraphTimeData(smpl, y);

        for(t = 0; t < nt; t ++) y[t] = MATE(mrhs, i, t);
        o_calc_lhs(titles[i], lhs, rhs);
        sprintf(buf, "%s : fitted", lhs);
        T_GraphLegend(0, 'L', buf, NULL);
        T_GraphTimeData(smpl, y);

        T_GraphEnd();

        sprintf(buf, "Equation %s : observed and fitted values", titles[i]);    /* JMP 26-02-98 */
        if(view) W_EndDisplay(buf, -ng, -i, -1, -1);
    }

    for(i = 0; i < ng && res == 1; i ++) {
        if(view) W_InitDisplay();

        T_GraphInit(A2M_GWIDTH, A2M_GHEIGHT, 0, 0, L_NAN, L_NAN, L_NAN, L_NAN, 0, A2M_BOXWIDTH, A2M_BACKBRUSH);
        sprintf(buf, "Equation %s : residuals", titles[i]);    /* JMP 26-02-98 */
        T_GraphTitle(buf);                                     /* JMP 26-02-98 */

        for(t = 0; t < nt; t ++) y[t] = MATE(mlhs, i, t) - MATE(mrhs, i, t);
        o_calc_lhs(titles[i], lhs, rhs);
        sprintf(buf, "%s : residuals", lhs, rhs);
        T_GraphLegend(0, 'L', buf, NULL);
        T_GraphTimeData(smpl, y);

        T_GraphEnd();

        sprintf(buf, "Equation %s : residuals", titles[i]);    /* JMP 26-02-98 */
        if(view) W_EndDisplay(buf, -ng, -i, -1, -1);
    }


    SW_nfree(y);
    return(0);
}


ODE_graph(view, mode, type, xgrid, ygrid, axis, ymin, ymax, smpl, names)
int     view, mode, type, xgrid, ygrid, axis;
double  ymin, ymax;
SAMPLE  *smpl;
char    **names;
{
    int old_mode = global_VM, rc;

    global_VM = mode;
    rc = o_var_gr(view, type, xgrid, ygrid, axis, ymin, ymax, smpl, KV_WS, names);

    global_VM = old_mode;
    return(rc);
}

void T_create_wtcfl() {}

o_var_gr_1(gnb, type, xgrid, ygrid, axis, ymin, ymax, smpl, dt, nt, kdb, names)
int     gnb, type, dt, nt, xgrid, ygrid, axis;
double  ymin, ymax;
SAMPLE  *smpl;
KDB     *kdb;
char    *names;
{
    char    *buf, **vars;
    int     i, t, ng, var_nb, rc = 0;
    double    *y;
    extern char *KLG_MODES[][3];        /* JMP38 01-10-92 */

    vars = (char **)SCR_vtoms(names, "+-");
    ng = SCR_tbl_size(vars);
    if(ng == 0) {
        B_seterrn(68);
        return(-1);
    }

    y = (double *) SW_nalloc(sizeof(double) * nt);

    T_GraphInit(A2M_GWIDTH, A2M_GHEIGHT, xgrid, ygrid, ymin, ymax, L_NAN, L_NAN, 0, A2M_BOXWIDTH, A2M_BACKBRUSH);
    /* GB 10/08/98 */
    buf = SCR_malloc(strlen(names) + 20);
    sprintf(buf, "%s (%s)", names, KLG_MODES[global_VM][0]);
    T_GraphTitle(buf);
    SCR_free(buf);

    for(i = 0 ; i < ng ; i++) {
        var_nb = K_find(kdb, vars[i]);
        if(var_nb < 0) {
            B_seterrn(64, vars[i]);
            rc = -1;
            goto fin;
        }

        for(t = 0; t < nt; t++)
            y[t] = (double ) KV_get(kdb, var_nb, dt + t, (int)global_VM);
        T_GraphLegend(0, "LLBL"[type], vars[i], NULL);
        /*        T_GraphLegend(0, "LBLL"[type], vars[i], NULL); */
        T_GraphTimeData(smpl, y);
    }

fin:
    T_GraphEnd();

    SW_nfree(y);
    SCR_free_tbl(vars);
    return(rc);
}

o_var_gr(view, type, xgrid, ygrid, axis, ymin, ymax, smpl, kdb, names)
int     view, type, xgrid, ygrid, axis;
double  ymin, ymax;
SAMPLE  *smpl;
KDB     *kdb;
char    **names;
{
    int     i, dt, nt, ng;

    /* GB 12/97
        if(view == 0) {
    	 if(B_dest == 1)
    	     if(PG_edit_window(vkp_fprintfile) < 0) return(-1);

    	 if(B_PrintDest(B_outfile) < 0) return(-1);
        }
    */
    ng = SCR_tbl_size(names);
    if(ng == 0) {
        B_seterrn(69);
        return(-1);
    }

    nt = smpl->s_nb;
    dt = PER_diff_per(&(smpl->s_p1), &(KSMPL(kdb)->s_p1));
    if(dt < 0 || dt + nt > KSMPL(kdb)->s_nb) {
        B_seterrn(70);
        return(-1);
    }

    for(i = 0; i < ng; i++) {
        if(view) W_InitDisplay();
        o_var_gr_1(i, type, xgrid, ygrid, axis, ymin, ymax, smpl, dt, nt, kdb, names[i]);
        if(view) W_EndDisplay(names[i], -ng, -i, -1, -1);
    }
    W_close();
    return(0); /* JMP38 01-10-92 */
}





