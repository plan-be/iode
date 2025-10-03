/**
 *  @header4iode
 * 
 *  Implementation of a RAS algorithm. 
 *  
 *  List of functions
 *  -----------------
 *      int RasExecute(char *pattern, char *xdim, char *ydim, Period *rper, Period *cper, int maxit, double eps) | Implementation of a RAS algorithm
 *  
 *  
 */
#include "scr4/s_mat.h"

#include "api/constants.h"
#include "api/b_args.h"
#include "api/b_errors.h"
#include "api/k_super.h"
#include "api/objs/objs.h"
#include "api/objs/variables.h"

#include "api/report/commands/commands.h"


static int RasSetVar(char  *name, int t, double var)
{
    int     pos;
    KDB     *kdb = K_WS[VARIABLES];

    pos = K_find(kdb, name);
    if(pos < 0) {
        std::string error_msg = "RAS: Variable '" + std::string(name) + "' not found";
        error_manager.append_error(error_msg);
        return(-1);
    }
    else          
        *KVVAL(kdb, pos, t) = var;

    return(0);
}

static double RasGetVar(char  *name, int t)
{
    int     pos;
    double  var;
    KDB     *kdb = K_WS[VARIABLES];

    pos = K_find(kdb, name);
    if(pos < 0) {
        std::string error_msg = "RAS: Variable '" + std::string(name) + "' not found";
        error_manager.append_error(error_msg);
        var = IODE_NAN;
    }
    else          
        var = *KVVAL(kdb, pos, t);

    // set if almost 0 to 0
    if(fabs(var) < 1e-10) var = 0.0;

    return(var);
}

static int RasCalc(MAT *A, double *row, double *col, int maxiter, double eps)
{
    int rc = 0;
    int i, j, iter,ncols, nrows,imax,jmax;
    double rsum,csum,dis,dismax,rdismax,cdismax,cdis,rdis;

    nrows = A->m_nl;
    ncols = A->m_nc;
    // Ensure that the sum of the controls are the same
    rsum = 0.;
    csum = 0.;
    for(i = 0; i < nrows; i++) rsum += row[i];
    for(j = 0; j < ncols; j++) csum += col[j];
    if(fabs(csum - rsum) > eps) {
        std::string error_msg = "RAS : sum of rows (" + std::to_string(rsum);
        error_msg += ") != sum of cols (" + std::to_string(csum) + ")";
        error_manager.append_error(error_msg);
        return(-1);
    }

    iter = 0;
top:
    iter++;
    rdismax = 0.;
    cdismax = 0.;
    // scale columns
    for(j = 0; j < ncols; j++) {
        if(col[j] == 0) continue;
        csum = 0.;

        for(i = 0; i < nrows; i++) csum += MATE(A, i, j);
        if(fabs(csum) > 0) csum = col[j]/csum;
        else csum = 0.;

        for(i = 0; i < nrows; i++) MATE(A, i, j) *= csum;
        dis = fabs(csum -1.);
        if(dis > cdismax) {
            cdismax = dis;
            cdis = csum -1.;
            jmax = j;
        }
    }
    // scale rows
    for(i = 0; i < nrows; i++) {
        if(row[i] == 0) continue;
        rsum = 0;
        for(j = 0; j < ncols; j++) rsum += MATE(A, i, j);
        if(fabs(rsum) > 0) rsum = row[i]/rsum;
        else rsum = 0.;
        for(j = 0; j < ncols; j++) MATE(A, i, j) *= rsum;
        dis = fabs(rsum -1.);
        if(dis > rdismax) {
            rdismax = dis;
            rdis = rsum -1.;
            imax = i;
        }
    }
    // convergence check
    dismax = cdismax > rdismax ? cdismax : rdismax;
    kmsg("RAS             %d iter, %f < %f", iter, dismax, eps);
    if(iter <= maxiter && dismax > eps) goto top;

    if(dismax > eps) {
        std::string error_msg = "RAS : diverges (" + std::to_string(dismax);
        error_msg += " > " + std::to_string(eps) + ")";
        error_manager.append_error(error_msg);
        rc = -1;
    }
    else {
        kmsg("RAS converged,  %d iter, %f < %f", iter, dismax, eps);
        rc = 0;
    }

    return(rc);
}

/**
 *  Implementation of a RAS algorithm.
 *  
 *  See https://iode.plan.be/doku.php?id=datarasvar for details.
 *  
 *  @param [in] char*   pattern     the variables that meet the following criteria are used: x is replaced with all values from $X, y with those from $Y 
 *  @param [in] char*   X_dimensie  list of the values that "x" from the pattern can take. ATTENTION: the last one in the list is the SUM over the x dimension 
 *  @param [in] char*   Y_dimensie  list of the values that "y" from the pattern can take. ATTENTION: the last one from the list is the SUM over the y dimension 
 *  @param [in] Period* ref_jaar    reference year: the year for which all data is known 
 *  @param [in] Period* h_jaar      reference year: the year for which only the sums are known 
 *  @param [in] int     maxit       maximum number of iterations (default=100) 
 *  @param [in] double  eps         convergence threshold (default=0.001) 
 *  @return     int                 0 on success, -1 on error
 *  
 *  @note Developed by gb@plan.be 
 */
int RasExecute(char *pattern, char *xdim, char *ydim,
               Period *rper, Period *cper, int maxit, double eps)
{
    int     rc = -1, nrows, ncols, crow, ccol, rt, ct;
    char    **xvars = NULL;
    char    **yvars = NULL;
    char    cvar[K_MAX_NAME + 1], rvar[K_MAX_NAME + 1];
    MAT     *A = NULL;
    double  *row = NULL;
    double  *col = NULL;
    double  var, fvar;
    KDB     *kdb = K_WS[VARIABLES];

    if(rper != NULL && cper != NULL) 
    {
        rt = rper->difference(kdb->sample->start_period);
        ct = cper->difference(kdb->sample->start_period);
        if(rt < 0 || ct < 0) 
            goto cleanup;

        xvars = B_ainit_chk(xdim, NULL, 0);
        nrows = SCR_tbl_size((unsigned char**) xvars) - 1;

        yvars = B_ainit_chk(ydim, NULL, 0);
        ncols = SCR_tbl_size((unsigned char**) yvars) - 1;

        if(nrows == 0 || ncols == 0) goto cleanup;

        A = M_alloc(nrows, ncols);
        if(A == NULL) return(-1);
        row = (double *) SCR_malloc(sizeof(double) * nrows);
        col = (double *) SCR_malloc(sizeof(double) * ncols);

        // get matrix from variables
        for(crow = 0; crow < nrows; crow++) {
            strcpy(rvar, pattern);
            SCR_replace((unsigned char*) rvar, (unsigned char*) "x", (unsigned char*) xvars[crow]);

            for(ccol = 0; ccol < ncols; ccol++) {
                strcpy(cvar, rvar);
                SCR_replace((unsigned char*) cvar, (unsigned char*) "y", (unsigned char*) yvars[ccol]);

                var = RasGetVar(cvar, rt);
                if(!IODE_IS_A_NUMBER(var)) goto cleanup;

                fvar = RasGetVar(cvar, ct); // fixed var for current year
                if(!IODE_IS_A_NUMBER(fvar))  MATE(A, crow, ccol) = var;
                else {
                    MATE(A, crow, ccol) = 0.0;
                    row[crow] -= fvar;
                    col[ccol] -= fvar;
                }
            }
        }

        strcpy(rvar, pattern);
        SCR_replace((unsigned char*) rvar, (unsigned char*) "y", (unsigned char*) yvars[ncols]);
        for(crow = 0; crow < nrows; crow++) {
            strcpy(cvar, rvar);
            SCR_replace((unsigned char*) cvar, (unsigned char*) "x", (unsigned char*) xvars[crow]);
            var = RasGetVar(cvar, ct);
            if(!IODE_IS_A_NUMBER(var)) goto cleanup;
            row[crow] += var;
        }

        strcpy(rvar, pattern);
        SCR_replace((unsigned char*) rvar, (unsigned char*) "x", (unsigned char*) xvars[nrows]);
        for(ccol = 0; ccol < ncols; ccol++) {
            strcpy(cvar, rvar);
            SCR_replace((unsigned char*) cvar, (unsigned char*) "y", (unsigned char*) yvars[ccol]);
            var = RasGetVar(cvar, ct);
            if(!IODE_IS_A_NUMBER(var)) goto cleanup;
            col[ccol] += var;
        }

        if(RasCalc(A, row, col, maxit, eps) < 0) goto cleanup;

        // write results back
        for(crow = 0; crow < nrows; crow++) {
            strcpy(rvar, pattern);
            SCR_replace((unsigned char*) rvar, (unsigned char*) "x", (unsigned char*) xvars[crow]);

            for(ccol = 0; ccol < ncols; ccol++) {
                strcpy(cvar, rvar);
                SCR_replace((unsigned char*) cvar, (unsigned char*) "y", (unsigned char*) yvars[ccol]);
                // keep var if fixed
                if(!IODE_IS_A_NUMBER(RasGetVar(cvar, ct)))
                    if(RasSetVar(cvar, ct, MATE(A, crow, ccol)) < 0) goto cleanup;
            }
        }
        rc = 0;

    }

cleanup:
    if(xvars != NULL) SCR_free_tbl((unsigned char**) xvars);
    if(yvars != NULL) SCR_free_tbl((unsigned char**) yvars);
    if(A != NULL) M_free(A);
    if(row != NULL) SCR_free((unsigned char**) row);
    if(col != NULL) SCR_free((unsigned char**) col);
    return(rc);
}
