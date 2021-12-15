# IODE. Group of functions "LEC"

## Summary
A LEC expression is compiled in 2 steps. 

### Step 1: LEC interpretor

During the first step (l_cc1.c), 2 tables, L_EXPR with atomic expressions and L_NAMES with the names found in the LEC expression are constructed. 
L_EXPR is organised in "stack-oriented" order. That step make use of l_token.c functions.

### Step 2: LEC serializing

In a second step (l_cc2.c), a "CLEC expression" is generated which is basically the serialization of the combination of L_EXPR and L_NAMES.

### Step 3. LEC linker

The CLEC expression can then be linked to the KDB's of variables and scalars (l_link.c). Each name is searched in the KDB's and their respective
positions is saved in the CLEC structure.

### Step 4: LEC calculator

Finally, the expression is computed by the fonction L_exec (l_exec.c).

## Example of the compilation of a LEC expression

Let's compile the expression 

    A + ln(B)

At the end of the first compilation step, two tables are created:

- L_NAMES which is a table with the names found in the LEC expression:

      A
      B

- L_EXPR that can be sketched as:

      Type       Value
      ----       -----
      L_VAR      0 (= pos of A in L_NAMES)
      L_OPENP
      L_VAR      1 (= pos of B in L_NAMES)
      L_CLOSEP
      L_LN        
      L_PLUS

During the second step (l_cc2.c), a "CLEC expression" is generated which is the serialization of the combination of L_EXPR and L_NAMES.

A simplified version of the resulting "executable" expression (kind of "Reverted Polish Notation") is:

    Size    Type      Value                        
    ----    ----
    21      L_VAR     CVAR: {0, 0, 0, {0,0,0}} = {pos of A in L_NAMES, lag, ref, period}
    21      L_VAR     CVAR: {1, 0, 0, {0,0,0}} = {pos of B in L_NAMES, lag, ref, period}
    2       L_LN      1 (nb of arguments)
    1       L_PLUS    

Finally, the CLEC *struct* regroups the following data:

 - total length of the CLEC struct
 - length of executable expression
 - nb of names
 - dupendo (reserved for equations)
 - table of LNAME's
 - executable expression described above

## LEC: groups of variables and functions

The variables and functions for managing LEC expressions are divided into the categories below:

- LEC syntax: variables defining the LEC syntax
- LEC utilities: LEC memory allocation + LEC error management
- LEC compiler: to check the syntax of a LEC expression and to produce a precompiled structure (CLEC)
- LEC linker: to link the CLEC structure to the variables and scalars in KDB's
- LEC execution: to calculate the value of a CLEC expression in the context of specific KDB's
- LEC virtual functions: functions that can be rewritten for use in other contexts than KDB databases

## Variables defining the LEC syntax

The LEC syntax depends on variables defined in l_token.c. These variables are also related to #define's in iode.h.

      - L_TABLE:      keywords recognised in a LEC expression, like '+' or 'ln'
      - L_PRIOR:      table of the operator execution priorities (which one is executed first, second...)
      - L_MIN_FARGS:  table of minimum number of arguments for the math functions (like log())
      - L_MAX_FARGS:  table of maximum number of arguments for the math functions
      - L_MIN_TARGS:  table of minimum number of arguments for the time functions (like grt())
      - L_MAX_TARGS:  table of minimum number of arguments for the time functions
      - L_MIN_MTARGS: table of minimum number of arguments for the functions with multiple time expressions
      - L_MAX_MTARGS: table of minimum number of arguments for the functions with multiple time expressions

These variables may be changed but with some care. 

## LEC utilities

### l_alloc.c
Functions to allocate and free standard memory (on the "heap") for the LEC group of functions. 
Could be replaced by another implementation if needed.

    - char *L_malloc(int lg)    Allocates lg bytes in conventional memory. Fills the allocated space with nulls.
    - void L_free(void *ptr)    Frees an allocated buffer. If ptr is null, does nothing.

### l_err.c
Function and variables to manage error messages.
     
    - int L_errno:        Last error number during LEC compilation
    - char* L_error():    Returns a static buffer containing the last LEC compilation error message.

## LEC compiler 

### l_token.c
Functions and associated variables to "open" a LEC expression and to read the expression one token at a time.

The variables defining the LEC syntax (L_TABLE, L_PRIOR, L_MIN...) are described above.

Main functions:

     - int L_open_all(char* file_or_string, int type)    Opens a file or a string for reading and assigns the open stream to L_YY.
     - void L_close()                                    Close the stream L_YY.
     - int L_get_token()                                 Main function to browse a LEC expression (in L_YY) token by token. 

### l_cc1.c
First step of LEC compilation. 

Main functions:

    - int L_cc1(int nb_names)           Creates:
                                            - L_EXPR  = ordered list of atomic expressions with references to L_NAMES
                                            - L_NAMES = list of names in the LEC expression
    - void L_alloc_expr(int nb)         Allocates or reallocates L_EXPR by blocks of 100 elements.
    - int L_sub_expr(ALEC* al, int i)   Computes the position of the beginning of a sub-expression

### l_cc2.c

Second step of LEC compilation, producing a "CLEC expression" which contains what is needed to efficiently evaluate the expression.

Main functions:

    CLEC *L_cc2(ALEC* expr)                          Second stage of LEC compilation. Generates an "executable" LEC expression.
    void L_move_arg(char *s1, char *s2, int lg)      Copies lg bytes from a buffer to another in reverse order. The 2 buffers may overlap.
    CLEC *L_cc_stream()                              Compiles L_YY, the open YY stream containing a LEC expression.
    CLEC *L_cc(char* lec)                            Compiles a LEC string. 


### l_eqs.c

Functions to compile LEC *equations*. LEC *equations* are made up of 2 LEC *expressions* separated by ":=".
Equations are used in 2 different contexts: estimation of coefficients and model simulation.

Main functions:
 
    CLEC* L_solve(char* eq, char* endo)  Compiles a LEC equation and tries to analytically solve the equation with respect to endo.
    int L_split_eq(char* eq)             Returns the position of ":=" in an equation or -1 if not found.
 
More details and examples can be found in the source file.

## LEC linker
The third step consists in linking the CLEC expression to the KDB of variables and scalars. Each name is searched in the KDB's and their 
position is placed in l_names[i].pos.

Three functions, implemented in k_lec.c, are called during the link process: L_findvar(), L_findscl() L_getsmpl(). 
The positions of variables and scalars returned by these functions will be used at execution time by L_getvar() and L_getscl().

### l_link.c

    int L_link(KDB* dbv, KDB* dbs, CLEC* cl)    Links a CLEC expression to KDB's of variables and scalars. Aligns PERIOD's to the SAMPLE of dbv.
    void L_link_endos(KDB *dbe, CLEC *cl)       Pseudo linking used to calculate the strong connex components of a model (SCC).

## LEC execution

Finally, the expression is calculated by the fonction L_exec().

### l_exec.c

Functions to evaluate a compiled and linked LEC expression. 

    L_REAL L_exec_sub(unsigned char* expr, int lg, int t, L_REAL* stack)     Execution of a CLEC sub expression.
    L_REAL L_exec(KDB* dbv, KDB* dbs, CLEC* expr, int t)                     Execution of a compiled and linked CLEC expression.

### l_exec_var.c
Functions to evaluate LEC "values":

    static L_REAL L_pi ()
    static L_REAL L_euro()
    static L_REAL L_e ()
    static L_REAL L_time(int t)
    static L_REAL L_i(int t)

### l_exec_ops.c
Functions to evaluate LEC "operators". 

    static L_REAL L_or (L_REAL a, L_REAL b)
    static L_REAL L_and (L_REAL a, L_REAL b)
    static L_REAL L_ge (L_REAL a, L_REAL b)
    static L_REAL L_gt (L_REAL a, L_REAL b)
    static L_REAL L_le (L_REAL a, L_REAL b)
    static L_REAL L_lt (L_REAL a, L_REAL b)
    static L_REAL L_eq (L_REAL a, L_REAL b)
    static L_REAL L_ne (L_REAL a, L_REAL b)
    static L_REAL L_plus (L_REAL a, L_REAL b)
    static L_REAL L_minus(L_REAL a, L_REAL b)
    static L_REAL L_times(L_REAL a, L_REAL b)
           L_REAL L_divide(L_REAL a, L_REAL b)
           L_REAL L_exp(L_REAL a, L_REAL b)

### l_exec_fns.c

Functions to evaluate LEC "functions". 

    L_REAL L_logn(L_REAL v)
    static L_REAL L_uminus(L_REAL* stack)
    static L_REAL L_uplus (L_REAL* stack)
    static L_REAL L_log(L_REAL* stack, int nargs)
    static L_REAL L_ln(L_REAL* stack)
    static L_REAL L_not(L_REAL* stack)
    static L_REAL L_expn(L_REAL* stack, int nargs)
    static L_REAL L_max(L_REAL* stack, int nargs)
    static L_REAL L_min(L_REAL* stack, int nargs)
    static L_REAL L_sin (L_REAL* stack)
    static L_REAL L_cos (L_REAL* stack)
    static L_REAL L_acos (L_REAL* stack)
    static L_REAL L_asin (L_REAL* stack)
    static L_REAL L_tan (L_REAL* stack)
    static L_REAL L_atan (L_REAL* stack)
    static L_REAL L_tanh (L_REAL* stack)
    static L_REAL L_sinh (L_REAL* stack)
    static L_REAL L_cosh (L_REAL* stack)
    static L_REAL L_abs (L_REAL* stack)
    static L_REAL L_sqrt (L_REAL* stack)
    static L_REAL L_int (L_REAL* stack)
    static L_REAL L_rad (L_REAL* stack)
    static L_REAL L_if(L_REAL* stack, int nargs)
    static L_REAL L_lsum(L_REAL* stack, int nargs)
    static L_REAL L_lmean(L_REAL* stack, int nargs)
    static L_REAL L_fnisan(L_REAL* stack, int nargs)
    static L_REAL L_lcount(L_REAL* stack, int nargs)
    static L_REAL L_lprod(L_REAL* stack, int nargs)
    static L_REAL L_sign(L_REAL* stack)
    static L_REAL L_lstderr(L_REAL* stack, int nargs)
    static L_REAL L_random(L_REAL* stack)
    static L_REAL L_floor(L_REAL* stack)
    static L_REAL L_ceil (L_REAL* stack)
    static L_REAL L_round(L_REAL* stack, int nargs)
    static L_REAL L_urandom(L_REAL* stack)
    static L_REAL L_grandom(L_REAL* stack)
    static L_REAL L_gamma(L_REAL* stack)
    static L_REAL L_div0(L_REAL *stack, int nargs)

### l_exec_tfn.c

Functions to evaluate LEC "time functions". 

    static L_REAL L_lag(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
    static L_REAL L_diff(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
    static L_REAL L_rapp(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
    static L_REAL L_dln(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
    static L_REAL L_grt(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
    static L_REAL L_mavg(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
    static L_REAL L_vmax(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
    static L_REAL L_vmin(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
    static L_REAL L_sum(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
    static L_REAL L_prod(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
           L_REAL L_mean(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
    static L_REAL L_stderr(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
    static L_REAL L_lastobs(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)


### l_exec_mtfn.c

Functions to evaluate LEC "time functions" with possibly  multiple arguments. 

    static L_REAL L_calccorr(unsigned char* expr1, short len1, unsigned char* expr2, short len2, int t, L_REAL* stack, int nargs)
    static L_REAL L_corr(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
    static L_REAL L_calccovar(unsigned char* expr1, short len1, unsigned char* expr2, short len2, int t, L_REAL* stack, int nargs, int orig)
    static L_REAL L_covar(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
    static L_REAL L_covar0(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
    static L_REAL L_var(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
    static L_REAL L_stddev(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
    static L_REAL L_index(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
    static L_REAL L_acf(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
    static int    L_calcvals(unsigned char* expr1, short len1, int t, L_REAL* stack, int* vt, L_REAL* vy, int notnul)
    static L_REAL L_interpol(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
    static L_REAL L_app(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
    static L_REAL L_dapp(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
    static L_REAL L_hpall(unsigned char* expr, short len, int t, L_REAL* stack, int nargs, int std)
    static L_REAL L_hp(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
    static L_REAL L_hpstd(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
    

### l_newton.c
### l_rand.c
### l_secant.c
### l_debug.c

## LEC virtual functions 

### k_lec.c
Implemention of the LEC library virtual functions for SCL and VAR references.

    IODE_REAL *L_getvar(KDB* kdb, int pos)
    IODE_REAL L_getscl(KDB* kdb, int pos)
    SAMPLE *L_getsmpl(KDB* kdb)
    int L_findscl(KDB* kdb, char *name)
    int L_findvar(KDB* kdb, char* name)

L_macro ??