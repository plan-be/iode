#include "iode.h"

double  M_c_line(m1, line, oper)
MAT *m1;
int line, oper;
{
    double  res = 0.0;
    int     i;

    if(line > NL1) return(res);
    for(i = 0; i < NC1; i++) {
        switch(oper) {
        case '+' :
            res += MATE1(line, i);
            break;
        case '-' :
            res -= MATE1(line, i);
            break;
        case '*' :
            res *= MATE1(line, i);
            break;
        default  :
            return(res);
        }
    }
    return(res);
}

double  E_div_0(a, b)
double  a, b;
{
    if(b == 0) return(0.0);

    return(a/b);
}

double E_sqrt(double val)
{
#ifndef WATCOM
    double  sqrt();
#endif
    if(val < 0)
        return(0);
    else
        return(sqrt(val));
}

E_output()
{
    int     i, j;
    double  sum, tmp /*, sum1 */;
#ifndef WATCOM
    double  sqrt();
#endif

    /* 1. OUTPUT BY COEFFICIENT */
    E_deg_freed();
    if(E_c_mcorr()) return(-1);
    if(E_c_ttests()) return(-1);

    /* 2. OUTPUT BY EQUATION */
    if(E_c_mcorru()) return(-1);
    for(i = 0 ; i < E_NEQ ; i++) {
        MATE(E_SSRES, 0, i)  = MATE(E_VCU, i, i) * E_T;
        MATE(E_STDERR, 0, i) =
            E_sqrt(E_div_0(MATE(E_VCU, i, i) * E_T, (E_T - MATE(E_NBCE, 0, i))));
        MATE(E_MEAN_Y, 0, i) = M_c_line(E_LHS, i, '+') / E_T;

        sum = 0.0;
        /*
        	sum1 = 0.0;
        */
        for(j = 0 ; j < E_T ; j++) {
            tmp = MATE(E_LHS, i, j) - MATE(E_MEAN_Y, 0, i);
            MATE(E_DEV, i, j) = tmp;
            sum += tmp * tmp;
            /*
            	    tmp = MATE(E_RHS, i, j) - MATE(E_MEAN_Y, 0, i);
            	    sum1 += tmp * tmp;
            */
        }

        MATE(E_STDEV, 0, i) = E_sqrt(sum / (E_T - 1));

        /*
        	MATE(E_RSQUARE, 0, i) = E_div_0(sum1, sum);
        */
        MATE(E_RSQUARE, 0, i) = 1 - E_div_0(E_T * MATE(E_VCU, i, i), sum);

        MATE(E_RSQUARE_ADJ, 0, i) = 1 - E_div_0(
                                        (1 - MATE(E_RSQUARE, 0, i)) * (E_T - 1), E_T - MATE(E_NBCE, 0, i));

        MATE(E_FSTAT, 0, i) = E_div_0(
                                  MATE(E_RSQUARE, 0, i) * (E_T - MATE(E_NBCE, 0, i)),
                                  (1 - MATE(E_RSQUARE, 0, i)) * (MATE(E_NBCE, 0, i) - 1) );

        MATE(E_STD_PCT, 0, i) = E_div_0(
                                    100 * MATE(E_STDERR, 0, i), MATE(E_MEAN_Y, 0, i));

        sum = 0.0;
        for(j = 0; j < E_T - 1 ; j++) {
            tmp = MATE(E_U, i, j + 1) - MATE(E_U, i, j);
            sum += tmp * tmp;
        }
        MATE(E_DW, 0, i) =  E_div_0(sum, E_T * MATE(E_VCU,i, i));
    }

    /* LOGLIK */
    E_c_loglik();

    /*
        E_print_mat("E_MCORR        ", E_MCORR       );
        E_print_mat("E_SSRES        ", E_SSRES       );
        E_print_mat("E_STDERR       ", E_STDERR      );
        E_print_mat("E_MEAN_Y       ", E_MEAN_Y      );
        E_print_mat("E_DEV          ", E_DEV         );
        E_print_mat("E_STDEV        ", E_STDEV       );
        E_print_mat("E_RSQUARE      ", E_RSQUARE     );
        E_print_mat("E_RSQUARE_ADJ  ", E_RSQUARE_ADJ );
        E_print_mat("E_FSTAT        ", E_FSTAT       );
        E_print_mat("E_DW           ", E_DW          );
        E_print_mat("E_STD_PCT      ", E_STD_PCT     );
        E_print_mat("E_LOGLIK       ", E_LOGLIK      );
    */
    return(0);
}

/* ========================================================================
Calcule le # de degr‚s de libert‚s par coefficient.

si le coef apparaŒt dans plus d'1 ‚quation, DF = T
sinon DF = T - nbre de coef estim‚s dans l'‚quation
======================================================================== */

void E_deg_freed()
{
    int     j, eq, cf, nbr, nbce;

    for(cf = 0, j = 0; cf < E_NC; cf++) {
        if(KSVAL(E_DBS, E_C_NBS[cf])->relax == 0) continue;
        nbr = 0;
        for(eq = 0; eq < E_NEQ; eq++) {
            if(E_scl_in_eq(cf, eq)) {
                nbr ++;
                nbce = MATE(E_NBCE, 0, eq);
            }
            if(nbr > 1) MATE(E_DF, 0, j) = E_T;
            else MATE(E_DF, 0, j) = E_T - nbce;
        }
        j++;
    }
}

double  E_c_umu()
{
    int         i, j;
    double      umu = 0.0;
    MAT         *E_UMU = M_alloc(1, 1);

    for(i = 0 ; i < E_NEQ  ; i++) {
        M_clear(E_UM);                                    /* UM = 0 */
        for(j = 0 ; j < E_NEQ ; j++) {
            M_extr(E_UMT, E_U, j, 0, 1, E_T);           /* UMTMP = U[..] */
            M_trans(E_UMTMP, E_UMT);
            M_scale(E_UMTMP, E_UMTMP, MATE(E_IVCU, i, j));/* UMTMP=UMTMP*IVCU[..] */
            M_calc(E_UM, E_UM, E_UMTMP, '+');             /* UM = UM + ,UMTMP */
        }

        M_extr(E_UMT, E_U, i, 0, 1, E_T);           /* UMTMP = U[..] */
        M_prod(E_UMU, E_UMT, E_UM);
        umu += MATE(E_UMU, 0, 0);
    }

    M_free(E_UMU);
    return(umu);
}

E_c_loglik()
{
    int     i;
    double  loglik;

    switch(E_MET) {
    case 0 :
    case 2 :
        for(i = 0 ; i < E_NEQ ; i++)
            MATE(E_LOGLIK, 0, i) =
                - (E_T * 0.5) * (1 + log(2 * M_PI * MATE(E_VCU, i, i)));
        break;

    default :
        loglik = - 0.5 * ( E_NEQ * E_T * log(2 * M_PI)
                           + E_T * log(M_det_1(E_VCU))
                           + E_c_umu() ) ;
        for(i = 0 ; i < E_NEQ ; i++) MATE(E_LOGLIK, 0, i) = loglik;
        break;
    }

    return(0);
}

E_c_mcorr()
{
    int         i, j;
    double      vii, vij, vjj;

    for(i = 0 ; i< E_NCE ; i++) {
        vii = MATE(E_VCC, i, i);
        for(j = 0 ; j <= i ; j++) {
            vjj = MATE(E_VCC, j, j);
            vij = MATE(E_VCC, i, j);
            if((vii * vjj) > 0.0)
                MATE(E_MCORR, i, j) = MATE(E_MCORR, j, i) =
                                          vij / E_sqrt(vii * vjj);
            else
                MATE(E_MCORR, i, j) = MATE(E_MCORR, j, i) = 0.0;
        }
    }
    return(0);
}

E_c_mcorru()
{
    int         i, j;
    double      vii, vij, vjj;

    for(i = 0 ; i < E_NEQ ; i++) {
        vii = MATE(E_VCU, i, i);
        for(j = 0 ; j <= i ; j++) {
            vjj = MATE(E_VCU, j, j);
            vij = MATE(E_VCU, i, j);
            MATE(E_MCORRU, i, j) = MATE(E_MCORRU, j, i) =
                                       E_div_0(vij, E_sqrt(vii * vjj));  /* JMP 05-10-00 */
        }
    }
    return(0);
}

E_c_ttests()
{
    int     i, j;
    SCL     *scl;

    for(i = 0, j = 0 ; i < E_NC ; i++) {
        scl = KSVAL(E_DBS, E_C_NBS[i]);
        scl->std = 0.0;
        if(scl->relax == 0) continue;
        if(E_MET == 4)
            scl->std = E_sqrt(MATE(E_VCC, j, j));
        else
            scl->std = E_sqrt(E_div_0(MATE(E_VCC, j, j) * E_T, MATE(E_DF, 0, j)));
        j++;
    }

    return(0);
}


