#include "iode.h"


B_DataPattern(char  *arg,int  type)
{
    char    **args, *lstname, *pattern;
    char    **xvars = NULL, **yvars = NULL;
    char    cvar[K_MAX_NAME + 1], rvar[K_MAX_NAME + 1], toappend[2*K_MAX_NAME +3];
    int     rc = -1, nrows = 0, ncols = 0, crow, ccol;
    int     nbargs;

    args = SCR_vtom(arg, ' ');
    nbargs = SCR_tbl_size(args);
    if(nbargs < 3) goto cleanup;
    else {
        lstname = args[0];

        pattern = args[1];
        xvars = B_ainit_chk(args[2], NULL, 0);
        nrows = SCR_tbl_size(xvars);

        if(nbargs > 3) {
            yvars = B_ainit_chk(args[3], NULL, 0);
            ncols = SCR_tbl_size(yvars);
        }

        if(nrows == 0) goto cleanup;
        for(crow = 0; crow < nrows; crow++) {
            strcpy(rvar, pattern);
            SCR_replace(rvar, "x", xvars[crow]);
            if(ncols == 0) {
                sprintf(toappend, "%s %s", lstname, rvar);
                if(crow == 0) B_DataUpdate(toappend, K_LST);
                else B_DataAppend(toappend, K_LST);
            }
            for(ccol = 0; ccol < ncols; ccol++) {
                strcpy(cvar, rvar);
                SCR_replace(cvar, "y", yvars[ccol]);
                if(B_DataExist(cvar, type) >= 0) {
                    sprintf(toappend, "%s %s", lstname, cvar);
                    if(crow == 0 && ccol == 0) B_DataUpdate(toappend, K_LST);
                    else B_DataAppend(toappend, K_LST);
                }
            }
        }
        rc = 0;
    }

cleanup:
    SCR_free_tbl(args);
    SCR_free_tbl(xvars);
    SCR_free_tbl(yvars);
    return(rc);
}


B_DataRasVar(arg)
char    *arg;
{
    char    **args;
    int     rc = 0, nbargs, maxit = 100;
    double  eps = 0.001;

    args = SCR_vtom(arg, ' ');
    nbargs = SCR_tbl_size(args);
    if(nbargs < 5) rc = -1;
    else {
        if(nbargs > 5) maxit = atoi(args[5]);
        if(nbargs > 6) eps = atof(args[6]);
        rc = RasExecute(args[0], args[1], args[2], PER_atoper(args[3]), PER_atoper(args[4]), maxit, eps);
    }

    SCR_free_tbl(args);
    return(rc);
}

B_DataCalcVar(arg)
char    *arg;
{
    char    name[K_MAX_NAME + 1], *lec;
    int     lg, t, nb;
    KDB     *kdb = K_WS[K_VAR];
    CLEC    *clec = 0;
    IODE_REAL    *val;

    int     pos;
    IODE_REAL    d;

    lg = B_get_arg0(name, arg, K_MAX_NAME + 1);

    lec = arg + lg + 1;
    SCR_strip(lec);

    nb = KSMPL(kdb)->s_nb;
    if((pos = K_find(kdb, name)) < 0) pos = K_add(kdb, name, NULL, &nb);
    if(pos < 0) return(-1);

    if(lec[0]) {
        clec = L_cc(lec);
        if(clec != 0 && !L_link(kdb, K_WS[K_SCL], clec)) {
            for(t = 0 ; t < KSMPL(kdb)->s_nb ; t++) {
                d = L_exec(kdb, K_WS[K_SCL], clec, t);
                *(KVVAL(kdb, pos, t)) = d;
            }
            SW_nfree(clec);
        }
        else {
            B_seterror("%.80s : %.80s", L_error(), lec);
            SW_nfree(clec);
            return(-1);
        }
    }

    return(0);
}

B_DataCreate_1(arg, ptype)
char    *arg;
int     *ptype;
{
    int     nb_per;
    char    deflt[41];
    KDB     *kdb = K_WS[*ptype];

    if(K_find(kdb, arg) >= 0) return(-1);

    switch(*ptype) {
    case K_CMT :
    case K_LST :
    case K_SCL :
        if(K_add(kdb, arg, NULL) < 0) return(-1);
        else return(0);

    case K_VAR :
        nb_per = KSMPL(kdb)->s_nb;
        if(K_add(kdb, arg, NULL, &nb_per) < 0) return(-1);
        else return(0);

    case K_EQS :
        sprintf(deflt, "%s := %s", arg, arg);
        return(B_DataUpdateEqs(arg, deflt, 0L, 0, 0L, 0L, 0L, 0L, 0));

    case K_IDT :
        sprintf(deflt, "%s", arg);
        if(K_add(kdb, arg, deflt) < 0) return(-1);
        else return(0);
    case K_TBL :
        return(B_DataUpdateTbl(arg, "TITLE;LEC"));
    }
    return(-1);
}

B_DataCreate(arg, type)
char    *arg;
int     type;
{
    return(B_ainit_loop(arg, B_DataCreate_1, &type));
}

B_DataDelete_1(arg, ptype)
char    *arg;
int     *ptype;
{
    int     pos;
    KDB     *kdb = K_WS[*ptype];

    pos = K_find(kdb, arg);
    if(pos < 0 || K_del(kdb, pos) < 0) {
        B_seterrn(29, arg);
        return(-1);
    }
    return(0);
}

B_DataDelete(arg, type)
char    *arg;
int     type;
{
    char    *lst;
    int     rc;

    lst = K_expand(type, NULL, arg, '*');  /* JMP 23-12-98 */
    rc = B_ainit_loop(lst, B_DataDelete_1, &type);
    SCR_free(lst);
    return(rc);
}


B_DataRename(arg, type)
char    *arg;
int     type;
{
    int     rc = 0;
    char    **args;

    if(type == K_EQS) return(-1); /* Rename of EQS has no sense */

    args = B_ainit_chk(arg, NULL, 2);
    if(args == NULL) return(-1);

    if(K_ren(K_WS[type], args[0], args[1]) < 0) {
        B_seterrn(OM_DREN_FAILED_2, args[0], args[1]);
        rc = -1;
    }

    A_free(args);
    return(rc);
}

B_DataDuplicate(arg, type)
char    *arg;
int     type;
{
    int     rc = 0;
    char    **args;
    KDB     *kdb = K_WS[type];

    if(type == K_EQS) {
        B_seterrn(OM_DDUP_NO_SENSE);
        return(-1); /* Duplicate of EQS has no sense */
    }

    args = B_ainit_chk(arg, NULL, 2);
    if(args == NULL) return(-1);

    if(K_dup(kdb, args[0], kdb, args[1]) < 0) {
        B_seterrn(OM_DDUP_FAILED_2, args[0], args[1]);
        rc = -1;
    }

    A_free(args);
    return(rc);
}

B_DataUpdateEqs(name, lec, cmt, method, smpl, instr, blk, tests, date)
char    *name, *lec, *cmt, *instr, *blk;
SAMPLE  *smpl;
float   *tests; /* FLOAT 12-04-98 */
int     method, date;
{
    int     pos, rc;
    EQ      *eq;

    pos = K_find(K_WS[K_EQS], name);
    if(pos < 0) eq = SW_nalloc(sizeof(EQ));
    else eq = KEVAL(K_WS[K_EQS], pos);

    if(lec != NULL) {
        SW_nfree(eq->lec);
        eq->lec = SCR_stracpy(lec);
    }
    if(cmt != NULL) {
        SW_nfree(eq->cmt);
        eq->cmt = SCR_stracpy(cmt);
    }
    if(instr != NULL) {
        SW_nfree(eq->instr);
        eq->instr = SCR_stracpy(instr);
    }
    if(blk != NULL) {
        SW_nfree(eq->blk);
        eq->blk = SCR_stracpy(blk);
    }

    if(method >= 0) eq->method = method;
    if(date > 0) eq->date = SCR_current_date();
    else eq->date = 0L;

    if(tests != NULL)  memcpy(&(eq->tests), tests, EQS_NBTESTS * sizeof(float));   /* FLOAT 12-04-98 */
    else memset(&(eq->tests), 0, EQS_NBTESTS * sizeof(float)); /* JMP 12-04-98 */

    if(smpl != NULL) memcpy(&(eq->smpl), smpl, sizeof(SAMPLE));
    /*    else memset(&(eq->smpl), 0, sizeof(SAMPLE)); */

    rc = K_add(K_WS[K_EQS], name, eq, name);
    if(rc < 0) {
        rc = -1;
        B_seterror(L_error());
    }
    else rc = 0;

    /* GB 27/9/96
        E_tests2scl(eq, E_T, E_NCE);
    */
    E_free(eq);
    return(rc);
}

char T_SEPS[] = ";\n\t";

B_DataUpdateTbl(name, arg)
char    *name, *arg;
{
    TBL     *tbl;
    char    **lecs, *lst;
    int     rc;
    char    *oldseps = A_SEPS;

    tbl = T_create(2);
    if(tbl == NULL) {
        B_seterror("Memory error");
        return(-1);
    }
    A_SEPS = T_SEPS;

    lst = K_expand(K_VAR, NULL, arg, '*');
    lecs = B_ainit_chk(lst, NULL, 0);
    SCR_free(lst);
    /*    lecs = B_ainit_chk(arg, 0L, 0);  */
    if(lecs == 0) goto add;
    A_SEPS = oldseps;
    T_auto(tbl, lecs[0], lecs + 1, 1, 1, 0);
    SCR_free_tbl(lecs);

add:
    rc = K_add(K_WS[K_TBL], name, tbl);

    if(rc >= 0) rc = 0;
    if(rc < 0) rc = -1; // Pour ‚viter return dans les rapports si rc = -2

    T_free(tbl);
    return(rc);
}

/*
Create an object. See Manual for more informations on syntax.

Example for vars:
     B_DataUpdate("A L 2001Y1 12 13 14"); // Create or update A in Level : 20001Y1 => 12, 2002Y1 => 13, ...

*/

B_DataUpdate(arg, type)
char    *arg;
int     type;
{
    int     pos, shift, lg, rc = 0,
                            nb_args, nb_upd, nb_p,
                            i, mode;
    double  var;
#ifndef WATCOM
    double  atof();
#endif
    KDB     *kdb = K_WS[type];
    SCL     scl;
    IODE_REAL    *val;
    PERIOD  *per = NULL;
    char    name[K_MAX_NAME + 1], **args = NULL;

    lg = B_get_arg0(name, arg, K_MAX_NAME + 1);
    pos = K_find(kdb, name);
    if(pos < 0) {
        if(B_DataCreate(name, type)) return(-1);
        pos = K_find(kdb, name);
    }

    switch(type) {
    case K_CMT : /* Name Val */
    case K_IDT :
    case K_LST :
        rc = K_add(kdb, name, arg + lg + 1, name);
        break;

    case K_EQS :
        rc = B_DataUpdateEqs(name, arg + lg + 1,
                             NULL, -1, NULL, NULL, NULL, NULL, 0);
        break;

    case K_TBL :
        rc = B_DataUpdateTbl(name, arg + lg + 1);
        break;

    case K_SCL : /* Name Val [Relax] */
        args = SCR_vtoms(arg, B_SEPS);
        nb_args = SCR_tbl_size(args);
        scl.val = 0.9;
        scl.relax = 1.0;
        scl.std = L_NAN;

        switch(nb_args) {
        case 2:
            scl.val = (IODE_REAL) atof(args[1]);
            break;

        case 3:
            scl.val = (IODE_REAL) atof(args[1]);
            scl.relax = (IODE_REAL) atof(args[2]);
            break;
        default :
            B_seterrn(OM_DUPD_SCL_ERR);
            rc = -1;
            break;
        }
        if(rc == 0) rc = K_add(kdb, args[0], &scl);
        break;

    case K_VAR : /* Name Period nVal */
        args = SCR_vtoms(arg, B_SEPS);
        nb_args = SCR_tbl_size(args);
        if (nb_args > 1) {
            nb_p = 1;
            switch(args[1][0]) {
            case 'd' :
            case 'D' :
                nb_p = 2;
                mode = K_DIFF;
                if(U_is_in(args[1][1], "Yy")) mode = K_DIFFY;
                break;
            case 'g' :
            case 'G' :
                nb_p = 2;
                mode = K_GRT;
                if(U_is_in(args[1][1], "Yy")) mode = K_GRTY;
                break;

            case 'l' :
            case 'L' :
                nb_p = 2;
            default  :
                mode = K_LEVEL;
                break;
            }
            nb_upd = nb_args - nb_p - 1;

            per = PER_atoper(args[nb_p]);
            if(per == 0) {
                B_seterror("Syntax error - Period not defined"); /* JMP 23-05-00 */
                rc = -1;
                break;
            }
            nb_upd = min(nb_upd, PER_diff_per(&(KSMPL(kdb)->s_p2), per) + 1);
            if (per == NULL
                    || ( shift = PER_diff_per(per, &(KSMPL(kdb)->s_p1))) < 0)
                /*                    || (PER_diff_per(&(KSMPL(kdb)->s_p2), per) + 1 - nb_upd) < 0) */
                rc = -1;
            else {
                nb_p ++;
                for(i = 0; i < nb_upd; i++) {
                    var = (double) atof(args[i + nb_p]);
                    if(var == 0.0 && !U_is_in(args[i + nb_p][0], "-0.+")) var = L_NAN; /* JMP 06-09-2004 */
                    KV_set(kdb, pos, shift + i, mode, var);
                }
            }
        }
        else rc = -1;

        if(rc < 0) B_seterrn(OM_DUPD_VAR_ERR_1, arg);
        break;
    }

    A_free(args);
    SCR_free(per);
    if(rc >= 0) rc = 0;
    if(rc < 0) rc = -1; // Pour ‚viter return dans les rapports si rc = -2
    return(rc);
}
/*
B_DataTextSearch(arg, type)
char    *arg;
int     type;
{
    B_DataSearch(arg, type, 0);
}

B_DataObjectSearch(arg, type)
char    *arg;
int     type;
{
    B_DataSearch(arg, type, 1);
}
*/

B_DataSearch(arg, type)
char    *arg;
int     type;
{
    int     rc = 0, word, ecase, names, forms, texts;
    char    **args = NULL, **lst, *str, buf[81];
    char    **K_grep();
    KDB     *kdb = K_WS[type];

    args = B_vtom_chk(arg, 7); /* pattern list */
    if(args == NULL) return(-1);
    word  = atoi(args[1]);
    ecase = atoi(args[2]);
    names = atoi(args[3]);
    forms = atoi(args[4]);
    texts = atoi(args[5]);

    SCR_strlcpy(buf + 1, args[0], 76);
    if(word == 1) {
        strcpy(buf, "*!");
        SCR_strlcpy(buf + 2, args[0], 76);
        strcat(buf, "!*");
    }
    else {
        buf[0] = '*';
        SCR_strlcpy(buf + 1, args[0], 78);
        strcat(buf, "*");
    }

    lst = K_grep(kdb, buf, ecase, names, forms, texts, '*');
    /*
        if(lst == NULL) {
    	B_seterrn(OM_PATTERN_1, buf);
    	rc = -1;
        }
        else {
    	str = SCR_mtov(lst, ';');
    	if(K_add(K_WS[K_LST], args[6], str) < 0) {
    	    B_seterrn(OM_LIST_SET_ERR_1, args[6]);
    	    rc = -1;
    	}
    	SCR_free(str);
        }
    */
    rc = KL_lst(args[6], lst, 200);
    SCR_free_tbl(lst);

    A_free(args);

    return(rc);
}

B_DataEditCnf(arg)
char    *arg;
{
    int     rc = 0, VM, VN;
    char    **args = NULL;

    args = B_vtom_chk(arg, 2); /* pattern list */
    if(args == NULL) return(-1);

    switch(toupper(args[0][0])) {
    case 'L' :
        VM = 0;
        break;
    case 'D' :
        VM = 1;
        break;
    case 'G' :
        VM = 2;
        break;
    default  :
        VM = 0;
        break;
    }

    VN = atoi(args[1]);          /* JMP38 01-10-92 */
    VN = max(-1, VN);
    VN = min(6, VN);
    BGUI_DataEditGlobal(VM, VN);
    A_free(args);
    return(rc);
}

int my_strcmp(const void *pa, const void *pb)
{
    char    **a = pa, **b = pb;
    return(strcmp(*a, *b));
}

B_DataListSort(arg)
char    *arg;
{
    int     rc = 0, p;
    char    *in, *out, *lst,
            **args = NULL, **lsti = NULL;

    args = B_vtom_chk(arg, 2); /* in out */
    if(args == NULL) in = out = arg;
    else {
        in = args[0];
        out = args[1];
    }

    p = K_find(KL_WS, in);

    if(p < 0) {
        B_seterrn(65, args[0]);
        rc = -1;
        goto done;
    }
    else lst = KLVAL(KL_WS, p);
    if(lst == NULL) {
        B_seterrn(65, args[0]);
        rc = -1;
        goto done;
    }

    lsti = B_ainit_chk(lst, NULL, 0);
    if(lsti == NULL) {
        rc = -1;
        goto done;
    }

    qsort(lsti, SCR_tbl_size(lsti), sizeof(char **), my_strcmp);
    lst = SCR_mtov(lsti, ';');  /* JMP 09-03-95 */

    if(K_add(K_WS[K_LST], out, lst) < 0) {
        B_seterrn(66, out);
        rc = -1;
    }

    SCR_free_tbl(lsti);
    SCR_free(lst);

done:
    A_free(args);
    return(rc);
}

/* now in b_gui.c */
//B_DataDisplayGraph(arg)
//char    *arg;
//{
//    return(B_DataEditGraph(1, arg));
//}
//
//B_DataPrintGraph(arg) /* GB 08-97 */
//char    *arg;
//{
//    return(B_DataEditGraph(0, arg));
//}
//
//B_DataEditGraph(view, arg) /* GB 08-97 */
//int     view;
//char    *arg;
//{
//    int     rc = 0, nb_args, i, mode, type, xgrid, ygrid, axis;
//    double  ymin, ymax;
//#ifndef WATCOM
//    double  atof();
//#endif
//    char    **args = NULL;
//    SAMPLE  *smpl = 0;
//
//    args = B_ainit_chk(arg, NULL, 0);
//    nb_args = SCR_tbl_size(args);    /* JMP 16-12-93 */
//    if(nb_args < 10) {
//        B_seterrn(67);
//        rc = -1;
//        goto fin;
//    }
//
//    mode = L_pos("LDGdg", args[0][0]); /* GB 10/08/98 */
//    mode = max(0, mode);
//
//    type  = B_argpos("LSBM", args[1][0]);
//    xgrid = B_argpos("MNJ",  args[2][0]);
//    ygrid = B_argpos("MNJ",  args[3][0]);
//    axis  = B_argpos("LGSP", args[4][0]);
//    if(memcmp(args[5], "--", 2) == 0) ymin = L_NAN;
//    else                              ymin = atof(args[5]);
//    if(memcmp(args[6], "--", 2) == 0) ymax = L_NAN;
//    else                              ymax = atof(args[6]);
//
//    smpl = PER_atosmpl(args[7], args[8]);
//    if(smpl == 0) {
//        B_seterrn(56);
//        rc = -1;
//        goto fin;
//    }
//
//    rc = ODE_graph(view, mode, type, xgrid, ygrid, axis, ymin, ymax,
//                    smpl, args + 9);
//
//fin:
//    A_free(args);
//    SCR_free(smpl);
//    return(rc);
//}


B_DataScan(arg, type)
char    *arg;
int     type;
{
    KDB     *tkdb = NULL;
    char    **objs;
    int     rc = -1;

    if(type != K_IDT && type != K_EQS && type != K_TBL) {
        B_seterrn(122);
        return(-1);
    }

    if(arg == NULL || arg[0] == 0) {
        /* Exo whole WS */
        return(K_scan(K_WS[type], "_EXO", "_SCAL"));
    }
    else {
        objs = B_ainit_chk(arg, NULL, 0);
        if (objs == NULL || SCR_tbl_size(objs) == 0)
            return(K_scan(K_WS[type], "_EXO", "_SCAL"));
        else {
            tkdb = K_refer(K_WS[type], SCR_tbl_size(objs), objs);
            if(tkdb == 0 || KNB(tkdb) == 0)            /* JMP 12-05-11 */
                rc = -1;                                /* JMP 12-05-11 */
            else                                        /* JMP 12-05-11 */
                rc = K_scan(tkdb, "_EXO", "_SCAL");     /* JMP 12-05-11 */

            K_free_kdb(tkdb);
            SCR_free_tbl(objs);
        }
    }
    return(rc);
}


B_DataExist(arg, type)
char    *arg;
int     type;
{
    KDB     *kdb = K_WS[type];

    return(K_find(kdb, arg));
}

B_DataAppend(arg, type)
char    *arg;
int     type;
{
    int     pos, lg;
    KDB     *kdb = K_WS[type];
    char    name[K_MAX_NAME + 1], *ptr, *nptr, *text;

    switch(type) {
    case K_CMT :
    case K_LST :
        break;

    case K_EQS :
    case K_IDT :
    case K_SCL :
    case K_TBL :
    case K_VAR :
        B_seterror("DataAppend : only lists and comments");
        return(-1);
    }

    lg = B_get_arg0(name, arg, K_MAX_NAME + 1);
    text = arg + lg + 1;
    pos = K_find(kdb, name);

    if(pos < 0)
        nptr = text;
    else {
        if(strlen(text) == 0) return(0); /* empty append */

        ptr = KOVAL(kdb, pos);
        nptr = SW_nalloc(strlen(ptr) + strlen(text) + 2);
        if(type == K_CMT)
            sprintf(nptr, "%s %s", ptr, text);
        else
            sprintf(nptr, "%s,%s", ptr, text);
    }

    pos = K_add(kdb, name, nptr);
    if(nptr != text) SW_nfree(nptr);

    if(pos < 0) return(-1);
    else return(0);
}

B_DataList(arg, type)
char    *arg;
int     type;
{
    int     rc = 0;
    char    **args = NULL, **lst, *str,
              *name, *pattern, *file;
    char    **K_grep();
    KDB     *kdb = K_WS[type];

    if((args = B_vtom_chk(arg, 3)) == NULL) {
        if((args = B_vtom_chk(arg, 2)) == NULL) return(-1);
        else {
            name    = args[0];
            pattern = args[1];
            file    = NULL;
        }
    }
    else {
        name    = args[0];
        pattern = args[1];
        file    = args[2];
    }

    if (file == NULL) lst = K_grep(kdb, pattern, 0, 1, 0, 0, '*');
    else {
        kdb = K_interpret(type, file);
        if(kdb == NULL) return(-1);

        lst = K_grep(kdb, pattern, 0, 1, 0, 0, '*');
        K_free(kdb);
    }

    rc = KL_lst(name, lst, 200);
    SCR_free_tbl(lst);
    A_free(args);

    return(rc);
}

unsigned char **Lst_times(unsigned char **l1, unsigned char **l2)
{
    int             i, inb, j, jnb, nl, ll3, ll1, ll2;
    unsigned char   **res = NULL;
    unsigned char   *l3 = NULL;

    inb = SCR_tbl_size(l1);
    jnb = SCR_tbl_size(l2);

    nl = 0;
    ll3 = 0;
    for(i = 0; i < inb; i++) {
        ll1 = strlen(l1[i]);
        for(j = 0; j < jnb; j++) {
            ll2 = strlen(l2[j]);

            if(ll3 < ll1 + ll2 + 1) {
                ll3 = ll1 + ll2 + 1;
                SCR_free(l3);
                l3 = SCR_malloc(ll3);
            }

            if(l3 != NULL) sprintf(l3, "%s%s", l1[i], l2[j]);
            SCR_add_ptr(&res, &nl, l3);
        }
    }

    SCR_free(l3);
    SCR_add_ptr(&res, &nl, NULL);
    return(res);
}

B_DataCalcLst(arg)
char    *arg;
{
    int             rc = 0, p1, p2;
    unsigned char   **args = NULL, **l1 = NULL, **l2 = NULL, **lst = NULL,
                      *res, *list1, *list2, *op;

    unsigned char   **SCR_union(), **SCR_inter(), **SCR_dif();

    /* arg: res list1 op list2 */
    if((args = B_vtom_chk(arg, 4)) == NULL) return(-1);
    res  = args[0];
    list1 = args[1];
    op   = args[2];
    list2 = args[3];

    p1 = K_find(K_WS[K_LST], list1);
    p2 = K_find(K_WS[K_LST], list2);
    if(p1 < 0 || p2 < 0) {
        B_seterror("List %s not in WS", (p1 < 0) ? list1:list2); // JMP 4/02/09
        rc = -1;
        goto done;
    }

    l1 = B_ainit_chk(KLVAL(K_WS[K_LST], p1), NULL, 0);
    l2 = B_ainit_chk(KLVAL(K_WS[K_LST], p2), NULL, 0);
    switch(op[0]) {
    case '+' :
        lst = SCR_union(l1, l2);
        break;
    case '*' :
        lst = SCR_inter(l1, l2);
        break;
    case '-' :
        lst = SCR_dif(l1, l2);
        break;
    case 'x' :
        lst = Lst_times(l1, l2);
        break;
    default  :
        rc = -1;
        goto done;
    }
    rc = KL_lst(res, lst, -1); /* GB 30-10-07 */


done :
    A_free(args);
    A_free(l1);
    A_free(l2);
    SCR_free_tbl(lst);
    return(rc);
}

int B_DataListCount(arg)
char    *arg;
{
    int     nb = 0;
    char    *lst,
            **lsti = NULL;

    lst = SCR_stracpy(KLVAL(KL_WS, K_find(KL_WS, arg)));
    if(lst == NULL) return(-1);

    lsti = B_ainit_chk(lst, NULL, 0);
    nb = SCR_tbl_size(lsti);
    SCR_free_tbl(lsti);
    SCR_free(lst);

    return(nb);
}

B_DataCompareEps(arg)
char    *arg;
{
    extern double K_CMP_EPS;

	// Set Vars threshold
	K_CMP_EPS = atof(arg);
	if(K_CMP_EPS < 1e-15) K_CMP_EPS = 1e-15;
	if(K_CMP_EPS > 1) K_CMP_EPS = 1;
	return(0);
}


B_DataCompare(arg, type)
char    *arg;
int     type;
{
    int     i, rc = 0,
               n1 = 0, n2 = 0, n3 = 0, n4 = 0;
    char    **args = NULL, *name,
            **l1 = NULL, **l2 = NULL, **l3 = NULL, **l4 = NULL,
            *file, *one, *two, *three, *fr;
    KDB     *kdb1 = K_WS[type], *kdb2;

    if((args = B_vtom_chk(arg, 5)) == NULL) return(-1);
    file  = args[0];
    one   = args[1];
    two   = args[2];
    three = args[3];
    fr   = args[4];

    kdb2 = K_interpret(type, file);
    if(kdb2 == NULL) return(-1);

    for(i = 0; i < KNB(kdb1); i++) {
        name = KONAME(kdb1, i);
        rc = K_cmp(name, kdb1, kdb2);
        switch(rc) {
        case 1 :
            SCR_add_ptr(&l1, &n1, name);
            break;
        case 2 :
            SCR_add_ptr(&l2, &n2, name);
            break; /* never reached */
        case 3 :
            SCR_add_ptr(&l3, &n3, name);
            break;
        case 4 :
            SCR_add_ptr(&l4, &n4, name);
            break;
        }
        if(rc > 2) K_del(kdb2, K_find(kdb2, name)) ;
    }

    for(i = 0; i < KNB(kdb2); i++) SCR_add_ptr(&l2, &n2, KONAME(kdb2, i));

    SCR_add_ptr(&l1, &n1, NULL);
    SCR_add_ptr(&l2, &n2, NULL);
    SCR_add_ptr(&l3, &n3, NULL);
    SCR_add_ptr(&l4, &n4, NULL);

/*
    rc = KL_lst(one,    l1, 200);
    rc = KL_lst(two,    l2, 200);
    rc = KL_lst(three,  l3, 200);
    rc = KL_lst(fr,     l4, 200);
*/
    rc = KL_lst(one,    l1, 10000);
    rc = KL_lst(two,    l2, 10000);
    rc = KL_lst(three,  l3, 10000);
    rc = KL_lst(fr,     l4, 10000);

    SCR_free_tbl(l1);
    SCR_free_tbl(l2);
    SCR_free_tbl(l3);
    SCR_free_tbl(l4);

    A_free(args);
    K_free(kdb2);

    return(rc);
}

