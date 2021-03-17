#include "iode.h"


int     E_errno, E_IT, E_MAXIT, E_CONV,
        E_NEQ, E_NCE, E_NC, E_NINSTR, E_T, E_FROM,
        *E_C_NBS;
KDB     *E_DBV, *E_DBS;
SAMPLE  *E_SMPL;
char    E_MET; IODE_REAL    E_CONV_TEST, E_EPS;
CLEC    **E_CRHS;
char    **E_LECS, **E_INSTRS, **E_ENDOS;

MAT     *E_RHS,
        *E_LHS,
        *E_U,
        *E_G,
        *E_VCC,
        *E_VCCTMP,
        *E_M,
        *E_MTMP,
        *E_MTMPP,
        *E_D,
        *E_C,
        *E_SMO,
        *E_NBCE,
        *E_VCU,
        *E_IVCU,
        *E_MCU,
        *E_GMU,
        *E_dC,
        *E_DF,
        *E_UM,
        *E_UMT,
        *E_UMTMP,
        *E_UVCCTMP,
        *E_GMUTMP,
        *E_SSRES,
        *E_RSQUARE_ADJ,
        *E_DW,
        *E_LOGLIK,
        *E_STDERR,
        *E_MEAN_Y,
        *E_STDEV,
        *E_RSQUARE,
        *E_FSTAT,
        *E_STD_PCT,
        *E_MCORR,
        *E_MCORRU,
        *E_DEV;

char    *E_ERRORS[] = {
    "E_NO_EQ_ERR"   ,
    "E_MEM_ERR"     ,
    "E_SYNTAX_ERR"  ,
    "E_LINK_ERR"    ,
    "E_DREG_ERR"    ,
    "E_NAN_ERR"     ,
    "E_VCC_SING_ERR",
    "E_VCU_SING_ERR",
    "E_GMG_SING_ERR"
};

/* =+===== E_PREP ================================================
   Construct list of series, lags and leads, max and min period
	from the equation list.
   Check for compatibility with WS data.
   Computes the instruments.
   Computes LHS
   Compile RHS

	ew : IO : struct est_work
   =============================================================== */

E_prep(lecs, instrs)
char    **lecs, **instrs;
{
    E_prep_reset();
    if(E_prep_lecs(lecs)) return(-1);
    if(E_prep_instrs(instrs)) return(-1);
    if(E_prep_coefs()) return(-1);
    if(E_prep_alloc()) return(-1);
    return(0);
}

E_prep_lecs(lecs)
char    **lecs;
{
    int     i, pos, t;
    CLEC    *clec = 0;
    double  x;

    E_NEQ = SCR_tbl_size(lecs);
    if(E_NEQ < 1) return(E_errno = E_NO_EQ_ERR);

    E_LHS = M_alloc(E_NEQ, E_T);
    if(E_LHS == 0) return(E_errno = E_MEM_ERR);
    E_CRHS = (CLEC **) SW_nalloc(E_NEQ * sizeof(CLEC *));
    if(E_CRHS == 0) return(E_errno = E_MEM_ERR);
    for(i = 0 ; i < E_NEQ ; i++) {
        pos = L_split_eq(lecs[i]);
        if(pos < 0) return(E_errno = E_SYNTAX_ERR);
        lecs[i][pos] = 0;
        clec = L_cc(lecs[i]);
        if(clec == 0) return(E_errno = E_SYNTAX_ERR);
        if(E_add_scls(clec, E_DBS)) return(E_errno = E_LINK_ERR); // JMP 13/11/2012
        if(L_link(E_DBV, E_DBS, clec) != 0) {
            SW_nfree(clec); // GB 14/11/2012
            return(E_errno = E_LINK_ERR);
        }
        for(t = 0 ; t < E_T ; t++) {
            x = L_exec(E_DBV, E_DBS, clec, t + E_FROM);
            if(!L_ISAN(x)) {
                SW_nfree(clec); // GB 14/11/2012
                return(E_errno = E_NAN_ERR);
            }
            MATE(E_LHS, i, t) = x;
        }
        SW_nfree(clec); // GB 14/11/2012
        lecs[i][pos] = ':';
        E_CRHS[i] = L_cc(lecs[i] + pos + 2);
        if(E_CRHS[i] == 0) return(E_errno = E_SYNTAX_ERR);
        if(E_add_scls(E_CRHS[i], E_DBS)) return(E_errno = E_LINK_ERR); // JMP 13/11/2012
        if(L_link(E_DBV, E_DBS, E_CRHS[i]) != 0) return(E_errno = E_LINK_ERR);
    }

    return(0);
}

// Creates scalars if needed
E_add_scls(CLEC *clec, KDB *dbs)
{
    int         j, pos;
    char        *name;

    for(j = 0 ; j < clec->nb_names ; j++) {
        name = clec->lnames[j].name;
        if(L_ISCOEF(name) && K_find(dbs, name) < 0)
            K_add(dbs, name, NULL);
    }
    return(0);
}


E_prep_instrs(instrs)
char    **instrs;
{
    int     i, pos, t;
    CLEC    *clec = 0;
    MAT     *minstr = 0, *mip = 0, *miip = 0, *miipi = 0, *mipiipi = 0;
    double  x;

    if(E_MET != 2 && E_MET != 3) return(0);
    E_NINSTR = SCR_tbl_size(instrs);
    if(E_NINSTR < 1) return(0);

    minstr  = M_alloc(E_T, E_NINSTR + 1);
    mip     = M_alloc(E_NINSTR + 1, E_T);
    miip    = M_alloc(E_NINSTR + 1, E_NINSTR + 1);
    miipi   = M_alloc(E_NINSTR + 1, E_NINSTR + 1);
    mipiipi = M_alloc(E_T, E_NINSTR + 1);
    E_D     = M_alloc(E_T, E_T);

    if(minstr == 0 || mip == 0 || miip == 0 ||
            miipi == 0 || mipiipi == 0 || E_D == 0) {
        E_errno = E_MEM_ERR;
        goto fin;
    }
    for(i = 0 ; i < E_T ; i++) MATE(minstr, i, 0) = 1.0;
    for(i = 0 ; i < E_NINSTR ; i++) {
        clec = L_cc(instrs[i]);
        if(clec == 0) {
            E_errno = E_SYNTAX_ERR;
            goto fin;
        }
        if(L_link(E_DBV, E_DBS, clec) != 0) {
            SW_nfree(clec);
            E_errno = E_LINK_ERR;
            goto fin;
        }
        for(t = 0 ; t < E_T ; t++) {
            x = L_exec(E_DBV, E_DBS, clec, t + E_FROM);
            if(!L_ISAN(x)) {
                SW_nfree(clec);
                E_errno = E_NAN_ERR;
                goto fin;
            }
            MATE(minstr, t, i + 1) = x;
        }
        SW_nfree(clec);
    }

    M_xprimx(miip, minstr);
    M_inv_1(miipi, miip);
    if(M_errno != 0) {
        E_errno = E_DREG_ERR;
        goto fin;
    }
    M_trans(mip, minstr);
    M_prod(mipiipi, minstr, miipi);
    M_prod(E_D, mipiipi, mip);

fin:
    M_free(minstr );
    M_free(mip    );
    M_free(miip   );
    M_free(miipi  );
    M_free(mipiipi);
    return(E_errno);
}

E_prep_coefs()
{
    CLEC    *clec;
    int     i, j, pos, k;

    E_NC = E_NCE = 0;
    for(i = 0 ; i < E_NEQ ; i++) E_NC += E_CRHS[i]->nb_names;
    E_C_NBS = (int *)SW_nalloc(E_NC * sizeof(int));
    if(E_C_NBS == 0) {
        E_errno = E_MEM_ERR;
        return(-1);
    }
    E_NC = 0;
    E_NBCE = M_alloc(1, E_NEQ);
    for(i = 0 ; i < E_NEQ ; i++) {
        clec = E_CRHS[i];
        for(j = 0 ; j < clec->nb_names ; j++) {
            if(L_ISCOEF(clec->lnames[j].name)) {
                pos = clec->lnames[j].pos;
                for(k = 0 ; k < E_NC ; k++)
                    if(E_C_NBS[k] == pos) break;
                if(k == E_NC) {
                    E_C_NBS[E_NC++] = pos;
                    if(KSVAL(E_DBS, pos)->relax > 0) E_NCE++;
                }
                if(KSVAL(E_DBS, pos)->relax > 0) MATE(E_NBCE, 0, i)++;
            }
        }
    }

    if(E_NCE == 0) { /* GB 26/02/97 */
        B_seterrn(77);
        E_errno = E_NO_SCALARS;
        return(-1);
    }

    E_C   = M_alloc(E_NC, 1);
    E_SMO = M_alloc(E_NC, 1);
    E_get_SMO();
    E_get_C();
    return(0);
}

E_get_C()
{
    int     i;
    IODE_REAL    c;

    for(i = 0 ; i < E_NC ; i++) {
        c = KSVAL(E_DBS, E_C_NBS[i])->val;
        if(KSVAL(E_DBS, E_C_NBS[i])->relax != 0.0 && fabs(c) < 1e-15) {
        	c = 0.1;
        	KSVAL(E_DBS, E_C_NBS[i])->val = c; // GB 24/01/2013
		}
        MATE(E_C, i, 0) = c;
    }
    return(0);
}

E_put_C()
{
    int     i;

    for(i = 0 ; i < E_NC ; i++)
        KSVAL(E_DBS, E_C_NBS[i])->val = MATE(E_C, i, 0);
    return(0);
}

E_get_SMO()
{
    int     i;

    for(i = 0 ; i < E_NC ; i++)
        MATE(E_SMO, i, 0) = KSVAL(E_DBS, E_C_NBS[i])->relax;

    return(0);
}

E_prep_alloc()
{
    E_U       = M_alloc(E_NEQ, E_T);
    E_VCU     = M_diag(E_NEQ, 1.0);
    E_IVCU    = M_diag(E_NEQ, 1.0);
    E_RHS     = M_alloc(E_NEQ, E_T);
    E_MCU     = M_alloc(E_NEQ, E_NEQ);
    E_G       = M_alloc(E_NCE, E_T * E_NEQ);
    E_VCC     = M_alloc(E_NCE, E_NCE);
    E_GMU     = M_alloc(E_NCE, 1);
    E_dC      = M_alloc(E_NCE, 1);
    E_M       = M_alloc(E_T, E_NCE);
    E_MTMP    = M_alloc(E_T, E_NCE);
    E_MTMPP   = M_alloc(E_NCE, E_T);
    E_VCCTMP  = M_alloc(E_NCE, E_NCE);
    E_UM      = M_alloc(E_T, 1);
    E_UMT     = M_alloc(1, E_T);
    E_UMTMP   = M_alloc(E_T, 1);
    E_UVCCTMP = M_alloc(E_NCE, E_T);
    E_GMUTMP  = M_alloc(E_NCE, 1);

    E_DF      = M_alloc(1, E_NCE);
    E_STDERR  = M_alloc(1, E_NEQ);
    E_SSRES   = M_alloc(1, E_NEQ);
    E_MEAN_Y  = M_alloc(1, E_NEQ);
    E_STDEV   = M_alloc(1, E_NEQ);
    E_RSQUARE = M_alloc(1, E_NEQ);
    E_RSQUARE_ADJ
        = M_alloc(1, E_NEQ);
    E_DW      = M_alloc(1, E_NEQ);
    E_FSTAT   = M_alloc(1, E_NEQ);
    E_LOGLIK  = M_alloc(1, E_NEQ);
    E_STD_PCT = M_alloc(1, E_NEQ);
    E_MCORR   = M_alloc(E_NCE, E_NCE);
    E_MCORRU  = M_alloc(E_NEQ, E_NEQ);
    E_DEV     = M_alloc(E_NEQ, E_T);

    if(M_errno) E_errno = E_MEM_ERR;
    return(E_errno);
}

E_free_work()
{
    int     i;

    if(E_CRHS) {
        for(i = 0 ; i < E_NEQ ; i++) SW_nfree(E_CRHS[i]);
        SW_nfree(E_CRHS);
    }
    SW_nfree(E_C_NBS);

    M_free(E_RHS);
    M_free(E_LHS);
    M_free(E_U);
    M_free(E_G);
    M_free(E_VCC);
    M_free(E_VCCTMP);
    M_free(E_M);
    M_free(E_MTMP);
    M_free(E_MTMPP);
    M_free(E_D);
    M_free(E_C);
    M_free(E_SMO);
    M_free(E_NBCE);
    M_free(E_VCU);
    M_free(E_IVCU);
    M_free(E_MCU);
    M_free(E_GMU);
    M_free(E_dC);
    M_free(E_DF);
    M_free(E_RSQUARE_ADJ);
    M_free(E_DW);
    M_free(E_LOGLIK);
    M_free(E_SSRES);
    M_free(E_STDERR);
    M_free(E_MEAN_Y);
    M_free(E_STDEV);
    M_free(E_RSQUARE);
    M_free(E_FSTAT);
    M_free(E_STD_PCT);
    M_free(E_MCORR);
    M_free(E_MCORRU);
    M_free(E_DEV);
    M_free(E_UM);
    M_free(E_UMT);
    M_free(E_UMTMP);
    M_free(E_UVCCTMP);
    M_free(E_GMUTMP);

    SCR_free_tbl(E_ENDOS);
    SCR_free_tbl(E_LECS);
    SCR_free_tbl(E_INSTRS);
    E_ENDOS = 0;
    E_LECS = 0;
    E_INSTRS = 0;

    E_prep_reset();
    return(0);
}

E_prep_reset()
{
    E_CRHS = 0;
    E_C_NBS = 0;

    E_NINSTR = 0;

    E_RHS = 0;
    E_LHS = 0;
    E_U = 0;
    E_G = 0;
    E_VCC = 0;
    E_VCCTMP = 0;
    E_M = 0;
    E_MTMP = 0;
    E_MTMPP = 0;
    E_D = 0;
    E_C = 0;
    E_SMO = 0;
    E_NBCE = 0;
    E_VCU = 0;
    E_IVCU = 0;
    E_MCU = 0;
    E_GMU = 0;
    E_dC = 0;
    E_DF = 0;
    E_SSRES = 0;
    E_RSQUARE_ADJ = 0;
    E_DW = 0;
    E_LOGLIK = 0;
    E_STDERR = 0;
    E_MEAN_Y = 0;
    E_STDEV = 0;
    E_RSQUARE = 0;
    E_FSTAT = 0;
    E_STD_PCT = 0;
    E_MCORR = 0;
    E_MCORRU = 0;
    E_DEV = 0;
    E_UM = 0;
    E_UMT = 0;
    E_UMTMP = 0;
    E_UVCCTMP = 0;
    E_GMUTMP = 0;
    return(0);
}






