
<Methods and algorithms>
Methods and algorithms
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&EN <Estimation methods>
&EN <Computational method for least squares techniques>
&EN <Standard statistics>
&EN <Simulation algorithm>

>
<Estimation methods>
Estimation methods
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&TI Introduction
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

The estimation methods owe largely to ~iFair~I (1984). These first two sections
are drawn from this book that we highty recommend.

Econometric models are generally nonlinear, simultaneous, and large. They
also tend to have error terms that are serially correlated. The estimation
methods of IODE try to handle these characteristics.

The notations that will be used in this chapter are as follows.

Write the model as :

.~a|kaa1.jpg|

where
&EN y~-t~= is an n-dimensional vector of endogenous variables,
&EN x~-t~= is a vector of predetermined variables,
&EN a~-i~= is a vector of unknown coefficients,
&EN u~-it~= is an error term.

Assume that the first m equations are stochastic, with the remaining u~-it~= (i = m+1,...,n)
identically zero for all t.

Let J~-t~= be the n x n Jacobian matrix whose ij element is ~a|kaa_1.jpg| (i, j = 1..n).

Also, let u~-i~= be the T-dimensional vector (u~-i1~=,..., u~-iT~= ,.., u~-m1~= ...,u~-mT~= )'.

Let ~ca~C denote the k-dimensional
vector (a~-1~=',..',a~-m~=') of all the unknown coefficients.

Finally, let Gt' be the ki x T matrix whose
t column is ~a"kaa_4.jpg", where ki is the dimension of ai, and let G' be the k x m.T
matrix,

~a"kaa_5.jpg"

where

~a"kaa_6.jpg".

These vectors and matrices will be used in the following section.

&TI Serial correlation and dynamic adjustment model
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

A convenient way of dealing with serially correlated error terms is to treat the serial
correlation coefficients as structural coefficients and to transform the equations into
equations with serially uncorrelated error terms. This introduces nonlinear restrictions
on the coefficients, but otherwise the equations are like any others with serially
uncorrelated coefficients.

Consider the ith equation of (l) and assume that uit, is first-order serially correlated:

.~a"kaa2.jpg"

where eit is not serially correlated.

Lagging (1) one period, multiplying by qi,
and subtracting the resulting expression from (1) yields :

.~a"kaa3.jpg"

The equation includes now yt-1 and xt-1
as variables, and ~a"kaa_7.jpg" as a coefficient, but it
is no more general than (1), and thus one can deal directly with (4) under the asumption
that serial correlation has been eliminated through transformation.

The same procedure can handle serial correlation of higher orders.

With respect to testing for serial correlation, it is well known that the Durbin-Watson
(DW) test is biased toward accepting the null hypothesis of no serial correlation if there is
a lagged dependent variable in the equation.



&TI Estimation techniques
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&EN Ordinary Least squares (OLS)
&EN Two-Stage Least Squares (2SLS)
&EN Three-Stage Least Squores (3SLS)
&EN Joint estimation of a non simultaneous system
..... &EN Full Information Maximum Likelihood (FIML)


&IT Ordinary Least squares (OLS)
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

The OLS technique is a special case of the 2SLS technique, where Di in (5) and (6)
below is the identity matrix. It is thus unnecessary to consider this technique separately
from the 2SLS technique.

&IT Two-Stage Least Squares (2SLS)
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

2SLS estimates of a~-i~= (say ƒ~-i~=) are obtained by minimizing


.~a"kaa4.jpg"


with respect to a~-i~=, where Z~-i~= is a T x K~-i~= matrix of predetermined variables. Z~-i~= and K~-i~=
can differ from equation to equation. An estimation of the covariance matrix of ƒ~-i~= (say V^~-2ii~=) is :

~a"kaa5.jpg"

where G^~-i~= is G~-i~= evaluated at ƒ~-i~=, and

~a"kaa_11.jpg".

The 2SLS estimator in this form is presented in Amemiya(L974).It handles the case
of nonlinearity in both variables and coefficients. In earlier work, Kelejian (1971)
considered the case of nonlinearity in variables only. Bierens (1981,p.106) has pointed
out that Amemiya s proof of consistency of this estimator is valid only in the case of
linearity in coefficients, that is, only in Kelejian's case. Bierens supplies a proof of
consistency and asymptotic normality in the general case.

&IT Three-Stage Least Squores (3SLS)
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

3SLS of a (say ƒ) are obtained by minimizing

~a"kaa6.jpg"

with respect to a, where S^ is a consistent estimate of S and Z is a T x K matrix of
predetermined variables. As estimate of the covarialce matrix of ƒ (say V^~-3~=) is

~a"kaa7.jpg"

where G^ is G evaluated at ƒ. S is usually estimated from the 2SLS estimated
residuals. This estimator is presented in Jorgenson and Laffont (1974), and it is further
discussed in Amemiya(1977). Both prove consistency and asymptotic normality of 3SLS.


&IT Joint estimation of a non simultaneous system
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Two aims can lead to jointly estimate several equations : equality restrictions
on coefficients of different equations, and the presumed existence of contemporaneous
correlation of the residuals (Seemingly unrelated equations : SURE). In this case, the joint
estimation of the equations increases the efficiency of the estimators.

Algorithmically, the method used is the Zellner method which consists in
minimizing a weighted sum of the residuals. The weighting matrix is made of
the contemporaneous covariance matrix of the residuals estimated with the
residuals of a first step OLS on the equations. The Zellner can then simply
be considered as a special case of the 3SLS method where the matrix of
instruments is an identity matrix. The matrix D above is identically equal
to the matrix

~a"kaa_13.jpg"
>

<Computational method for least squares techniques>
Computational method for least squares techniques
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&TI Linear and nonlinear least squares
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

The least squares estimation routine has been designed to be very general and
involve all the least squares methods: linear and nonlinear, single or system of equations,
uses of instrumental variables or not, with or without correction for contemporaneous
autocorrelation of residuals. The core of the computation is made of an iterative process
which will be shown with the help of a little example.

Assume that the following equation is to be estimated:

~a"kaa16.jpg"

where y is a (T,1) vector of observations of the dependent variable, X is the (T,Q)
matrix of observations of the independent variables. The (Q, 1) vector a of coefficient are
to be estimated. and e is a (T,1) vector of residuals, normally distributed with mean O and
standard deviation s. It is also assumed that E(ee')=s~+2~= I, I being the (T,T) identity matrix.
The function f is not necessarily linear in the variables and in the coefficients.

The estimator of a is the vector ƒ which minimizes the sum of squares of the residuals
of the equation (17):

~a"kaa17.jpg"

Let S=ee' be the sum of squares of the residuals. Equation (17) can be linearized by
a Taylor expansion limited to the first term:

~a"kaa18.jpg"

where v=u(ƒ~-O~=) + e, and u(ƒ~-O~=) represents the terms neglected in the Taylor expansion,
ƒ~-O~= is a starting value for ƒ, and y~-O~= is defined by y~-O~= = f(X, ƒ~-O~=). Equation (18) is linear in
~a"kaa_26.jpg". These parameters can be estimated by OLS, i.e. by

~a"kaa19.jpg"

A new value for the coefficients can then be computed

~a"kaa20.jpg"


This operation will be iterated until ~a"kaa_27.jpg", with k the iteration number, becomes
sufficiently close to zero. At this stage, ƒ~-k~= ƒ, and then y~-k~= = f(X, ƒ), u~-k~= = 0 and S = e'e.

The iterative process could well not converge, although it can be proved that, if at
each iteration ~a"kaa_27.jpg" is weighted by ~a"kaa_30.jpg", with ~a"kaa_31.jpg", so that ~a"kaa_32.jpg", the process
must converge for sufliciently small values of ~a"kaa_30.jpg", provided that there is a minimum.

It is very easy to approximate the derivative of the f function, this is done numerically.
In the linear case, the approximation is exact and gives the matrix of regressor
observations. The iterative process reaches a solution within two iterations. In the
nonlinear case, the matrix of derivatives is no more the matrix of observations, and a
progressive correction process of the coefficients is repeated until the correction to apply
is sufficiently small, i.e. smaller than a convergence threshold given by the user. At this
point, the estimators minimize a weighted sum of the residuals of the equations, the S =
S(ƒ) function, the complexity of which is determined by the degree of non linearity of the
system of equations. Nothing guaranties in very special nonlinear cases that the process
will eventually converge, or converge within the maximum number of iterations which was
given by the user.

A weight (~a"kaa_30.jpg" in the foregoing) can be introduced to accelerate convergence for each
coefficient, these weights are called step relaxation parameters or smoothing parameters,
by default their values are one, this value can be updated in the equation update screen.
The step relaxation parameters are important for another reason: if the value of one or
more relaxation parameters is set to zero, the initial value of the corresponding
coefficients will be unchanged. This is a way to constraint a coefficient to a predetermined
value.

The same iterative procedure is applied for a system of equations, y~-k~= and u~-k~= are no
longer vectors of the T computed values of the endogenous variable and residuals, but are
matrices of rank T times the number of equations of the system.

~a"kaa21.jpg"

~a"kaa22.jpg"

~a"kaa23.jpg"

2SLS and 3SLS are incorporated in the procedure by first calculating a weighting
matrix (a metric) using the specified instrumental variables, and then using this metric
in the iterative computation of the least squares estimators of ~a"kaa_35.jpg". In this case, (22)
becomes

~a"kaa24.jpg"

with ~a"kaa_35a.jpg"

3SLS and Zellner method require an additional iterative process : the first iterative
process allows to compute an estimate of the covariance matrix of the contemporaneous
residuals, the second iterative process reestimates the coefficients by correcting the
metric matrix. (22) becomes :

.~a"kaa25.jpg"

with ~a"kaa_36.jpg", and S estimated during the first step.

The least squares estimation program is very general. It can handle linear or nonlinear
equations, single equations or a system of equations. The estimated coefficients
have the BLUE properties if the model is linear, but only the asymptotic properties are
ensured for non-linear models.

The estimated covariance matrix of the coefficients is exact for the linear models, but
only asymptotically exact for non-linear models.

&TI FIML
ÄÄÄÄÄÄÄÄ

The algorithm used in KaA to calculate FIML estimates is a Gauss-Newton one.
Numerical derivatives of the likelihood function are computed like in the least squares
algorithm. The weighting matrix is the same as in the 3SLS procedure, implying that the user must specify
a meaningful set of instrumental variables. (Moreover, the 3SLS estimates are strongly recommended as starting values.)

>
<Standard statistics>
Standard statistics

A limited number of statistics are given as standard output by the estimation
program.

Per coefficient : the value of the coefficient, the standard error of the coefficient, and
the t-statistics. In the non-linear case the standard errors of the coefficients only
represent a lower limit of the true standard errors which will be reached asymptotically.

When the model is a linear single equation with a constant term, the total variance
of the endogenous variable is equal to the variance explained by the regression plus the
residual variance. The R2 statistics represents the explained share of the total variance
in percents. This statistics is corrected for the number of coefficients. Out of this limited
case, the interpretation of this statistics is difficult and hazardous.

The standard error gives a more straightforward interpretation of the quality of the
adjustment.
>
<Simulation algorithm>
Simulation algorithm
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&TI The Gauss-Seidel technique
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Most macroeconometric models are solved using the Gauss-Seidel technique. It is a
simple technique and in most cases works remarkably well. This technique is
used in IODE to simulate models. The Gauss-Seidel technique is easy
to describe by means of an example.

Assume that the model consists of three equations, and let X~-it~= denote the vector of
predetermined variables in equation i. The model is as follows :

.~a"kaa26.jpg"

.~a"kaa27.jpg"

.~a"kaa28.jpg"

The technique requires that the equations be rewritten with each endogenous
variable on the LHS of one equation. This is usually quite easy for macroeconometric
models, since most equations have an obvious LHS variable. If, say, the LHS variable for
an estimated equation is ~cln(y~-2t~= / y~-1t~=),~C then y~-2t~= can be written on the LHS by taking
exponents and multiplying the resulting expression by y~-3t~=. The program can do that job
for you since, for each equation, it requires the definition of the endogenous variable
specified by the equation, provided that the endogenous variable appears only once in
the equation. The endogenous variable can appear either on the LHS or in the RHS.
Simulation of a model requires moreover that the endogenous variable be specified only
once in the full set of equations.

When these transformations of the equations are done, the model can be rewritten
as follows :

.~a"kaa293031.jpg"

In order to solve the model, values of the coefficients and the error terms are needed.
Given these values and given values of the predetermined variables, the solution proceeds
as follows. Initial values of the endogenous variables are guessed. These are either the
actual values or extrapolations from the previous period according to some rules. IODE
allow various strategies. Given these values the equations can be
computed successively and produce a new set of values for the endogenous variables.
Given this new set of values, the equations can again be solved to get another set.
Convergence is reached if for each endogenous variables the values on successive
iterations are within some prescribed tolerance level.

There is no guarantee that the Gauss-Seidel method will converge. The advantage of
the technique, however, is that it can usually be made to converge (assuming an actual
solution exists) with sufficient damping. By damping is meant the following. Let y~-k-1~=
denote the solution value of y for iteration k-1 and let y^~-k~= denote the value computed by
solving the equation on iteration k. Instead of using y~-k~= as the solution value for iteration
k, one can adjust y~-k-1~= only partway toward y~-k~=.

.~a"kaa32.jpg"

If ~a"kaa_37.jpg" is 1, there is no damping.

The question of what to use as a stopping rule is not as easy as it might sound. The
stopping rule can either be in absolute or percentage terms. In absolute terms it is

.~a"kaa33.jpg"

and in percentage it is

.~a"kaa34.jpg"

where ~a"kaa_38.jpg", is the tolerance criterion for variable i. If damping
is used y~-it,k~= must be replaced ~a"kaa_39.jpg".

A global criterion can also be used

.~a"kaa35.jpg"

The problem is to choose tJee values for ~a"kaa_40.jpg". It is inconvenient to have to choose
different values of the tolerance criterion for different variables, and one would like to use
just one value of e throughout. This is not, however, a sensible procedure if the units of
the variables differ and if the absolute criterion is used. The problem is lessened if the
percentage criterion is used, but in this case one must be concerned with variables that
can be close to zero or can be zero.

The solution adopted in IODE is to compute a stopping rule which combines the three
afore-mentioned criteria

.~a"kaa36.jpg"

		     ~a"kaa36b.jpg"

&TI Equation reordering
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

To increase the simulation speed, the model can be splitted Into connex components.
Each component will be reorganized by an heuristical algorithm to obtain a quasi subdiagonal
model.

Let E denote tle incidence matrix:

.~a"kaa37.jpg"

After decomposition, the incidence matrix E has the following form :

~a"kaa_41.jpg"

&TI Connex components construction
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Let VARS be the list of endogenous variables

&EN 1. Take the first remaining endogenous variable in VARS : ENDO~-0~=
&EN 2. Compute SUCC = list of variables depending on ENDO~-0~= (recursively)
&EN 3. Compute PRE = list of variables from which depends ENDO~-0~= (recursively)
&EN 4. Compute CFC~-i~= intersection between SUCC and PRE : new connex component
&EN 5. Let VARS = VERS \ CFC~-i~
&EN 6. If ~a"kaa_44.jpg"go to 1, else stop

&IT Reordering of the connex components
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Construct the E' matrix :

.~a"kaa38.jpg"

To make E' lower triangular, apply the following algorithm (O is the new order of
connex components) :
&EN 1. O={}
&EN 2. Choose ~a"kaa_44.jpg"
&EN 3. Set ~a"kaa_45.jpg"
&EN 4. Add i in O
&EN 5. Go to 2 if at least one CFC remains

&IT Triangulation of a connex component
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Each connex component (CFC) can be reorganized to minimize the computation time
required to solve the corresponding system. The following algorithm reduces
approximatively the number of iterations by a factor 2. It is based on the fact that to
maximize the information during an iteration, the extra-diagonal elements of E must be
as close as possible to the diagonal.

~a"kaa_46.jpg"


&IT Goal seeking
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Given n pair ENDOi-EXOi variables, the solution of the system of equations must
now be computed for the new set of variables consisting of the initial group in wich the
ENDO variables for the given list have been replaced by EXO variables.

If we where to use a NEWTON algoritm or an assimilated method, we would just have
to solve the system for the new set of variables. In the case of the GAUSS algorithm however
one must declare a new output variable for each equation.

As an example suppose we do one endo-exo exchange. (for more pairs we just repeat
the proces).

The goal is to find a pair of equations, the first of wich contains EXO and the
endogenious variable of the second being ENDO

We therefore construct a structured tree as below where an arrow indicates
that a variable belongs to the definition equation of another variable.

.~a"kaa_50.jpg"

When ENDO is found (ENDO: ENDOi..j) we replace the endogenous variable
of this equation by the one of the higher level in the tree structure and repeat this until the
endogenous variable of the first equation is replaced by EXO.

Once the output variables are redefined in this way, the modified equations are retranslated
before execution or, if analytical transform is not possible, solved with a
discrete NEWTON method during simulation.
>


.............#include "fiml.m"











