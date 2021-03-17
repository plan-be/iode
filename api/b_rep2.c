#include "iode.h"

/*================ Fonctions @ des rapports =============================*/

/*--------------------------- GENERALES -------------------------------*/

U_ch *RPF_take(args)
U_ch    **args;
{
    U_ch    *res = 0;
    int     n, lg;

    if(SCR_tbl_size(args) != 2) return(res);
    n = atol(args[0]);
    lg = strlen(args[1]);
    if(n > 0) {
        res = SCR_malloc(n + 1);
        if(n >= lg) {
            strcpy(res, args[1]);
            memset(res + lg, ' ', n - lg);
            res[n] = 0;
            return(res);
        }
        else {
            memcpy(res, args[1], n);
            res[n] = 0;
            return(res);
        }
    }
    else {
        n = -n;
        res = SCR_malloc(n + 1);
        if(n >= lg) {
            memset(res, ' ', n - lg);
            strcpy(res + n - lg, args[1]);
            return(res);
        }
        else {
            memcpy(res, args[1] + lg - n, n);
            res[n] = 0;
            return(res);
        }
    }
}

U_ch *RPF_drop(args)
U_ch    **args;
{
    U_ch    *res = 0;
    int     n, lg;

    if(SCR_tbl_size(args) != 2) return(res);
    n = atol(args[0]);
    lg = strlen(args[1]);
    if(lg == 0) return(res);

    if(n > 0) {
        if(n >= lg) return(SCR_malloc(1));
        res = SCR_malloc(lg - n + 1);
        strcpy(res, args[1] + n);
        return(res);
    }
    else {
        n = -n;
        if(n >= lg) return(SCR_malloc(1));
        res = SCR_malloc(lg - n + 1);
        SCR_strlcpy(res, args[1], lg - n);
        return(res);
    }
}

U_ch *RPF_replace(args)
U_ch    **args;
{
    U_ch    *res = 0;

    if(SCR_tbl_size(args) != 3) return(res);
    res = SCR_areplace(args[0], args[1], args[2]);
    return(res);
}

U_ch *RPF_equal(args)
U_ch    **args;
{
    U_ch    *res = 0;

    if(SCR_tbl_size(args) != 2)
        res = SCR_stracpy("0");
    else {
        if(strcmp(args[0], args[1]))
            res = SCR_stracpy("0");
        else
            res = SCR_stracpy("1");
    }
    return(res);
}

U_ch *RPF_date(args)
U_ch    **args;
{
    U_ch    *res, *fmt;

    res = SCR_malloc(60);
    if(args[0]) fmt = args[0];
    else        fmt = "dd-mm-yyyy";
    SCR_long_to_fdate(SCR_current_date(), res, fmt);
    return(res);
}

U_ch *RPF_time(args)
U_ch    **args;
{
    U_ch    *res, *fmt;

    res = SCR_malloc(60);
    if(args[0]) fmt = args[0];
    else        fmt = "hh:mm:ss";
    SCR_long_to_ftime(SCR_current_time(), res, fmt);
    return(res);
}

U_ch *RPF_upper(args)
U_ch    **args;
{
    U_ch    *res = SCR_mtov(args, ' ');

    SCR_upper(res);
    return(res);
}

U_ch *RPF_sqz(args)
U_ch    **args;
{
    U_ch    *res = SCR_mtov(args, ' ');

    SCR_sqz(res);
    return(res);
}

U_ch *RPF_strip(args)
U_ch    **args;
{
    U_ch    *res = SCR_mtov(args, ' ');

    SCR_strip(res);
    return(res);
}


U_ch *RPF_fmtint(args)
U_ch    **args;
{
    U_ch    buf[512];

    switch(SCR_tbl_size(args)) {
    case 0 :
        return(SCR_mtov(args, ' '));
    case 2 :
        return(SCR_stracpy(SCR_fmt_long(buf, args[1], atol(args[0]))));
    default :
        return(SCR_stracpy(SCR_fmt_long(buf, "999999", atol(args[0]))));
    }
}

U_ch *RPF_lower(args)
U_ch    **args;
{
    U_ch    *res = SCR_mtov(args, ' ');

    SCR_lower(res);
    return(res);
}

U_ch    *RPF_MONTHSE[] = {
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December"
};

U_ch    *RPF_MONTHSF[] = {
    "Janvier",
    "F‚vrier",
    "Mars",
    "Avril",
    "Mai",
    "Juin",
    "Juillet",
    "Ao–t",
    "Septembre",
    "Octobre",
    "Novembre",
    "Decembre"
};

U_ch    *RPF_MONTHSN[] = {
    "Januari",
    "Februari",
    "Maart",
    "April",
    "Mei",
    "Juni",
    "Juli",
    "Augustus",
    "September",
    "Oktober",
    "November",
    "December"
};

U_ch *RPF_month(args)
U_ch    **args;
{
    int     m, lang = 'F';

    if(SCR_tbl_size(args) < 1) return((U_ch *)0);
    m = atoi(args[0]);
    if(m < 1 || m > 12) return((U_ch *)0);
    if(SCR_tbl_size(args) >= 2) lang = SCR_upper_char(args[1][0]);
    switch(lang) {
    case 'N' :
        return(SCR_stracpy(RPF_MONTHSN[m - 1]));
    case 'F' :
        return(SCR_stracpy(RPF_MONTHSF[m - 1]));
    default  :
        return(SCR_stracpy(RPF_MONTHSE[m - 1]));
    }
}

U_ch *RPF_ansi(args)
U_ch    **args;
{
    U_ch    *buf;

    if(SCR_tbl_size(args) < 1) return((U_ch *)0);
    buf = SCR_stracpy(args[0]);
    return(SCR_OemToAnsi(buf, args[0]));
}


/*--------------------------- FILES -------------------------------*/
U_ch *RPF_fappend(args)
U_ch    **args;
{
    FILE    *fd;
    int     i;

    if(SCR_tbl_size(args) < 1) return(NULL);
    fd = fopen(args[0], "a+");
    if(fd == 0) return(NULL);
    for(i = 1 ; args[i] ; i++) {
        if(strcmp(args[i], "NL") == 0) fprintf(fd, "\n");
        else                fprintf(fd, "%s", args[i]);
    }
    fclose(fd);
    return(SCR_malloc(1));
}

U_ch *RPF_fdelete(args)
U_ch    **args;
{
    FILE    *fd;
    int     i;

    if(SCR_tbl_size(args) < 2) return(NULL);
    unlink(args[0]);
    return(SCR_malloc(1));
}

/*--------------------------- OBJECT EXTRACTIONS ----------------------------*/
U_ch *RPF_sstderr(args)
U_ch    **args;
{
    int     i, pos;
    SCL     *scl;
    U_ch    *res = 0, buf[80];

    for(i = 0 ; args[i] ; i++) {
        if(i > 0) res = SCR_strafcat(res, " ");
        pos = K_find(K_WS[K_SCL], args[i]);
        if(pos < 0 || (scl = KSVAL(K_WS[K_SCL], pos)) == 0) {
            strcpy(buf, "--");
        }
        else {
            if(L_ISAN(scl->std))
                sprintf(buf, "%lf", (double) scl->std);
            else
                strcpy(buf, "--");

        }
        res = SCR_strafcat(res, buf);
    }
    return(res);
}

U_ch *RPF_srelax(args)
U_ch    **args;
{
    int     i, pos;
    SCL     *scl;
    U_ch    *res = 0, buf[80];

    for(i = 0 ; args[i] ; i++) {
        if(i > 0) res = SCR_strafcat(res, " ");
        pos = K_find(K_WS[K_SCL], args[i]);
        if(pos < 0 || (scl = KSVAL(K_WS[K_SCL], pos)) == 0) {
            strcpy(buf, "--");
        }
        else {
            if(L_ISAN(scl->relax))
                sprintf(buf, "%lf", (double) scl->relax);
            else
                strcpy(buf, "--");

        }
        res = SCR_strafcat(res, buf);
    }
    return(res);
}

U_ch *RPF_ttitle(args)
U_ch    **args;
{
    U_ch    *T_get_title();
    U_ch    *tit, *res = 0, buf[128];
    TBL     *tbl;
    int     pos, i;

    for(i = 0 ; args[i] ; i++) {
        if(i > 0) res = SCR_strafcat(res, "\n");
        pos = K_find(K_WS[K_TBL], args[i]);
        if(pos < 0 || (tbl = KTVAL(K_WS[K_TBL], pos)) == 0) {
            sprintf(buf, "Table %s not found", args[i]);
            res = SCR_strafcat(res, buf);
        }
        else {
            res = SCR_strafcat(res, T_get_title(tbl));
            T_free(tbl);
        }
    }
    return(res);
}

U_ch *RPF_cvalue(args)
U_ch    **args;
{
    U_ch    *res = 0, buf[128];
    int     pos, i;
    KDB     *kdb = K_WS[K_CMT];

    if(kdb == NULL) return(res);

    for(i = 0 ; args[i] ; i++) {
        if(i > 0) res = SCR_strafcat(res, ";");
        pos = K_find(kdb, args[i]);
        if(pos < 0) {
            sprintf(buf, "Cmt %s not found", args[i]);
            res = SCR_strafcat(res, buf);
        }
        else {
            res = SCR_strafcat(res, KCVAL(kdb, pos));
            res = SCR_replace(res, "\n", " ");
        }
    }
    return(res);
}

U_ch *RPF_vvalue(args)
U_ch    **args;
{
    U_ch    *res = 0, buf[128];
    IODE_REAL    *val;
    SAMPLE  *smpl;
    KDB     *kdb = K_WS[K_VAR];
    int     pos, i, j;

    if(kdb == NULL) return(res);

    for(i = 0 ; args[i] ; i++) {
        if(i > 0) res = SCR_strafcat(res, ";");
        pos = K_find(kdb, args[i]);
        if(pos < 0) {
            sprintf(buf, "VAR %s not found", args[i]);
            res = SCR_strafcat(res, buf);
        }
        else {
            smpl = (SAMPLE *) KDATA(kdb);
            val = KVVAL(kdb, pos, 0);
            for(j = 0 ; j < smpl->s_nb; j++, val++) {
                IodeFmtVal(buf, *val);
                res = SCR_strafcat(res, buf);
                res = SCR_strafcat(res, " ");
            }
        }
    }
    return(res);
}

U_ch *RPF_lvalue(args)
U_ch    **args;
{
    U_ch    *res = 0, buf[128];
    int     pos, i;
    KDB     *kdb = K_WS[K_LST];

    if(kdb == NULL) return(res);

    for(i = 0 ; args[i] ; i++) {
        if(i > 0) res = SCR_strafcat(res, ",");
        pos = K_find(kdb, args[i]);
        if(pos < 0) {
            sprintf(buf, "List %s not found", args[i]);
            res = SCR_strafcat(res, buf);
        }
        else {
            res = SCR_strafcat(res, KLVAL(kdb, pos));
            res = SCR_replace(res, ";", ",");
        }
    }
    return(res);
}


U_ch *RPF_ivalue(args)                   /* JMP 18-12-01 */
U_ch    **args;
{
    U_ch    *res = 0, buf[128];
    int     pos, i;
    KDB     *kdb = K_WS[K_IDT];

    if(kdb == NULL) return(res);

    for(i = 0 ; args[i] ; i++) {
        if(i > 0) res = SCR_strafcat(res, ";");
        pos = K_find(kdb, args[i]);
        if(pos < 0) {
            sprintf(buf, "Idt %s not found", args[i]);
            res = SCR_strafcat(res, buf);
        }
        else {
            res = SCR_strafcat(res, KILEC(kdb, pos));
            res = SCR_replace(res, "\n", " ");
        }
    }
    return(res);
}

U_ch *RPF_evalue(args)                   /* GB  14-02-05 */
U_ch    **args;
{
    U_ch    *res = 0, buf[128];
    int     pos, i;
    KDB     *kdb = K_WS[K_EQS];

    if(kdb == NULL) return(res);

    for(i = 0 ; args[i] ; i++) {
        if(i > 0) res = SCR_strafcat(res, ";");
        pos = K_find(kdb, args[i]);
        if(pos < 0) {
            sprintf(buf, "Eqs %s not found", args[i]);
            res = SCR_strafcat(res, buf);
        }
        else {
            res = SCR_strafcat(res, KELEC(kdb, pos));
            res = SCR_replace(res, "\n", " ");
        }
    }
    return(res);
}

U_ch *RPF_eqsample(args)            /* JMP 27-07-11 */
U_ch    **args;
{
    U_ch    *res = 0;
    int     pos;
    KDB     *kdb = K_WS[K_EQS];

    if(kdb == NULL) return(res);             // Equation WS  empty
    if(SCR_tbl_size(args) != 1) return(res); // 1! eq

    res = SCR_malloc(80);
    pos = K_find(kdb, args[0]);

    if(pos < 0) sprintf(res, "[Eqs %s not found]", args[0]);
    else        PER_smpltoa(&(KESMPL(kdb, pos)), res);

    return(res);
}

U_ch *RPF_eqsamplefromto(args, fromto)            /* JMP 27-07-11 */
U_ch    **args;
{
    U_ch    *res = 0;
    int     pos;
    KDB     *kdb = K_WS[K_EQS];
    SAMPLE  *smpl;

    if(kdb == NULL) return(res);             // Equation WS  empty
    if(SCR_tbl_size(args) != 1) return(res); // 1! eq

    res = SCR_malloc(80);
    pos = K_find(kdb, args[0]);
    smpl = &KESMPL(kdb, pos);

    if(pos < 0) sprintf(res, "[Eqs %s not found]", args[0]);
    else {
        if(fromto == 0) PER_pertoa(&smpl->s_p1, res);
        else            PER_pertoa(&smpl->s_p2, res);
    }
    if(res[0] == 0) strcpy(res, " "); // pour ‚viter de quitter le rapport si sample vide

    return(res);
}

U_ch *RPF_eqsamplefrom(U_ch **args) {
    return(RPF_eqsamplefromto(args, 0));
}
U_ch *RPF_eqsampleto  (U_ch **args) {
    return(RPF_eqsamplefromto(args, 1));
}


U_ch *RPF_eqlhsrhs(args, lhsrhs)            /* JMP 27-07-11 */
U_ch    **args;
int		lhsrhs;
{
    U_ch    *eq = 0;
    int     pos, poscolon;
    KDB     *kdb = K_WS[K_EQS];

    if(kdb == NULL) return(eq);             // Equation WS  empty
    if(SCR_tbl_size(args) != 1) return(eq); // 1! eq

    pos = K_find(kdb, args[0]);

    if(pos < 0) {
        eq = SCR_malloc(80);
        sprintf(eq, "[Eqs %s not found]", args[0]);
    }
    else {
        eq = SCR_stracpy(KELEC(kdb, pos));
        poscolon = L_split_eq(eq);
        if(poscolon > 0) {
            if(lhsrhs == 0) eq[poscolon] = 0;
            else            strcpy(eq, eq + poscolon + 2);
        }
    }

    return(eq);
}

U_ch *RPF_eqlhs(U_ch **args) {
    return(RPF_eqlhsrhs(args, 0));
}
U_ch *RPF_eqrhs(U_ch **args) {
    return(RPF_eqlhsrhs(args, 1));
}


U_ch *RPF_count(args)
U_ch    **args;
{
    U_ch    *res;
    int     n;

    res = SCR_malloc(20);
    n = SCR_tbl_size(args);
    sprintf(res, "%d", n);
    return(res);
}

U_ch *RPF_index(args)
U_ch    **args;
{
    U_ch    *res;
    int     n, pos;

    n = SCR_tbl_size(args) - 1;
    if(n < 1) return(SCR_malloc(1));
    pos = atol(args[0]);
    if(pos > n || pos < 1) return(SCR_malloc(1));
    return(SCR_stracpy(args[pos]));
}

U_ch *RPF_sample(args)
U_ch    **args;
{
    U_ch    *res = 0, buf[128];
    SAMPLE  *smpl;
    KDB     *kdb = K_WS[K_VAR];
    char     what = 'F';

    smpl = (SAMPLE *) KDATA(kdb);

    if(args[0]) what = args[0][0];

    switch(toupper(what)) {
    case 'B':
        sprintf(buf, "%s", PER_pertoa(&(smpl->s_p1), 0L));
        break;
    case 'E':
        sprintf(buf, "%s", PER_pertoa(&(smpl->s_p2), 0L));
        break;
    default :
        sprintf(buf, "%s %s", PER_pertoa(&(smpl->s_p1), 0L),
                PER_pertoa(&(smpl->s_p2), 0L));
        break;
    }

    res = SCR_strafcat(res, buf);
    return(res);
}

U_ch *RPF_memory(args)
U_ch    **args;
{
    U_ch    *res = 0, buf[128];

    SWSTAT  st;

    SW_status(&st);

    sprintf(buf, "NBA=%ld NBB=%ld ALLOC=%ld FREE=%ld USED=%ld",
            st.st_nballoc - st.st_nbfree,
            st.st_nbblks,
            (st.st_convalloc)/(1024 * 1024),
            (st.st_convafree + st.st_convfree)/(1024L*1024L),
            (st.st_convalloc - st.st_convafree))/(1024L*1024L);

    res = SCR_strafcat(res, buf);
    return(res);
}

/****************** CHRONO ***************************/

static long RPF_CHRONO = 0;

U_ch *RPF_ChronoReset()
{
    RPF_CHRONO = WscrGetMS();
    return(SCR_malloc(1));
}

U_ch *RPF_ChronoGet()
{
    U_ch    buf[128];
    long	ms;

    ms = WscrGetMS() - RPF_CHRONO;
    sprintf(buf, "%ld", ms);
    return(SCR_stracpy(buf));
}

RPF_vsliste1(cl, tbl, nb, type)
CLEC    *cl;
U_ch    ***tbl;
int     *nb, type;
{
    int     j, k;

    for(j = 0 ; j < cl->nb_names ; j++) {
        if(L_ISCOEF(cl->lnames[j].name) && type != 'S') continue;
        if(!L_ISCOEF(cl->lnames[j].name) && type != 'V') continue;
        for(k = 0 ; k < *nb ; k++)
            if(strcmp(cl->lnames[j].name, (*tbl)[k]) == 0) break;
        if(*nb == 0 || k == *nb) SCR_add_ptr(tbl, nb, cl->lnames[j].name);
    }

    return(0);
}

U_ch *RPF_vsliste(args, type)
U_ch    **args;
int     type;
{
    EQ      *eq;
    U_ch    **tbl = 0, *res;
    int     i, nb = 0, pos;

    if(SCR_tbl_size(args) < 1) return(NULL);

    for(i = 0 ; args[i] ; i++) {
        pos = K_find(K_WS[K_EQS], args[i]);
        if(pos < 0) continue;
        eq = KEVAL(K_WS[K_EQS], pos);
        RPF_vsliste1(eq->clec, &tbl, &nb, type);
        E_free(eq);
    }

    SCR_add_ptr(&tbl, &nb, 0L);

    res = SCR_mtov(tbl, ';');
    SCR_free_tbl(tbl);
    return(res);
}

U_ch **RPF_unique(tbl)
U_ch    **tbl;
{
    U_ch    **tbl2 = 0;
    int     nl = 0, i, j;

    if(tbl == 0 || tbl[0] == 0) return(tbl2);
    SCR_add_ptr(&tbl2, &nl, tbl[0]);
    for(i = 1 ; tbl[i] ; i++) {
        for(j = 0 ; j < nl ; j++) {
            if(strcmp(tbl[i], tbl2[j]) == 0) break;
        }
        if(j == nl)
            SCR_add_ptr(&tbl2, &nl, tbl[i]);
    }
    SCR_add_ptr(&tbl2, &nl, 0L);
    return(tbl2);
}

U_ch *RPF_vliste(args)
U_ch    **args;
{
    return(RPF_vsliste(args, 'V'));
}

U_ch *RPF_sliste(args)
U_ch    **args;
{
    return(RPF_vsliste(args, 'S'));
}

U_ch *RPF_expand(args, type)
U_ch    **args;
int     type;
{
    extern char *K_expand();
    U_ch        *res, *tmp, **tbl = 0, **tbl2;
    int         nb = 0, i;


    for(i = 0 ; args[i] ; i++) {
        tmp = K_expand(type, 0L, args[i], '*');
        if(tmp) SCR_add_ptr(&tbl, &nb, tmp);
        SCR_free(tmp);
    }
    SCR_add_ptr(&tbl, &nb, 0L);
    res = SCR_mtov(tbl, ';');
    SCR_free_tbl(tbl);
    tbl = SCR_vtom(res, ';');
    SCR_free(res);
    tbl2 = RPF_unique(tbl);
    SCR_free_tbl(tbl);
    res = SCR_mtov(tbl2, ';');
    SCR_free_tbl(tbl2);
    return(res);
}

U_ch *RPF_cexpand(U_ch **args) {
    return(RPF_expand(args, K_CMT));
}
U_ch *RPF_eexpand(U_ch **args) {
    return(RPF_expand(args, K_EQS));
}
U_ch *RPF_iexpand(U_ch **args) {
    return(RPF_expand(args, K_IDT));
}
U_ch *RPF_lexpand(U_ch **args) {
    return(RPF_expand(args, K_LST));
}
U_ch *RPF_sexpand(U_ch **args) {
    return(RPF_expand(args, K_SCL));
}
U_ch *RPF_texpand(U_ch **args) {
    return(RPF_expand(args, K_TBL));
}
U_ch *RPF_vexpand(U_ch **args) {
    return(RPF_expand(args, K_VAR));
}

/****************** SIMULATION ***************************/
int RPF_CalcPeriod(args)
U_ch    **args;
{
    PERIOD 	*per;
    int     t;

    // Check args
    if(SCR_tbl_size(args) == 0) return(-1); // Erreur, arg required
    per = PER_atoper(args[0]);
    if(per == 0) return(-1); // Error

    // Calc diff bw per and WS sample
    t = PER_diff_per(per, &(KSMPL(KV_WS)->s_p1));
    if(t < 0)  return(-1);
    if(PER_diff_per(per, &(KSMPL(KV_WS)->s_p2)) > 0) {
        SCR_free(per);
        return(-1);
    }
    SCR_free(per);
    return(t);
}

U_ch *RPF_SimNorm(args)
U_ch    **args;
{
    int		t;
    char	buf[128];

    t = RPF_CalcPeriod(args);
    if(t < 0) return(SCR_malloc(1));

    // Check si déjà simulation
    if(KSIM_NORMS == 0) return(SCR_stracpy("0")); // Pas encore de simulation

    // Return norme t
    sprintf(buf, "%lg", KSIM_NORMS[t]);
    return(SCR_stracpy(buf));
}

U_ch *RPF_SimNIter(args)
U_ch    **args;
{
    int		t;
    char	buf[128];

    t = RPF_CalcPeriod(args);
    if(t < 0) return(SCR_malloc(1));

    // Check si déjà simulation
    if(KSIM_NITERS == 0) return(SCR_stracpy("0")); // Pas encore de simulation

    // Return norme t
    sprintf(buf, "%d", KSIM_NITERS[t]);
    return(SCR_stracpy(buf));
}

U_ch *RPF_SimMaxit()
{
    U_ch    buf[128];

    sprintf(buf, "%d", KSIM_MAXIT);
    return(SCR_stracpy(buf));
}

U_ch *RPF_SimEps()
{
    U_ch    buf[128];

    sprintf(buf, "%lg", KSIM_EPS);
    return(SCR_stracpy(buf));
}

U_ch *RPF_SimRelax()
{
    U_ch    buf[128];

    sprintf(buf, "%lf", KSIM_RELAX);
    return(SCR_stracpy(buf));
}

/*=================== VTake, ... ==============================*/
extern char    *RP_VSEPS;

/* Take n first (or last if n < 0) elements of a argument list.
   E.g : $define A @take(-2,A;B;C)  
         A becomes B;C
*/

U_ch *RPF_vtake(args)
U_ch    **args;
{
    U_ch    *res = 0, *tmp, *arg;
    int     n, lg;

    if(RP_VSEPS == 0) RP_vseps(0);
    if(SCR_tbl_size(args) < 1) return(SCR_malloc(1));
    arg = SCR_mtov(args, RP_VSEPS[0]);
    args = SCR_vtoms(arg, RP_VSEPS);
    SCR_free(arg);
    lg = SCR_tbl_size(args) - 1;
    if(lg == 0) {
        SCR_free_tbl(args);
        return(SCR_malloc(1));
    }
    n = atol(args[0]);
    if(n > 0) {
        n = min(lg, n);
        tmp = args[n + 1];
        args[n + 1] = 0;
        res = SCR_mtov(args + 1, RP_VSEPS[0]);
        args[n + 1] = tmp;
    }
    else {
        n = lg - min(lg, -n);
        res = SCR_mtov(args + n + 1, RP_VSEPS[0]);
    }
    SCR_free_tbl(args);
    return(res);
}

U_ch *RPF_vdrop(args)
U_ch    **args;
{
    U_ch    *res = 0, *tmp, *arg;
    int     n, lg;

    if(RP_VSEPS == 0) RP_vseps(0);
    if(SCR_tbl_size(args) < 1) return(SCR_malloc(1));
    arg = SCR_mtov(args, RP_VSEPS[0]);
    args = SCR_vtoms(arg, RP_VSEPS);
    SCR_free(arg);
    lg = SCR_tbl_size(args) - 1;
    if(lg == 0) {
        SCR_free_tbl(args);
        return(SCR_malloc(1));
    }
    n = atol(args[0]);
    if(n < 0) {
        n = max(lg + n, 0);
        tmp = args[n + 1];
        args[n + 1] = 0;
        res = SCR_mtov(args + 1, RP_VSEPS[0]);
        args[n + 1] = tmp;
    }
    else {
        n = min(lg, n);
        res = SCR_mtov(args + n + 1, RP_VSEPS[0]);
    }
    SCR_free_tbl(args);
    return(res);
}

U_ch *RPF_vcount(args)
U_ch    **args;
{
    char    buf[128], *arg;
    
    if(RP_VSEPS == 0) RP_vseps(0);
    arg = SCR_mtov(args, RP_VSEPS[0]);
    args = SCR_vtoms(arg, RP_VSEPS);
    SCR_free(arg);
    
    sprintf(buf,  "%d", SCR_tbl_size(args));
    SCR_free_tbl(args);
    return(SCR_stracpy(buf));
}

U_ch *RPF_getdir() 
{
    U_ch    dir[512];
   
    if(SCR_getcwd(0, dir) == 0)
        return(SCR_stracpy(""));
    else
        return(SCR_stracpy(dir));
}

U_ch *RPF_chdir (U_ch **args) 
{
    int     rc = 0;
    
    if(SCR_tbl_size(args) > 0) 
        chdir(args[0]);
    
    ODE_settitle(); 
    return(RPF_getdir());
}

U_ch *RPF_mkdir (U_ch **args) 
{
    int     rc = 0;
    
    if(SCR_tbl_size(args) > 0) 
        mkdir(args[0]);
    
    return(SCR_stracpy(""));
}

U_ch *RPF_rmdir (U_ch **args) 
{
    int     rc = 0;
    
    if(SCR_tbl_size(args) > 0) 
        rmdir(args[0]);
    
    return(SCR_stracpy(""));
}

U_ch *RPF_void(U_ch **args)    
{
    return(SCR_stracpy(""));
}

/*========= D‚finition des fonctions @ des rapports =========*/

typedef struct _rpfn_ {
    U_ch    *name;
    U_ch    *(*fn)();
} RPFN;


extern U_ch *RPS_Open(), *RPS_Query(), *RPS_Next(), *RPS_Close(),
       *RPS_Record(), *RPS_Field(), *RPS_NbFlds();

RPFN RP_FNS[] = {
    "upper",    RPF_upper,
    "date",     RPF_date,
    "time",     RPF_time,
    "lower",    RPF_lower,

    "take",     RPF_take,                /* JMP 18-12-01 */
    "drop",     RPF_drop,                /* JMP 18-12-01 */
    "count",    RPF_count,               /* JMP 18-12-01 */
    "index",    RPF_index,               /* JMP 18-12-01 */
    "fmt",      RPF_fmtint,              /* JMP 06-11-2003 */
    "sqz",      RPF_sqz,                 /* JMP 10-11-2004 */
    "strip",    RPF_strip,               /* JMP 10-11-2004 */

    "ttitle",   RPF_ttitle,
    "cvalue",   RPF_cvalue,
    "lvalue",   RPF_lvalue,
    "vvalue",   RPF_vvalue,
    "sample",   RPF_sample,
    "replace",  RPF_replace,
    "month",    RPF_month,
    "ansi",     RPF_ansi,
    "equal",    RPF_equal,

    "ivalue",   RPF_ivalue,              /* JMP 18-12-01 */

    "evalue",   RPF_evalue,              /* GB  14-02-05 */

    "eqsample",    RPF_eqsample,      /* JMP 27-07-11 */
    "eqsamplefrom",RPF_eqsamplefrom,  /* JMP 27-07-11 */
    "eqsampleto",  RPF_eqsampleto,    /* JMP 27-07-11 */
    "eqlhs",       RPF_eqlhs,         /* JMP 27-07-11 */
    "eqrhs",       RPF_eqrhs,         /* JMP 27-07-11 */

    "vliste",   RPF_vliste,
    "sliste",   RPF_sliste,

    "cexpand",  RPF_cexpand,
    "eexpand",  RPF_eexpand,
    "iexpand",  RPF_iexpand,
    "lexpand",  RPF_lexpand,
    "sexpand",  RPF_sexpand,
    "texpand",  RPF_texpand,
    "vexpand",  RPF_vexpand,

    "fappend",  RPF_fappend,
    "fdelete",  RPF_fdelete,

    "sstderr",  RPF_sstderr,

    "sqlopen",  RPS_Open,
    "sqlquery", RPS_Query,
    "sqlnext",  RPS_Next,
    "sqlrecord",RPS_Record,
    "sqlfield", RPS_Field,
    "sqlnbflds",RPS_NbFlds,
    "sqlclose", RPS_Close,

    "srelax",   RPF_srelax,

    "memory",   RPF_memory,

    "chronoreset", RPF_ChronoReset,
    "chronoget",   RPF_ChronoGet,

    "simeps",		RPF_SimEps,
    "simmaxit",		RPF_SimMaxit,
    "simrelax",		RPF_SimRelax,
    "simnorm",		RPF_SimNorm,
    "simniter",		RPF_SimNIter,
    
    //"vseps",		RPF_vseps,
    "vtake",		RPF_vtake,
    "vdrop",		RPF_vdrop,
    "vcount",		RPF_vcount,
    
    "getdir",       RPF_getdir, // 9/4/2019
    "chdir",        RPF_chdir,  // 9/4/2019
    "mkdir",        RPF_mkdir,  // 9/4/2019
    "rmdir",        RPF_rmdir,  // 9/4/2019

    "void",         RPF_void,   // 9/4/2019
    0, 0
};

/*========= Interpr‚tation et ex‚cution des fonctions @ des rapports =========*/

char    *RP_extractpar(buf, i, chars)
char    *buf, *chars;
int     *i;
{
    int     nb = 0, k, instr = 0;
    char    *ptr, *res;

    for(k = *i ; buf[k] ; k++) {
        if(buf[k] == '"') {
            if(instr) instr = 0;
            else      instr = 1;
            continue;
        }
        if(instr) continue;
        if(buf[k] == chars[1] && nb <= 1) break;
        if(buf[k] == chars[1]) nb--;
        else if(buf[k] == chars[0]) nb++;
        else if(nb == 0 && !SCR_is_alpha(buf[k]) && buf[k] != ' ') return(NULL); /* JMP 17-03-00 */
    }
    if(buf[k] == 0) return(NULL);

    k -= *i;
    res = SW_nalloc(k + 1);
    memcpy(res, buf + *i, k);
    *i += k;

    return(res);
}


U_ch *RP_gfn(str)
U_ch    *str;
{
    U_ch    *res = NULL;
    int     allc = 0;  // JMP 28/8/2012

    if(str == NULL) return(NULL);

    if(U_pos('%', str) >= 0 || U_pos('{', str) >= 0 || U_pos('@', str) >= 0 ) {
        RP_expand(&res, str);
        str = SCR_stracpy(res);
        allc = 1;  // JMP 28/8/2012
        SW_nfree(res);
    }

    RP_fneval(&res, str);
    if(allc) SCR_free(str); // JMP 28/8/2012
    return(res);
}

RP_fneval(res, str)
char    **res, *str;
{
    U_ch    **tbl;
    int     i, rc = 0;

    *res = 0;
    tbl = SCR_vtoms3(str, "(,", 1);
    SCR_strip(tbl[0]);
    SCR_lower(tbl[0]);
    for(i = 0 ; RP_FNS[i].name ; i++)
        if(strcmp(RP_FNS[i].name, tbl[0]) == 0) break;

    if(RP_FNS[i].name == 0) {
        rc = -1;
        goto fin;
    }
    *res = (*RP_FNS[i].fn)(tbl + 1);
    if(*res == 0) rc = -1;
fin:
    SCR_free_tbl(tbl);
    return(rc);
}


