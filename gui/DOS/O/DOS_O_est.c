#include "iodedos.h"

char    *OE_INSTR = NULL,
         *OE_BLK = NULL;
int     OE_CF = 0;

char    OE_METHOD = 0;
SAMPLE  OE_SMPL;
KDB     *OE_DBE = NULL,
         *OE_DBS = NULL;

int     *OE_VEC = NULL,
         OE_NB = 0, OE_CUR = 0,
         OE_DONE = 0, OE_EST, OE_MAXIT = 100;

double    OE_EPS = 0.0000001;


/* Interface functions */
ODE_blk_begin()
{
    OE_DONE = 0;                /* JMP38 27-09-92 */
    if(ODE_blk_init() < 0) return(-1);

    if(vkp_eqs_NAME[0] == ' ') return(0);

    ODE_blk_fn();
    return(0);
}

ODE_blk_fn()
{
    if(ODE_blk_check()) return(-1);
    ODE_blk_cur();
    return(0);
}

ODE_blk_name_fn()
{
    if(ODE_blk_fn()) return(-1);
    return(ODE_blk_save_cur());
}

ODE_blk_end_fn()
{
    if(ODE_ask(OM_STOP) != 0) return(-1);

    OE_NB = OE_CUR = 0;
    memset(&(OE_SMPL), 0, sizeof(SAMPLE));
    SW_nfree(OE_VEC);
    OE_VEC = NULL;
    SW_nfree(OE_INSTR);
    OE_INSTR = NULL;
    SW_nfree(OE_BLK);
    OE_BLK = NULL;
    K_free(OE_DBE);
    OE_DBE = NULL;
    K_free(OE_DBS);
    OE_DBS = NULL;
    E_free_work();

    SCR_PG_ABORT = 1;
    OE_EST = 0;
    PG_reset(vkp_eqs);
    return(0);
}

ODE_blk_next_fn()
{
    if(ODE_blk_save_cur()) return(-1);

    //  if(ODE_blk_check()) return(-1);

    if(ODE_blk_check()) {
        B_display_last_error();
        return(-1);
    }
    OE_CUR++;
    ODE_blk_cur();
    return(0);
}

ODE_blk_coef_fn()
{
    if(ODE_blk_save_cur()) return(-1);

//    if(ODE_blk_check()) return(-1);
    if(ODE_blk_check()) {
        B_display_last_error();
        return(-1);
    }

    if(ODE_blk_coef() < 0) return(-1);

    OE_CF = 1;
    ODE_scroll(OE_DBS, NULL);
    OE_CF = 0;
    return(0);
}


ODE_blk_est_fn()
{
    SAMPLE          *smpl  = NULL;

    if(ODE_blk_save_cur()) return(-1);

    OE_METHOD = vkp_eqs_METHOD;
    if(vkp_eqs_FROM[0] == ' ' || vkp_eqs_TO[0] == ' ') {
        B_seterrn(89);
        goto err;
    }

    smpl = PER_atosmpl(vkp_eqs_FROM, vkp_eqs_TO);
    if(smpl == NULL) goto err;
    memcpy(&(OE_SMPL), smpl, sizeof(SAMPLE));
    SW_nfree(smpl);

    SW_nfree(OE_INSTR);
    OE_INSTR = SCR_stracpy(vkp_eqs_INSTRS);

    ODE_blk_fn();
    if(ODE_blk_coef() < 0) return(-1);
    if(ODE_blk_instr() < 0) return(-1);

    ODE_blk_est();

    if(OE_DONE) {
        /*        vkp_eqs_DATE = SCR_current_date(); */
        ODE_blk_cur_atests();
        ODE_blk_cur_tests();

        PG_display_flds(vkp_eqs);
    }
    return(0);
err:
    B_display_last_error();
}

ODE_blk_save_fn()
{
    if(ODE_blk_save_cur()) return(-1);
    if(ODE_blk_check()) {
        B_display_last_error();
        return(-1);
    }

    if(OE_DONE == 0) {
        if(ODE_blk_coef() < 0) return(-1);
        if(ODE_blk_instr() < 0) return(-1);
    }

    ODE_blk_save();
    return(0);
}

ODE_blk_res_fn()
{
    char    **endos = NULL, **coefs = 0, **E_txt = NULL;
    int     i, t, choice, nbc = 0, E_ntxt = 0;
    MAT     *E_RES = NULL;

    if(OE_DONE != 1) {
        B_seterrn(210);
        B_display_last_error();
        return(-1);
    }
    endos = SCR_vtom(OE_BLK, ';');
    for(i = 0 ; i < E_NC ; i++) {
        if(KSVAL(E_DBS, E_C_NBS[i])->relax == 0) continue;
        SCR_add_ptr(&coefs, &nbc, KONAME(E_DBS, E_C_NBS[i]));
    }
    SCR_add_ptr(&coefs, &nbc, 0L);

    while(1) {
        choice = MN_edit(vkm_results);
        switch(choice) {
        case 3  :
            ODE_blk_coef_fn();
            break;
        case 4  :
            M_scroll(E_MCORR, "Correlation of coefficients", coefs, coefs, NULL, 12, 12);
            break;
        case 5 :
            E_RES = M_alloc(11, E_NEQ);
            SCR_add_ptr(&E_txt, &E_ntxt,    "Correlation");
            SCR_add_ptr(&E_txt, &E_ntxt,    "St dev of residuals");
            SCR_add_ptr(&E_txt, &E_ntxt,    "Mean of YOBS");
            SCR_add_ptr(&E_txt, &E_ntxt,    "� Residuals�");
            SCR_add_ptr(&E_txt, &E_ntxt,    "Standard error");
            SCR_add_ptr(&E_txt, &E_ntxt,    "Standard error in %");
            SCR_add_ptr(&E_txt, &E_ntxt,    "F-stat");
            SCR_add_ptr(&E_txt, &E_ntxt,    "R�");
            SCR_add_ptr(&E_txt, &E_ntxt,    "R� adjusted");
            SCR_add_ptr(&E_txt, &E_ntxt,    "Durbin-Watson");
            SCR_add_ptr(&E_txt, &E_ntxt,    "Log Likelihood");
            SCR_add_ptr(&E_txt, &E_ntxt,    NULL);

            M_set(E_RES, E_MCORRU,      0, 0);
            M_set(E_RES, E_STDEV,       1, 0);
            M_set(E_RES, E_MEAN_Y,      2, 0);
            M_set(E_RES, E_SSRES,       3, 0);
            M_set(E_RES, E_STDERR,      4, 0);
            M_set(E_RES, E_STD_PCT,     5, 0);
            M_set(E_RES, E_FSTAT,       6, 0);
            M_set(E_RES, E_RSQUARE,     7, 0);
            M_set(E_RES, E_RSQUARE_ADJ, 8, 0);
            M_set(E_RES, E_DW,          9, 0);
            M_set(E_RES, E_LOGLIK,      10,0);

            M_scroll(E_RES, "Results by equation", E_txt, endos, NULL, 25, 12);
            SCR_free_tbl(E_txt);
            M_free(E_RES);
            E_txt = NULL;
            E_ntxt = 0;
            break;
        case 6 :
            o_estgr(E_ENDOS, E_SMPL, E_LHS, E_RHS, 1, 0);
            break;
        case 7 :
            o_estgr(E_ENDOS, E_SMPL, E_LHS, E_RHS, 1, 1);
            break;
        case 9 :                                /* JMP38 27-09-92 */
            PG_edit_window(vkp_eqs_gr);
            break;
        case 10:                                /* JMP38 27-09-92 */
            PG_edit_window(vkp_eqs_print);
            break;
        default :
            goto fin;
        }
    }
fin:
    SCR_free_tbl(endos);
    SCR_free_tbl(coefs);
    return(0);
}


/* Sub functions */
ODE_blk_init()
{
    if(OE_DBE == NULL) {
        OE_DBE = K_create(K_OBJ, UPPER_CASE);
        if(OE_DBE == NULL) goto err;
    }

    if(OE_DBS == NULL) {
        OE_DBS = K_create(SCALARS, LOWER_CASE);
        if(OE_DBS == NULL) {
            K_free(OE_DBE);
            OE_DBE = NULL;
            goto err;
        }
    }

    return(0);

err :
    B_seterrn(211);
    B_display_last_error();
    return(-1);
}

ODE_blk_check()
{
    int     i, j, pb, pos, nblk = 0, ntblk, rc = 0;
    char    *name = NULL, *buf = NULL,
             lec[K_MAX_NAME + 10]; /* IODE64K */
    char    **blk = NULL, **tblk = NULL;
    EQ      *eq;

    SW_nfree(OE_BLK);
    OE_BLK = NULL;
    OE_NB = 0;

    if(vkp_eqs_BLK != NULL && vkp_eqs_BLK[0] != '\0')
        buf = SCR_stracpy(vkp_eqs_BLK);
    else
        buf = SCR_stracpy("");

    A_SEPS = " ,;\n\t\r";
    SCR_upper(buf);
    tblk = A_init(buf);
    ntblk = SCR_tbl_size(tblk);
    if(tblk == NULL) {
        rc = -1;
        goto cleanup;
    }

    /* Suppress dbl names */
    for(i = 0; i < ntblk; i++) {
        for(j = 0; j < nblk; j++) {
            if(strcmp(tblk[i], blk[j]) == 0) break;
        }
        if(j == nblk) SCR_add_ptr(&blk, &nblk, tblk[i]);
    }

    SCR_add_ptr(&blk, &nblk, 0L);
    nblk = SCR_tbl_size(blk);
    SCR_free_tbl(tblk);

    /* Check names */
    for(i = 0; i < nblk; i++)         /* JMP 24-02-98 */
        if(K_key(blk[i], UPPER_CASE) < 0) {
            rc = -1;
            B_seterrn(213);
            goto cleanup;
        }

    /* make sure variable is in block ! */
    SCR_free(buf);
    buf = SCR_stracpy(vkp_eqs_NAME);
    SCR_strip(buf);
    for(i = 0; i < nblk; i++)  if(strcmp(blk[i], buf) == 0) break;
    if(i == nblk) {
        SCR_add_ptr(&blk, &nblk, buf);
        SCR_add_ptr(&blk, &nblk, NULL);
    }

    OE_NB = SCR_tbl_size(blk);
    if(OE_VEC != NULL) SW_nfree(OE_VEC);
    OE_VEC = (int *) SW_nalloc(OE_NB * sizeof(int));
    if(OE_VEC == NULL) {
        rc = -1;
        goto cleanup;
    }

    for(i = 0; i < OE_NB; i++) {
        name = SCR_stracpy(blk[i]);
        SCR_sqz(name);

        pb = K_find(OE_DBE, name);
        if(pb < 0) {
            pos = K_find(KE_WS, name);
            if(pos < 0)  {
                sprintf(lec, "%s := 0", name);
                ODE_blk_save_1(name, lec, " ", NULL);
            }
            else {
                eq = KEVAL(KE_WS, pos);
                ODE_blk_save_1(name, eq->lec, eq->cmt, eq->tests);
                E_free(eq);
            }
        }

        SCR_free(name);
    }

    /* save position of block elements */
    for(i = 0; i < OE_NB; i++) {
        name = SCR_stracpy(blk[i]);
        SCR_sqz(name);

        pb = K_find(OE_DBE, name);
        OE_VEC[i] = pb;

        SCR_free(name);
    }

    OE_BLK = SCR_mtov(blk, ';');
    SCR_strfacpy(&(vkp_eqs_BLK), OE_BLK);

    PG_display_flds(vkp_eqs);

cleanup:
    SCR_free(buf);
    SCR_free_tbl(blk);
    return(rc);
}

ODE_blk_cur()
{
    int     pos;
    char    *pack;

    if(OE_CUR >= OE_NB) OE_CUR = 0;

    pos = OE_VEC[OE_CUR];
    pack = KOVAL(OE_DBE, pos);

    strcpy(vkp_eqs_NAME, KONAME(OE_DBE, pos));
    SCR_strfacpy(&(vkp_eqs_LEC), P_get_ptr(pack, 0));
    SCR_strfacpy(&(vkp_eqs_CMT), P_get_ptr(pack, 1));
    ODE_blk_cur_tests();

    PG_display_flds(vkp_eqs);
    return(0);
}

ODE_blk_cur_tests()
{
    int     pos;
    char    *pack;
    float   *tests; /* FLOAT 12-04-98 */

    if(OE_CUR >= OE_NB) OE_CUR = 0;

    pos = OE_VEC[OE_CUR];
    pack = KOVAL(OE_DBE, pos);

    tests = P_get_ptr(pack, 3);
    if (tests == NULL) return(0); /* no tests */

    vkp_eqs_FSTAT   = tests[6];
    vkp_eqs_RSQUARE = tests[8];
    vkp_eqs_DW      = tests[9];
    vkp_eqs_LLIK    = tests[10];
    return(0);
}

ODE_blk_cur_atests()
{
    int     i, pos, len;
    char    *pack, *npack;
    float   *tests, ctests[EQS_NBTESTS]; /* FLOAT 12-04-98 */

    if(OE_CUR >= OE_NB) OE_CUR = 0;

    for(i = 0; i < OE_NB; i++) {
        pos = OE_VEC[i];
        pack = KOVAL(OE_DBE, pos);

        memset(ctests, 0, EQS_NBTESTS * sizeof(float)); /* FLOAT 12-04-98 */
        ctests[0] = MATE(E_MCORRU,      0, i);
        ctests[1] = MATE(E_STDEV,       0, i);
        ctests[2] = MATE(E_MEAN_Y,      0, i);
        ctests[3] = MATE(E_SSRES,       0, i);
        ctests[4] = MATE(E_STDERR,      0, i);
        ctests[5] = MATE(E_STD_PCT,     0, i);
        ctests[6] = MATE(E_FSTAT,       0, i);
        ctests[7] = MATE(E_RSQUARE,     0, i);
        ctests[8] = MATE(E_RSQUARE_ADJ, 0, i);
        ctests[9] = MATE(E_DW,          0, i);
        ctests[10]= MATE(E_LOGLIK,      0, i);

        tests = P_get_ptr(pack, 3);
        if (tests == NULL) {
            len = P_len(pack);
            npack = SW_nalloc(len);
            memcpy(npack, pack, len);

            npack = P_add(npack, ctests, EQS_NBTESTS * sizeof(float)); /* JMP 12-04-98 */
            len = P_len(npack);
            K_add(OE_DBE, KONAME(OE_DBE, pos), npack, &len);

            SW_nfree(npack);
        }
        else
            memcpy(tests, ctests, EQS_NBTESTS * sizeof(float)); /* FLOAT 12-04-98 */
    }
    return(0);
}


int ODE_blk_save_1(name, lec, cmt, tests)
char    *name, *lec, *cmt;
float   *tests;
{
    int     pos, len;
    char    *pack = NULL;
    CLEC    *clec = NULL, cdummy;
    float   tdummy[EQS_NBTESTS]; /* FLOAT 12-04-98 */

    pack = P_create();
    pack = P_add(pack, lec, strlen(lec) + 1);
    pack = P_add(pack, cmt, strlen(cmt) + 1);

    clec = L_solve(lec, name);
    if(clec == NULL) {
        memset(&cdummy, 0, sizeof(CLEC));
        pack = P_add(pack, (char *) &cdummy, sizeof(CLEC));
    }
    else pack = P_add(pack, (char *) clec, clec->tot_lg);

    if(tests != NULL) pack = P_add(pack, tests, EQS_NBTESTS * sizeof(float));  /* FLOAT 12-04-98 */

    len = P_len(pack);
    pos = K_add(OE_DBE, name, pack, &len);

    SW_nfree(pack);
    SW_nfree(clec);
    return(pos);
}

ODE_blk_save_cur()
{
    int     pos;
    char    *lec, *cmt, *pack;
    float   *tests, ctests[EQS_NBTESTS]; /* FLOAT 12-04-98 */

    if(OE_VEC == 0) {
        B_seterror("Equation empty");
        B_display_last_error();
        return(-1);
    }

    pos = OE_VEC[OE_CUR];
    lec = SCR_stracpy(vkp_eqs_LEC);
    if(lec == 0) {
        B_seterror("Equation empty");
        B_display_last_error();
        return(-1);
    }
    cmt = SCR_stracpy(vkp_eqs_CMT);

    /* tests */
    pack = KOVAL(OE_DBE, pos);
    tests = P_get_ptr(pack, 3);
    if(tests == NULL) memset(ctests, 0, EQS_NBTESTS * sizeof(float)); /* FLOAT 12-04-98 */
    else memcpy(ctests, tests, EQS_NBTESTS * sizeof(float)); /* FLOAT 12-04-98 */

    OE_VEC[OE_CUR] = ODE_blk_save_1(KONAME(OE_DBE, pos), lec, cmt, ctests);
    SCR_free(lec);
    SCR_free(cmt);
    return(0);
}

ODE_blk_coef()
{
    int     i, j, pb, pos, len, nbc = 0;
    char    *name, *pack, *lec;
    CLEC    *clec;
    SCL     scl;

    for(i = 0; i < OE_NB; i++) {
        pack = KOVAL(OE_DBE, OE_VEC[i]);
        len = P_get_len(pack, 2);
        clec = (CLEC *) SW_nalloc(len);
        memcpy(clec, (CLEC *) P_get_ptr(pack, 2), len);
        if(clec->tot_lg == 0) {
            B_seterrn(212, KONAME(OE_DBE, OE_VEC[i]), L_error());
            B_display_last_error();

            OE_CUR = i;
            ODE_blk_cur();
            SW_nfree(clec);
            return(-1);
        }
        for(j = 0 ; j < clec->nb_names ; j++) {
            name = clec->lnames[j].name;
            if(L_ISCOEF(name)) {
                nbc ++;
                pb = K_find(OE_DBS, name);
                if(pb < 0) {
                    pos = K_find(KS_WS, name);
                    if(pos < 0) K_add(OE_DBS, name, NULL);
                    else {
                        memcpy(&scl, KSVAL(KS_WS, pos), sizeof(SCL));
                        K_add(OE_DBS, name, &scl);
                    }
                }
            }
        }
        SW_nfree(clec);
    }

    /* del scalars of temp. omitted eqs */
    for(i = 0; i < KNB(OE_DBE) && nbc < KNB(OE_DBS); i++) {
        for(j = 0; j < OE_NB; j++) if(i == OE_VEC[j]) break;
        if(j != OE_NB) continue; /* eqs not omitted */

        /* delete scalars of omitted eqs */
        pack = KOVAL(OE_DBE, i);
        len = P_get_len(pack, 2);
        clec = (CLEC *) SW_nalloc(len);
        memcpy(clec, (CLEC *) P_get_ptr(pack, 2), len);

        if(*(char *) clec == 0) {
            SW_nfree(clec);
            continue;
        }

        for(j = 0 ; j < clec->nb_names ; j++) {
            name = clec->lnames[j].name;
            if(L_ISCOEF(name)) {
                pb = K_find(OE_DBS, name);
                if(pb < 0) continue;
                K_del(OE_DBS, pb);
            }
        }

        SW_nfree(clec);
    }
    return(0);
}

ODE_blk_save()
{
    int     i, len, pos;
    char    *pack, *name;
    SCL     *scl;
    EQ      *eq;
    SAMPLE  *smpl;

    for(i = 0; i < OE_NB; i++) {
        pos = OE_VEC[i];
        name = KONAME(OE_DBE, pos);
        pack = KOVAL(OE_DBE, pos);

        if(OE_DONE) {
            B_DataUpdateEqs(name, P_get_ptr(pack, 0), P_get_ptr(pack, 1),
                            OE_METHOD, &(OE_SMPL),
                            OE_INSTR, OE_BLK, (float *) P_get_ptr(pack, 3), 1); /* FLOAT 12-04-98 */


            /* GB 27/07/98 */
            eq = KEVAL(K_WS[EQUATIONS], K_find(K_WS[EQUATIONS], name));
            E_tests2scl(eq, i, E_T, E_NCE);
        }
        else {
            smpl = PER_atosmpl(vkp_eqs_FROM, vkp_eqs_TO);
            if(smpl == NULL) smpl = &(OE_SMPL);
            B_DataUpdateEqs(name, P_get_ptr(pack, 0), P_get_ptr(pack, 1),
                            OE_METHOD, smpl,
                            OE_INSTR, OE_BLK, NULL, 0);
            if(smpl != &(OE_SMPL)) SW_nfree(smpl);
        }
        /*
        	    B_DataUpdateEqs(name, P_get_ptr(pack, 0), P_get_ptr(pack, 1),
        		OE_METHOD, &(OE_SMPL),
        		OE_INSTR, OE_BLK, NULL, 0);
        */
    }

    K_merge(KS_WS, OE_DBS, 1);
    ODE_blk_end_fn();
    return(0);
}


ODE_blk_instr()
{
    char    **instr;
    CLEC    *clec;
    int     i, ni;

    SW_nfree(OE_INSTR);
    OE_INSTR = SCR_stracpy(vkp_eqs_INSTRS);

    instr = SCR_vtom(OE_INSTR, ';');
    ni = SCR_tbl_size(instr);
    for(i = 0; i < ni; i++) {
        clec = L_cc(instr[i]);
        if(clec == 0) {
            B_seterrn(213, instr[i], L_error());
            B_display_last_error();

            SCR_free(instr);
            SW_nfree(OE_INSTR);
            OE_INSTR = NULL;
            return(-1);
        }
    }

    SCR_free(instr);
    return(0);
}

int ODE_blk_est()
{
    char    **endos, **lecs, **instrs,
            *pack, file[20], met;
    int     i, n,
            nl = 0, error = 0;

    OE_DONE = 0;
    E_free_work();

    endos = SCR_vtom(OE_BLK, ';');
    instrs = SCR_vtom(OE_INSTR, ';');
    for(i = 0; i < OE_NB; i++) {
        pack = KOVAL(OE_DBE, OE_VEC[i]);
        SCR_add_ptr(&lecs, &nl, P_get_ptr(pack, 0));
    }
    SCR_add_ptr(&lecs, &nl, NULL);

    error = E_est(endos, lecs, KV_WS, OE_DBS, &(OE_SMPL), OE_METHOD,
                  instrs, OE_MAXIT, OE_EPS);

    if(error == 0) OE_DONE = 1;
    else           B_display_last_error();

    /*
        SCR_free_tbl(endos);
        SCR_free_tbl(lecs);
        SCR_free_tbl(instrs);
    */
    return(0);
}

ODE_blk_unitroot()
{
    SB_StatUnitRoot();
    OE_DONE = 0;
    return(0);
}

ODE_blk_gr_print()
{
    B_LANG = vkp_eqs_gr_LANG;
    if(B_dest == 1)
        if(PG_edit_window(vkp_fprintfile) < 0) return(-1);

    if(B_PrintDest(B_outfile) < 0) return(-1);

    o_estgr(E_ENDOS, E_SMPL, E_LHS, E_RHS, 0, vkp_eqs_gr_TYPE);

    W_close();
    return(0);
}

ODE_blk_print_res()
{
    if(C_PrintDestDecLang(0, -1, (int) vkp_eqs_print_LANG)) /* GB 08-97 */
        return(-1);

    E_print_results((int)vkp_eqs_print_CORR,
                    (int)vkp_eqs_print_CORRU,
                    (int)vkp_eqs_print_OBS,
                    (int)vkp_eqs_print_GROBS,    /* JMP 23-03-98 */
                    (int)vkp_eqs_print_GRRES);   /* JMP 23-03-98 */

    /*    if(vkp_eqs_print_GROBS) o_estgr(E_ENDOS, E_SMPL, E_LHS, E_RHS, 0, 0);
        if(vkp_eqs_print_GRRES) o_estgr(E_ENDOS, E_SMPL, E_LHS, E_RHS, 0, 1);
    */
    W_close();
    return(0);
}


ODE_blk_dadj_fn(eqs)
char    *eqs;
{
    if(ODE_blk_save_cur()) return(-1);

    SCR_strfacpy(&(vkp_eqsadj_LEC), vkp_eqs_LEC);
    PG_edit_window(vkp_eqsadj);
    SCR_strfacpy(&(vkp_eqs_LEC), vkp_eqsadj_LEC);

    PG_display_flds(vkp_eqs);
    return(0);
}




