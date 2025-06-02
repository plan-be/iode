.. _lec_language:

The LEC Language
================

Whether for *equations*, construction *identities*, or charts, an appropriate mathematical 
formula language is essential. The *LEC* language ("*Langage Econométrique Condensé*") has 
been designed to be well-suited for formulas involving time series. 
It is used whenever a formula is needed in IODE.

The *LEC* language has been designed to be close to the way formulas are written in the literature.

Example 1: Consumption equation:

.. math::

    C_t = a + b~\frac{Y_t}{P_t} + c * C_{t-1}

is translated into the *LEC* language as follows::

    C := a + b * Y / P + c * C[-1]     
    
Example 2: Production equation:

.. math::

    ln(Q_t) = a~ln(K_t) + (1 - a)~ln(L_t) + c * t + B

is translated into the *LEC* language as follows::

    ln Q := a * ln K + (1 - a) * ln L + c * t + b

The elements of the LEC language can be classified into several categories:

- :ref:`constants <constants>`
- :ref:`variables <variables>`
- :ref:`scalars <scalars>`
- :ref:`logical operators <logical_operators>`
- :ref:`algebraic operators <algebraic_operators>`
- :ref:`mathematical functions <math_functions>`
- :ref:`time functions <time_functions>`
- :ref:`lists or macros <lists>`
- :ref:`lags, leads, and period values <lags>`
- :ref:`comments <comments>`

At the end of the section, the following points are detailed:

- :ref:`operator precedence <op_precedence>`
- :ref:`writing equations <writing_equations>`
- :ref:`summary of the syntax <syntax_summary>`


.. _constants:

Constants
---------

Numeric constants
~~~~~~~~~~~~~~~~~

Numeric constants are simple values that can be integers or real numbers:

- Integer constants::

    2, 51234, 12345634

- Real constants in floating-point format::

    2.234, 0.000123

- Real constants in exponential format::

    2.234e-3, 1E5, 1.2E-03


Temporal constants
~~~~~~~~~~~~~~~~~~

Variable t
^^^^^^^^^^

The IODE *variables*, which are time series, are defined over a limited range of periods: 
the *sample* of the *variables* workspace determines this range of periods. 
For example, 1970Y1 to 2000Y1 means that the series are annual (Y) and defined from 1970 to 2000.

You can use temporal constants in the LEC language with a similar format. 
They are used to take the value of a series at a fixed period, for example::

    1990Y1    
    1970Q4    
    2010M11

They can also be useful in operations such as::

    X * (t < 1990Y1) + Y * (t >= 1990Y1)

The formula above equals :math:`X` if the current execution period is before 1990, and :math:`Y` otherwise. 
The special variable :math:`t` represents the index of the current execution period.

Temporal constants take an integer value, which is the index in the time series of the current 
execution period. Suppose the *variables* workspace is defined on a *sample* from 1970Y1 to 1995Y1. 
The numeric equivalent of the temporal constant 1970Y1 is 0. Similarly, 1980Y1 is 10, etc.
Then, why not simply indicate 0 or 10 instead of 1970Y1 or 1990Y1? Simply because the same formula 
can be executed on *variables* workspaces with different *samples* and remain correct. If the *sample* 
extends from 1975Y1 to 1995Y1, the value of 1980Y1 is no longer 10 as in the first case, but 5. 
The formula has not changed, but its execution adapts to the context.

The constant :math:`t` therefore indicates the index of the current execution period. 
It has an integer value.

Let's take another example. Suppose the current *variables* workspace is defined on the *sample* 
1990Q1 - 1999Q4 and a model simulation is running over the period 1992Q1 to 1994Q4. 
The value of :math:`t` will change at each simulated period: in 1992Q1, :math:`t` is 8, 
in 1992Q2, :math:`t` is 9, and so on.

In the formula::

    if(t < 1993Q1, 0, 12.3)

1993Q1 is 12 and :math:`t` is 8, 9, 10, 11, ...19, depending on the current simulation period. 
The value of the formula remains 0 as long as :math:`t` is less than 12, i.e., until 1992Q4, 
and 12.3 afterwards.

The same principle can be applied to the calculation of *identities*, which are also defined 
on a *sample* and thus make the value of :math:`t` evolve.

Variable i
^^^^^^^^^^

A *LEC* formula is always calculated for a given time :math:`t`, but in a sub-expression, 
:math:`t` can be different. This is the case, for example, for the operators d(), dln(), 
sum(), ... In those operators, the difference between the period :math:`t` of the formula 
and the period :math:`t` of the current sub-expression is defined by the special variable 
:math:`i`.

For example, the expression::

    sum(t-2, t-4, A / (1 - i)**2),

is equivalent to:

.. math::

    \frac{A[-2]}{(1-(-2))^2} + \frac{A[-3]}{(1-(-3))^2} + \frac{A[-4]}{(1-(-4))^2}

Without :math:`i`, such an expression cannot be written in *LEC*.

Thus, if you calculate an expression for a given :math:`t`, :math:`i` takes the following 
values depending on the case::

    - A + i == A + 0
    - d(A+i) == A + 0 - (A[-1] + -1)
    - l(i+1, A) == "A[-(i+1)]" == "A[-(0+1)]" == A[-1]
    - sum(t-1, t-2, i**2) == (-1)**2 + (-2)**2
    - sum(t-1, t-2, l(i-2, A) / i**2) 
      == l((-1)-2, A) / (-1)**2 + l((-2)-2, A) / (-2)**2
      == l(-3, A) / 1 + A[+3] + A[+4] / 4

Language constants
~~~~~~~~~~~~~~~~~~

Three mathematical constants are defined in the language:

- ``pi`` which is 3.141592653589 
- ``e`` which is 2.7182818284 
- ``euro`` which is 40.3399 


.. _variables:

Variables
---------

*Variables* represent numerical time series. Their names are always written in uppercase letters 
to distinguish them from *scalars*.

*Variable* names can contain up to 20 characters. These characters must be alphabetic (A-Z), 
numeric (0-9), or the underscore character '_'. 
Each *variable* name must begin with an uppercase alphabetic character.

For example::

    A, B_PNB, A123456789

are valid *variable* names.

But::

    _A1, 1A34, A_123456789, z_AV, A-2, B.X

are invalid names (*z_AV* is a *scalar* name).

Writing a variable in a formula means taking its value at the current execution period 
(whether in a model, a set of *identities*, or a *table*).

To take its value at the previous period (*lag*), write::

    PNB[-1]

The value at the next period (*lead*) is written as::

    PNB[+1]

The value at the period 1990Y1 (*degeneration*) is written as::

    PNB[1990Y1]

This syntax is more general and can be applied to entire formulas.


.. _scalars:

Scalars
-------

*Scalars* are dimensionless variables, defined by a single value. These are, for example, 
estimated coefficients in an *equation* or constant values over time.

*Scalar* names can contain up to 20 characters. These characters must be alphabetic (A-Z), 
numeric (0-9), or the underscore character '_'. 
Each name must begin with a lowercase alphabetic character.

For example::

    a, c1, a_123456789

are valid *scalar* names.

But::

    _a1, 1A34, a_123456789, Z_av

are invalid names (*Z_av* is a *variable* name).

The *lag* ([-1]), *lead* ([+1]), and *degeneration* ([1990Y1]) operations applied to a *scalar* 
have no effect, but are allowed.


.. _logical_operators:

Logical Operators
-----------------

The logical operators available are (presented in order of precedence):

- ``not`` or ``!``: negation of an expression 
- ``or``: logical OR between two expressions 
- ``and``: logical AND between two expressions 
- ``<``: less than 
- ``<=``: less than or equal to 
- ``=``: equality 
- ``!=`` or ``<>``: not equal 
- ``>=``: greater than or equal to 
- ``>``: greater than 

The logical operators return a value of 0 when the condition expressed is false and 
1 if the values satisfy the condition.

Example::

    Let X = 1           
        Y = 0           
        Z = 2     
    
    At the current period

    the formulas         have the values    
    ------------------------------------    
    !X                                 0    
    X and !Y                           1    
    X or !Y                            1    
    !(X + Y)                           0    
    !(2.32 + X)                        0    
    X == 0 and Y == 0 or Z == 2        1    
    Z < 1 * 3                          1

Note that an ``if()`` function also exists.


.. _algebraic_operators:

Algebraic Operators
-------------------

Algebraic calculations are performed using the following operators:

- ``+``: addition
- ``-``: subtraction
- ``/``: division
- ``*``: multiplication
- ``**``: exponentiation

These have their usual meanings and precedence.


.. _math_functions:

Mathematical Functions
----------------------

Many mathematical functions are implemented in the *LEC* language. 
Some functions take one argument, others several. 
Sometimes, a function accepts a variable number of arguments, with missing arguments possibly 
taking a default value.

In general, a function with only one argument does not require parentheses. Thus::

    ln X + 2        is identical to     ln(X) + 2    
    d X             is identical to     d(X)

All function names are written entirely in lowercase. 
Therefore, coefficients cannot take one of the reserved function names.

In the following syntax, the following notations are used:

- *expr*: any LEC expression (formula)
- *brackets* []: optional part
- *ellipsis* ...: repetition of the last argument as many times as desired

The mathematical functions are:

- ``-expr``: unary minus
- ``+expr``: unary plus
- ``log([expr], expr)``: logarithm in any base
- ``ln(expr)``: natural logarithm
- ``exp([expr,] expr)``: exponential in any base
- ``max(expr, ...)``: maximum of a list of values
- ``min(expr, ...)``: minimum of a list of values
- ``lsum(expr, expr, ...)``: sum of formulas
- ``sin(expr)``: sine
- ``cos(expr)``: cosine
- ``acos(expr)``: arccosine
- ``asin(expr)``: arcsine
- ``tan(expr)``: tangent
- ``atan(expr)``: arctangent
- ``tanh(expr)``: hyperbolic tangent
- ``sinh(expr)``: hyperbolic sine
- ``cosh(expr)``: hyperbolic cosine
- ``abs(expr)``: absolute value
- ``sqrt(expr)``: square root
- ``int(expr)``: integer part
- ``rad(expr)``: converts degrees to radians
- ``if(expr, expr, expr)``: condition
- ``sign(expr)``: sign of an expression
- ``isan(expr)``: returns 0 if expr is NAN and 1 otherwise
- ``lmean(expr, ...)``: returns the mean of the expressions
- ``lprod(expr, ...)``: returns the product of the expressions
- ``lcount(expr, ...)``: returns the number of expressions
- ``floor(expr)``: integer part of the expression
- ``ceil(expr)``: integer part of the expression plus 1
- ``round(expr [, n])``: rounds *expr* to the nth decimal place. If *n* is not defined, it is set to 0.
- ``random(expr)``: provides a random number between -expr/2 and +expr/2.

Some operators require further explanation:

Unary plus
~~~~~~~~~~

The unary plus has no effect: ++++++X is simply X.

Logarithm
~~~~~~~~~

ln(X) is the natural logarithm of X. 
log(X) is equal to ln(X) while log(10, X) is the base 10 logarithm of X.

Exponential
~~~~~~~~~~~

exp(X) is equivalent to :math:`e^X` and exp(10, X) is :math:`10^X`.

Max, min, and lsum
~~~~~~~~~~~~~~~~~~

max(1, X, Y, Z + 2) takes the maximum value of all values passed as arguments 
(at the current period). The min() function takes the minimum. 
The lsum() function sums all values passed as arguments.

In all three cases, the number of arguments must be between 2 and 50.

Integer part
~~~~~~~~~~~~

int(X) returns the nearest integer to X::

 int(2.2) = 2 
 int(2.6) = 3 
 int(2.5) = 3

If function
~~~~~~~~~~~

The `if()` function simplifies the writing of conditions: the first argument is the condition, 
the second the value if the condition is true, the third the value otherwise. 
Note that all three arguments are evaluated, and if an error occurs in the execution of any of 
them, the formula returns the value :math:`NA` (not available) even if it is the unused expression 
that caused the error.

Example::

    if(t < 1992Y1, 2, X)   is 2 before 1992Y1                                
                              X from 1992Y1 onward    
    if(t < 1992Y1, 2, X/0) is NA from 1992Y1 onward                                 
                              2 before 1992Y1

Sign function
~~~~~~~~~~~~~

The sign function returns the sign of an expression::

    sign(expr) is                    
        1 if expr >= 0                    
        -1 if expr < 0

Random function
~~~~~~~~~~~~~~~

The random function provides a pseudo-random number between *-expr/2* and *+expr/2*. 
Its syntax is::

    random(expr)


.. _time_functions:

Time Functions
--------------

Many equations and formulas require calculations involving time such as::

    c1 * (PNB - PNB[-1]) + c2 * (VXA - VXA[-1])

Time operators make this writing simpler by replacing it with::

    c1 * d(PNB) + c2 * d(VXA)

In the following syntax, the following notations are used:

- *expr*: any LEC expression (formula)
- *brackets* []: optional part

LEC has time operators to calculate period differences, growth rates, moving averages, 
standard deviations, etc. Here is the list:

- ``l([expr,] expr)``: lag of an expression
- ``d([expr,] expr)``: period difference
- ``r([expr,] expr)``: period ratio
- ``dln([expr,] expr)``: difference of logarithms of periods
- ``grt([expr,] expr)``: growth rate
- ``ma([expr,] expr)``: moving average
- ``mavg([expr,] expr)``: moving average (identical to ma)
- ``vmax([expr,[expr,]] expr)``: maximum over a range of periods
- ``vmin([expr,[expr,]] expr)``: minimum over a range of periods
- ``sum([expr,[expr,]] expr)``: sum over a range of periods
- ``prod([expr,[expr,]] expr)``: product over a range of periods
- ``mean([expr,[expr,]] expr)``: mean over a range of periods
- ``index([expr,[expr,]] expr1, expr2)``: index of a value
- ``acf([expr,[expr,]] expr, expr)``: autocorrelation factor
- ``var([from [,to],] expr)``: variance
- ``covar([from [,to],] expr1, expr2)``: covariance
- ``covar0([from [,to],] expr1, expr2)``: covariance around the origin
- ``corr([from [,to],] x, y)``: correlation
- ``stderr([expr,[expr,]] expr)``: standard deviation (unbiased)
- ``stddev([from [,to],] expr)``: standard deviation (biased)
- ``lastobs([from [,to],] expr)``: computes the last observation over a given period
- ``interpol(expr)``: provides a value for *expr* at :math:`t` by interpolating or extrapolating
- ``app(expr1, expr2)``: provides a value for *expr1* at :math:`t` using series *expr2* as a related series by a geometric method
- ``dapp(expr1, expr2)``: provides by differences a value for *expr1* at :math:`t` using series *expr2* as a related series
- ``hp([[from,]to],expr)``: Hodrick-Prescott filter with log transformation of *expr*
- ``hpstd([[from,]to],expr)``: calculation without log transformation of *expr*
- ``appdif(expr1, expr2)``: alias for dapp()

Below you will find the precise meaning of all time operators.

Lag of an expression
~~~~~~~~~~~~~~~~~~~~

The expression *l(expr1, expr2)* is equivalent to *expr2[expr1]*. 
This formulation allows you to calculate or parameterize lags. 
For example, the expression::

    X[c1 + 2]

is syntactically incorrect, whereas::

    l(c1 + 2, X)

is perfectly correct and equivalent.

If *expr1* is not defined, it is replaced by 1. Thus::

    l(X) is identical to l(1, X)

Period difference
~~~~~~~~~~~~~~~~~

The expression *d(expr1, expr2)* is equivalent to *expr2 - expr2[expr1]*, 
where the result of *expr1* is considered a lag if negative and a lead if positive. 
For example::

    d(2, X + Y)  is identical to (X + Y) - (X + Y)[-2]

*expr1* can be any expression. If *expr1* is not defined, it is replaced by 1. Thus::

    d(X) is identical to d(1, X)

Period ratio
~~~~~~~~~~~~

The expression *r(expr1, expr2)* is equivalent to *expr2 / expr2[expr1]*, 
where the result of *expr1* is considered a lag if negative and a lead if positive. 
For example::

    r(-2, X + Y)  is identical to (X + Y) / (X + Y)[+2]

*expr1* can be any expression. If *expr1* is not defined, it is replaced by 1. Thus::

    r(X) is identical to r(1, X)

Difference of logarithms of periods
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The expression *dln(expr1, expr2)* is equivalent to *ln(expr2) - ln(expr2)[expr1]*, 
where the result of *expr1* is considered a lag if negative and a lead if positive. 
For example::

    dln(3 - 2, X + Y) is equivalent to ln(X + Y) - ln(X + Y)[-1]

*expr1* can be any expression. If *expr1* is not defined, it is replaced by 1. Thus::

    dln(X) is identical to dln(1, X)

Growth rate
~~~~~~~~~~~

The expression *grt(expr1, expr2)* is equivalent to::

    100 * (expr2 / expr2[expr1] - 1)

where the result of *expr1* is considered a *lag* if negative and a *lead* if positive. 
For example::

    grt(2, X + Y) is equivalent to 100 * ((X + Y) / (X + Y)[-2] - 1)

*expr1* can be any expression. If *expr1* is not defined, it is replaced by 1. Thus::

    grt(X) is identical to grt(1, X)

Moving average
~~~~~~~~~~~~~~

The expression *ma(expr1, expr2)* gives the average of the last *expr1* values of 
the expression *expr2*. If *expr1* is negative or zero, the current value of *expr2* 
is returned by this expression.

*expr1* can be any expression. If *expr1* is not defined, it is replaced by 1. Thus::

    ma(X) is identical to ma(1, X) and thus to X

Maximum over a range of periods
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The expression *vmax(expr1, expr2, expr3)* computes the maximum of the expression *expr3* 
over the range of periods defined by indices *expr1* and *expr2*. These last two expressions 
can be any, but their result is rounded to the integer part and considered as an index of the 
series calculated by *expr3*.

For example::

    vmax(1970Y1, 1990Y1, X + Y)

computes the maximum of the series X + Y over the period 1970Y1 to 1990Y1.

Used with two arguments, the second (missing) is replaced by :math:`t`, i.e., the current 
index during execution.

Used with a single argument, the first argument is set to 0 (origin of the series) and the 
second to :math:`t` (current index).

Thus::

    vmax(1970Y1, X + Y)  is   vmax(1970Y1, t, X + Y)    
    vmax(X + Y)          is   vmax(0, t, X + Y)

Minimum over a range of periods
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The expression *vmin(expr1, expr2, expr3)* computes the minimum of the expression *expr3* 
over the range of periods defined by indices *expr1* and *expr2*. These last two expressions 
can be any, but their result is rounded to the integer part and considered as an index of the 
series calculated by *expr3*.

For example::

    vmin(t - 2, t, X + Y)

computes the minimum of the series X + Y over the last three values of the series 
(relative to the current calculation period).

Used with two arguments, the second (missing) is replaced by :math:`t`, i.e., the current 
index during execution.

Used with a single argument, the first argument is set to 0 (origin of the series) and the 
second to :math:`t` (current index).

Thus::

    vmin(1970Y1, X + Y)  is   vmin(1970Y1, t, X + Y)    
    vmin(X + Y)          is   vmin(0, t, X + Y)    
    vmin(t, t, X + Y)    is   (X + Y)

Sum over a range of periods
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The expression *sum(expr1, expr2, expr3)* computes the sum of the expression *expr3* over 
the range of periods defined by indices *expr1* and *expr2*. These last two expressions can 
be any, but their result is rounded to the integer part and considered as an index of the 
series calculated by *expr3*.

For example::

    sum(0, t, X + Y)

computes the total of the series X + Y from its origin to the current calculation period.

Used with two arguments, the second (missing) is replaced by :math:`t`, i.e., the current 
index during execution.

Used with a single argument, the first argument is set to 0 (origin of the series) and the 
second to :math:`t` (current index).

Thus::

    sum(1970Y1, X + Y)   is   sum(1970Y1, t, X + Y)    
    sum(X + Y)           is   sum(0, t, X + Y)

Product over a range of periods
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The expression *prod(expr1, expr2, expr3)* computes the product of the expression *expr3* over 
the range of periods defined by indices *expr1* and *expr2*. These last two expressions can 
be any, but their result is rounded to the integer part and considered as an index of the 
series calculated by *expr3*.

For example::

    prod(1985Y1, t - 1, X / X[85Y1])

computes the cumulative product of the series X rebased to 85 from 1985 to the 
current period - 1.

Used with two arguments, the second (missing) is replaced by :math:`t`, i.e., the current 
index during execution.

Used with a single argument, the first argument is set to 0 (origin of the series) and the 
second to :math:`t` (current index).

Thus::

    prod(1970Y1, X + Y)  is   prod(1970Y1, t, X + Y)    
    prod(X + Y)          is   prod(0, t, X + Y)

Mean over a range of periods
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The expression *mean(expr1, expr2, expr3)* computes the mean of the expression *expr3* over 
the range of periods defined by indices *expr1* and *expr2*. These last two expressions can 
be any, but their result is rounded to the integer part and considered as an index of the 
series calculated by *expr3*.

For example::

    mean(0, t, X)

computes the mean of the series X over the period from the origin to the current 
calculation year.

Used with two arguments, the second (missing) is replaced by :math:`t`, i.e., the current 
index during execution.

Used with a single argument, the first argument is set to 0 (origin of the series) and 
the second to :math:`t` (current index).

Thus::

    mean(1970Y1, X + Y)  is   mean(1970Y1, t, X + Y)    
    mean(X + Y)          is   mean(0, t, X + Y)    
    mean(t - 1, t, X)    is   ma(2, X)    
    mean(t, t, X)        is   X

Index of a value in a series
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The expression *index(2.5, X)* returns the position of 2.5 in the series X.

Autocorrelation factor
~~~~~~~~~~~~~~~~~~~~~~

The expression *acf([expr,[expr,]] expr1, expr2)* returns the autocorrelation factor of 
expression *expr2* of degree *expr1* over the (optional) period passed as the first argument. 
If the period is not defined, it is [0, t].

The degree (*expr1*) cannot exceed a quarter of the number of observations in the considered 
period. Otherwise, the function returns :math:`NA` (Not Available)::

    acf(1970Y1, X + Y)  is   acf(1970Y1, t, X + Y)    
    acf(X + Y)          is   acf(0, t, X + Y)

Variance
~~~~~~~~

The expression *var([from [,to],] expr)* returns the variance of *expr* over the range of 
periods [from, to]. *var([from [,to],] X)* computes the expression:     

.. math::

    \frac{1}{n}~\sum^n_{i=1} (X_i-X_m)^2     
     
where :math:`X_m` is the mean of X and :math:`n` is the number of observations.

Used with a single argument, the first argument is set to 0 (origin of the series) and 
the second to :math:`t` (current index).

Thus::

    var(1970Y1, X + Y)  is   var(1970Y1, t, X + Y)    
    var(X + Y)          is   var(0, t, X + Y)

Covariance and covariance around the origin
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The expression *covar([from [,to],] expr1, expr2)* returns the covariance of *expr1* and 
*expr2* over the period [from,to]. *covar0()* returns the covariance around the origin.

*covar([from [,to],] X, Y)* computes the expression:

.. math::

    \frac{1}{n}~\sum^n_{i=1} (X_i-X_m)~(Y_i-Y_m)

where :math:`X_m` is the mean of X, :math:`Y_m` is the mean of Y, and :math:`n` is the 
number of observations.

*covar0([from [,to],] X, Y)* computes the expression:

.. math::

    \frac{1}{n}~\sum^n_{i=1} (X_i)~(Y_i) - X_m~Y_m

Used with a single argument, the first argument is set to 0 (origin of the series) and 
the second to :math:`t` (current index).

Thus::

    covar(1970Y1, X, Y)  is   covar(1970Y1, t, X, Y)    
    covar(X, Y)          is   covar(0, t, X, Y)

Correlation
~~~~~~~~~~~

The expression *corr([from [,to],] X, Y)* returns the correlation between X and Y:

.. math::

    corr(X, Y) = \frac{covar(X, Y)}{\sqrt{var(X) * var(Y)}}

Used with a single argument, the first argument is set to 0 (origin of the series) and 
the second to :math:`t` (current index).

Thus::

    corr(1970Y1, X, Y)  is   corr(1970Y1, t, X, Y)    
    corr(X, Y)          is   corr(0, t, X, Y)

Standard deviation over a range of periods
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The expression *stderr(expr1, expr2, expr3)* computes the standard deviation of the 
expression *expr3* over the range of periods defined by indices *expr1* and *expr2*. 
These last two expressions can be any, but their result is rounded to the integer part 
and considered as an index of the series calculated by *expr3*.

For example::

    stderr(1970Y1, 1990Y1, X)

computes the standard deviation of the series X over the period 1970 to 1990.

Used with two arguments, the second (missing) is replaced by :math:`t`, i.e., the current 
index during execution.

Used with a single argument, the first argument is set to 0 (origin of the series) and 
the second to :math:`t` (current index).

Thus::

    stderr(1970Y1, X + Y)  is   stderr(1970Y1, t, X + Y)    
    stderr(X + Y)          is   stderr(0, t, X + Y)

The same syntax applies to the *stddev()* function. Unlike *stderr()*, *stddev()* is 
a biased estimator:

.. math::

    stddev(x) = \sqrt{var(x)} 

    stderr(x) = \sqrt{\sum^n_{i=1} \frac{(x_i-x_m)^2}{n-1}}

Last observation calculation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This function allows you to obtain the value of the last non-NA value of a series over 
a given range of periods. Its syntax is::

    lastobs([from [,to],] expr)

Interpolation calculation
~~~~~~~~~~~~~~~~~~~~~~~~~

This function provides a value for *expr* at :math:`t` by interpolating or extrapolating 
known values. This interpolation or extrapolation is only done linearly::

    interpol(expr)

If you have A in 90 and 92, *interpol(A)* in 91 will be *(A[90Y1]+A[92Y1])/2*. 
Several successive values may be missing. In this case, a weighting is used according to 
the proximity of available values.

Related series
~~~~~~~~~~~~~~

The *app* function provides a value for an expression when the observation is missing by 
using another series as a related series. Its syntax is::

    app(expr1, expr2)

The calculation is done geometrically based on the closest known values in the series 
to be completed.

The result of the function *app(A, B)* is obtained as follows at time :math:`t`:

- if :math:`A[t]` is defined, :math:`A[t]` is returned
- if :math:`A[t]` is not defined, find the values t0 and t1 around t such that 
  :math:`A[t0]` and :math:`A[t1]` are defined and nonzero. If neither t0 nor t1 can be found, 
  returns NaN. In this case, A is NaN over the entire period.
- if only t0 is defined, returns:
.. math::
    B[t] * \frac{A[t0]}{B[t0]}
- if only t1 is defined, returns 
.. math::
    B[t] * \frac{A[t1]}{B[t1]}
- if t0 and t1 are defined, first computes 

.. math::

    \delta = \frac{\frac{A[t1]}{A[t0]}}{\frac{B[t1]}{B[t0]}} 

then returns 

.. math::

    A[t0] * \frac{B[t]}{B[t0]} * \delta^{\frac{t-t0}{t1-t0}}

Smoothing
~~~~~~~~~

The *hp()* and *hpstd()* functions compute a *smoothed* series based on 
the Hodrick-Prescott method::

    hp([[from,]to],expr):     calculation based on the logarithm of expr    
    hpstd([[from,]to],expr):  calculation without log transformation of expr

The parameters are:

- *from*: calculation start period, first year by default
- *to*: calculation end period, last year by default
- *expr*: any LEC expression. Must be strictly positive for *hp()*


.. _lists:

Lists or Macros
---------------

It is possible to parameterize *LEC* expressions by introducing IODE *list* names (managed 
and defined in the *lists* workspace). These *lists* also allow you to shorten the writing 
of equations.

It is also possible to use *LEC* expressions to generate *lists* by using asterisks or 
question marks. In this case, the expression to *expand* should be enclosed in single quotes.

Named lists
~~~~~~~~~~~

An IODE *list* has a name that follows the same syntax rules as a variable (i.e. maximum 20 
characters, capital letters). In the *equation*, *lists* are preceded by a dollar sign ``$`` 
to distinguish them from the *variables*.

Suppose the *list* A is defined as::

    PNB * c1 + c2

The expression::

    A + B + $A + 2

will be equivalent to::

    A + B + PNB * c1 + c2 + 2

WARNING: The IODE *lists* must exist at the time the formulas are compiled. 
Once compiled, the *LEC* forms are not recompiled unless the *LEC* formula itself is modified. 
Therefore, changes made to the IODE *list* definitions will only impact the *LEC* forms 
(*equations*, *identities*, *tables*) if the *LEC* forms are recompiled.

Computed lists
~~~~~~~~~~~~~~

When the operator/function allows it (lsum, max, etc.), it is possible to use wildcards 
in the *LEC* language. However, to distinguish the asterisk from the multiplication operator, 
the IODE *list* to be expanded must be enclosed in single quotes (''). 
For example::

        lsum('A*')

is equivalent to::

    lsum(A1, A2, AX)

if A1, A2, and AX are the only series starting with 'A' in the *variables* workspace.

Combinations of series names can be used as well::

    max('*G;*F')

The maximum number of operands for the operators/functions is 255.

WARNING: Names containing wildcards in *LEC* forms are resolved based on the content of 
the current *variables* workspace. The compiled *LEC* form is stored with these names. 
If you change the content of the *variables* workspace, some series may no longer exist 
or new ones may appear. Since the compiled form is not changed automatically, if you want 
to adapt the *LEC* form to the new content, you must recompile the *LEC* form by editing it.


.. _lags:

Lags, Leads, and Period Values
------------------------------

The variables used in a *LEC* expression always have an implicit time dimension. 
The expression::

    A + PNB

means::

    A[t] + PNB[t]

However, the latter notation (with [t]) is syntactically incorrect.

You can apply three types of time index modifications to variables or, more generally, 
to expressions:

- lag (delay)
- lead (advance)
- period fixing

In all three cases, you add to the modified expression a value in brackets representing 
the lag, lead, or period::

    expr[-lag]    
    expr[+lead]    
    expr[period]

A lag or lead must be an integer preceded by the sign - (for lag) or + (for lead), indicating 
the number of time periods by which you want to shift the expression. The period is a temporal 
constant, such as 1970Q3.

Combinations of lag, lead, and period fixing are possible. The rule is simple: as soon as a 
period is fixed, lags and leads no longer affect the expression. Lags and leads are simply 
added together.

The application of lags, leads, and period fixing is performed before the calculation of 
functions. Thus, max(A, B)[-1] is identical to max(A[-1], B[-1]).

Here are some examples::

    Expression               Equivalent or meaning    
    -------------------------------------------------------------------------    
    X[-1]                    X[t-1] (syntactically incorrect)    
    (X + Y + c1)[1970Y1]     X[1970Y1] + Y[1970Y1] + c1    
    (A + B[+1])[-2]          A[-2] + B[-1]    
    (A[70Y1] + B)[-1][-2]    A[70Y1] + B[-3]    
    (A[+1] + B[70Y1])[80Y1]  A[81Y1] + B[70Y1]    
    (a + 2)[-1]              a + 2    
    d(X[-2])                 X[-2] - X[-3]    
    (grt(X))[-2]             X[-2] / X[-3]    
    max(A, B)[1970Y1]        max(A[70Y1], B[70Y1])    
    d(A)[1970Y1]             0 (A[70Y1] - A[70Y1])    
    t[-1]                    t (since t is a constant)    
    mean(X)[-1]              NA (out of sample)    
    mean(1, t, X)[-1]        is NA in the first year    
    A[1]                     incorrect (sign + or - is missing)    
    B[70Y1 + 1]              incorrect (cannot mix fixed period and lag/lead)    
    C[t-1]                   incorrect (t cannot appear in the brackets)


.. _comments:

Comments
--------

Comments can be introduced in a LEC expression. 
Simply place them between /\* and \*/.

For example::

    A + B + /* first part  */    
    C       /* temporary series */    
    + D     /* continuation     */

Introducing a comment also allows you to temporarily remove part of an equation::

    ln Y := c1 + /* c2 * ln Z + */ c3 * ln Y[-1]

An alternative method is to place at the end of an expression a semicolon followed by 
free text::

    ln Y := c1 + c2 * ln Y[-1] ; free comment

WARNING: Unlike /\* and \*/, the semicolon ends the equation: all text that follows 
is ignored, including on the following lines::

    A + B +    
    C       ; temporary series    
    + D     /* continuation */

is equivalent to::

   A + B + C


.. _op_precedence:

Operator Precedence
-------------------

The defined operator precedences are standard. In order of increasing precedence:

- the logical operator ``or``
- the logical operator ``and``
- comparison operators ``<``, ``<=``, etc.
- addition ``+`` and subtraction ``-`` operators
- multiplication ``*`` and division ``/`` operators
- exponentiation operator ``**``
- :ref:`mathematical <math_functions>` and :ref:`time <time_functions>` functions

By *has precedence over*, it means *is executed after* in a formula. Thus::

    2.2 * X < 100

is 1 (true) if (2.2 * X) is less than 100, and 0 (false) otherwise.

Operators with equal precedence are executed from left to right::

    2 - 1 + 2

equals 3 because it is equivalent to (2 - 1) + 2.

Parentheses can be used to change this order::

    2.2 * (X < 100)

equals 2.2 if X is less than 100, and 0 otherwise.

Algebraic operators have precedence over functions, whether mathematical or time functions::

    ln X + 2

is equivalent to::

    ln(X) + 2


.. _writing_equations:

Writing Equations
-----------------

An equation is simply the juxtaposition of two *LEC* expressions, separated by the 
characters ``:=`` ::

    expr1 := expr2

All the rules described above for *LEC* expressions apply to each term of the *equation*.

Since an *equation* is named after the endogenous *variable*, this *variable* must appear 
in the *equation*. It may appear more than once.


.. _syntax_summary:

Syntax Summary
--------------

The following lines summarize all elements of the language and their syntax::

    numeric constants      : 2, 2.0, 2.12E2 0.001e-03 
    temporal constants     : 1990Y1, 80S2, 78Q4, 2003M12 
    language constants     : pi, e, t 
    variables              : A, A_1, A123456789 
    scalars                : a, a_1, x123456789 
    logical operators      : not expr                         
                             expr or expr                         
                             expr and expr                         
                             expr < expr                         
                             expr <= expr                         
                             expr = expr                         
                             expr != expr                         
                             expr >= expr                         
                             expr > expr 
    algebraic operators    : expr + expr                         
                             expr - expr                         
                             expr / expr                         
                             expr * expr                         
                             expr ** expr 
    mathematical functions : -expr                         
                             +expr                         
                             log([expr], expr)                         
                             ln(expr)                         
                             exp([expr,] expr)                         
                             max(expr, ...)                         
                             min(expr, ...)                         
                             lsum(expr, expr, ...)                         
                             sin(expr)                         
                             cos(expr)                         
                             acos(expr)                         
                             asin(expr)                         
                             tan(expr)                         
                             atan(expr)                         
                             tanh(expr)                         
                             sinh(expr)                         
                             cosh(expr)                         
                             abs(expr)                         
                             sqrt(expr)                         
                             int(expr)                         
                             rad(expr)                         
                             if(expr, expr, expr)                         
                             sign(expr)                         
                             isan(expr)                         
                             lmean(expr, ...)                         
                             lprod(expr, ...)                         
                             lcount(expr, ...)                         
                             floor(expr)                         
                             ceil(expr)                         
                             round(expr [, n])                         
                             random(expr)  
    time functions         : l([expr,] expr)                         
                             d([expr,] expr)                         
                             r([expr,] expr)                         
                             dln([expr,] expr)                         
                             grt([expr,] expr)                         
                             ma([expr,] expr)                         
                             mavg([expr,] expr)                         
                             vmax([expr,[expr,]] expr)                         
                             vmin([expr,[expr,]] expr)                         
                             sum([expr,[expr,]] expr)                         
                             prod([expr,[expr,]] expr)                         
                             mean([expr,[expr,]] expr)                         
                             index([expr,[expr,]] expr1, expr2)                         
                             acf([expr,[expr,]] expr, expr)                         
                             var([from [,to],] expr)                         
                             covar([from [,to],] expr1, expr2)                         
                             covar0([from [,to],] expr1, expr2)                         
                             corr([from [,to],] x, y)                         
                             stderr([expr,[expr,]] expr)                         
                             stddev([from [,to],] expr)                         
                             lastobs([from [,to],] expr)                         
                             interpol(expr)                         
                             app(expr1, expr2)                         
                             dapp(expr1, expr2) 
    lists or macros        : $LISTNAME 
    lags, leads, periods   : [+n] [-n] [1990Y1] 
    comments               : /* Comment */ 
    equations              : expr := expr

