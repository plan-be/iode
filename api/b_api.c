/**
 *  @header4iode
 * 
 *  IODE API developed for iode.pyx (Cython implementation of some IODE functions).
 *  
 *    
 *  List of functions
 *  -----------------
 *  
 */
 
#include "iode.h"
#include "iodeapi.h"

int IodeInit()
{
    RPF_ChronoReset();
    SW_MIN_MEM = 120 * 1024L;
    SW_MIN_SEGS = 2;
    SW_init(1);
    K_init_ws(0);

    //B_LANG = 0;   // JMP 18-04-2022
    //B_NBDEC = -1; // JMP 18-04-2022
    K_LANG = 0;     // JMP 18-04-2022
    K_NBDEC = -1;   // JMP 18-04-2022
    A_SEPS = " ,;\n\t\r";
    A_NO_EXPANDSTAR_DFT = 1; // JMP 12-11-2012 -> suppress default filemane wildcard behavior

    return(0);
}


int IodeEnd()
{
    SW_end();
    W_close();
    K_end_ws(0);                
    L_free_anames();            
    BUF_free();                 
    RP_proc_free_all();         
    RP_free_bufs();             
    // B_reset_error();  
    B_clear_last_error();      // JMP 16/01/2022 
    SCR_free(ODE_INIFILE);     // JMP 30/10/2022
    ODE_INIFILE = 0;
    
    return(0);
}

char *IodeVersion()
{
   return(IODE_VERSION);
}

int  IodeLoadVar(char *name)
{
    return(IodeLoad(name, K_VAR));
}

int  IodeLoad(char *name, int type)
{
    KDB     *kdb;

    if(B_WsLoad(name, type) < 0) return(-1);
    else {
        kdb = K_WS[type];
        return(KNB(kdb));
    }
}

int  IodeSaveVar(char *name)
{
    return(IodeSave(name, K_VAR));
}

int  IodeSave(char *name, int type)
{
    return(B_WsSave(name, type));
}

int IodeClearWs()
{
    int type;

    for(type = K_CMT; type <= K_VAR; type++)
        K_clear(K_WS[type]);
    
    return(0);
}

char **IodeContents(char *pattern, int type)
{
    return(K_grep(K_WS[type], pattern, 0, 1, 0, 0, '*'));
}


char *IodeContentsStr(char *pattern, int type)
{
	char *str;
	char **res = K_grep(K_WS[type], pattern, 0, 1, 0, 0, '*');
    
	if(res != NULL)
		str = SCR_mtov(res, ',');
    SCR_free_tbl(res);
    return(str);
}


double IodeGetVar(char *name, int t, int mode)
{
    int     pos;
    double  value;
    SAMPLE  *smpl;

    smpl = (SAMPLE *) KDATA(K_WS[K_VAR]);
    if(t > smpl->s_nb) return(L_NAN);

    pos = K_find(K_WS[K_VAR], name);
    if(pos < 0) return(L_NAN);

    value = KV_get(K_WS[K_VAR], pos, t, mode);
    return(value);
}

double *IodeGetVector(char *name, int *lg)
{
    int     pos;
    double  *value = NULL;
    SAMPLE  *smpl;

    smpl = (SAMPLE *) KDATA(K_WS[K_VAR]);
    *lg = smpl->s_nb;

    pos = K_find(K_WS[K_VAR], name);
    if(pos >= 0) {
        value = KVVAL(K_WS[K_VAR], pos, 0);
    }
    return(value);
}

int IodeSetVector(char *name, double *value, int len)
{
    int     pos, lg;
    SAMPLE  *smpl;

    smpl = (SAMPLE *) KDATA(K_WS[K_VAR]);
    lg = smpl->s_nb;

    pos = K_find(K_WS[K_VAR], name);
    if (pos >= 0 ) {
    	memcpy(KVVAL(K_WS[K_VAR], pos, 0), value, lg * sizeof(double));
        return(0);
	}
    else 
        return(-1);
}



double *IodeGetSampleVector(int *lg)
{
    double  *value = NULL;
    SAMPLE  *smpl;
    PERIOD	*from;
    int		t, per;

    smpl = (SAMPLE *) KDATA(K_WS[K_VAR]);
    *lg = smpl->s_nb;

    value = (double *) SW_nalloc(smpl->s_nb * sizeof(double));
    from = &smpl->s_p1;
    for(t = 0;  t < smpl->s_nb; t++) {
        per = PER_nbper(from);
        value[t] = (IODE_REAL) from->p_y + (from->p_s + t - 1)/per
                   + ((from->p_s + t - 1) % per)/ (IODE_REAL) per;
    }

    return(value);
}

char *IodeGetSampleString()
{
    SAMPLE  *smpl;
    char	*text = SW_nalloc(50);
    char    b1[20], b2[20];

    smpl = (SAMPLE *) KDATA(K_WS[K_VAR]);


    sprintf(text, "%s %s", PER_pertoa(&(smpl->s_p1), b1),
            PER_pertoa(&(smpl->s_p2), b2));
    return(text);
}



int IodeSetVar(char *name, int t, int mode, double value)
{
    int     pos;
    SAMPLE  *smpl;

    smpl = (SAMPLE *) KDATA(K_WS[K_VAR]);
    if(t > smpl->s_nb) return(-1);

    pos = K_find(K_WS[K_VAR], name);
    if(pos < 0) return(-1);

    KV_set(K_WS[K_VAR], pos, t, mode, value);
    return(0);
}


char    *IodeGetCmt(char *name)
{
    int     pos;

    pos = K_find(K_WS[K_CMT], name);
    if(pos < 0) return("Not in WS");

    return(KCVAL(K_WS[K_CMT], pos));
}

char    *IodeGetLst(char *name)
{
    int     pos;

    pos = K_find(K_WS[K_LST], name);
    if(pos < 0) return("Not in WS");

    return(KLVAL(K_WS[K_LST], pos));
}


char    *IodeGetEqs(char *name)
{
    int     pos;

    pos = K_find(K_WS[K_EQS], name);
    if(pos < 0) return("Not in WS");

    return(KELEC(K_WS[K_EQS], pos));
}

double  IodeGetScl(char *name)
{
    int     pos;
    SCL     *scl;

    pos = K_find(K_WS[K_SCL], name);
    if(pos < 0) return(L_NAN);

    scl = KSVAL(K_WS[K_SCL], pos);
    return(scl->val);
}

double  *IodeGetScls(char *name)
{
    int     pos;
    SCL     *scl;

    pos = K_find(K_WS[K_SCL], name);
    if(pos < 0) return(0);

    scl = KSVAL(K_WS[K_SCL], pos);
    return((double *) scl);
}

int  IodeSetScl(char *name, double value)
{
    int     pos;
    SCL     *scl;

    pos = K_find(K_WS[K_SCL], name);
    if(pos < 0) return(-1);

    scl = KSVAL(K_WS[K_SCL], pos);
    scl->val = value;
    return(0);
}

int IodeSetNbDec(int nbdec)
{
    //B_NBDEC = nbdec; // JMP 18-04-2022
    K_NBDEC = nbdec; // JMP 18-04-2022
    return(0);
}

int IodeGetNbDec()
{
    // return(B_NBDEC); // JMP 18-04-2022
    return(K_NBDEC);    // JMP 18-04-2022
}


char* IodeGetTbl(char *name, char *gsmpl)
{
    int     pos, nc, nl;
    char    *ptr;

    pos = K_find(K_WS[K_TBL], name);
    if(pos < 0) return("Not in WS");

    //ptr = IodeDdeCreateTbl(pos, gsmpl, &nc, &nl, B_NBDEC); // JMP 18-04-2022
    ptr = IodeDdeCreateTbl(pos, gsmpl, &nc, &nl, K_NBDEC);   // JMP 18-04-2022
    SCR_OemToAnsi(ptr, ptr);
    return(ptr);
}

char* IodeGetTblTitle(char *name)
{
    int     pos;
    char    *ptr;
    TBL     *tbl;

    pos = K_find(K_WS[K_TBL], name);
    if(pos < 0 || (tbl = KTVAL(K_WS[K_TBL], pos)) == 0)
        return("Not in WS");

    /* returns anly ptr to string, NO ALLOC is done */
    ptr = T_get_title(tbl);
    SCR_OemToAnsi(ptr, ptr);
    return(ptr);
}

TBL* IodeGetTblDefinition(char *name)
{
    int     pos;
    TBL     *tbl;

    if(name ==  NULL) pos = 0;
    else pos = K_find(K_WS[K_TBL], name);
    if(pos < 0 || (tbl = KTVAL(K_WS[K_TBL], pos)) == 0)
        return(NULL);
    else return(tbl);
}

int IodeSetTblFile(int ref, char *filename)
{
    return(K_load_RWS(ref, filename));
}

int IodeSimulate(char *byear, char *eyear, double eps, double relax, int maxit)
{
    char    buf[100];

    KSIM_EPS   = eps;
    KSIM_RELAX = relax;
    KSIM_MAXIT = maxit;

    sprintf(buf, "%s %s", byear, eyear);

    return(B_ModelSimulate(buf));
}


int IodeGetMaxt()
{
    SAMPLE  *smpl;

    smpl = (SAMPLE *) KDATA(K_WS[K_VAR]);
    return(smpl->s_nb);
}

char    *IodeGetPeriod(int t)
{
    SAMPLE  *smpl;
    PERIOD  per;

    smpl = (SAMPLE *) KDATA(K_WS[K_VAR]);
    memcpy(&per, PER_addper(&smpl->s_p1, t), sizeof(PERIOD));;
    return(PER_pertoa(&per, 0));
}

int IodeGett(char *period)
{
    int     t;
    SAMPLE  *smpl;
    PERIOD  *per, p;

    smpl = (SAMPLE *) KDATA(K_WS[K_VAR]);
    p = smpl->s_p1;
    per = PER_atoper(period);
    if(per == 0) return(-1);

    t = PER_diff_per(per, &p);
    SW_nfree(per);
    return(t);
}


int IodeGetChart(char *name, char *gsmpl)
{
    int     pos;
    TBL     *tbl;

    pos = K_find(K_WS[K_TBL], name);
    if(pos < 0 || (tbl = KTVAL(K_WS[K_TBL], pos)) == 0)
        return(-1);

    return(APIPrepareChart(tbl, gsmpl));
}

int IodeFreeChart(int hdl)
{
    return(APIChartFree(hdl));
}

int IodeChartNl(int hdl)
{
    return(APIChartNl(hdl));
}

int IodeChartNc(int hdl)
{
    return(APIChartNc(hdl));
}

char    IodeChartType(int hdl, int i)
{
    return(APIChartType(hdl, i));
}


int    IodeChartAxis(int hdl, int i)
{
    return(APIChartAxis(hdl, i));
}

char    *IodeChartTitle(int hdl, int i)
{
    return(APIChartTitle(hdl, i));
}

double  *IodeChartData(int hdl, int i)
{
    return(APIChartData(hdl, i));
}


int IodeExecArgs(char *filename, char **args)
{
    int     rc;
    char    buf[10240], *targs;

    rc = B_PrintDest("dummy D");
    SCR_strlcpy(buf, filename, 240);
    K_set_ext(buf, buf, K_REP);

    if(args) {
        targs = SCR_mtov(args, ' ');
        if(strlen(targs) > 10000) targs[10000] = 0;
        strcat(buf, " ");
        strcat(buf, targs);
        SCR_free(targs);
    }

    rc = B_ReportExec(buf);
    W_close();

    return(rc);
}

int IodeExec(char *filename)
{
    return(IodeExecArgs(filename, NULL));
    /*    int     rc;
        char    buf[1024];

        rc = B_PrintDest("dummy D");
        SCR_strlcpy(buf, filename, 1023);
        K_set_ext(buf, buf, K_REP);
        rc = B_ReportExec(buf);
        W_close();
        return(rc);
    */
}




