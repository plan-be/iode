#include <iodedos.h>

SB_WsLoad()
{
    PG_edit_window(vkp_wsload);
    return(0);
}

C_WsLoad(key)
int     key;
{
    int     i;

    for(i = 0 ; i <= K_VAR - COMMENTS ; i++)
	if(key == SCR_F2 + i || key == SCR_F10) {
	    if(B_WsLoad(vkp_wsload_vCMT[i], COMMENTS + i)) goto err;
	    if(i == K_VAR - COMMENTS) SB_ResetSample();
	}

    return(0);
err:
    B_display_last_error();
    return(-1);
}

SB_WsCopy()
{
    PG_edit_window(vkp_wscopy);
    return(0);
}

C_WsCopy()
{
    char    *buf;
    int     rc;

    if(vkp_wscopy_OBJS == NULL) {
	B_seterror(B_msg(135)); /* JMP 24-06-98 */
	B_display_last_error(); /* JMP 24-06-98 */
	return(-1);
	}
    else {
	buf = SW_nalloc(strlen(vkp_wscopy_OBJS) + 1024);   /* JMP 15-09-2015 */
	if(buf == NULL) return(-1);
	if(vkp_wscopy_TYPE == K_VAR &&
		vkp_wscopy_FROM[0] != 0 && vkp_wscopy_TO[0] != 0)
	    sprintf(buf, "%s %s %s %s",
		    vkp_wscopy_FILE,
		    vkp_wscopy_FROM, vkp_wscopy_TO,
		    vkp_wscopy_OBJS);
	else
	    sprintf(buf, "%s %s", vkp_wscopy_FILE, vkp_wscopy_OBJS);
    }

    if(B_WsCopy(buf, vkp_wscopy_TYPE)) {
	B_display_last_error();
	rc = -1;
	}
    else rc = 0;

    SW_nfree(buf);
    return(rc);
}

SB_WsMerge()
{
    PG_edit_window(vkp_wsmerge);
    return(0);
}

C_WsMerge()
{
    int     rc;
    char    buf[K_MAX_FILE + 1];

    sprintf(buf, "%s", vkp_wsmerge_FILE);
    if(B_WsMerge(buf, vkp_wsmerge_TYPE)) {
	B_display_last_error();
	rc = -1;
	}
    else rc = 0;

    return(rc);
}

SB_WsSave()
{
    C_InitWsSave();
    PG_edit_window(vkp_wssave);
    return(0);
}

C_InitWsSave()
{
    int     i;

    for(i = 0 ; i <= K_VAR - COMMENTS ; i++) {
	SCR_strlcpy(vkp_wssave_vCMT[i], KNAMEPTR(K_WS[COMMENTS + i]), 254);  
	vkp_wssave_vCNB[i] =  KNB(K_WS[COMMENTS + i]);
	}
    return(0);
}

C_WsSave()
{
    extern int  K_LZH;
    int     i, rc, klzh = K_LZH;           /* JMP 28-05-00 */

/*    C_WsName(); */

    for(i = 0 ; i <= K_VAR - COMMENTS ; i++) {
	if(SCR_LKEY == SCR_F2 + i || SCR_LKEY == SCR_F10) {
	    K_LZH = vkp_wssave_vCC[i]; /* JMP 28-05-00 */
	    rc = B_WsSave(vkp_wssave_vCMT[i], COMMENTS + i);
	    K_LZH = klzh;              /* JMP 28-05-00 */
	    if(rc) {
		B_display_last_error();
		return(-1);
		}
	    }
	}

    return(0);
}

SB_WsDescr()
{
    C_InitWsDescr();
    PG_edit_window(vkp_wsdescr);
    return(0);
}

C_InitWsDescr()
{
    int     i;

    for(i = 0 ; i <= K_VAR - COMMENTS ; i++)
	SCR_strlcpy(vkp_wsdescr_vDCMT[i], KDESC(K_WS[COMMENTS + i]), 50);
    return(0);
}

C_WsDescr()
{
    int     i;

    for(i = 0 ; i <= K_VAR - COMMENTS ; i++)
	B_WsDescr(vkp_wsdescr_vDCMT[i], COMMENTS + i);
    return(0);
}


SB_WsClear()
{
    C_InitWsClear();
    PG_edit_window(vkp_wsclear);
    return(0);
}


C_InitWsClear()
{
    int     i;

    for(i = 0 ; i <= K_VAR - COMMENTS ; i++)
	vkp_wsclear_vCNB[i] =  KNB(K_WS[COMMENTS + i]);

    return(0);
}

C_WsClear()
{
    int     i;

    for(i = 0 ; i <= K_VAR - COMMENTS ; i++)
	if((SCR_LKEY == SCR_F2 + i || SCR_LKEY == SCR_F10))
		 B_WsClear(0L, COMMENTS + i);

    return(0);
}

SB_WsSample()
{
    C_InitWsSample();
    PG_edit_window(vkp_wssample);
    return(0);
}

C_InitWsSample()
{
    SAMPLE  *smpl = KSMPL(K_WS[K_VAR]);
    char    buf[15];

    strcpy(vkp_wssample_OFROM, PER_pertoa(&(smpl->s_p1), buf));
    strcpy(vkp_wssample_NFROM, PER_pertoa(&(smpl->s_p1), buf));
    strcpy(vkp_wssample_OTO,   PER_pertoa(&(smpl->s_p2), buf));
    strcpy(vkp_wssample_NTO,   PER_pertoa(&(smpl->s_p2), buf));
    return(0);
}

C_WsSample()
{
    char    buf[50];

    sprintf(buf, "%s %s", vkp_wssample_NFROM, vkp_wssample_NTO);
    if(B_WsSample(buf)) {
	B_display_last_error();
	return(-1);
    }
    else {
	ODE_error(OM_SMPL_MOD);
	SB_ResetSample();
	return(0);
    }
}

SB_WsExtrapolate()
{
    PG_edit_window(vkp_extrap);
    return(0);
}

C_WsExtrapolate()
{
    int     lg = 0;
    char    *buf;

    if(vkp_extrap_VARS != NULL) lg = strlen(vkp_extrap_VARS);

    buf = SW_nalloc(lg + 30);
    if(buf == NULL) return(-1);

    sprintf(buf, "%d %s %s %s", vkp_extrap_METHOD,
				vkp_extrap_FROM, vkp_extrap_TO,
				((lg == 0) ? "" : vkp_extrap_VARS));

    if(B_WsExtrapolate(buf)) {
	B_display_last_error();
	return(-1);
    }

    SW_nfree(buf);
    return(0);
}


SB_WsHtoL()
{
    PG_edit_window(vkp_wshtol);
    return(0);
}

C_WsHtoL()
{
    char    *buf = NULL;
    int     rc = 0;

    if(vkp_wshtol_LAST != NULL) {
	buf = SW_nalloc(strlen(vkp_wshtol_LAST) + 1024); /* JMP 15-09-2015 */ // Long names
	sprintf(buf, "%s %s", vkp_wshtol_FILE, vkp_wshtol_LAST);
	if(B_WsHtoLLast(buf)) {
	    B_display_last_error();
	    rc = -1;
	}
	SW_nfree(buf);
    }

    if(vkp_wshtol_MEAN != NULL) {
	buf = SW_nalloc(strlen(vkp_wshtol_MEAN) + 1024); /* JMP 15-09-2015 */ // Long names
	sprintf(buf, "%s %s", vkp_wshtol_FILE, vkp_wshtol_MEAN);
	if(B_WsHtoLMean(buf)) {
	    B_display_last_error();
	    rc = -1;
	}
	SW_nfree(buf);
    }

    if(vkp_wshtol_SUM != NULL) {
	buf = SW_nalloc(strlen(vkp_wshtol_SUM) + 1024); /* JMP 15-09-2015 */ // Long names
	sprintf(buf, "%s %s", vkp_wshtol_FILE, vkp_wshtol_SUM);
	if(B_WsHtoLSum(buf)) {
	    B_display_last_error();
	    rc = -1;
	}
	SW_nfree(buf);
    }

    return(rc);
}


SB_WsLtoH()
{
    PG_edit_window(vkp_wsltoh);
    return(0);
}

C_WsLtoH()
{
    char    *buf = NULL;
    int     rc = 0;

    if(vkp_wsltoh_STOCK != NULL) {
	buf = SW_nalloc(strlen(vkp_wsltoh_STOCK) + 1024); /* JMP 15-09-2015 */ // Long names
	sprintf(buf, "%c %s %s",
		"LCS"[vkp_wsltoh_METHOD], vkp_wsltoh_FILE, vkp_wsltoh_STOCK);
	if(B_WsLtoHStock(buf)) {
	    B_display_last_error();
	    rc = -1;
	}
	SW_nfree(buf);
    }

    if(vkp_wsltoh_FLOW != NULL) {
	buf = SW_nalloc(strlen(vkp_wsltoh_FLOW) + 1024); /* JMP 15-09-2015 */ // Long names
	sprintf(buf, "%c %s %s",
		"LCS"[vkp_wsltoh_METHOD], vkp_wsltoh_FILE, vkp_wsltoh_FLOW);
	if(B_WsLtoHFlow(buf)) {
	    B_display_last_error();
	    rc = -1;
	}
	SW_nfree(buf);
    }

    return(rc);
}

SB_WsSeasonAdj()
{
    PG_edit_window(vkp_wsseasadj);
    return(0);
}

C_WsSeasonAdj()
{
    char    *buf = NULL;
    int     rc = 0;

    if(vkp_wsseasadj_SERIES != NULL) {
	buf = SW_nalloc(strlen(vkp_wsseasadj_SERIES) + 1024); /* JMP 15-09-2015 */ // Long names
	sprintf(buf, "%s %s %s", vkp_wsseasadj_FILE, vkp_wsseasadj_SERIES, vkp_wsseasadj_EPS);
	if(B_WsSeasonAdj(buf)) {
	    B_display_last_error();
	    rc = -1;
	}
	SW_nfree(buf);
    }

    return(rc);
}

SB_ResetSample()
{
    SB_ResetFromTo(vkp_dataeditgraph_FROM, vkp_dataeditgraph_TO);
    SB_ResetFromTo(vkip_exec_FROM, vkip_exec_TO);
    SB_ResetFromTo(vkip_sim_FROM, vkip_sim_TO);
    return(0);
}


SB_ResetFromTo(from, to)
char    *from, *to;
{
    char    f[10], t[10];
    PERIOD  *p1, *p2;

    PER_pertoa(&(KSMPL(K_WS[K_VAR])->s_p1), f);
    PER_pertoa(&(KSMPL(K_WS[K_VAR])->s_p2), t);
    p1 = PER_atoper(from);
    p2 = PER_atoper(to);

    if(p1 == NULL || PER_nbper(p1) != PER_nbper(&(KSMPL(K_WS[K_VAR])->s_p1))
	|| PER_diff_per(p1, &(KSMPL(K_WS[K_VAR])->s_p1)) < 0)
		    SCR_strlcpy(from, f, 10);

    if(p2 == NULL || PER_nbper(p2) != PER_nbper(&(KSMPL(K_WS[K_VAR])->s_p2))
	|| PER_diff_per(p2, &(KSMPL(K_WS[K_VAR])->s_p2)) > 0
	|| PER_diff_per(&(KSMPL(K_WS[K_VAR])->s_p1), p2) > 0)
		    SCR_strlcpy(to, t, 10);

    SW_nfree(p1);
    SW_nfree(p2);
    return(0);
}


SB_WsTrend()
{
    SAMPLE  *smpl = KSMPL(K_WS[K_VAR]);

    switch(PER_nbper(&(smpl->s_p1))) {
	case 4  :    vkp_wstrend_LAMBDA = 1600; break;
	case 12 :    vkp_wstrend_LAMBDA = 1600; break;
	default :    vkp_wstrend_LAMBDA = 100; break;
    }

    PG_edit_window(vkp_wstrend);
    return(0);
}

C_WsTrend()
{
    char    *buf = NULL;
    int     rc = 0;

    if(vkp_wstrend_SERIES != NULL) {
        buf = SW_nalloc(strlen(vkp_wstrend_SERIES) + 1024); /* JMP 15-09-2015 */ // Long names
    /*	sprintf(buf, "%s %d %s",
                  vkp_wstrend_FILE, (int) vkp_wstrend_LAMBDA, vkp_wstrend_SERIES);
    JMP 22-01-2019 CORR 6.58 (cf VL) : lambda devient float */              
        sprintf(buf, "%s %lf %s",           // !!! %d => %lf
                  // vkp_wstrend_FILE, (int) vkp_wstrend_LAMBDA, vkp_wstrend_SERIES);  // erreur 25/3/2019
                  vkp_wstrend_FILE,  vkp_wstrend_LAMBDA, vkp_wstrend_SERIES);  // CORR 25/3/2019

        
        if(vkp_wstrend_LOG) rc = B_WsTrend(buf);
        else                rc = B_WsTrendStd(buf); // JMP 12/4/2019
        
        if(rc)              B_display_last_error();
        
        SW_nfree(buf);
        }

    return(rc);
}


SB_WsAggregate()
{
    return(0);
}


SB_StatUnitRoot()
{
    vkp_unitroot_RES = NULL;
    PG_edit_window(vkp_unitroot);
    return(0);
}

C_StatUnitRoot()
{
    int     rc = 0, i, j, o,
	    drift, trend, order,
	    pos, nb;
    char    *buf = NULL,
	    *res, **series = NULL;
    double    **df;
    extern  double    *B_StatUnitRoot_1();

    if(vkp_unitroot_SERIES != NULL) {
	series = SCR_vtom(vkp_unitroot_SERIES, ';');
	nb = SCR_tbl_size(series);
	if(nb < 1) {
	    vkp_unitroot_RES = NULL;
	    goto done;
	}

	res = vkp_unitroot_RES = SCR_malloc(nb * 80 * (20 + vkp_unitroot_ORDER));
	if(res == NULL) goto done;

	df = (double **) SCR_malloc(4 * nb * sizeof(double *));

	for(j = 0; j < nb; j++) {
	    sprintf(res + strlen(res),"%-13s %-9s %-9s %-9s %-9s\n",
		    "", "DF", "Drift", "Trend", "ADF");
	    buf = SCR_malloc(strlen(series[j]) + 100);
	    for(i = 0; i < 4; i ++) {
		drift = order = trend = 0;
		if(i == 1 || i == 2) drift = 1;
		if(i == 2)           trend = 1;
		if(i == 3)           order = (int) vkp_unitroot_ORDER;

		sprintf(buf, "%d %d %d %s",
			  drift, trend, order, series[j]);
		df[j*4 + i] = B_StatUnitRoot_1(buf, 0);
	    }

	    sprintf(res + strlen(res), "%-13s ", SCR_strip(series[j]));
	    for(i = 0; i < 4; i ++) {
		if(df[j*4 + i] == 0)
		    sprintf(res + strlen(res), "%9s ", "   --    ");
		else {
		    SCR_fmt_dbl(df[j*4 + i][2], res + strlen(res), 9, 6);
		    sprintf(res + strlen(res), " ");
		}
	    }
	}

	for(j = 0; j < nb; j++) {
	    sprintf(res + strlen(res), "\n\n");
	    for(i = 0; i < 4; i ++) {
		drift = order = trend = 0;
		if(i == 1 || i == 2) drift = 1;
		if(i == 2)           trend = 1;
		if(i == 3)           order = (int) vkp_unitroot_ORDER;

		sprintf(res + strlen(res),
		    "ADF(%s, drift = %d, trend = %d, order = %d) = ",
		    series[j], drift, trend, order);
		if(df[j*4 + i] == 0) {
		    sprintf(res + strlen(res), "--\n ");
		    continue;
		}
		else {
		    SCR_fmt_dbl(df[j*4 + i][2], res + strlen(res), 9, 6);
		    sprintf(res + strlen(res), "\n");
		}

		pos = 0;
		sprintf(res + strlen(res),"%-13s %-9s %-9s %-9s\n",
		    "", "Value", "Std Error", "T-Statistic");
		sprintf(res + strlen(res),"%-13s %9.6lf %9.6lf %9.6lf\n",
		    "ADF",
		    df[j*4 + i][pos], df[j*4 + i][pos + 1], df[j*4 + i][pos + 2]);
		pos += 3;

		if(drift) {
		    sprintf(res + strlen(res),"%-13s %9.6lf %9.6lf %9.6lf\n",
			"Drift",
			df[j*4 + i][pos], df[j*4 + i][pos + 1], df[j*4 + i][pos + 2]);
		    pos += 3;
		}

		if(trend) {
		    sprintf(res + strlen(res),"%-13s %9.6lf %9.6lf %9.6lf \n",
			"Trend",
			df[j*4 + i][pos], df[j*4 + i][pos + 1], df[j*4 + i][pos + 2]);
		    pos += 3;
		}

		for(o = 1 ; o <= order ; o++) {
		    sprintf(res + strlen(res),"0rder %-7d %9.6lf %9.6lf %9.6lf \n",
			o,
			df[j*4 + i][pos], df[j*4 + i][pos + 1], df[j*4 + i][pos + 2]);
		    pos += 3;
		}
		sprintf(res + strlen(res), "\n");
	    }
	}

	SW_nfree(buf);
	SCR_free_tbl(series);
	for(i = 0; i < 4 * nb; i ++) SW_nfree(df[i]);
	SCR_free(df);
    }

done:
    PG_display_flds(vkp_unitroot);
    return(rc);
}


SAMPLE  *K_ask_smpl()
{
    SAMPLE  *smpl = KSMPL(K_WS[K_VAR]);
    char    buf[15];

    strcpy(vkp_asksample_FROM, PER_pertoa(&(smpl->s_p1), buf));
    strcpy(vkp_asksample_TO, PER_pertoa(&(smpl->s_p2), buf));

    PG_edit_window(vkp_asksample);
    smpl = PER_atosmpl(vkp_asksample_FROM, vkp_asksample_TO);
    if(smpl == 0 || smpl->s_nb <= 0) return(NULL);

    return(smpl);
}

/*
SB_WsTimeSeries()
{
    SAMPLE  *smpl = KSMPL(K_WS[K_VAR]);
    char    buf[15];

//    strcpy(vkp_wstimeseries_FROM, PER_pertoa(&(smpl->s_p1), buf));
//    strcpy(vkp_wstimeseries_TO,   PER_pertoa(&(smpl->s_p2), buf));
    PG_edit_window(vkp_wstimeseries);
    return(0);
}
*/

/* now in b_ws.cdouble
 IODE_REAL    C_evallec(lec, t)
char    *lec;
{
    doubleclec;
    IODE_REAL    x = L_NAN;

    SCR_strip(lec);
    if(lec[0]) {
	clec = L_cc(lec);
	if(clec == NULL) {
	    B_seterror("Syntax error %.80s", L_error());
	    return(x);
	}
	if(clec != 0 && !L_link(K_WS[K_VAR], K_WS[K_SCL], clec))
	    x = L_exec(K_WS[K_VAR], K_WS[K_SCL], clec, t);
	SW_nfree(clec);
    }

    return(x);
}
*/
T_GraphGrid(axis, label, value)
int     axis;double
char    *label;
 IODE_REAL    value;
{
    char    ax;

    if(!L_ISAN(value)) return;

    ax = "xyz"[axis];
    if (label != NULL) W_printf(".g%clabs (%f \"%s\")\n", ax, value, label);
    else W_printf(".g%cgrids M %f\n", ax, value);
}

/*
C_WsTimeSeries()
{
    char    *buf = NULL,
	    *from   = vkp_wstimeseries_FROM,
	    to[10],
	   double= vkp_wstimeseries_SERIES;
    int     rc = 0, t, dt, nt, i, lag;
    IODE_REAL    *y = NULL,
	    mean, var, stddev, acf[24];
    SAMPLE  *smpl = NULL;
    PERIOD  *toper;

    strcpy(to, vkp_wstimeseries_TO);
    buf = SW_nalloc(strlen(series) + 100);
    SCR_strip(series);
    smpl = PER_pertosmpl(PER_atoper(from), PER_atoper(to));
    lag = atoi(vkp_wstimeseries_LAG);
    if(smpl == NULL) goto err;

    t = PER_diff_per(&(smpl->s_p2), &(KSMPL(K_WS[K_VAR])->s_p1));
    nt = smpl->s_nb;
    dt = PER_diff_per(&(smpl->s_p1), &(KSMPL(K_WS[K_VAR])->s_p1));

    if(vkp_wstimeseries_SERIES != NULL) {
	sprintf(buf, "mean(%s, %s, %s)", from, to, series);
	mean = vkp_wstimeseries_MEAN = C_evallec(buf, t);

	sprintf(buf, "var(%s, %s, %s)", from, to, series);
	var = vkp_wstimeseries_VAR = C_evallec(buf, t);

	sprintf(buf, "stderr(%s, %s, %s)",  from, to, series);
	vkp_wstimeseries_STDERR = C_evallec(buf, t);

	sprintf(buf, "stddev(%s, %s, %s)", from, to, series);
	stddev = vkp_wstimeseries_STDDEV = C_evallec(buf, t);

//        for(i = 0; i < 24; i++) {
//            toper = PER_addper(&(smpl->s_p1), nt - i - 1);
//            PER_pertoa(toper, to);
//            sprintf(buf, "sum(%s, %s, (%s -%lf)*(%s[+%d]-%lf) )",
//                from, to,
//                series, mean, series, i, mean);
//
//            acf[i] = C_evallec(buf, t)/(var * nt);
//            sprintf(buf, "ACF[%d]=%lf", i, acf[i]);
//            AddDebug(buf);
//        }
//        E_DickeyFuller(smpl, series);
    }
    else doubledouble

    y = (IODE_REAL *)SW_nalloc(sizeof(IODE_REAL) * nt);
    sprintf(buf, "%s", vkp_wstimeseries_SERIES);
    for(t = 0; t < nt; t ++) y[t] = C_evallec(buf, dt + t);

    if(lag == 0) {
	W_InitDisplay();
	T_GraphInit(A2M_GWIDTH, A2M_GHEIGHT,
	    0, 2, L_NAN, L_NAN, L_NAN, L_NAN, 0, A2M_BOXWIDTH, A2M_BACKBRUSH);
	T_GraphTitle(buf);

	T_GraphLegend(0, 'L', buf, NULL);
	T_GraphTimeData(smpl, y);

	T_GraphGrid(1, "Mean", mean);

	T_GraphEnd();
	W_EndDisplay(buf, -1, -1, -1, -1);
    }
    else {
	W_InitDisplay();
	T_GraphInit(A2M_GWIDTH, A2M_GHEIGHT,
	    0, 2, L_NAN, L_NAN, L_NAN, L_NAN, 0, A2M_BOXWIDTH, A2M_BACKBRUSH);
	sprintf(buf, "%s vs %s[-%d]", vkp_wstimeseries_SERIES,
				      vkp_wstimeseries_SERIES, lag);
	T_GraphTitle(buf);

	T_GraphXYLegend(0, 'S', buf, NULL);
	T_GraphXYData(nt - lag, y, y + lag);

	T_GraphEnd();
	W_EndDisplay(buf, -1, -1, -1, -1);
    }

err:
    SW_nfree(y);
    SW_nfree(buf);
    PG_display_flds(vkp_wstimeseries);
    return(rc);
}


*/
