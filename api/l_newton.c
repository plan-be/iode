#include "iode.h"

#define SOLVE_MAXIT 100 // Maximum allowed number of iterations.
#define SOLVE_ERROR L_NAN
#define SOLVE_ERROR1 (L_NAN - 1e20)
#define SOLVE_ERROR2 (L_NAN + 1e20)

/******
Using a combination of Newton-Raphson and bisection, and the root of a function bracketed
between x1 and x2. The root, returned as the function value SolveNewtonSecant, will be rened until
its accuracy is known within  xacc. funcd is a user-supplied routine that returns both the
function value and the first derivative of the function.
******/

/**** NOUVELLE VERSION (NE MARCHE PAS ENCORE)

SolveIsError(double val)
{
    if(L_ISAN(val)) return(0);
    return(1);
//    if(val < SOLVE_ERROR2 && val > SOLVE_ERROR1) return(1);
//    return(0);

}

SolveNewtonSecantfn(double (*fn)(double), double x, double *fx, double *fpx)
{
    double h;

    h = 0.00000001;
    *fx = fn(x);
    *fpx = (fn(x + h) - *fx) / h;
    return(0);
}

double SolveNewtonSecant(double (*fn)(double), double x1, double x2, double feps, int maxit)
{
    void nrerror(char error_text[]);
    int j;
    double df,dx,dxold,f,fh,fl;
    double temp,xh,xl,rts;

    SolveNewtonSecantfn(fn, x1, &fl, &df);
    if(SolveIsError(fl) || SolveIsError(df)) return(SOLVE_ERROR);
    SolveNewtonSecantfn(fn, x2, &fh, &df);
    // (*funcd)(x1,&fl,&df);
    // (*funcd)(x2,&fh,&df);
    if((fl > 0.0 && fh > 0.0) || (fl < 0.0 && fh < 0.0)) return(SOLVE_ERROR);

    if(fl == 0.0) return x1;
    if(fh == 0.0) return x2;
    if(fl < 0.0) { // Orient the search so that f(xl) < 0.
	xl=x1;
	xh=x2;
	}
    else {
	xh=x1;
	xl=x2;
	}

    rts=0.5*(x1+x2);    //Initialize the guess for root,
    dxold=fabs(x2-x1);  // the stepsize before last
    dx=dxold;           // and the last step.
    SolveNewtonSecantfn(fn, rts, &f, &df);

    for(j = 1 ; j <= maxit ; j++) {
	if(SolveIsError(f) || SolveIsError(df)) return(SOLVE_ERROR);
	// printf("Iter %d\n", j);
	if((((rts - xh) * df - f) * ((rts - xl) * df -f) > 0.0) || // Bisect if Newton out of range,
	   (fabs(2.0*f) > fabs(dxold*df))) {          // or not decreasing fast enough.
	    dxold=dx;
	    dx=0.5*(xh-xl);
	    rts=xl+dx;
	    if(xl == rts) return rts; //Change in root is negligible.
	    }
	else { //Newton step acceptable. Take it.
	    dxold=dx;
	    dx=f/df;
	    temp=rts;
	    rts -= dx;
	    if (temp == rts) return rts;
	    }

	// printf("f=%lf, dx=%lf, rts=%lf\n", f, dx, rts);

	if(fabs(f) < feps) return rts; // Convergence criterion.
	SolveNewtonSecantfn(fn, rts, &f, &df);

	if(f < 0.0) // Maintain the bracket on the root.
	    xl=rts;
	else
	    xh=rts;
    }

    // printf("Maximum number of iterations exceeded in SolveNewtonSecant");
    return(SOLVE_ERROR);
}


double SolveFindBracket(double (*fn)(double), double x1, double sign)
{
    double  fx, x, xt = x1, step;
    int     i;

    // Chercher entre x1-n et x1 + n par pas progressif 1, 3, 7, 11, 16, ...
    for(i = 0 ; i < 20 ; i++) {
	xt += i;
	fx = fn(xt);
	if(!SolveIsError(fx) && fx * sign >= 0) return(xt);
	fx = fn(-xt);
	if(!SolveIsError(fx) && fx * sign >= 0) return(-xt);
	}

    // Chercher entre x1 - 1 et x1 + 1  par pas de 1/n
    step = 1.0;
    for(i = 1 ; i < 10 ; i++) {
	step = step * (1.0 / i);
	xt = x1 + step;
	fx = fn(xt);
	if(!SolveIsError(fx) && fx * sign >= 0) return(xt);
	xt = x1 - step;
	fx = fn(xt);
	if(!SolveIsError(fx) && fx * sign >= 0) return(xt);
	}

    return(SOLVE_ERROR);
}

SolveFindPoint2(double (*fn)(double), double x0, double *x1, int sign)
{
    double  xt, xts, fxt, fxts, vals[2], fvals[2], step;
    int     i, k;

    *x1 = SolveFindBracket(fn, x0, sign);
    if(!SolveIsError(*x1)) return(0);

    vals[0] = SolveFindBracket(fn, x0, -sign);
    if(SolveIsError(vals[0])) return(-1);
    vals[1] = SolveFindBracket(fn, vals[0] * 1.0001, -sign);
    if(SolveIsError(vals[1])) return(-1);
    fvals[0] = fn(vals[0]);
    fvals[1] = fn(vals[1]);

    for(k = 0 ; k < 100 ; k++) {
	if(sign * (fvals[0] - fvals[1]) <= 0) {
	    fxt      = fvals[0];
	    fvals[0] = fvals[1];
	    fvals[1] = fxt;
	    xt       = vals[0];
	    vals[0]  = vals[1];
	    vals[1]  = xt;
	    }

	if(fabs(fvals[0] - fvals[1]) < 1e-15) break;
	fxts = SOLVE_ERROR;
	xt = vals[0] - fvals[0] * (vals[1] - vals[0]) / (fvals[1] - fvals[0]);
	fxt = fn(xt);
	if(!SolveIsError(fxt)) {
	    if(sign * fxt >= 0) {*x1 = xt; return(0);}
	    if(SolveIsError(fxts)) {xts = xt; fxts = fxt;}
	    else {
		if(fabs(fxt) < fabs(fxts)) {xts = xt; fxts = fxt;}
		}
	    }

	step = 1.0;
	for(i = 1 ; i < 20 ; i++) {
	    step = step * (1.0 / i);
	    xt = vals[0] - step * fvals[0] * (vals[1] - vals[0]) / (fvals[1] - fvals[0]);
	    fxt = fn(xt);
	    if(!SolveIsError(fxt)) {
		if(sign * fxt >= 0) {*x1 = xt; return(0);}
		if(SolveIsError(fxts)) {xts = xt; fxts = fxt;}
		else {
		    if(fabs(fxt) < fabs(fxts)) {xts = xt; fxts = fxt;}
		    }
		}
	    }

	step = 1.0;
	for(i = 1 ; i < 10 ; i++) {
	    step = step * i;
	    xt = vals[0] - step * fvals[0] * (vals[1] - vals[0]) / (fvals[1] - fvals[0]);
	    fxt = fn(xt);
	    if(!SolveIsError(fxt)) {
		if(sign * fxt >= 0) {*x1 = xt; return(0);}
		if(SolveIsError(fxts)) {xts = xt; fxts = fxt;}
		else {
		    if(fabs(fxt) < fabs(fxts)) {xts = xt; fxts = fxt;}
		    }
		}
	    }

	if(SolveIsError(fxts)) return(-1);

	fvals[1] = fvals[0];
	vals[1] = vals[0];
	fvals[0] = fxts;
	vals[0] = xts;
	}

    // dernier essai


    return(-1);
}

SolveFindBrackets(double (*fn)(double), double x0, double *x1, double *x2)
{
    if(SolveFindPoint2(fn, x0, x1, -1.0) < 0) return(-1);
    if(SolveFindPoint2(fn, x0, x2, 1.0) < 0) return(-1);
    return(0);
}


int     SOLVE_VARNB, SOLVE_T;
double  SOLVE_SHIFT = 0.0;
CLEC    *SOLVE_CLEC;
KDB     *SOLVE_DBV, *SOLVE_DBS;

double SolveIodeFn(double x)
{
    double  fx;
    IODE_REAL    *d_ptr;

    d_ptr = L_getvar(SOLVE_DBV, SOLVE_VARNB);
    d_ptr[SOLVE_T] = x;

    fx = L_exec(SOLVE_DBV, SOLVE_DBS, SOLVE_CLEC, SOLVE_T);
    return(fx - SOLVE_SHIFT);
}


double L_zero(dbv, dbs, clec, t, varnb, eqvarnb)
KDB     *dbv, *dbs;
CLEC    *clec;
int     t, varnb, eqvarnb;
{
    IODE_REAL    *d_ptr;
    double  x, x1, xl, xh;

    SOLVE_DBV = dbv;
    SOLVE_DBS = dbs;
    SOLVE_CLEC = clec;
    SOLVE_VARNB = varnb;
    SOLVE_T = t;

    d_ptr = L_getvar(dbv, varnb);
    x1 = d_ptr[t];
    if(varnb == eqvarnb) SOLVE_SHIFT = 0.0;
    else SOLVE_SHIFT = x1;
    if(!L_ISAN(x1)) x1 = 1.0;

    if(SolveFindBrackets(SolveIodeFn, x1, &xl, &xh)) return(L_NAN);
    x = SolveNewtonSecant(SolveIodeFn, xl, xh, 1e-7, 100);
    return(x);
}
*/

/**** ANCIENNE VERSION ***/
double  L_newton_1();
int KSIM_DEBUGNEWTON = 0;

double L_zero(dbv, dbs, clec, t, varnb, eqvarnb)
KDB     *dbv, *dbs;
CLEC    *clec;
int     t, varnb, eqvarnb;
{
    double  x;

    x = L_newton(dbv, dbs, clec, t, varnb, eqvarnb);
    if(!L_ISAN(x)) x = L_secant(dbv, dbs, clec, t, varnb, eqvarnb);

    return(x);
}

double L_newton(dbv, dbs, clec, t, varnb, eqvarnb)
KDB     *dbv, *dbs;
CLEC    *clec;
int     t, varnb, eqvarnb;
{
    double      x;

    x = L_newton_1(0, dbv, dbs, clec, t, varnb, eqvarnb);
    if(!L_ISAN(x)) x = L_newton_1(1, dbv, dbs, clec, t, varnb, eqvarnb);
    return(x);
}


double L_newton_1(algo, dbv, dbs, clec, t, varnb, eqvarnb)
int     algo;
KDB     *dbv, *dbs;
CLEC    *clec;
int     t, varnb, eqvarnb;
{
    double  oldx, x, fx, fxh, h = 1e-6, eps = 1e-6, ax, afx, dx = 0.0, ox;
    int     it = 0, subit;
    IODE_REAL    *d_ptr, shift;
    extern  int KSIM_DEBUG;

    d_ptr = L_getvar(dbv, varnb) + t;
    oldx = x = d_ptr[0];
    if(!L_ISAN(x)) {
        if(KSIM_DEBUG) L_debug("Eq %s - Endo %s -> x is NA\n", KONAME(dbv, eqvarnb), KONAME(dbv, varnb));
        return((double)L_NAN);
    }

    if(varnb == eqvarnb || clec->dupendo) {   /* JMP 13-12-01 */
        shift = 0.0;
        ax = fabs(x);
    }
    else {
        shift = *(L_getvar(dbv, eqvarnb) + t);
        if(!L_ISAN(shift)) return((double)L_NAN); // added
        ax = fabs(shift);
    }

    if(KSIM_DEBUGNEWTON)
        L_debug("Eq %s - Endo %s : shift=%lf\n", KONAME(dbv, eqvarnb), KONAME(dbv, varnb), shift);


    if(algo && ax > 1.0) eps *= ax; /* GB 20-06-96 */
    while(it < 50) {
        d_ptr = L_getvar(dbv, varnb) + t;
        d_ptr[0] = x;
        ax = fabs(x);

        fx = L_exec(dbv, dbs, clec, t);
        if(KSIM_DEBUGNEWTON) L_debug("   - f(%lg) = %lg\n", x, fx);

        if(!L_ISAN(fx)) {
            if(KSIM_DEBUGNEWTON) { // Message iniquement en cas de pb
                L_debug("Eq %s - Endo %s : shift=%lf\n", KONAME(dbv, eqvarnb), KONAME(dbv, varnb), shift);
                L_debug("   - f(%lg) = %lg\n", x, fx);
                L_debug("   -> cannot compute f(%lg)\n", x);
            }
            goto err;
        }

        fx -= shift;
        afx = fabs(fx);

        if(afx < eps) {
            if(KSIM_DEBUGNEWTON) L_debug("   -> %lf\n", x);
            return(x);
        }
        if(ax > 1.0) h = 1e-4 * ax;
        else         h = 1e-4;

        d_ptr = L_getvar(dbv, varnb) + t;
        d_ptr[0] = x + h;
        fxh = L_exec(dbv, dbs, clec, t);
        if(!L_ISAN(fxh)) {
            if(KSIM_DEBUGNEWTON) L_debug("   -> cannot compute f(%lf+%ld)\n", x, h);
            goto err;
        }

        fxh -= shift;
        if(fabs(fxh - fx) < 1e-8) {
            if(KSIM_DEBUGNEWTON) L_debug("   -> f(x)=%lf == f(x+h)=%lf", fx, fxh);
            goto err;
        }
        if(KSIM_DEBUGNEWTON) L_debug("     h = %lg; fx=%lg; fxh=%lg\n", h, fx, fxh);
        dx = h * fx / (fxh - fx);
        ox = x;
        x -= dx;
        d_ptr[0] = x;
        fx = L_exec(dbv, dbs, clec, t);
        if(!L_ISAN(fx)) x = ox - dx / 4;
        it++;
    }

    if(KSIM_DEBUGNEWTON) L_debug("    #it > %d\n", it);

err:
    d_ptr[0] = oldx;
    return((double)L_NAN);
}















