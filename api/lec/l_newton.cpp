 /**
 *  @header4iode
 * 
 *  Tries to find a root of a compiled LEC equation using a combination of Newton-Raphson 
 *  and bisection (secant) method.
 *  
 *  
 *  Methodology
 *  ===========
 *  Depending on their form, LEC equations can be solved analytically with respect to their endogenous variable-- or not.
 *  
 *  For example, an equation of the type
 *   
 *      d(Y / X) := c1 + c2 * ln(Y / Z)
 *          
 *  cannot be solved analytically because Y appears more than once (on the left and/or on the right member).
 *  
 *  Conversely, the equation 
 *  
 *      d Y : = c1 + c2 * X + c3 * t
 *      
 *  is invertible with respect to Y:
 *  
 *      Y : = Y[-1] + c1 + c2 * X + c3 * t
 *  
 *  During the Gauss-Seidel iteration, we must calculate the value of the endogenous variable for each equation. 
 *  We have 3 possibilities.
 *  
 *  Case 1: the equation is analytically solved with respect to its endogenous
 *  --------------------------------------------------------------------------
 *  Consider the LEC equation below:
 *  
 *      d Y := c1 + c2 * X 
 *      
 *  If its endogenous variable is Y, it will be translated internally as follows:
 *  
 *      Y[-1] + c1 + c2 * X
 *      
 *  When simulating the model, this formula is calculated and the result is stored in Y.
 *  
 *  In the code below, this is the case where clec->dupendo is zero AND varnb == eqvarnb.
 *  
 *  Case 2: the equation must be numerically solved
 *  -----------------------------------------------
 *  The equation 
 *  
 *      ln Y := c1 + c2 * X / ln Y
 *      
 *  contains twice the variable Y and will be translated into  
 *  
 *      ln Y - (c1 + c2 * X / ln Y)
 *  
 *  Note that the result of this formula is 0, and not Y, contrary to case 1.
 *  
 *  When calculating the model, this equation must be solved numerically with respect to Y before storing the result in Y.
 *  
 *  In the code, this is the case where clec->dupendo is non-zero.
 *  
 *  Case 3: ENDO-EXO exchanges
 *  -------------------------
 *  If an equation is inverted in an endo-exo exchange (e.g. Y becomes exogenous and X becomes endogenous), 
 *  the equation will be solved numerically as above (case 2). Obviously, one must take into account the fact 
 *  that the translated version of the equation provides the value of Y (and not 0) 
 *  and that Y must be subtracted from the calculation.
 *  
 *  Indeed, the equation 
 *  
 *     d Y := c1 + c2 * X
 *  
 *  is transformed by IODE into 
 *  
 *      Y := c1 + c2 * X - Y[-1]
 *  
 *  Hence, we will search the value of X such that
 *  
 *       0 == Y[-1] + c1 + c2 * X - Y 
 *       
 *  In the code, this is the case where varnb != eqvarnb.
 *  
 *  
 *  Newton-Raphson algorithm
 *  ========================
 *  
 *  Search for x such as |f(x)| < KSIM_EPSNEWTON
 *  
 *  We start by calculating shift = the difference between 0 and the value obtained 
 *  by the LEC formula of the equation (according to the case 1, 2 or 3).
 *  
 *  We then loop (maximum KSIM_NEWTON_MAXIT iterations) to search for x : 
 *      
 *      # check convergence
 *      if |f(x) - shift| < KSIM_NEWTON_EPS: x is a solution
 *      
 *      # Define derivative step h 
 *      h = 1e-4    
 *      if |x| < 1.0 then h *= |x|
 *      
 *      # Move x in the direction of the local tangent 
 *      dx = h.f(x) /(f(x+h)-f(x))
 *      if f(x - dx) is NaN: dx = dx / 4
 *      x = x - dx
 *      
 *  The loop also stops if x or f(x) becomes NaN.
 *  
 *    
 * 
 *  List of functions
 *  ----------------- 
 *
 *      double L_zero(KDB* dbv, KDB* dbs, CLEC* clec, int t, int varnb, int eqvarnb)    Solves numerically a LEC equation for one period of time with respect to a given variable. 
 *                                                                                      If the Newton-Raphson method does not reach a solution, tries a bisection (secant) method. 
 *      double L_newton(KDB* dbv, KDB* dbs, CLEC* clec, int t, int varnb, int eqvarnb)  Tries to solve a LEC equation by the Newton-Raphson method. 
 *  
 */
#include <math.h>
#include "api/lec/lec.h"
#include "api/simulation/simulation.h"


static double  L_newton_1();

/**
 *  Solves numerically a LEC equation for one period of time with respect to a given variable (varnb).  
 *  If the Newton-Raphson method does not reach a solution, tries a bisection (secant) method. 
 *  
 *  
 *  @param [in] KDB*    dbv     KDB of VAR with which the equation has been linked
 *  @param [in] KDB*    dbs     KDB of Scalar with which the equation has been linked
 *  @param [in] CLEC*   clec    compiled LEC expression 
 *  @param [in] int     t       time of calculation (index in dbv SAMPLE)
 *  @param [in] int     varnb   position of the endogenous variable in dbv
 *  @param [in] int     eqvarnb position of the initial endogenous variable (i.e. equation name) in dbv
 *  @return     double          root of the equation (varnb value that solves the equation)
 *  
 */
double L_zero(KDB* dbv, KDB* dbs, CLEC* clec, int t, int varnb, int eqvarnb)
{
    double  x;

    x = L_newton(dbv, dbs, clec, t, varnb, eqvarnb);
    if(!IODE_IS_A_NUMBER(x)) x = L_secant(dbv, dbs, clec, t, varnb, eqvarnb);

    return(x);
}


/**
 *  Tries to solve a LEC equation by the Newton-Raphson method.
 *  
 *  Subfunction of L_newton().
 *  
 *  The convergence threshold eps is set to 1e-6 by default. However, if algo is set to 1 and 
 *  the absolute value of the endogenous variable before the first iteration is > 1.0, 
 *  then eps is multiplied by the endogenous value. 
 *  
 *  TODO: Some parameters (eps, h) are hard-coded and should be "globalised".
 *  
 *  @param  [in]    int  algo              if not null, the convergence criteria eps is multiplied by the 
 *                                         value of f(x) if ||f(x)|| > 1.0.
 *  @global [in]    int  KSIM_DEBUG        if not null, calls L_debug() to save a trace of the result 
 *  @global [in]    int  KSIM_NEWTON_DEBUG if not null, calls L_debug() to save a trace of each iteration of the solver
 *  
 *  See L_zero() for the description of the other parameters.
 *  
 */
static double L_newton_1(int algo, KDB* dbv, KDB* dbs, CLEC* clec, int t, int varnb, int eqvarnb)
{
    double  oldx, x, fx, fxh, ax, afx, dx = 0.0, ox;
    double  h = CSimulation::KSIM_NEWTON_STEP;
    double  eps = CSimulation::KSIM_NEWTON_EPS;
    int     it = 0;
    double    *d_ptr, shift;

    d_ptr = L_getvar(dbv, varnb) + t;
    oldx = x = d_ptr[0];
    if(!IODE_IS_A_NUMBER(x)) {
        if(CSimulation::KSIM_DEBUG) L_debug("Eq %s - Endo %s -> x is NA\n", KONAME(dbv, eqvarnb), KONAME(dbv, varnb));
        return((double)IODE_NAN);
    }

    // Case 1: equation Y : = f(X) analytically solved
    // or Case 3 (endo-exo)
    if(varnb == eqvarnb || clec->dupendo) {   /* JMP 13-12-01 */
        shift = 0.0;
        ax = fabs(x);
    }
    // Case 2: 0 := f(X,Y) 
    else {
        shift = *(L_getvar(dbv, eqvarnb) + t);
        if(!IODE_IS_A_NUMBER(shift)) return((double)IODE_NAN); 
        ax = fabs(shift);
    }

    if(CSimulation::KSIM_NEWTON_DEBUG)
        L_debug("Eq %s - Endo %s : shift=%lf\n", KONAME(dbv, eqvarnb), KONAME(dbv, varnb), shift);


    if(algo && ax > 1.0) eps *= ax; /* GB 20-06-96 */
    while(it < CSimulation::KSIM_NEWTON_MAXIT) {
        d_ptr = L_getvar(dbv, varnb) + t;
        d_ptr[0] = x;
        ax = fabs(x);

        fx = L_exec(dbv, dbs, clec, t);
        if(CSimulation::KSIM_NEWTON_DEBUG) L_debug("   - f(%lg) = %lg\n", x, fx);

        if(!IODE_IS_A_NUMBER(fx)) {
            if(CSimulation::KSIM_NEWTON_DEBUG) { // Message iniquement en cas de pb
                L_debug("Eq %s - Endo %s : shift=%lf\n", KONAME(dbv, eqvarnb), KONAME(dbv, varnb), shift);
                L_debug("   - f(%lg) = %lg\n", x, fx);
                L_debug("   -> cannot compute f(%lg)\n", x);
            }
            goto err;
        }

        fx -= shift;
        afx = fabs(fx);

        if(afx < eps) {
            if(CSimulation::KSIM_NEWTON_DEBUG) L_debug("   -> %lf\n", x);
            return(x);
        }
        if(ax > 1.0) h = 1e-4 * ax;
        else         h = 1e-4;

        d_ptr = L_getvar(dbv, varnb) + t;
        d_ptr[0] = x + h;
        fxh = L_exec(dbv, dbs, clec, t);
        if(!IODE_IS_A_NUMBER(fxh)) {
            if(CSimulation::KSIM_NEWTON_DEBUG) L_debug("   -> cannot compute f(%lf+%ld)\n", x, h);
            goto err;
        }

        fxh -= shift;
        if(fabs(fxh - fx) < 1e-8) {
            if(CSimulation::KSIM_NEWTON_DEBUG) L_debug("   -> f(x)=%lf == f(x+h)=%lf", fx, fxh);
            goto err;
        }
        if(CSimulation::KSIM_NEWTON_DEBUG) L_debug("     h = %lg; fx=%lg; fxh=%lg\n", h, fx, fxh);
        dx = h * fx / (fxh - fx);
        ox = x;
        x -= dx;
        d_ptr[0] = x;
        fx = L_exec(dbv, dbs, clec, t);
        if(!IODE_IS_A_NUMBER(fx)) x = ox - dx / 4;
        it++;
    }

    if(CSimulation::KSIM_NEWTON_DEBUG) L_debug("    #it > %d\n", it);

err:
    d_ptr[0] = oldx;
    return((double)IODE_NAN);
}


/**
 *  Tries to solve a LEC equation by the Newton-Raphson method. 
 *  
 *  Calls first L_newton_1() with the algo param fixed to 0 (convergence criterion set to 1e-6).
 *  If no solution is found, calls L_newton_1() with algo = 1 to multiply eps by the endogenous value.
 *  
 *  See L_zero() for the parameter description.
 *    
 */
double L_newton(KDB* dbv, KDB* dbs, CLEC* clec, int t, int varnb, int eqvarnb)
{
    double      x;

    x = L_newton_1(0, dbv, dbs, clec, t, varnb, eqvarnb);
    if(!IODE_IS_A_NUMBER(x)) x = L_newton_1(1, dbv, dbs, clec, t, varnb, eqvarnb);
    return(x);
}



/**** Tentative new version (not yet operational) ***/
 
/******
Using a combination of Newton-Raphson and bisection, and the root of a function bracketed
between x1 and x2. The root, returned as the function value SolveNewtonSecant, will be rened until
its accuracy is known within  xacc. funcd is a user-supplied routine that returns both the
function value and the first derivative of the function.
******/


/*
#define SOLVE_MAXIT 100 // Maximum allowed number of iterations.
#define SOLVE_ERROR IODE_NAN
#define SOLVE_ERROR1 (IODE_NAN - 1e20)
#define SOLVE_ERROR2 (IODE_NAN + 1e20)


SolveIsError(double val)
{
    if(IODE_IS_A_NUMBER(val)) return(0);
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
    double    *d_ptr;

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
    double    *d_ptr;
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
    if(!IODE_IS_A_NUMBER(x1)) x1 = 1.0;

    if(SolveFindBrackets(SolveIodeFn, x1, &xl, &xh)) return(IODE_NAN);
    x = SolveNewtonSecant(SolveIodeFn, xl, xh, 1e-7, 100);
    return(x);
}
*/