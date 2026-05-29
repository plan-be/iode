.. _tutorial_lec_language:

This tutorial is a practical introduction to the *LEC* language
(*Langage Econometrique Condense*), the formula language used in IODE.

You will learn how to:

- write valid LEC expressions
- use variables, scalars, and constants
- work with lags, leads, and fixed periods
- use mathematical and time functions
- write equations with ``:=``

For a full explanation of the syntax and features, see the 
:ref:`The LEC Language <lec_language>` section.

What Is LEC?
~~~~~~~~~~~~

LEC is designed for formulas involving time series.
It is used in equations, identities, and table formulas.

Example 1:

.. math::

	C_t = a + b \frac{Y_t}{P_t} + c C_{t-1}

LEC form::

	C := a + b * Y / P + c * C[-1]

Example 2:

.. math::

	\ln(Q_t) = a\ln(K_t) + (1-a)\ln(L_t) + c t + b

LEC form::

	ln Q := a * ln K + (1 - a) * ln L + c * t + b


Basic Building Blocks
~~~~~~~~~~~~~~~~~~~~~

Numeric constants
^^^^^^^^^^^^^^^^^

You can use integers, floating-point numbers, and scientific notation::

	2
	0.125
	1.2E-03

Language constants
^^^^^^^^^^^^^^^^^^

LEC provides three predefined constants:

- ``pi``
- ``e``
- ``euro``

Temporal constants and t
^^^^^^^^^^^^^^^^^^^^^^^^

Temporal constants represent periods::

	1990Y1
	1970Q4
	2010M11

The symbol ``t`` is the index of the current execution period.

Example::

	X * (t < 1990Y1) + Y * (t >= 1990Y1)

This formula returns ``X`` before 1990Y1 and ``Y`` from 1990Y1 onward.


Variables and Scalars
~~~~~~~~~~~~~~~~~~~~~

Variables
^^^^^^^^^

Variables are time series and must start with an uppercase letter.

Valid examples::

	A
	B_PNB
	A123456789

Invalid examples::

	_A1
	1A34
	z_AV
	A-2

Scalars
^^^^^^^

Scalars are single values and must start with a lowercase letter.

Valid examples::

	a
	c1
	a_123456789

Invalid examples::

	_a1
	1A34
	Z_av


Time Indexing (Lag, Lead, Fixed Period)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You can shift any expression in time:

- lag: ``expr[-n]``
- lead: ``expr[+n]``
- fixed period: ``expr[1990Y1]``

Examples::

	PNB[-1]
	PNB[+1]
	PNB[1990Y1]

Important rule:

- once a fixed period is applied, additional lags/leads no longer change that part

Examples::

	(A + B[+1])[-2]          => A[-2] + B[-1]
	(A[70Y1] + B)[-1][-2]    => A[70Y1] + B[-3]


Algebraic and Logical Operators
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Algebraic operators:

- ``+``, ``-``, ``*``, ``/``, ``**``

Logical operators:

- ``not`` or ``!``
- ``or``
- ``and``
- ``<``, ``<=``, ``=``, ``!=`` (or ``<>``), ``>=``, ``>``

Logical expressions evaluate to ``1`` (true) or ``0`` (false).

Examples::

	X and !Y
	Z < 1 * 3
	X = 0 and Y = 0 or Z = 2


Mathematical Functions
~~~~~~~~~~~~~~~~~~~~~~

Common functions:

- ``ln(expr)``, ``log([base], expr)``, ``exp([base,] expr)``
- ``max(...)``, ``min(...)``, ``lsum(...)``
- ``abs(expr)``, ``sqrt(expr)``, ``round(expr [, n])``
- ``if(cond, value_if_true, value_if_false)``

Single-argument functions can be written without parentheses::

	ln X + 2
	d X

is equivalent to::

	ln(X) + 2
	d(X)

Conditional example::

	if(t < 1992Y1, 2, X)


Time Functions
~~~~~~~~~~~~~~

LEC includes many time-series operators.

Most used ones (by default *k* = 1):

- ``l([k,] expr)``: lag expression
- ``d([k,] expr)``: period difference
- ``r([k,] expr)``: period ratio
- ``dln([k,] expr)``: difference of logs
- ``grt([k,] expr)``: growth rate in %
- ``ma([k,] expr)`` / ``mavg([k,] expr)``: moving average

Examples::

	d(PNB)
	dln(X)
	grt(4, X)
	ma(3, X)

Range functions:

- ``sum([from,[to,]] expr)``
- ``prod([from,[to,]] expr)``
- ``mean([from,[to,]] expr)``
- ``vmax([from,[to,]] expr)``
- ``vmin([from,[to,]] expr)``

Examples::

	sum(0, t, X)
	mean(t - 3, t, X)
	vmax(1970Y1, 1990Y1, X + Y)

Statistics functions include ``var``, ``covar``, ``corr``, ``stderr``, and ``stddev``.


Special Variable ``i`` in Sub-Expressions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Inside some time functions (such as ``sum``), the special variable ``i`` is the offset
between the current formula period and the sub-expression period.

Example::

	sum(t-2, t-4, A / (1 - i)**2)

This lets you write dynamic expressions that depend on the internal iteration period.


Lists and Macros
~~~~~~~~~~~~~~~~

Named lists
^^^^^^^^^^^

Use ``$LISTNAME`` to insert a list expression::

	A + B + $A + 2

If list ``A`` is ``PNB * c1 + c2``, this becomes::

	A + B + PNB * c1 + c2 + 2

Computed lists with wildcards
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Use single quotes around wildcard patterns::

	lsum('A*')
	max('*G;*F')

Note: wildcard expansion is resolved at compile time against the current 
variables workspace.


Comments
~~~~~~~~

Two comment styles are available.

Inline block comments::

	A + B + /* first part */ C + D

Trailing comments with ``;``::

	ln Y := c1 + c2 * ln Y[-1] ; free comment

Warning: ``;`` ends the equation. Everything after it is ignored.


Operator Precedence
~~~~~~~~~~~~~~~~~~~

From lowest to highest priority:

1. ``or``
2. ``and``
3. comparisons (``<``, ``<=``, ``=``, ...)
4. ``+`` and ``-``
5. ``*`` and ``/``
6. ``**``
7. mathematical and time functions

Use parentheses to force evaluation order.


Writing Equations
~~~~~~~~~~~~~~~~~

An equation has the form::

	expr1 := expr2

Example::

	ln Y := c1 + c2 * ln Z + c3 * ln Y[-1]

The endogenous variable should appear in the equation.


Quick Syntax Reminder
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: text

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
