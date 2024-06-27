
<IODE: LEC implementation>
IODE: LEC implementation
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..sep |
..esc ~
..ignore ¯


&TI Summary
¯¯¯¯¯¯¯¯
A LEC expression is compiled, linked and evaluated in 4 steps. 

&IT Step 1: LEC interpreter
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
During the first step (l_cc1.c), 2 tables, L_EXPR with atomic expressions and L_NAMES with the names found in the LEC expression are constructed. 
L_EXPR is organised in "stack-oriented" order. That step makes use of l_token.c functions.

&IT Step 2: LEC serializing
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
In a second step (l_cc2.c), a "CLEC expression" is generated which is basically the serialization of the combination of L_EXPR and L_NAMES.

&IT Step 3. LEC linker
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
The CLEC expression can then be linked to the KDB's of variables and scalars (l_link.c). Each name is searched in the KDB's and their respective
positions is saved in the CLEC structure.

&IT Step 4: LEC calculator
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Finally, the expression is computed by the fonction L_exec (l_exec.c).


&TI Example of a LEC expression compilation
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Let's compile the following expression:

&CO
    A + ln(B)
&TX

At the end of the first compilation step, two tables are created:

L_NAMES which is a table with the names found in the LEC expression:
..tb
|Names
|A    
|B    
..te    
   
and L_EXPR that can be sketched as:
..tb
|Type    | Value
|L_VAR   | 0 (= pos of A in L_NAMES)
|L_OPENP | 
|L_VAR   | 1 (= pos of B in L_NAMES)
|L_CLOSEP| 
|L_LN    | 
|L_PLUS  | 
..te

During the second step (l_cc2.c), a "CLEC expression" is generated which is the serialization of the combination of L_EXPR and L_NAMES.

A simplified version of the resulting "executable" expression (kind of "Reverted Polish Notation") is:
..tb
| Size| Type   | Value                                                             
| 21  | L_VAR  | CVAR: {0, 0, 0, {0,0,0}} = {pos of A in L_NAMES, lag, ref, period}
| 21  | L_VAR  | CVAR: {1, 0, 0, {0,0,0}} = {pos of B in L_NAMES, lag, ref, period}
| 2   | L_LN   | 1 (nb of arguments)                                               
| 1   | L_PLUS |                                                                   
..te

Finally, the CLEC *struct* regroups the following data:

&EN total length of the CLEC struct
&EN length of executable expression
&EN nb of names
&EN dupendo (reserved for equations)
&EN table of LNAME's
&EN executable expression described above

~bGroups of functions and variables~B

&EN <LEC syntax>: variables defining the LEC syntax
&EN <LEC utilities>: LEC memory allocation + LEC error management
&EN <LEC compiler>: to check the syntax of a LEC expression and to produce a precompiled structure (CLEC)
&EN <LEC linker>: to link the CLEC structure to the variables and scalars in KDB's
&EN <LEC execution>: to calculate the value of a CLEC expression in the context of specific KDB's
&EN <LEC virtual functions>: functions that can be rewritten for use in other contexts than KDB databases

>


<LEC syntax>
LEC syntax
¯¯¯¯¯¯¯¯¯¯¯¯
The LEC syntax depends on variables defined in l_token.c. 
These variables are also related to ##define's in iode.h.

&TI l_token.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
|Variable         |Description   
|~cL_TABLE      ~C|keywords recognised in a LEC expression, like '+' or 'ln'                             
|~cL_PRIOR      ~C|table of the operator execution priorities (which one is executed first, second...)   
|~cL_MIN_FARGS  ~C|table of minimum number of arguments for the math functions (like log())              
|~cL_MAX_FARGS  ~C|table of maximum number of arguments for the math functions                           
|~cL_MIN_TARGS  ~C|table of minimum number of arguments for the time functions (like grt())              
|~cL_MAX_TARGS  ~C|table of minimum number of arguments for the time functions                           
|~cL_MIN_MTARGS ~C|table of minimum number of arguments for the functions with multiple time expressions 
|~cL_MAX_MTARGS ~C|table of minimum number of arguments for the functions with multiple time expressions 
..te

These variables may be changed (but with some care). 
>

<LEC utilities>
LEC utilities
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&TI l_alloc.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Functions to allocate and free standard memory (on the "heap") for the LEC group of functions. 
Could be replaced by another implementation if needed.

..tb
| Syntax | Description
| ~cchar *L_malloc(int lg)   | Allocates lg bytes in conventional memory. Fills the allocated space with nulls.
| ~cvoid L_free(void *ptr)   | Frees an allocated buffer. If ptr is null, does nothing.
..te

&TI l_err.c
¯¯¯¯¯¯¯¯¯¯¯¯
Function and variables to manage error messages.
..tb
| Syntax | Description
| ~cint L_errno       | Last error number during LEC compilation
| ~cchar* L_error()   | Returns a static buffer containing the last LEC compilation error message.
..te

&TI l_debug.c
¯¯¯¯¯¯¯¯¯¯¯¯
Function to save debugging info in a text file.
..tb
| Syntax | Description
| ~cvoid L_debug(char* fmt, ...) | appends a message to the file L_DEBUG_FILENAME ("simul.dbg" by default).
| ~bbGlobal variable
| ~cchar* L_DEBUG_FILENAME = 0;  | name of the output file (default "simul.dbg")
..te

>



<LEC compiler> 
LEC compiler 
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&TI l_token.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Functions and associated variables to "open" a LEC expression and to read the expression one token at a time.

The variables defining the LEC syntax (L_TABLE, L_PRIOR, L_MIN...) are described above.

Main functions:
..tb
| Syntax | Description
| ~cint L_open_all(char* file_or_string, int type)   | Opens a file or a string for reading and assigns the open stream to L_YY.
| ~cvoid L_close()                                   | Close the stream L_YY.
| ~cint L_get_token()                                | Main function to browse a LEC expression (in L_YY) token by token. 
..te

&TI l_cc1.c
¯¯¯¯¯¯¯¯¯¯¯
First step of LEC compilation. 

Main functions:

..tb
| Syntax | Description
| ~cint L_cc1(int nb_names)          | Creates L_EXPR(ordered list of atomic expressions with references to L_NAMES) and L_NAMES (list of names in the LEC expression)
| ~cvoid L_alloc_expr(int nb)        | Allocates or reallocates L_EXPR by blocks of 100 elements.
| ~cint L_sub_expr(ALEC* al, int i)  | Computes the position of the beginning of a sub-expression
..te

&TI l_cc2.c
¯¯¯¯¯¯¯¯¯¯¯
Second step of LEC compilation, producing a "CLEC expression" which contains what is needed to efficiently evaluate the expression.

Main functions:

..tb
| Syntax | Description
| ~cCLEC *L_cc2(ALEC* expr)                      | Second stage of LEC compilation. Generates an "executable" LEC expression.
| ~cvoid L_move_arg(char *s1, char *s2, int lg)  | Copies lg bytes from a buffer to another in reverse order. The 2 buffers may overlap.
| ~cCLEC *L_cc_stream()                          | Compiles L_YY, the open YY stream containing a LEC expression.
| ~cCLEC *L_cc(char* lec)                        | Compiles a LEC string. 
..te

&TI l_eqs.c
¯¯¯¯¯¯¯¯¯¯¯
Functions to compile LEC *equations*. LEC *equations* are made up of 2 LEC *expressions* separated by ":=".
Equations are used in 2 different contexts: estimation of coefficients and model simulation.

Main functions:

..tb
| Syntax | Description
| ~cCLEC* L_solve(char* eq, char* endo) | Compiles a LEC equation and tries to analytically solve the equation with respect to endo.
| ~cint L_split_eq(char* eq)            | Returns the position of ":=" in an equation or -1 if not found.
..te

More details and examples can be found in the source file.
>

<LEC linker>
LEC linker
¯¯¯¯¯¯¯¯¯¯¯
The third step consists in linking the CLEC expression to the KDB of variables and scalars. Each name is searched in the KDB's and their 
position is placed in l_names[i].pos.

Three functions, implemented in k_lec.c, are called during the link process: L_findvar(), L_findscl() L_getsmpl(). 
The positions of variables and scalars returned by these functions will be used at execution time by L_getvar() and L_getscl().

&TI l_link.c
¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax | Description
| ~cint L_link(KDB* dbv, KDB* dbs, CLEC* cl)   | Links a CLEC expression to KDB's of variables and scalars. Aligns PERIOD's to the SAMPLE of dbv.
| ~cvoid L_link_endos(KDB *dbe, CLEC *cl)      | Pseudo linking used to calculate the strong connex components of a model (SCC).
..te
>

<LEC execution>
LEC execution
¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Finally, the expression is calculated by the fonction L_exec().

&TI l_exec.c
¯¯¯¯¯¯¯¯¯¯¯

Functions to evaluate a compiled and linked LEC expression. 
..tb
| Syntax | Description
| ~cL_REAL L_exec_sub(unsigned char* expr, int lg, int t, L_REAL* stack) | Execution of a CLEC sub expression.
| ~cL_REAL L_exec(KDB* dbv, KDB* dbs, CLEC* expr, int t)                 | Execution of a compiled and linked CLEC expression.
| ~cL_REAL* L_cc_link_exec(char* lec, KDB* dbv, KDB* dbs)                | Compiles, links and executes a LEC expression.
..te

&TI l_exec_var.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Functions to evaluate LEC constants:

..tb
| Syntax | Description
| ~cstatic L_REAL L_pi ()          
| ~cstatic L_REAL L_euro()
| ~cstatic L_REAL L_e ()
| ~cstatic L_REAL L_time(int t)
| ~cstatic L_REAL L_i(int t)
..te

&TI l_exec_ops.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Functions to evaluate LEC "operators". 
..tb
| Syntax | Description
| ~cstatic L_REAL L_or (L_REAL a, L_REAL b)
| ~cstatic L_REAL L_and (L_REAL a, L_REAL b)
| ~cstatic L_REAL L_ge (L_REAL a, L_REAL b)
| ~cstatic L_REAL L_gt (L_REAL a, L_REAL b)
| ~cstatic L_REAL L_le (L_REAL a, L_REAL b)
| ~cstatic L_REAL L_lt (L_REAL a, L_REAL b)
| ~cstatic L_REAL L_eq (L_REAL a, L_REAL b)
| ~cstatic L_REAL L_ne (L_REAL a, L_REAL b)
| ~cstatic L_REAL L_plus (L_REAL a, L_REAL b)
| ~cstatic L_REAL L_minus(L_REAL a, L_REAL b)
| ~cstatic L_REAL L_times(L_REAL a, L_REAL b)
| ~c       L_REAL L_divide(L_REAL a, L_REAL b)
| ~c       L_REAL L_exp(L_REAL a, L_REAL b)
..te

&TI l_exec_fns.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Functions to evaluate LEC "functions". 

..tb
| Syntax | Description
| ~cL_REAL L_logn(L_REAL v)
| ~cstatic L_REAL L_uminus(L_REAL* stack)
| ~cstatic L_REAL L_uplus (L_REAL* stack)
| ~cstatic L_REAL L_log(L_REAL* stack, int nargs)
| ~cstatic L_REAL L_ln(L_REAL* stack)
| ~cstatic L_REAL L_not(L_REAL* stack)
| ~cstatic L_REAL L_expn(L_REAL* stack, int nargs)
| ~cstatic L_REAL L_max(L_REAL* stack, int nargs)
| ~cstatic L_REAL L_min(L_REAL* stack, int nargs)
| ~cstatic L_REAL L_sin (L_REAL* stack)
| ~cstatic L_REAL L_cos (L_REAL* stack)
| ~cstatic L_REAL L_acos (L_REAL* stack)
| ~cstatic L_REAL L_asin (L_REAL* stack)
| ~cstatic L_REAL L_tan (L_REAL* stack)
| ~cstatic L_REAL L_atan (L_REAL* stack)
| ~cstatic L_REAL L_tanh (L_REAL* stack)
| ~cstatic L_REAL L_sinh (L_REAL* stack)
| ~cstatic L_REAL L_cosh (L_REAL* stack)
| ~cstatic L_REAL L_abs (L_REAL* stack)
| ~cstatic L_REAL L_sqrt (L_REAL* stack)
| ~cstatic L_REAL L_int (L_REAL* stack)
| ~cstatic L_REAL L_rad (L_REAL* stack)
| ~cstatic L_REAL L_if(L_REAL* stack, int nargs)
| ~cstatic L_REAL L_lsum(L_REAL* stack, int nargs)
| ~cstatic L_REAL L_lmean(L_REAL* stack, int nargs)
| ~cstatic L_REAL L_fnisan(L_REAL* stack, int nargs)
| ~cstatic L_REAL L_lcount(L_REAL* stack, int nargs)
| ~cstatic L_REAL L_lprod(L_REAL* stack, int nargs)
| ~cstatic L_REAL L_sign(L_REAL* stack)
| ~cstatic L_REAL L_lstderr(L_REAL* stack, int nargs)
| ~cstatic L_REAL L_random(L_REAL* stack)
| ~cstatic L_REAL L_floor(L_REAL* stack)
| ~cstatic L_REAL L_ceil (L_REAL* stack)
| ~cstatic L_REAL L_round(L_REAL* stack, int nargs)
| ~cstatic L_REAL L_urandom(L_REAL* stack)
| ~cstatic L_REAL L_grandom(L_REAL* stack)
| ~cstatic L_REAL L_gamma(L_REAL* stack)
| ~cstatic L_REAL L_div0(L_REAL *stack, int nargs)
..te

&TI l_exec_tfn.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Functions to evaluate LEC "time functions". 
..tb
| Syntax | Description
| ~cstatic L_REAL L_lag(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
| ~cstatic L_REAL L_diff(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
| ~cstatic L_REAL L_rapp(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
| ~cstatic L_REAL L_dln(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
| ~cstatic L_REAL L_grt(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
| ~cstatic L_REAL L_mavg(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
| ~cstatic L_REAL L_vmax(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
| ~cstatic L_REAL L_vmin(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
| ~cstatic L_REAL L_sum(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
| ~cstatic L_REAL L_prod(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
| ~c       L_REAL L_mean(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
| ~cstatic L_REAL L_stderr(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
| ~cstatic L_REAL L_lastobs(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
..te

&TI l_exec_mtfn.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Functions to evaluate LEC "time functions" with possibly  multiple arguments. 

..tb
| Syntax | Description
| ~cstatic L_REAL L_calccorr(unsigned char* expr1, short len1, unsigned char* expr2, short len2, int t, L_REAL* stack, int nargs)
| ~cstatic L_REAL L_corr(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
| ~cstatic L_REAL L_calccovar(unsigned char* expr1, short len1, unsigned char* expr2, short len2, int t, L_REAL* stack, int nargs, int orig)
| ~cstatic L_REAL L_covar(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
| ~cstatic L_REAL L_covar0(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
| ~cstatic L_REAL L_var(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
| ~cstatic L_REAL L_stddev(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
| ~cstatic L_REAL L_index(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
| ~cstatic L_REAL L_acf(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
| ~cstatic int    L_calcvals(unsigned char* expr1, short len1, int t, L_REAL* stack, int* vt, L_REAL* vy, int notnul)
| ~cstatic L_REAL L_interpol(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
| ~cstatic L_REAL L_app(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
| ~cstatic L_REAL L_dapp(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
| ~cstatic L_REAL L_hpall(unsigned char* expr, short len, int t, L_REAL* stack, int nargs, int std)
| ~cstatic L_REAL L_hp(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
| ~cstatic L_REAL L_hpstd(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
..te

&TI l_hodrick.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Hodrick-Prescott filter. These functions are subfunctions of L_hp*() defined on l_exec_mtfn.c.

..tb
| Syntax | Description
| ~cint HP_calc(double *f_vec, double *t_vec, int nb, double lambda, int std) | Hodrick-Prescott filter. 
| ~cvoid HP_test(double *f_vec, double *t_vec, int nb, int *beg, int *dim)       | Prepares HP_calc()
..te
>
<LEC virtual functions>
LEC virtual functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&TI k_lec.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Implemention of the LEC library virtual functions for SCL and VAR references.

..tb
| Syntax                                    | Description    
| ~cdouble *L_getvar(KDB* kdb, int pos)  | Retrieves a pointer to the first element of a VAR.  
| ~cdouble L_getscl(KDB* kdb, int pos)   | Retrieves a scalar value.
| ~cSAMPLE *L_getsmpl(KDB* kdb)             | Retrieves the sample of a KDB.
| ~cint L_findscl(KDB* kdb, char *name)     | Retrieves a scalar position.
| ~cint L_findvar(KDB* kdb, char* name)     | Retrieves a variable position.
..te

>