/**
 * @header4iode
 *
 * Functions to compile LEC *equations*.
 * 
 * Main functions
 * --------------
 *    CLEC* L_solve(char* eq, char* endo)  Compiles a LEC equation and tries to analytically solve the equation with respect to endo.
 *    int L_split_eq(char* eq)             Returns the position of ":=" in an equation or -1 if not found.
 * 
 * Context
 * -------
 *      LEC *equations* are made up of 2 LEC *expressions* separated by ":=":  
 *           LHS := RHS
 *      
 *      Equations are used in 2 different contexts: 
 *           - estimation of coefficients 
 *           - model simulation
 *      
 *      The way LHS need to be compiled differs according to the context.
 *
 *      To estimate (the coefficients of) the equation "LHS := RHS", where RHS contains the coefficients, one needs (roughly) to
 *      calculate LHS and then to find the values of the coefficients to minimize |LHS - RHS|. LHS may be a simple endogenous variable 
 *      or a complex expression: "PIB" and "dln PIB" are both valid LHS in the case of an estimation.
 *      
 *      A model simulation using a Gauss-Seidel-like method, in contast, requires the evaluation of the endogenous for each equation
 *      in an iterative process. In the example above, the equation "dln PIB := RHS" should ideally be transformed in "PIB := exp(ln PIB[-1] + RHS)".
 *      When this is impossible, another numerical (sub-iterative) method is to be used to find the value of the endogenous variable.
 * 
 * 
 */

#include "api/lec/lec.h"

/**
 * Struct specific for equation compilation.
*/
typedef struct _slec {
    ALEC*   sl_expr[2];     // Tables of ALEC's for LHS and RHS equations  members 
    int     sl_lg[2];       // Length of each sl_expr table
    int     sl_mbr;         // member containing sl_endo (will be inverted if possible)
    int     sl_nmbr;        // member not containing sl_endo (will be left unchanged)
    int     sl_op;          // last operator|function
    int     sl_nargs;       // number of args of the last op|fn
    char*   sl_endo;        // name of the endogenous variable
} SLEC;

// Static functions declarations
static ALEC* L_cc1_alloc(char* lec, int nb_names);
static int L_cc1_eq(SLEC* sl, char* eq);
static void L_append(SLEC* sl, int mbr);
static void L_front(SLEC* sl, int mbr);
static void L_append_op(int op);
static void L_append_fn(int op, int nargs);
static void L_append_const(int a);
static int L_invert(char* eq, char* endo, int *dupendo);
CLEC* L_solve(char* eq, char* endo);
int L_split_eq(char* eq);
static int L_count_endo(ALEC* al, int lg, char* endo);
static int L_split_expr(SLEC* sl);

// Defines

#define EXPR        (sl)->sl_expr
#define LG          (sl)->sl_lg
#define MBR         (sl)->sl_mbr
#define NMBR        (sl)->sl_nmbr
#define ENDO        (sl)->sl_endo
#define OP          (sl)->sl_op
#define NARGS       (sl)->sl_nargs
#define APP(x)      L_append(sl, x)
#define FRT(x)      L_front(sl, x)
#define APP_OP(x)   L_append_op(x)
#define APP_FN(x,y) L_append_fn(x, y)
#define APP_VAL(y)  L_append_const(y)


/**
 * Compiles a LEC expression and allocates the ALEC table resulting from the compilation. 
 * 
 * As this function is used to compile *equations*, it is called twice, once for the left member (LHS)
 * and once for the right member (RHS). 
 * For that reason:
 *      nb_names = 0 for first call (no names are already known) 
 *      nb_names = L_NB_NAMES for second member (all names in LHS are already in L_NAMES).
      
 * L_errno contains 0 or the error number.
 * 
 * @param [in] lec           char*   LEC expression to compile
 * @param [in] nb_names      int     number of names in L_NAMES before compilation
 * @return                   ALEC*   allocated table of ALEC's or NULL on error
*/
static ALEC* L_cc1_alloc(char* lec, int nb_names)
{
    ALEC    *alec = 0;

    if(L_open_string(lec)) return(alec);
    if(L_cc1(nb_names) != 0) return(alec);
    L_close();
    alec = (ALEC *)L_malloc(L_NB_EXPR * sizeof(ALEC));
    memcpy(alec, L_EXPR, L_NB_EXPR * sizeof(ALEC));
    return(alec);
}


/**
 * Splits an equation in LHS and RHS and compiles (step 1 only) each element separately. 
 * The error L_ASSIGN_ERR is returned if ":=" is not found in the equation.
 * 
 * The result is stored in a SLEC struct (see above).
 * Only the step 1 of the compilation is performed at this stage.
 * 
 * @param [in, out]     sl    SLEC*   struct where the A
 * @param [in]          eq    char*   text of the equation
 * @return                    int     error code or 0 on success
 *                                      
*/
static int L_cc1_eq(SLEC* sl, char* eq)
{
    int     pos;

    /* SLIT EQ AND COMPILE EACH MEMBER */
    pos = L_split_eq(eq);
    if(pos < 0) return(L_errno = L_ASSIGN_ERR);

    // Compiles left member
    eq[pos] = 0;        // close the left member
    EXPR[0] = L_cc1_alloc(eq, 0);
    LG[0]   = L_NB_EXPR - 1;
    L_alloc_expr(-1);   // Clean up L_EXPR
    eq[pos] = ':';      // Reset the original text

    // Compiles the right member
    if(EXPR[0] == 0) return(L_errno);
    EXPR[1] = L_cc1_alloc(eq + pos + 2, L_NB_NAMES);
    LG[1]   = L_NB_EXPR - 1;
    L_alloc_expr(-1);
    if(EXPR[1] == 0) {
        L_free(EXPR[0]);
        return(L_errno);
    }
    return(0);
}


/**
 * Appends to L_EXPR the member mbr (LHS or RHS) contained in sl. 
 * 
 * @param [in] sl   SLEC*   
 * @param [in] mbr  int     position of the member to append 
*/
static void L_append(SLEC* sl, int mbr)
{
    L_alloc_expr(L_NB_EXPR + LG[mbr]);
    memcpy(L_EXPR + L_NB_EXPR, EXPR[mbr], sizeof(ALEC) * LG[mbr]);
    L_NB_EXPR += LG[mbr];
}


/**
 * Add at the beginning of L_EXPR the member mbr of the SLEC struct sl.
 * 
 * @param [in] sl   SLEC*   container of the compiled equation
 * @param [in] mbr  int     member of sl to copy to L_EXPR
*/
static void L_front(SLEC* sl, int mbr)
{

    L_alloc_expr(L_NB_EXPR + LG[mbr]);
    L_move_arg((char *)(L_EXPR + LG[mbr]), (char *)(L_EXPR), sizeof(ALEC) * L_NB_EXPR);
    memcpy(L_EXPR, EXPR[mbr], sizeof(ALEC) * LG[mbr]);
    L_NB_EXPR += LG[mbr];
}


/**
 * Appends an operator to L_EXPR.
 * 
 * @param [in]  op  int     operator (defined in iode.h)
*/
static void L_append_op(int op)
{
    L_alloc_expr(L_NB_EXPR + 1);
    L_EXPR[L_NB_EXPR].al_type = op;
    L_NB_EXPR++;
}


/**
 * Appends a function and its number of arguments to L_EXPR.
 *
 * @param [in] op     int   function id
 * @param [in] nargs  int   number of arguments of the function
*/
static void L_append_fn(int op, int nargs)
{
    L_alloc_expr(L_NB_EXPR + 1);
    L_EXPR[L_NB_EXPR].al_type = op;
    L_EXPR[L_NB_EXPR].al_val.v_nb_args = nargs;
    L_NB_EXPR++;
}


/**
 * Appends a long constant to L_EXPR.
 * 
 * @param [in] a    int     value of the constant
*/
static void L_append_const(int a)
{
    L_alloc_expr(L_NB_EXPR + 1);
    L_EXPR[L_NB_EXPR].al_type = L_LCONST;
    L_EXPR[L_NB_EXPR].al_val.v_long = a;
    L_NB_EXPR++;
}


/**
 * Tries to analytically solve an equation with respect to a specified endogenous variable. The result
 * is thus the equivalent of a LEC *expression*, not *equation*. That transformed expression is used in simuations.
 * 
 * The result is stored in L_EXPR (table of ALEC's).
 * 
 * If the endogenous variable is found more than once, the equation (say "LHS := RHS") is replaced by "0 := LHS - RHS" and dupendo is set to 1.
 *
 * For example,
 *   L_invert("ln X := a + b * ln Y", "X", &dupendo) => "exp(a + b * ln Y)"    with dupendo = 0
 *   L_invert("ln X := a + b * X ",   "X", &dupendo) => "ln X - (a + b * X)"   with dupendo = 1
 * 
 * 
 * L_errno can take the values below:
 *      - L_DUP_ERR    : duplicate endo in equation (cannot be solved)
 *		- L_INVERT_ERR : cannot invert an operator  (ex "(X<t)*2 := Y")
 *		- L_ASSIGN_ERR : sign := not found
 *      - other        : standard compilation errors or 0 on success
 *    
 * @param [in]   eq         char*   text of the LEC equation
 * @param [in]   endo       char*   name of the endogenous variable 
 * @param [out]  dupendo    int*    0 if the equation has been inverted, 1 if endo is present more than once.
 * @return                  int     0 on success and L_errno on error
*/
static int L_invert(char* eq, char* endo, int *dupendo)
{
    SLEC    slec, *sl = &slec;
    ALEC    *expr;
    int     count0, count1;

    // Compiles the 2 members of eq and put the result in slec
    if(L_cc1_eq(sl, eq)) return(L_errno);

    /* FIND MEMBER CONTAINING ENDO AND SET INFO IN sl */
    *dupendo = 0;
    ENDO = endo;
    count0 = L_count_endo(EXPR[0], LG[0], ENDO);
    count1 = L_count_endo(EXPR[1], LG[1], ENDO);
    if(count0 + count1 == 0) {
        L_free(EXPR[0]);
        L_free(EXPR[1]);
        return(L_errno = L_DUP_ERR);
    }
    if(count0 + count1 >= 2) {
        // Result = {EXPR[0], EXPR[1], L_MINUS, L_EOE} i.e. F(x) = LHS - RHS 
        *dupendo = 1;
        L_NB_EXPR = 0;
        L_append(sl, 0);    
        L_append(sl, 1);
        APP_OP(L_MINUS);
        APP_OP(L_EOE);
        L_free(EXPR[0]);
        L_free(EXPR[1]);
        return(0);
    }
    if(count0 == 0) {
        // If endo not present in LHS, set mbr to 1 and nmbr to 0
        MBR = 1;
        NMBR = 0;
    }
    else {
        // Else set mbr to 0 and nmbr to 1
        MBR = 0;
        NMBR = 1;
    }

    // Create an empty L_EXPR and move the member nmbr into L_EXPR
    L_NB_EXPR = 0;
    L_append(sl, NMBR);
    L_free(EXPR[NMBR]);

    // EXPR[MBR] is the expression containing endo and must thus be inverted
    expr = EXPR[MBR];

    /* LOOP ON OPERATORS */
    while(L_split_expr(sl) == 0)
        switch(OP) {
            case L_PLUS   :             
                APP(NMBR);              // Appends EXPR[NMBR] to L_EXPR
                APP_OP(L_MINUS);        // Appends + to L_EXPR
                break;
            case L_TIMES  :
                APP(NMBR);
                APP_OP(L_DIVIDE);
                break;
            case L_MINUS  :
                if(MBR == 1) {
                    FRT(NMBR);
                    APP_OP(L_MINUS);
                }
                else {
                    APP(NMBR);
                    APP_OP(L_PLUS);
                }
                break;
            case L_DIVIDE :
                if(MBR == 1) {
                    FRT(NMBR);
                    APP_OP(L_DIVIDE);
                }
                else {
                    APP(NMBR);
                    APP_OP(L_TIMES);
                }
                break;
            case L_LN     :
                APP_FN(L_EXPN, 1);
                break;
            case L_EXPN   :
                APP_FN(L_LN, 1);
                break;
            case L_UPLUS  :
                break;
            case L_UMINUS :
                APP_FN(L_UMINUS, 1);
                break;
            case L_COS    :
                APP_FN(L_ACOS, 1);
                break;
            case L_ACOS   :
                APP_FN(L_COS, 1);
                break;
            case L_SIN    :
                APP_FN(L_ASIN, 1);
                break;
            case L_ASIN   :
                APP_FN(L_SIN, 1);
                break;
            case L_TAN    :
                APP_FN(L_ATAN, 1);
                break;
            case L_ATAN   :
                APP_FN(L_TAN, 1);
                break;
            case L_SQRT   :
                APP_VAL(2);
                APP_FN(L_EXP, 2);
                break;
            case L_EXP    :
                if(MBR == 0) {
                    APP_FN(L_LN, 1);
                    APP(NMBR);
                    APP_OP(L_DIVIDE);
                    APP_FN(L_EXPN, 1);
                }
                else {
                    FRT(NMBR);
                    APP_FN(L_LOG, 2);
                }
                break;
            /*
            case L_LOG    : if(MBR == 0) {
            		}
            	    else {
            		FRT(NMBR); APP_OP(L_EXP);
            		}
            	    break;
            /* JMP 22-06-00 */
            case L_LOG    :
                APP_FN(L_EXP, 1);
                break; /* JMP 22-06-00 */
            case L_DIFF   :
                if(NARGS == 2) APP(NMBR);
                APP(MBR);
                APP_FN(L_LAG, NARGS);
                APP_OP(L_PLUS);
                break;
            case L_DLN    :
                if(NARGS == 2) APP(NMBR);
                APP(MBR);
                APP_FN(L_LAG, NARGS);
                APP_FN(L_LN, 1);
                APP_OP(L_PLUS);
                APP_FN(L_EXPN, 1);
                break;
            case L_GRT    :
                APP_VAL(100);
                APP_OP(L_DIVIDE);
                APP_VAL(1);
                APP_OP(L_PLUS);
                if(NARGS == 2) APP(NMBR);
                APP(MBR);
                APP_FN(L_LAG, NARGS);
                APP_OP(L_TIMES);
                break;
            case L_RAPP   :
                if(NARGS == 2) APP(NMBR);
                APP(MBR);
                APP_FN(L_LAG, NARGS);
                APP_OP(L_TIMES);
                break;
            default :
                L_free(expr);
                return(L_errno = L_INVERT_ERR);
        }

    APP_OP(L_EOE);
    L_free(expr);
    return(L_errno);
}


/**
 * Compiles a LEC equation and tries to analytically solve the equation with respect to endo.
 * 
 * Generates a CLEC form with the result and set clec->dupendo to 1 if the
 * generated form is of the form "0 := LHS - RHS")
 * 
 * @param [in] eq       char*
 * @param [in] endo     char* 
 * @return              CLEC*
*/
CLEC* L_solve(char* eq, char* endo)
{
    CLEC    *clec = 0;
    int     dupendo = 0;

    L_invert(eq, endo, &dupendo);
    switch(L_errno) {
        case 0              :
            clec = L_cc2(L_EXPR);
            if(clec != 0) clec->dupendo = dupendo;
            break;

        case L_DUP_ERR      :
        case L_INVERT_ERR   :
        default             :
            break;
    }
    L_alloc_expr(-1);
    return(clec);
}



/**
 * Returns the position of ":=" in an equation or -1 if not found.
 * 
 * @param [in] eq   char*   equation to analyse
 * @return          int     position of := in the equation or -1 if not found.
*/
int L_split_eq(char* eq)
{
    int i;

    for(i = 0 ; eq[i] != 0 ; i++)
        if(eq[i] == ':' && eq[i + 1] == '=') return(i);

    return(-1);
}


/**
 * Counts the number of occurences of a variable in a ALEC table of atomic expressions (result of l_cc1()).
 * The lagged (A[-1]) and timed (A[2000Y1]) variables are not taken into account.
 * 
 * @param [in]  al      ALEC*   table of ALEC elements
 * @param [in]  lg      int     nb of elements in al
 * @param [in]  endo    char*   name of the variable to search
 * @return              int     number of occurence of endo in ALEC
*/
static int L_count_endo(ALEC* al, int lg, char* endo)
{
    int     count, i;

    for(count = 0, i = 0; i < lg; i++, al++)
        if(al->al_type == L_VAR &&
                al->al_val.v_var.per.p_s == 0 &&
                al->al_val.v_var.lag == 0 &&
                strcmp(L_NAMES[al->al_val.v_var.pos], endo) == 0)
            count++;

    return(count);
}


/**
 * If EXPR[MBR] is not trivial, splits EXPR[MBR] into two parts and replaces EXPR[MBR] and EXPR[NMBR] by these 2 expressions.
 * 
 * ------------------------------------------------------
 * Example: 
 *
 * Expression containing the endogenous : "EXO + ln ENDO"
 * 
 * At the beginning of the function:
 * 
 *  EXPR[MBR]:  | EXO  |
 *              | (    |
 *              | ENDO |
 *              | )    |
 *              | ln   |
 *              | +    |
 * 
 *  At the end of the function:
 *     EXPR[0] =   EXO
 *     EXPR[1] =  | (    |
 *                | ENDO |
 *                | )    |
 *                | ln   |
 *     MBR = 1
 *     NMBR = 0
 * -------------------------------------------------------
 * 
 * @param [in, out]     sl  SLEC*   current state of the equation
 * @return                  int     0 on success, L_DUP_ERR if the sub expression cannot be determined
*/
static int L_split_expr(SLEC* sl)
{
    ALEC    *al;
    int     lg, pos;

    al = EXPR[MBR];     // ALEC containing the ENDO
    lg = LG[MBR] - 1;   // size of al
ag:
    OP = al[lg].al_type;                // Last element (operator or L_VAR ...) in al. In example, '+'
    NARGS = al[lg].al_val.v_nb_args;    
    switch(OP) {
        case L_VAR :                    // if the last element is L_VAR, we have only one item and the process is termined (?)
            return(1);                  
        case L_CLOSEP :                 // if the last element is L_CLOSEP, the first on must be L_OPENP (?)
            al++ ;                      // we drop the first and last elements of al
            lg -= 2;
            goto ag;                    // we try the next element
                                        
    }
    pos = L_sub_expr(al, lg - 1);               // Search the beginning of the sub expression ending at pos lg-1. In example : 1
    if(pos < 0) return(L_errno = L_DUP_ERR);    
    EXPR[0] = al;                               // We move the elements before the sub expression to EXPR[0]
    LG[0] = pos;
    EXPR[1] = al + pos;                         // and the sub expression to EXPR[1]
    LG[1] = lg - pos;

    // Find the member where endo is present
    if(L_count_endo(al, pos, ENDO) > 0) {       
        MBR = 0 ;                                   
        NMBR = 1;                               
    }
    else {
        MBR = 1 ;
        NMBR = 0;
    }
    return(0);
}




