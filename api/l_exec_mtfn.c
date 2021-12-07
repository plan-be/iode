/**
 * @header4iode
 *
 * Functions to evaluate LEC "time functions with possibly  multiple arguments. 
 * 
 * Example 
 *      sum(2000Y1, 2010Y1, A) computes the sum of elements of A between 2000 and 2010.
 *   
 * The table (*L_TFN_FN)[] (at the end of this file) contains the pointers 
 * to the functions from L_LAG to L_LASTOBS (see iode.h).
 *
 * Function signature:
 *
 *      L_REAL <fnname>(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
 *  
 *  where:
 *      - expr points to the current position in the CLEC expression (in "X + sum(A + B))", expr points to "A + B")
 *      - len is the length of the (sub-) expression expr (here the length of "A+B" compiled)
 *      - t is the current execution time (index in the SAMPLE) 
 *      - from receives the first position for the calculation 
 *      - to receives the last position for the calculation     
 *  
 */

#include "iode.h"

/* ============ MTFN ============ */

extern void     L_tfn_args(int t, L_REAL* stack, int nargs, int* from, int* to);
extern L_REAL   L_mean(unsigned char* expr, short len, int t, L_REAL* stack, int nargs); 
extern KDB      *L_EXEC_DBV, *L_EXEC_DBS; 

static L_REAL L_calccorr(unsigned char* expr1, short len1, unsigned char* expr2, short len2, int t, L_REAL* stack, int nargs) 
{
    L_REAL  sxx = 0.0, syy = 0.0, sxy = 0.0, vx, vy, meanx, meany;
    int     from, to, j, n;

    meanx = L_mean(expr1, len1, t, stack, nargs - 1);
    if(!L_ISAN(meanx)) return(L_NAN);
    meany = L_mean(expr2, len2, t, stack, nargs - 1);
    if(!L_ISAN(meany)) return(L_NAN);

    L_tfn_args(t, stack, nargs - 1, &from, &to);
    for(j = from ; j <= to ; j++) {
        vx = L_exec_sub(expr1, len1, j, stack);
        if(!L_ISAN(vx)) return(L_NAN);
        vy = L_exec_sub(expr2, len2, j, stack);
        if(!L_ISAN(vy)) return(L_NAN);

        sxx += (vx - meanx) * (vx - meanx);
        syy += (vy - meany) * (vy - meany);
        sxy += (vx - meanx) * (vy - meany);
    }

    if(sxx * syy <= 0) return(L_NAN);
    return(sxy / sqrt(sxx * syy));
}

static L_REAL L_corr(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)  /* JMP 17-04-98 */
{
    char    *expr1, *expr2;
    short   len1, len2;

    memcpy(&len1, expr, sizeof(short));
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    return(L_calccorr(expr + sizeof(short), len1,
                      expr + len1 + 2 * sizeof(short), len2,
                      t, stack, nargs));
}

static L_REAL L_calccovar(unsigned char* expr1, short len1, unsigned char* expr2, short len2, int t, L_REAL* stack, int nargs, int orig) 
{
    L_REAL  sxx = 0.0, syy = 0.0, sxy = 0.0, vx, vy, meanx, meany;
    int     from, to, j, n;

    L_tfn_args(t, stack, nargs - 1, &from, &to);
    n = 1 + to - from;
    if(n == 0) return(L_NAN);

    meanx = L_mean(expr1, len1, t, stack, nargs - 1);
    if(!L_ISAN(meanx)) return(L_NAN);
    meany = L_mean(expr2, len2, t, stack, nargs - 1);
    if(!L_ISAN(meany)) return(L_NAN);

    for(j = from ; j <= to ; j++) {
        vx = L_exec_sub(expr1, len1, j, stack);
        if(!L_ISAN(vx)) return(L_NAN);
        vy = L_exec_sub(expr2, len2, j, stack);
        if(!L_ISAN(vy)) return(L_NAN);

        if(orig) sxy += vx * vy;
        else     sxy += (vx - meanx) * (vy - meany);
    }

    return(sxy / n);
}

static L_REAL L_covar(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs) 
{
    char    *expr1, *expr2;
    short   len1, len2;

    memcpy(&len1, expr, sizeof(short));
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    return(L_calccovar(expr + sizeof(short), len1,
                       expr + len1 + 2 * sizeof(short), len2,
                       t, stack, nargs, 0));
}

static L_REAL L_covar0(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs) 
{
    char    *expr1, *expr2;
    short   len1, len2;

    memcpy(&len1, expr, sizeof(short));
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    return(L_calccovar(expr + sizeof(short), len1,
                       expr + len1 + 2 * sizeof(short), len2,
                       t, stack, nargs, 1));
}

static L_REAL L_var(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
{
    char    *expr1;
    short   len1;

    memcpy(&len1, expr, sizeof(short));
    return(L_calccovar(expr + sizeof(short), len1,
                       expr + sizeof(short), len1,
                       t, stack, nargs + 1, 0));
}

static L_REAL L_stddev(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
{
    L_REAL  v1;

    v1 = L_var(expr, nvargs, t, stack, nargs);
    if(!L_ISAN(v1) || v1 < 0) return(L_NAN);
    else return(sqrt(v1));
}

static L_REAL L_index(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
{
    char    *expr1, *expr2;
    short   len1, len2;
    int     from, to, j, n;
    L_REAL  vx, vy;

    memcpy(&len1, expr, sizeof(short));
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    expr1 = expr + sizeof(short);
    expr2 = expr + len1 + 2 * sizeof(short);

    L_tfn_args(t, stack, nargs - 1, &from, &to);
    n = 1 + to - from;
    if(n == 0) return(L_NAN);

    vx = L_exec_sub(expr1, len1, t, stack);
    if(!L_ISAN(vx)) return(L_NAN);

    for(j = from ; j <= to ; j++) {
        vy = L_exec_sub(expr2, len2, j, stack);
        if(!L_ISAN(vy)) return(L_NAN);

        if(fabs(vx - vy) < 1e-30) return((L_REAL)j);
    }

    return(L_NAN);
}

static L_REAL L_acf(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs) /* JMP 17-04-98 */
{
    char    *expr1, *expr2;
    short   len1, len2;
    int     from, to, j, n, k;
    L_REAL  vx, vy, meanx, sxy = 0.0, sxy0 = 0.0;

    memcpy(&len1, expr, sizeof(short));
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    expr1 = expr + sizeof(short);
    expr2 = expr + len1 + 2 * sizeof(short);

    L_tfn_args(t, stack, nargs - 1, &from, &to);
    n = 1 + to - from;
    if(n == 0) return(L_NAN);

    vx = L_exec_sub(expr1, len1, t, stack);
    if(!L_ISAN(vx)) return(L_NAN);
    k = (int)vx;
    if(k < 0 || k > n / 4) return(L_NAN);

    meanx = L_mean(expr2, len2, t, stack, nargs - 1);
    if(!L_ISAN(meanx)) return(L_NAN);

    for(j = from ; j <= to - k ; j++) {
        vx = L_exec_sub(expr2, len2, j, stack);
        if(!L_ISAN(vx)) return(L_NAN);
        vy = L_exec_sub(expr2, len2, j + k, stack);
        if(!L_ISAN(vy)) return(L_NAN);

        sxy += (vx - meanx) * (vy - meanx);
    }

    for(j = from ; j <= to ; j++) {
        vx = L_exec_sub(expr2, len2, j, stack);
        if(!L_ISAN(vx)) return(L_NAN);
        sxy0 += (vx - meanx) * (vx - meanx);
    }

    return(sxy / sxy0);
}


/* Retourne dans vy et dans vt les valeurs définies les plus proches autour
    de expr[t].
*/
static int L_calcvals(unsigned char* expr1, short len1, int t, L_REAL* stack, int* vt, L_REAL* vy, int notnul)
{
    int     j, nobs;

    /* 2. Calc val after t */
    vy[0] = vy[1] = L_NAN;
    nobs = (L_getsmpl(L_EXEC_DBV))->s_nb;
    for(vt[1] = t + 1 ; vt[1] < nobs ; vt[1]++) {
        vy[1] = L_exec_sub(expr1, len1, vt[1], stack);
        if(L_ISAN(vy[1]) && (notnul == 0 || fabs(vy[1]) > 1e-15)) break;
    }

    /* 3. Calc val before t */
    for(vt[0] = t - 1 ; vt[0] >= 0 ; vt[0]--) {
        vy[0] = L_exec_sub(expr1, len1, vt[0], stack);
        if(L_ISAN(vy[0]) && (notnul == 0 || fabs(vy[0]) > 1e-15)) break;
    }

    /* if NO value after AND before t, return L_NAN */
    if(!L_ISAN(vy[0]) && !L_ISAN(vy[1])) return(-1);
    if(L_ISAN(vy[0]) && L_ISAN(vy[1])) return(0);

    /* if no value after, calc before t0 */
    if(!L_ISAN(vy[1])) {
        vy[1] = vy[0];
        vt[1] = vt[0];
        vy[0] = L_NAN;
        for(vt[0] = vt[1] - 1 ; vt[0] >= 0 ; vt[0]--) {
            vy[0] = L_exec_sub(expr1, len1, vt[0], stack);
            if(L_ISAN(vy[0]) && (notnul == 0 || fabs(vy[0]) > 1e-15)) break;
        }
    }

    /* if no value before, calc after t1 */
    else {
        vy[0] = vy[1];
        vt[0] = vt[1];
        vy[1] = L_NAN;
        for(vt[1] = vt[0] + 1 ; vt[1] < nobs ; vt[1]++) {
            vy[1] = L_exec_sub(expr1, len1, vt[1], stack);
            if(L_ISAN(vy[1])) break;
        }
    }

    return(0);
}

static L_REAL L_interpol(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs) /* JMP 17-04-98 */
{
    char    *expr1;
    short   len1;
    int     j, nobs, vt[2];
    L_REAL  vx, vy[2], itc;

    memcpy(&len1, expr, sizeof(short));
    expr1 = expr + sizeof(short);

    /* 1. Calc val in t */
    vx = L_exec_sub(expr1, len1, t, stack);
    if(L_ISAN(vx)) return(vx);

    /* 2. Calc values around t */
    L_calcvals(expr1, len1, t, stack, vt, vy, 0);
    nobs = (L_getsmpl(L_EXEC_DBV))->s_nb;

    /* 3. Calc result */
    if(!L_ISAN(vy[0]) && !L_ISAN(vy[1])) return(L_NAN);
    if(vt[0] < 0) return(vy[1]);
    if(vt[1] >= nobs) return(vy[0]);
    itc = (vy[0] * vt[1] - vy[1] * vt[0]) / (vt[1] - vt[0]);
    return(itc + t * (vy[1] - vy[0]) / (vt[1] - vt[0]));
}

static L_REAL L_app(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs) /* JMP 17-04-98 */
{
    char    *expr1, *expr2;
    short   len1, len2;
    int     j, nobs, vt[2];
    L_REAL  vx, vy[2], itc, ayt, ay[2], delta;

    memcpy(&len1, expr, sizeof(short));
    expr1 = expr + sizeof(short);
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    expr2 = expr + len1 + 2 * sizeof(short);

    /* 1. Calc val in t */
    vx = L_exec_sub(expr1, len1, t, stack);
    if(L_ISAN(vx)) return(vx);

    /* 2. Calc values around t */
    L_calcvals(expr1, len1, t, stack, vt, vy, 1);
    nobs = (L_getsmpl(L_EXEC_DBV))->s_nb;

    /* if NO value after AND before t, return L_NAN */
    if(!L_ISAN(vy[0]) && !L_ISAN(vy[1])) return(L_NAN);

    /* Valeurs apparent‚es */
    ayt = L_exec_sub(expr2, len2, t, stack);
    if(!L_ISAN(ayt)) return(L_NAN);
    ay[0] = ay[1] = L_NAN; /* JMP 19-07-07 */
    if(vt[0] >= 0)   ay[0] = L_exec_sub(expr2, len2, vt[0], stack);
    if(vt[1] < nobs) ay[1] = L_exec_sub(expr2, len2, vt[1], stack);

    // Deux valeurs trouv‚es dans la s‚rie initiale
    // !! Les deux valeurs doivent exister dans la s‚rie apparent‚e
    if(L_ISAN(ay[0]) && L_ISAN(ay[1])) {
        if(vt[0] < t && vt[1] > t) {
            /*            return(ayt * (vy[0] / ay[0] + vy[1] / ay[1]) / 2); */
            // delta = (vy[1]/vy[0])/(ay[1]/ay[0]);
            if(fabs(vy[0] * ay[1]) < 1e-15) return(L_NAN);
            delta = (vy[1] * ay[0]) / (vy[0] * ay[1]);
            if(delta < 0) return(L_NAN);
            delta = pow(delta, 1.0/(vt[1]-vt[0]));
            if(_isnan(delta)) return(L_NAN); /* JMP 18-01-02 */
            return(vy[0] * (ayt/ay[0] * pow(delta, t - vt[0])));
        }
        else {
            j = 0;
            if(vt[0] < t) j = 1;
            if(fabs(ay[j]) < 1e-15) return(L_NAN); /* JMP 19-07-07 */
            return(ayt * (vy[j] / ay[j]));
        }
    }
    // Seule la valeur en t0 est d‚finie : res(t) <- APP(t) * (ORIG(t0) / APP(t0))
    if(L_ISAN(ay[0])) {
        if(fabs(ay[0]) < 1e-15) return(L_NAN); /* JMP 19-07-07 */
        return(ayt * (vy[0] / ay[0]));
    }
    // Seule la valeur en t1 est d‚finie : res(t) <- APP(t) * (ORIG(t1) / APP(t1))
    if(L_ISAN(ay[1])) {
        if(fabs(ay[1]) < 1e-15) return(L_NAN); /* JMP 19-07-07 */
        return(ayt * (vy[1] / ay[1]));
    }
    return(L_NAN);
}

static L_REAL L_dapp(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs) /* GB 14-11-00 */
{
    char    *expr1, *expr2;
    short   len1, len2;
    int     j, nobs, vt[2];
    L_REAL  vx, vy[2], itc, ayt, ay[2], delta;

    memcpy(&len1, expr, sizeof(short));
    expr1 = expr + sizeof(short);
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    expr2 = expr + len1 + 2 * sizeof(short);

    /* 1. Calc val in t */
    vx = L_exec_sub(expr1, len1, t, stack);
    if(L_ISAN(vx)) return(vx);

    /* 2. Calc values around t */
    L_calcvals(expr1, len1, t, stack, vt, vy, 0);
    nobs = (L_getsmpl(L_EXEC_DBV))->s_nb;

    /* if NO value after AND before t, return L_NAN */
    if(!L_ISAN(vy[0]) && !L_ISAN(vy[1])) return(L_NAN);

    /* Valeurs apparent‚es */
    ayt = L_exec_sub(expr2, len2, t, stack);
    if(!L_ISAN(ayt)) return(L_NAN);
    if(vt[0] >= 0)   ay[0] = L_exec_sub(expr2, len2, vt[0], stack);
    if(vt[1] < nobs) ay[1] = L_exec_sub(expr2, len2, vt[1], stack);

    if(L_ISAN(ay[0]) && L_ISAN(ay[1])) {
        if(vt[0] < t && vt[1] > t) {
            delta = (ay[1] - ay[0] + vy[0] - vy[1]) / (vt[1]-vt[0]);
            return(ayt + vy[0] - ay[0] - (t - vt[0]) * delta);
        }
        else {
            j = 0;
            if(vt[0] < t) j = 1;
            return(ayt + (vy[j] - ay[j]));
        }
    }
    if(L_ISAN(ay[0]))                return(ayt + (vy[0] - ay[0]));
    if(L_ISAN(ay[1]))                return(ayt + (vy[1] - ay[1]));
    return(L_NAN);
}

static L_REAL L_hpall(unsigned char* expr, short len, int t, L_REAL* stack, int nargs, int std)
{
    char    *expr1, *expr2;
    short   len1, len2;
    IODE_REAL    v, *itmp = NULL, *otmp = NULL;
    //int     from, to, j, lg, lambda, nbna, dim;      /* JMP 7-3-2019 */
    int     from, to, j, lg, nbna, dim;         /* JMP 7-3-2019 */
    IODE_REAL   lambda;                         /* JMP 7-3-2019 */

    memcpy(&len1, expr, sizeof(short));
    expr1 = expr + sizeof(short);
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    expr2 = expr + len1 + 2 * sizeof(short);

    v = L_exec_sub(expr1, len1, t, stack);
    if(!L_ISAN(v)) return(L_NAN);
    lambda = v;

    L_tfn_args(t, stack, nargs - 1, &from, &to);
    if(t < from || t > to) goto err;
    lg = to - from + 1;
    if(lg < 5) goto err;
    itmp = (IODE_REAL *) SCR_malloc(lg * sizeof(IODE_REAL));
    otmp = (IODE_REAL *) SCR_malloc(lg * sizeof(IODE_REAL));

    if(itmp == NULL || otmp == NULL) goto err;
    for(j = from ; j <= to ; j++) {
        itmp[j - from] = L_exec_sub(expr2, len2, j, stack);
        // if(!L_ISAN(itmp[j - from])) goto err;            /* JMP 26-07-11 */
    }

    /*
        for(j = from, nbna = 0 ; j <= to ; j++, nbna++) {
    	if(L_ISAN(itmp[j - from])) break;
        }
        if(lg - nbna < 5) goto err;
    */

    HP_test(itmp, otmp, lg, &nbna, &dim);           /* JMP 26-07-11 */
    if(dim < 5) goto err;                           /* JMP 26-07-11 */
    HP_calc(itmp + nbna, otmp + nbna, dim, lambda, std); /* JMP 26-07-11 */ // JMP 12/4/2019

    SCR_free(itmp);
    v = otmp[t - from];
    SCR_free(otmp);
    //DebugActif = 1;
    //Debug("t=%d, lambda=%lf, v=%lf, from=%d, to=%d\n", t, lambda, v, from, to);
    return(v);

err:
    SCR_free(itmp);
    SCR_free(otmp);
    return(L_NAN);
}

static L_REAL L_hp(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    return(L_hpall(expr, len, t, stack, nargs, 0));
}


static L_REAL L_hpstd(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    return(L_hpall(expr, len, t, stack, nargs, 1));
}


/**
 *  Table of function pointers L_MTFN_FN
 */
L_REAL(*L_MTFN_FN[])() = { 
    L_corr,         // L_CORR      L_M
    L_covar,        // L_COVAR     L_M
    L_covar0,       // L_COVAR0    L_M
    L_var,          // L_VARIANCE  L_M
    L_stddev,       // L_STDDEV    L_M
    L_index,        // L_INDEX     L_M
    L_acf,          // L_ACF       L_M
    L_interpol,     // L_INTERPOL  L_M
    L_app,          // L_APP       L_M
    L_hp,           // L_HP        L_M
    L_dapp,         // L_DAPP      L_M
    L_hpstd         // L_HPSTD     L_M
};                     

