#include "iode.h"

/* ===================================================================
   Estimation of an equation system
   return : 0 or -1
   =============================================================== */

E_est(endos, lecs, dbv, dbs, smpl, met, instrs, maxit, eps)
char    **endos, **lecs, **instrs, met;
SAMPLE  *smpl;
KDB     *dbv, *dbs;
int     maxit;
double  eps;
{
    int     rc = 0;

    E_errno = 0;
    M_errno = 0;
    E_DBV  = dbv;
    E_DBS  = dbs;
    E_SMPL = smpl;
    E_MET  = met;
    E_EPS  = eps;
    E_MAXIT= maxit;
    E_LECS = lecs;
    E_INSTRS = instrs;
    E_ENDOS = endos;
    E_FROM = PER_diff_per(&(smpl->s_p1), &(KSMPL(dbv)->s_p1));
    E_T    = smpl->s_nb;

    if(E_prep(lecs, instrs) == 0 && E_gls() == 0) E_output();
    if(E_errno != 0) E_error_n(E_errno);
    return(E_errno);
}


/* =+==== E_GLS =========================================================
   Estimation of an equation system by different methods :
	LSQ, IV, 3SLS, ZELLNER
   ====================================================================== */

E_gls()
{
    int         step = 0;
    int         conv = 0;

    E_IT = 0;
    E_CONV = 0;
    E_CONV_TEST = 9999.99;

again: /* second step for Zellner and 3 stages methods */
    while(conv == 0 && E_IT < E_MAXIT) {
	if(E_residuals() || E_jacobian()) goto err;
	if(E_MET == 4) {
	    if(E_c_vcu() || E_c_ivcu()) goto err;
	    }
	if(E_c_gmg() || E_c_gmu() || E_deltac()) goto err;
	E_IT++;
	conv = E_testcv();
	E_msg("Estimating : iteration %d (||eps|| = %g)", E_IT, E_CONV_TEST);
	if(E_adaptcoef()) goto err;
	}

    E_c_gmg(); /* TMP ??? */
    if(E_residuals() || E_c_vcu() || E_c_ivcu()) goto err;

    switch(E_MET) {
	case 3 : /* 3 stages */
	case 1 : /* Zellner */
	    if(step == 0) {
		if(E_c_mcu()) goto err;
		conv = 0;
		step = 1;
		goto again; /* next step for Z and 3 */
		}
	    if(E_c_ivcc()) goto err;
	    break;

	default : /* LSQ, IV */
	    if(E_c_mcu() || E_c_vcc()) goto err;
	    break;
	}

    if(E_IT < E_MAXIT && conv != 0) E_CONV = 1;
    else E_CONV = 0;
    E_msg("Solution%s reached after %d iteration(s). Creating results file ...",
			((E_CONV == 0) ? " not" : ""), E_IT);
    return((E_CONV == 1) ? 0 : -1);

err :
    E_msg("Estimation not possible: Error %d", E_errno);
    return(-1);
}

E_residuals()
{
    int         i;
    int         nb_elem;

    /* COMPUTE RHS */
    if(E_c_rhs() != 0) return(-1);
    M_calc(E_U, E_LHS, E_RHS, '-');
    return(0);
}

double E_rhs_ij(i, j)
int     i, j;
{
    return(L_exec(E_DBV, E_DBS, E_CRHS[i], j + E_FROM));
}

E_c_rhs()
{
   int      i, j;
   double   x;

    for(i = 0 ; i < E_NEQ ; i++) {
	for(j = 0 ; j < E_T ; j++) {
	    x = E_rhs_ij(i, j);
	    if(L_ISAN(x)) MATE(E_RHS, i, j) = x;
	    else  {
		E_errno = E_NAN_ERR;
		return(-1);
		}
	    }
	}

    return(0);
}

E_jacobian()
{
    double  h = 1e-4, oldc;
    int     i, j;

    for(i = 0, j = 0 ; i < E_NC ; i++) {
	if(KSVAL(E_DBS, E_C_NBS[i])->relax != 0) {      // Uniquement pour les coef estimés (relax <> 0)
	    oldc = KSVAL(E_DBS, E_C_NBS[i])->val;       // Stocke l'ancienne valeur du coef
	    if(fabs(oldc) < 1e-15) oldc = 0.1;          //   ou 0.1 si coef proche de nul
	    KSVAL(E_DBS, E_C_NBS[i])->val = oldc * (1.0 + h); // coef augmenté de h pourcents
	    if(0 != E_mod_residuals(i, j, oldc * h)) {  /* compute G : (NCE, T*N) */
		// PROBLEME : reset et sort avec -1
		KSVAL(E_DBS, E_C_NBS[i])->val = oldc;   // remet l'ancienne valeur du coef
		return(-1);
		}
	    KSVAL(E_DBS, E_C_NBS[i])->val = oldc;       /* reset coef */
	    j++;
	    }
	}

    return(0);
}

E_scl_in_eq(coef_nb, eq_nb)
int     coef_nb, eq_nb;
{
    CLEC    *clec = E_CRHS[eq_nb];
    int    j;

    for(j = 0 ; j < clec->nb_names ; j++)
	if(L_ISCOEF(clec->lnames[j].name) &&
	   E_C_NBS[coef_nb] == clec->lnames[j].pos) return(1);

    return(0);
}

E_mod_residuals(i_nb, j_nb,  h)
int             i_nb, j_nb;
double          h;
{
    int     i, j;
    double  x;

    // Pour tte équation
    for(i = 0 ; i < E_NEQ ; i++) {
	if(E_scl_in_eq(i_nb, i)) {
	    // Si le scalaire i est dans l'éq : calculer le RHS (pour chaque année)
	    //  et sauver la dérivée dans la matrice E_G  : f(x + h) - f(x) ) / h
	    for(j = 0 ; j < E_T ; j++) {
		x = E_rhs_ij(i, j);
		if(x >= MAXFLOAT) x = L_NAN;
		if(L_ISAN(x))
		    MATE(E_G, j_nb, i * E_T + j) =
			(x - MATE(E_RHS, i, j)) / h;
		else  {
		    E_errno = E_NAN_ERR;
		    return(-1);
		    }
		}
	    }
	else
	    // Sinon, placer des 0 dans la matrice E_G pour le coef en question
	    for(j = 0 ; j < E_T ; j++) MATE(E_G, j_nb, i * E_T + j) = 0;
	}

    return(0);
}

E_c_gmg()
{
    int i, j;

    M_clear(E_VCC);
    for(i = 0 ; i < E_NEQ  ; i++) {
	M_clear(E_M);                                   /* M = 0 */
	for(j = 0 ; j < E_NEQ ; j++) {
	    M_extr(E_MTMPP, E_G, 0, E_T * j, E_NCE, E_T); /* MTMP = G[..] */
	    M_trans(E_MTMP, E_MTMPP);        /* MTMP = MTMP' */
	    M_scale(E_MTMP, E_MTMP, MATE(E_IVCU, i, j));/* MTMP=MTMP*IVCU[..] */
	    M_calc(E_M, E_M, E_MTMP, '+');              /* M = M + ,MTMP */
	    }
	if(E_NINSTR > 1) M_prod(E_MTMP, E_D, E_M);      /* MTMP = D.M */
	else M_copy(E_MTMP, E_M);

	M_extr(E_MTMPP, E_G, 0, E_T * i, E_NCE, E_T);    /* M = G[..] */
	M_prod(E_VCCTMP, E_MTMPP, E_MTMP);               /* VCCTMP = M.MTMP */
	M_calc(E_VCC, E_VCCTMP, E_VCC, '+');             /* VCC=VCCTMP+VCC */
	}

    return(0);
}

E_c_gmu()
{
    int     i, j;

    M_clear(E_GMU);
    for(i = 0 ; i < E_NEQ  ; i++) {
	M_clear(E_UM);                                    /* UM = 0 */
	for(j = 0 ; j < E_NEQ ; j++) {
	    M_extr(E_UMT, E_U, j, 0, 1, E_T);        /* UMTMP = U[..] */
	    M_trans(E_UMTMP, E_UMT);
	    M_scale(E_UMTMP, E_UMTMP, MATE(E_IVCU, i, j));/* UMTMP=UMTMP*IVCU[..] */
	    M_calc(E_UM, E_UM, E_UMTMP, '+');             /* UM = UM + ,UMTMP */
	    }

	if(E_NINSTR >= 1) M_prod(E_UMTMP, E_D, E_UM);     /* UMTMP = D.UM */
	else M_copy(E_UMTMP, E_UM);

	M_extr(E_MTMPP, E_G, 0, E_T * i, E_NCE, E_T);   /* M = G[..] */
	M_prod(E_GMUTMP, E_MTMPP, E_UMTMP);            /* UVCCTMP = M.UMTMP */
	M_calc(E_GMU, E_GMUTMP, E_GMU, '+');           /* GMU=UVCCTMP+GMU */

	}
    return(0);
}

E_deltac()
{

    M_solve(E_dC, E_VCC, E_GMU);
    if(M_errno) return(E_errno = E_VCC_SING_ERR);
    return(0);
}

E_testcv()
{
    int         i, j;
    double      sum = 0, tmp, ci, dci;

    E_CONV = 0;
    E_get_C();
    for(i = 0, j = 0 ; i < E_NC ; i++) {
	if(MATE(E_SMO, i, 0) == 0) continue; /* relax == 0 */

	ci  = MATE(E_C, i, 0);
	dci = MATE(E_dC, j, 0);
	if(ci != 0) {
	    tmp = fabs(dci / ci);
	    if(tmp < fabs(ci)) sum += tmp * tmp;
	    else sum += ci * ci;
	    }
	j++;
	}
    E_CONV_TEST = sqrt(sum);
    if(E_CONV_TEST <= E_EPS) E_CONV = 1;
    return(E_CONV);
}

E_adaptcoef()
{
    int i, j;

    for(i = 0, j = 0; i < E_NC; i++) {
	if(MATE(E_SMO, i, 0) == 0) continue;

	/* C = C + dC * lambda */
	MATE(E_C, i, 0) += MATE(E_dC, j, 0) * MATE(E_SMO, i, 0);
	j++;
    }
    E_put_C();
    return(0);
}

E_c_vcu()
{
    M_xxprim(E_VCU, E_U);
    M_scale(E_VCU, E_VCU, 1.0 / E_T);
    return(0);
}

E_c_ivcu()
{
    M_inv_1(E_IVCU, E_VCU);
    if(M_errno) return(E_errno = E_VCU_SING_ERR);
    return(0);
}

E_c_mcu()
{
    int     i, j;
    double  x;

    M_clear(E_MCU);
    for(i = 0 ; i < E_NEQ ; i++)
	for(j = 0 ; j <= i ; j++) {
	    x = sqrt(MATE(E_VCU, i, i) * MATE(E_VCU, j, j));
	    if(!L_IS0(x))
		MATE(E_MCU, i, j) = MATE(E_MCU, j, i) = MATE(E_VCU, i, j) / x;
	    }

    return(0);
}

E_c_vcc()
{
    int         i;

    M_clear(E_IVCU);
    /*
    for(i = 0 ; i < E_NEQ ; i++)
	MATE(E_IVCU, i, i) = 1.0 / MATE(E_VCU, i, i);
    */
    E_c_ivcu();
    E_c_gmg();
    if(E_c_ivcc()) return(E_errno);
    E_c_vcu();
    return(0);
}

E_c_ivcc()
{
    M_inv_1(E_VCCTMP, E_VCC);
    if(M_errno != 0) return(E_errno = E_VCC_SING_ERR);
    M_copy(E_VCC, E_VCCTMP);
    return(0);
}
/*
E_print_mat(tit, m)
char    *tit;
MAT     *m;
{
    int     i, j;

    printf("\n%s - [%d,%d]\n", tit, M_NL(m), M_NC(m));
    for(i = 0 ; i < M_NL(m) ; i++) {
	printf("\n%3d : ", i);
	for(j = 0 ; j < M_NC(m) ; j++) printf("%f ", MATE(m, i, j));
	}
}

*/


