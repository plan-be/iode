
<IODE: Model Simulation>
..sep |
..esc ~
..ignore ¯

IODE: Model Simulation
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯

&TI The Gauss-Seidel algorithm
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯

A macroeconomic model is a system of nonlinear equations that must be solved with respect to its endogenous variables, say {yi}. 

Most macroeconometric models are solved using the Gauss-Seidel technique. It is a
simple technique and in most cases works remarkably well. This technique is
used in IODE to simulate models. The Gauss-Seidel algorithm is based on the fixed-point theorem
 (see https://en.wikipedia.org/wiki/Fixed-point_theorems) and is easy
to describe by means of an example.

Assume that the model consists of n equations: 
&CO
f1(y1, y2, y3..., yn) = 0
f2(y1, y2, y3..., yn) = 0
f3(y1, y2, y3..., yn) = 0
  ...                   
fn(y1, y2, y3..., yn) = 0
&TX

The Gauss-Seidel technique requires to rewrite the equations in such a way that each endogenous
variable is placed on the left member of the equations. 

For example, if the equation is 
&CO
    ln(y1 / y2) : = y3 + x1
&TX

then y2 can be moved to the right side of the equation:
&CO
    y1 = y2 * exp(y3 + x1)
&TX
    
IODE tries to do that for each equation, provided that the endogenous variable appears only once in
the equation. The endogenous variable can appear either on the left or on the right.

When these transformations of the equations are done, the model can be rewritten as follows :
&CO
    y1 = f1(y2, y3..., yn)
    y2 = f2(y1, y3..., yn)
    y3 = f3(y1, y2..., yn)
     ...                 
    yn = fn(y1, y2..., yn-1)
&TX

Note: when an equation cannot be transformed in the form yn = fn(...), a "sub-algorithm" based on the Newton-Raphson 
or secant method is used to numerically solve the equation with respect to its endogenous variable. 
See below for more information on that sub-algorithm.

The Gauss-Seidel algorithm then proceeds as follows. 

Initial values of the endogenous variables are selected. These are either the
actual values or extrapolations from the previous period according to some rules. IODE
allow various strategies. 

Given these values the equations can be computed successively and produce a new set of values for the endogenous variables {y1...yn}.
With this new set of values, the equations can again be calculated to get another set {y1...yn}, etc.

Convergence is reached if, for each endogenous variable, the values of successive
iterations are within some predefined tolerance level.

There is no guarantee that the Gauss-Seidel method will converge. The advantage of
the technique, however, is that it can usually be made to converge (assuming an actual
solution exists) with sufficient "damping", as described below.

Let
&CO 
    yi[k-1] denote the solution value of endogenous yi for iteration k-1 
    yi[k]   denote the value computed by solving the equation on iteration k. 
&TX
    
Instead of using yi[k] as the new value for iteration k, one can adjust yi[k] by combining yi[k-1] and yi[k]:
&CO
    y1[k] = y1[k-1] + lambda * (y[k] - y[k-1]) where 0 << lamda <<= 1
&TX

If lambda == 1 is 1, there is no damping.

The solution is reached when the difference between 2 iterations is under a defined threshold for each endogenous {y1...yn}.


&TI Endo-Exo exchanges
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
It is possible to exchange the status of an exogenous variable with that of an endogenous variable. 
That allows, when the value of an endogenous variable is known in advance, 
for example during the first simulation period, to block this endogenous variable by letting an exogenous variable vary 
so that the known value of the endogenous variable is preserved. 

The method used is to find a path between the exo and the endo by scanning the equations of the model and to modify 
the "status" of the variables accordingly as explained below.
 
If "exo" appears in the equation "endo", the path is trivial and we simply replace the endogeneous of that equation.
Note that this requires the use of a Newton-Raphson method to solve the equation with respect 
to the (new) endo (ex exo) variable during the simulation process. 

If exo does not appear in the equation, we analyse each other endogenous variable in the equation to see 
if their defining equation contains the variable "exo". 
If so, the status in this equation is exchanged and the path between exo and endo is found in one step. 
If not, we continue the process recursively until we have found a path between "endo" and "exo".

The example below should clarify the process.
&CO 
    EQ                              ENDO
    ------------------------------------
    y1 = f1(y2, x1)                 y1
    y2 = f2(y3, x1)                 y2
    y3 = f3(y1, y2, x1, x2)         y3
&TX

Suppose the exchange ~cy1-x2~C. 

&CO
x2 does not appear in f1. The only endogenous var in f1 is y2 whose equation is f2. 
x2 does not appear in f2. The only endogenous var in f2 is y3 whose equation is f3. 
x2 DOES appear in f3. 
    ->> x2 becomes the new endogenous of the f3. But as y3 must keep its endogenous "status", we have to change f2 as well.
    ->> y3 becomes the new endogenous of the f2. Again, y2 must keep its endogenous "status", thus it becomes endo of f1
    ->> y2 becomes the new endogenous of the f1. 
&TX

Finally, whe obtain the following reorganisation of the model:
&CO
    EQ                              ENDO
    ------------------------------------
    y1 = f1(y2, x1)                 y2
    y2 = f2(y3, x1)                 y3
    y3 = f3(y1, y2, x1, x2)         x2
&TX

The model is thus solve with respect to {y2,y3,x2} instead of {y1,y2,y3}. The value of y1 if therefore left unchanged.


&TI Solving a single equation
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Depending on their form, LEC equations can be solved analytically with respect to their endogenous variable - or not.

For example, an equation of the type
&CO
    d(Y / X) := c1 + c2 * ln(Y / Z)
&TX

cannot be solved analytically because Y appears more than once (on the left and/or on the right member).

Conversely, the equation 
&CO
    d Y : = c1 + c2 * X + c3 * t
&TX
    
is invertible with respect to Y:
&CO
    Y : = Y[-1] + c1 + c2 * X + c3 * t
&TX

During the Gauss-Seidel iteration, we must calculate the value of the endogenous variable for each equation. 
We have 3 possibilities listed below.

&IT Case 1: the equation is analytically solved with respect to its endogenous
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Consider the LEC equation
&CO
    d Y := c1 + c2 * X 
&TX
    
If its endogenous variable is Y, it will be translated internally by IODE as follows:
&CO
    Y[-1] + c1 + c2 * X
&TX
    
When simulating the model, this formula is calculated and the result is stored in Y.

In the code below, this is the case where clec->>dupendo is zero AND varnb == eqvarnb (dupendo == 0 means that the equation 
contains exactly one occurence of Y).

&IT Case 2: the equation must be numerically solved
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
The equation 
&CO
    ln Y := c1 + c2 * X / ln Y
&TX

contains twice the variable Y and will be translated into  
&CO
    ln Y - (c1 + c2 * X / ln Y)
&TX

Note that the result of this formula is 0, and not Y, unlike case 1.

When calculating the model, this equation must be solved numerically with respect to Y before storing the result in Y.

In the code, this is the case where clec->>dupendo is non-zero (Y appears more than once).

&IT Case 3: ENDO-EXO exchanges
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
If an equation is inverted in an endo-exo exchange (e.g. Y becomes exogenous and X becomes endogenous), 
the equation will be solved numerically as above (case 2). Obviously, one must take into account the fact 
that the translated version of the equation provides the value of Y (and not 0) 
Y must therefore be subtracted from the calculation.

Indeed, the equation 
&CO
    d Y := c1 + c2 * X
&TX    
is transformed by IODE into 
&CO
    Y := c1 + c2 * X - Y[-1]
&TX
Hence, we will search the value of X such that
&CO
     0 := Y[-1] + c1 + c2 * X - Y 
&TX
     
In the code, this is the case where varnb != eqvarnb.

&TI The Newton-Raphson algorithm
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯

Search for x such as 
&CO
|f(x)| << KSIM_EPSNEWTON
&TX

We start by calculating shift = the difference between 0 and the value obtained 
by the LEC formula of the equation (according to the case 1, 2 or 3 described above).

We then loop (maximum KSIM_NEWTON_MAXIT iterations) to find the solution x : 
&CO
// check convergence
if |f(x) - shift| << KSIM_NEWTON_EPS: x is a solution

// Define derivative step h 
h = 1e-4    
if |x| << 1.0 then h *= |x|

// Move x in the direction of the local tangent 
dx = h.f(x) /(f(x+h)-f(x))
if f(x - dx) is NaN: dx = dx / 4
x = x - dx
&TX

Obviously, the loop also stops if x or f(x) becomes NaN.


&TI The Secant algorithm
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
That (basic) secant method first requires to determine an interval [xl, xr] containing a root of the 
equation (xl/xr stands for x-lec-ft/right). 
In other words, the sign of f(xl) must be opposite to that of f(xr).

Then the size of that interval is decreased until 
&CO
    |xl - xr| becomes << eps 
&TX
by applying the formula below:
&CO
    xr = xl + (xr - xl) * f(xl) / (f(xl) -f(xh))
&TX    
If the sign of f(xr) has changed, switch xr and xl. 
  
Continue until |xl - xr| is << eps


&TI Model solver
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&IT k_sim_main.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Main functions for model simulations.

..tb
| Syntax                                                                                | Description
| ~cint K_simul(KDB* dbe, KDB* dbv, KDB* dbs, SAMPLE* smpl, char** endo_exo, char** eqs)| Simulates a model defined by a set of equations and optional replacements endo-exo.
| ~cvoid K_simul_free()                                                                 | Frees all temporary allocated memory for the simulation.
| ~cdouble K_calc_clec(int eqnb, int t, int varnb, int msg)                          | Tries to find a value for varnb[t] that satifies the equality in the equation eqnb. 
..te


&IT k_sim_order.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Functions to reorder a model to optimize the Gauss-Seidel simulation algorithm. 

..tb
| Syntax                                           | Description
| ~cint KE_order(KDB* dbe, char** eqs)             | Reorders a model before the simulation to optimise the execution order of the set of equations.
| ~cint KE_poseq(int posendo)                      | Searches the equation whose endogenous is the variable posendo. 
| ~cvoid KE_tri(KDB* dbe, int** tmp, int passes)   | Sort the equations by making successive 'pseudo-triangulation' passes.
..te

&IT k_sim_scc.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Alternative functions to reorder and simulate very large models. 

The reordering algorithm being CPU intensive for very large models, it is better to first reorder the model and save that order 
in such a way as to be able to run several simulations with the same order later on. 

..tb
| Syntax                                                                                           | Description
| ~cint KE_ModelCalcSCC(KDB* dbe, int tris, char* pre, char* inter, char* post)                        | Reorders the model defined by dbe and saves 3 lists with prolog, epilog and interdependent blocks.     
| ~cint K_simul_SCC(KDB* dbe, KDB* dbv, KDB* dbs, SAMPLE* smpl, char** pre, char** inter, char** post) | Simulates a model in the order given by 3 lists of tables of equation names: pre, inter and post.
..te

&IT k_sim_exo2endo.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
It is possible to exchange the status of an exogenous variable with that of an endogenous variable. 
This allows, when the value of an endogenous variable is known in advance, 
for example during the first simulation period, to block this endogenous variable by letting an exogenous variable vary 
so that the known value of the endogenous variable is preserved. 

Another way to view the process is to say that the model is solved with respect to another list of variables.

..tb
| Syntax                                     | Description
| ~cint KE_exo2endo(int posendo, int posexo) | Modify the model to solve it with respect to another set of variables
..te


&TI Single equation solver
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
When an equation is not analytically solved during the compilation process or if it has been inverted 
by the endo-exo exchange, that equation must be solved numerically during the simulation.

Two methods are used: a simple Newton-Raphson method or a secant method in case of non convergence.

&IT l_newton.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                     | Description
| ~cdouble L_zero(KDB* dbv, KDB* dbs, CLEC* clec, int t, int varnb, int eqvarnb)   | Solves numerically a LEC equation for one period of time with respect to a given variable. If the Newton-Raphson method does not reach a solution, tries a bisection (secant) method. 
| ~cdouble L_newton(KDB* dbv, KDB* dbs, CLEC* clec, int t, int varnb, int eqvarnb) | Tries to solve a LEC equation by the Newton-Raphson method. 
..te

&IT l_secant.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                     | Description
| ~cstatic double L_fx(double x, int t)                                             | Computes the value of f(x) in time t
| ~cdouble L_secant(KDB* dbv, KDB* dbs, CLEC* clec, int t, int varnb, int eqvarnb)  | Tries to find a solution to the equation clec by a secant method. 
..te

>


