# IODE. Group of functions "LEC"

## Summary
A LEC expression is compiled in 2 steps. 
During the first step (l_cc1.c), 2 tables, L_EXPR with atomic expressions and L_NAMES with the names found in the LEC expression are constructed. 
In a second step (l_cc2.c), a "CLEC expression" is generated which is basically the serialization of the combination of L_EXPR and L_NAMES.

The CLEC expression can then be linked to the KDB's of variables and scalars (l_link.c). Each name is searched in the KDB's and their 
position is saved in l_names[i].pos.

Finally, the expression is computed by the fonction L_exec (l_exec.c)_.

### Example of compiling a LEC expression

Let's compille the expression 

    A+ln(B)

After the first compilation step, the resulting L_NAME is a table like {"A", "B"} and L_EXPR can be simplified as:

    Type       Value
    ----       -----
    L_VAR      0 = pos of A in L_NAMES
    L_OPENP
    L_VAR_     1 = pos of B in L_NAMES
    L_CLOSEP
    L_LN_      

In a second step (l_cc2.c), a "CLEC expression" is generated which is basically the serialization of the combination of L_EXPR and L_NAMES.

A simplified version of the resulting executable expression (sort of reverted polish notation) is :

    Size    Type      Value                        
    ----    ----
    21      L_VAR     CVAR: {0, 0, 0, {0,0,0}} = {pos of A in L_NAMES, lag, ref, period}
    0       L_OPENP
    21      L_VAR     CVAR: {1, 0, 0, {0,0,0}} = {pos of B in L_NAMES, lag, ref, period}
    0       L_CLOSEP  
    2       L_LN      1 (nb of arguments)

Finally, the CLEC struct regroups the following data:

    - total length of the CLEC struct
	- length of executable expression
    - nb of names
    - dupendo (reserved for equations)
    - table of LNAME's
    - executable expression described above

## Groups of variables and functions

The group of variables and functions for managing LEC expressions is divided into the categories below:
- variables defining the LEC syntax
- utilities: memory allocation / error management
- compilation: to check the syntax of the expression and to produce a precompiled structure (CLEC)
- linking: to link the CLEC structure to the variables and scalars in KDB's
- execution: to calculate the value of a CLEC expression in the context of specific KDB's
- virtual functions: functions that can be rewritten for use in other contexts than KDB databases

## Variables defining the LEC syntax
The LEC syntax depends on variables defined in l_token.c. These variables are also related to #defined in iode.h.

      - L_TABLE:      keywords recognised is a LEC expression, like '+' or 'ln'
      - L_PRIOR:      table of the operator execution priorities (which one is executed first, second...)
      - L_MIN_FARGS:  table of minimum number of arguments for the math functions (like log())
      - L_MAX_FARGS:  table of maximum number of arguments for the math functions
      - L_MIN_TARGS:  table of minimum number of arguments for the time functions (like grt())
      - L_MAX_TARGS:  table of minimum number of arguments for these functions
      - L_MIN_MTARGS: table of minimum number of arguments for the functions with multiple time expressions
      - L_MAX_MTARGS: table of minimum number of arguments for these functions

These variables may be changed but with some care. 

## LEC utilities

### l_alloc.c
Functions to allocate and free standard memory (on the "heap") for the LEC group of functions. 
May be replaced by another implementation if needed.

    - char *L_malloc(int lg)    Allocates lg bytes in conventional memory. Fills the allocated space with nulls.
    - void L_free(void *ptr)    Frees an allocated buffer. If ptr is null, does nothing.

### l_err.c
Function and variables to manage error messages.
     
    - int L_errnon:       Last error number during LEC compilation
    - char* L_error():    Returns a static buffer containing the last LEC compilation error message.

## LEC compilation 

### l_token.c
Functions and associated variables to "open" a LEC expression and to read the expression one token at a time.

The variables (L_TABLE, L_PRIOR, L_MIN...) are described above.

The exposed functions are:
    - int L_open_all(char* file_or_string, int type)    Opens a file or a string for reading and assigns the open stream to L_YY.
    - void L_close()                                    Close the stream L_YY.
    - int L_get_token()                                 Main function to browse a LEC expression (in L_YY) token by token. 

### l_cc1.c
First step of LEC compilation. 

Main functions :

    - int L_cc1(int nb_names)           Creates:
                                            - L_EXPR  = ordered list of atomic expressions with references to L_NAMES
                                            - L_NAMES = list of names in the LEC expression
    - void L_alloc_expr(int nb)         Allocates or reallocates L_EXPR by blocks of 100 elements.
    - int L_sub_expr(ALEC* al, int i)   Computes the position of the beginning of a sub-expression

### l_cc2.c

Second step of LEC compilation, producing a "CLEC expression" which contains what is needed to efficiently executable the expression.

    CLEC *L_cc2(ALEC* expr)                          Second stage of LEC compilation. Generates an "executable" LEC expression.
    void L_move_arg(char *s1, char *s2, int lg)      Copies lg bytes from a buffer to another in reverse order. The 2 buffers may overlap.
    CLEC *L_cc_stream()                              Compiles L_YY, the open YY stream containing a LEC expression.
    CLEC *L_cc(char* lec)                            Compiles a LEC string. 



### l_eqs.c



## LEC linking
The third step consists in linking the CLEC expression to the KDB of variables and scalars. Each name is searched in the KDB's and their 
position is placed in l_names[i].pos.

### l_link.c

## LEC execution

Finally, the expression is calculated by the fonction L_exec().



### l_exec.c
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