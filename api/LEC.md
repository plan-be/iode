# IODE. Group of functions "LEC language"

The group of functions for managing LEC expressions is divided into these main categories:
- compilation: to check the syntax of the expression and to produce a precompiled structure (CLEC)
- linking: to link the CLEC structure to the variables and scalars in KDB
- execution: to calculate the 
- errors: to manage error messages
- virtual functions

LEC syntax depends on some variables defined in l_token.c:

      - L_TABLE:      keywords recognised is a LEC expression, like '+' or 'ln'
      - L_PRIOR:      table of the operator execution priorities (which one is executed first, second...)
      - L_MIN_FARGS:  table of minimum number of arguments for the math functions (like log())
      - L_MAX_FARGS:  table of maximum number of arguments for the math functions
      - L_MIN_TARGS:  table of minimum number of arguments for the time functions (like grt())
      - L_MAX_TARGS:  table of minimum number of arguments for these functions
      - L_MIN_MTARGS: table of minimum number of arguments for the functions with multiple time expressions
      - L_MAX_MTARGS: table of minimum number of arguments for these functions

These variables may be changed but with some care. 

## LEC compilation 

### l_token.c
Functions and associated variables to "open" a LEC expression and to read the expression one token at a time.

The variables (L_TABLE, L_PRIOR, L_MIN...) are described above.

The exposed functions are:
    - int L_open_all(char* file_or_string, int type)    Opens a file or a string for reading and assigns the open stream to L_YY.
    - void L_close()                                    Close the stream L_YY.
    - int L_get_token()                                 Main function to browse a LEC expression (in L_YY) token by token. 

### l_cc1.c
First step of LEC compilation. During this stage, 2 tables, one with atomic expressions and 
one with the names found in the expression are filled. 

In a second step (l_cc2.c), a "CLEC expression" is generated. 
which is basically the serialization of the combination of L_EXPR and L_NAMES.

Main functions :

    - int L_cc1(int nb_names)           First step of LEC compilation.
    - void L_alloc_expr(int nb)         Allocates or reallocates L_EXPR by blocks of 100 elements.
    - int L_sub_expr(ALEC* al, int i)   Computes the position of the beginning of a sub-expression

### l_cc2.c

### l_eqs.c



## LEC linking


### l_link.c

## LEC execution

### l_exec.c
### l_newton.c
### l_rand.c
### l_secant.c
### l_debug.c

## LEC general functions

### l_err.c
Function and variable store and generate error messages.
    
    int L_errno:        Last error number during LEC compilation
    char* L_error():    Returns a static buffer containing the last LEC compilation error message.

### l_alloc.c
Functions to allocate and free standard memory (on the "heap"). 

    - char *L_malloc(int lg)    Allocates lg bytes in conventional memory. Fills the allocated space with nulls.
    - void L_free(void *ptr)    Frees an allocated buffer. If ptr is null, does nothing.

## Users functions 

### k_lec.c
Implemention of the LEC library virtual functions for SCL and VAR references.

    IODE_REAL *L_getvar(KDB* kdb, int pos)
    IODE_REAL L_getscl(KDB* kdb, int pos)
    SAMPLE *L_getsmpl(KDB* kdb)
    int L_findscl(KDB* kdb, char *name)
    int L_findvar(KDB* kdb, char* name)

L_macro ??