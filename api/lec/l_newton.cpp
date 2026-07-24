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
 *  In the code below, this is the case where clec->duplicated_endo is zero AND varnb == eqvarnb.
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
 *  In the code, this is the case where clec->duplicated_endo is non-zero.
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
 *      double zero(KDBVariablesPtr dbv, KDBScalarsPtr dbs, int t, int varnb, int eqvarnb)    Solves numerically a LEC equation for one period of time with respect to a given variable. 
 *                                                                                      If the Newton-Raphson method does not reach a solution, tries a bisection (secant) method. 
 *      double newton(KDBVariablesPtr dbv, KDBScalarsPtr dbs, int t, int varnb, int eqvarnb)  Tries to solve a LEC equation by the Newton-Raphson method. 
 *  
 */
#include <math.h>
#include "api/lec/lec.h"
#include "api/simulation/simulation.h"

/**
 *  Solves numerically this LEC equation for one period of time with respect to a given variable (varnb).  
 *  If the Newton-Raphson method does not reach a solution, tries a bisection (secant) method. 
 *  
 *  
 *  @param [in] KDBVariablesPtr dbv     KDB of VAR with which the equation has been linked
 *  @param [in] KDBScalarsPtr   dbs     KDB of Scalar with which the equation has been linked
 *  @param [in] int             t       time of calculation (index in dbv Sample)
 *  @param [in] int             varnb   position of the endogenous variable in dbv
 *  @param [in] int             eqvarnb position of the initial endogenous variable (i.e. equation name) in dbv
 *  @return     double                  root of the equation (varnb value that solves the equation)
 *  
 */
double CLEC::zero(KDBVariablesPtr dbv, KDBScalarsPtr dbs, const int t, const int varnb, const int eqvarnb)
{
    double x = this->newton(dbv, dbs, t, varnb, eqvarnb);
    if(!IODE_IS_A_NUMBER(x)) 
        x = this->secant(dbv, dbs, t, varnb, eqvarnb);
    return x;
}


/**
 *  Tries to solve a LEC equation by the Newton-Raphson method.
 *  
 *  Subfunction of newton().
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
 *  See zero() for the description of the other parameters.
 */
double CLEC::newton_sub(const int algo, KDBVariablesPtr dbv, KDBScalarsPtr dbs, const int t, 
    const int varnb, const int eqvarnb)
{
    double  oldx, x, fx, fxh, ax, afx, dx = 0.0, ox;
    double  h = CSimulation::KSIM_NEWTON_STEP;
    double  eps = CSimulation::KSIM_NEWTON_EPS;
    int     it = 0;
    double  *d_ptr, shift;

    d_ptr = L_getvar(dbv, varnb) + t;
    oldx = x = d_ptr[0];
    if(!IODE_IS_A_NUMBER(x)) 
    {
        if(CSimulation::KSIM_DEBUG) L_debug("Eq %s - Endo %s -> x is NA\n", dbv->get_name(eqvarnb), dbv->get_name(varnb));
        return (double) IODE_NAN;
    }

    // Case 1: equation Y : = f(X) analytically solved
    // or Case 3 (endo-exo)
    if(varnb == eqvarnb || this->duplicated_endo) 
    {
        shift = 0.0;
        ax = fabs(x);
    }
    // Case 2: 0 := f(X,Y) 
    else 
    {
        shift = *(L_getvar(dbv, eqvarnb) + t);
        if(!IODE_IS_A_NUMBER(shift)) 
            return (double) IODE_NAN; 
        ax = fabs(shift);
    }

    if(CSimulation::KSIM_NEWTON_DEBUG)
        L_debug("Eq %s - Endo %s : shift=%lf\n", dbv->get_name(eqvarnb), dbv->get_name(varnb), shift);

    if(algo && ax > 1.0) 
        eps *= ax;
    
    while(it < CSimulation::KSIM_NEWTON_MAXIT) 
    {
        d_ptr = L_getvar(dbv, varnb) + t;
        d_ptr[0] = x;
        ax = fabs(x);

        auto this_shared = std::make_shared<CLEC>(*this);
        fx = L_exec(dbv, dbs, this_shared, t);
        if(CSimulation::KSIM_NEWTON_DEBUG) 
            L_debug("   - f(%lg) = %lg\n", x, fx);

        if(!IODE_IS_A_NUMBER(fx)) 
        {
            if(CSimulation::KSIM_NEWTON_DEBUG) 
            {
                L_debug("Eq %s - Endo %s : shift=%lf\n", dbv->get_name(eqvarnb), dbv->get_name(varnb), shift);
                L_debug("   - f(%lg) = %lg\n", x, fx);
                L_debug("   -> cannot compute f(%lg)\n", x);
            }
            goto err;
        }

        fx -= shift;
        afx = fabs(fx);
        if(afx < eps) 
        {
            if(CSimulation::KSIM_NEWTON_DEBUG) 
                L_debug("   -> %lf\n", x);
            return x;
        }

        if(ax > 1.0) 
            h = 1e-4 * ax;
        else         
            h = 1e-4;

        d_ptr = L_getvar(dbv, varnb) + t;
        d_ptr[0] = x + h;
        auto this_shared2 = std::make_shared<CLEC>(*this);
        fxh = L_exec(dbv, dbs, this_shared2, t);
        if(!IODE_IS_A_NUMBER(fxh)) {
            if(CSimulation::KSIM_NEWTON_DEBUG) 
                L_debug("   -> cannot compute f(%lf+%ld)\n", x, h);
            goto err;
        }

        fxh -= shift;
        if(fabs(fxh - fx) < 1e-8) 
        {
            if(CSimulation::KSIM_NEWTON_DEBUG) 
                L_debug("   -> f(x)=%lf == f(x+h)=%lf", fx, fxh);
            goto err;
        }
        if(CSimulation::KSIM_NEWTON_DEBUG) 
            L_debug("     h = %lg; fx=%lg; fxh=%lg\n", h, fx, fxh);
        dx = h * fx / (fxh - fx);
        ox = x;
        x -= dx;
        d_ptr[0] = x;
        auto this_shared3 = std::make_shared<CLEC>(*this);
        fx = L_exec(dbv, dbs, this_shared3, t);
        if(!IODE_IS_A_NUMBER(fx)) 
            x = ox - dx / 4;
        it++;
    }

    if(CSimulation::KSIM_NEWTON_DEBUG) 
        L_debug("    #it > %d\n", it);

err:
    d_ptr[0] = oldx;
    return (double) IODE_NAN;
}


/**
 *  Tries to solve this LEC equation by the Newton-Raphson method. 
 *  
 *  Calls first newton_sub() with the algo param fixed to 0 (convergence criterion set to 1e-6).
 *  If no solution is found, calls newton_sub() with algo = 1 to multiply eps by the endogenous value.
 *  
 *  See zero() for the parameter description.
 *    
 */
double CLEC::newton(KDBVariablesPtr dbv, KDBScalarsPtr dbs, const int t, 
    const int varnb, const int eqvarnb)
{
    double x = this->newton_sub(0, dbv, dbs, t, varnb, eqvarnb);
    if(!IODE_IS_A_NUMBER(x)) 
        x = this->newton_sub(1, dbv, dbs, t, varnb, eqvarnb);
    return x;
}
