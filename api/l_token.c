/**
 *  @header4iode
 *  
 *  LEC tokens: functions and associated variables to open a stream and to read a token from the open stream.
 *  
 *  The variables listed below define the recognised LEC tokens and operators. They refer to definitions in iode.h.
 *  
 *  The elements (or tokens) in a LEC formula can be divided in groups. Each group has a unique identifier defined in iode.h.
 *  
 *      - starting at L_OP:     operators (+, -, *...)
 *      - starting at L_FN:     mathematical functions (ln, exp, sin...)
 *      - starting at L_TFN:    time functions (d, l, grt...)
 *      - starting at L_MTFN:   time functions using multiple time args (? todo: check this)
 *      - starting at L_VAL:    numerical constants (pi, e, t...)
 *  
 *  Variables that can be changed 
 *  -----------------------------
 *      L_TABLE:        keywords recognised is a LEC expression. Each keyword is a pair (char*, int) containing
 *                      the text of the token and its unique numerical value (defined in iode.h) 
 *  
 *      L_PRIOR:        table of the 13 operator priorities, from L_OP to L_EXP (see iode.h)
 *
 *      L_MIN_FARGS:    table of minimum number of arguments for the functions, from L_FN to L_DIV0
 *      L_MAX_FARGS:    table of maximum number of arguments for these functions
 *      
 *      L_MIN_TARGS:   table of minimum number of arguments for the time functions, from L_TFN to L_LASTOBS
 *      L_MAX_TARGS:   table of minimum number of arguments for these functions
 *  
 *      L_MIN_MTARGS:   table of minimum number of arguments for the time functions, from L_MTFN to L_HPSTD
 *      L_MAX_MTARGS:   table of minimum number of arguments for these functions
 *  
 *      char*(*L_expand_super)(char*): alternative implementation of L_expand(): see k_lec.c
 *  
 *  Functions
 *  ---------
 *      int L_open_all(char* filename, int type)    Opens a file or a string for reading and assigns the open stream to L_YY.
 *      void L_close()                              Close the stream L_YY.
 *      static int L_lex()                          Reads the next keyword (from definitions in L_TABLE) on L_YY.
 *      static int L_read()                         Reads the next entry on L_YY but does not search the token in L_TABLE.     
 *      static void L_unread()                      Rewinds the stream L_YY on one token.
 *      static int L_macro()                        Reads the next token on L_YY after having read a $-sign. The next token is expected to be a macro name. 
 *      static int L_read_string()                  Reads characters on the L_YY stream until reaching a single quote (') char or EOF.
 *      static int L_string()                       Reads a string and expands its contents if * is part of the string. 
 *      static int L_getc()                         Reads the next char on L_YY.
 *      static void L_ungetc(int ch)                Records (pushes) a char on the stream L_YY. 
 *      static void L_skip()                        Skips the tokens on L_YY until having reached "end of expression" or end of file.
 *      int L_get_token()                           Main function to browse a LEC expression token by token in the stream L_YY. 
 *      static int L_get_int()                      Reads an integer or a PERIOD on the stream L_YY.
 *  
 */

#include "iode.h"

/* WARNING !!! : IF YOU MODIFY L_TABLE (below), DON'T FORGET TO CHANGE
		    L_PRIOR,
		    L_MAX_FARGS,  L_MIN_FARGS,
		    L_MAX_TARGS,  L_MIN_TARGS 
            L_MAX_MTARGS, L_MIN_MTARGS 
*/

// --- PRIORITY OF OPERATORS  and number of functions args. See iode.h FOR POSITIONS in the vectors ---
int L_PRIOR[13] = {2, 3, 4, 4, 4, 4, 4, 4, 5, 5, 6, 6, 7};
int L_MIN_FARGS[] = {1, 1, 1, 1, 1, 1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 2};
int L_MAX_FARGS[] = {1, 1, 2, 1, 1, 2, 255, 255, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 255, 255, 1, 255, 255, 1, 255, 1, 1, 1, 2, 1, 2, 1, 2};
int L_MAX_TARGS[]  = {2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 0, 3}; /* JMP 17-04-98 */
int L_MIN_TARGS[]  = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1}; /* JMP 17-04-98 */
int L_MAX_MTARGS[]  = {4, 4, 4, 3, 3, 4, 4, 1, 2, 4, 2, 4}; /* GB 14-11-00 */ // JMP 12/4/2019
int L_MIN_MTARGS[]  = {2, 2, 2, 1, 1, 2, 2, 1, 2, 2, 2, 2}; /* GB 14-11-00 */ // JMP 12/4/2019

// --- LEC tokens ---
YYKEYS L_TABLE[] = {
    "!",            L_NOT,
    "!=",           L_NE,
    "(",            L_OPENP,
    "()",           L_OCPAR,
    ")",            L_CLOSEP,
    "*",            L_TIMES,
    "**",           L_EXP,
    "+",            L_PLUS,
    "+ ",           L_UPLUS,
    ",",            L_COMMA,
    "/",            L_DIVIDE,
    ":",            L_COLON,
    ";",            L_EOE,
    /*    ";",            L_COMMA,*/
    "<",            L_LT,
    ">=",           L_GE,
    "<=",           L_LE,
    "<>",           L_NE,
    "=",            L_EQ,
    ">",            L_GT,
    "-",            L_MINUS,
    "- ",           L_UMINUS,
    "abs",          L_ABS,
    "acf",          L_ACF,
    "acos",         L_ACOS,
    "and",          L_AND,
    "app",          L_APP,
    "appdif",       L_DAPP,
    "asin",         L_ASIN,
    "atan",         L_ATAN,
    "ceil",         L_CEIL,  /* JMP 18-10-2004 */
    "cos",          L_COS,
    "cosh",         L_COSH,
    "corr",         L_CORR, /* JMP 17-04-98 */
    "covar",        L_COVAR, /* JMP 17-04-98 */
    "covar0",       L_COVAR0, /* JMP 17-04-98 */
    "d",            L_DIFF,
    "dapp",         L_DAPP,
    "dlag",         L_DLAG,
    "dln",          L_DLN,
    "e",            L_E,
    "euro",         L_EURO,
    "exp",          L_EXPN,
    "gamma",        L_GAMMA,
    "div0",         L_DIV0,
    "grandom",      L_GRANDOM,
    "grt",          L_GRT,
    "hp",           L_HP,
    "hpstd",        L_HPSTD, // JMP 12/4/2019
    "if",           L_IF,
    "index",        L_INDEX,
    "int",          L_INT,
    "interpol",     L_INTERPOL,
    "isan",         L_FNISAN,
    "floor",        L_FLOOR, /* JMP 18-10-2004 */
    "l",            L_LAG,
    "lastobs",      L_LASTOBS,
    "ln",           L_LN,
    "lcount",       L_LCOUNT,
    "lmean",        L_LMEAN,
    "log",          L_LOG,
    "lprod",        L_LPROD,
    "lstderr",      L_LSTDERR,
    "lsum",         L_LSUM,
    "ma",           L_MAVG,
    "mavg",         L_MAVG,
    "max",          L_MAX,
    "mean",         L_MEAN,
    "min",          L_MIN,
    "not",          L_NOT,
    "or",           L_OR,
    "pi",           L_PI,
    "prod",         L_PROD,
    "r",            L_RAPP,
    "rad",          L_RAD,
    "random",       L_RANDOM,
    "round",        L_ROUND, /* JMP 18-10-2004 */
    "sign",         L_SIGN,
    "sin",          L_SIN,
    "sinh",         L_SINH,
    "sqrt",         L_SQRT,
    "stddev",       L_STDDEV,
    "stderr",       L_STDERR,
    "sum",          L_SUM,
    "t",            L_TIME,
    "tan",          L_TAN,
    "tanh",         L_TANH,
    "urandom",      L_URANDOM,
    "var",          L_VARIANCE,
    "vmax",         L_VMAX,
    "vmin",         L_VMIN,
    "[",            L_OPENB,
    "]",            L_CLOSEB,
    "^",            L_EXP,
    "i",            L_I
};


// --- GLOBAL VARIABLES USED BY THE COMPILER ---

TOKEN   L_TOKEN;    // Global containing the last read token
YYFILE*  L_YY;      // LEC stream the compiler is reading from

// --- FUNCTIONS ---

/**
 *  Opens a file or a string for reading and assigns the open stream to L_YY. 
 *  
 *  On success, the global L_YY contains an open YY stream pointing to the LEC expression.
 *  
 *  @param [in] filename    char*   pointer to the filename or to the string to be opened
 *  @param [in] type        int     YY_MEM if the LEC expression is in the string pointed to by filename
 *                                  YY_FILE if filename is the name of the file containing the LEC expression (not used)
 *  @return                 int     O on success, -1 if the file cannot be open
 *  
 */
int L_open_all(char* filename, int type)
{
    static int  sorted = 0;

    YY_CASE_SENSITIVE = 1;
    if(sorted == 0) {
        qsort(L_TABLE, sizeof(L_TABLE) / sizeof(YYKEYS), sizeof(YYKEYS), YY_strcmp);
        sorted = 1;
    }

    L_YY = YY_open(filename, L_TABLE, sizeof(L_TABLE) / sizeof(YYKEYS), type);
    if(L_YY == 0) return(-1);
    return(0);
}


/**
 *  Close the stream L_YY.
 */
void L_close()
{
    if(L_YY == 0) return;
    YY_close(L_YY);
    L_YY = 0;
}


/**
 *  Reads the next keyword (from definitions in L_TABLE) on the open stream L_YY .
 *  
 *  @detail see http://xon.be/scr4/libs1/libs157.htm for more details on YY group of functions.
 *  
 *  @return     int     next token in the LEC expression: 
 *                          - YY_EOF if L_YY is NULL or if end of file|string is reached
 *                          - token identifier (L_TABLE[pos].yk_def) if the token is a LEC token 
 *                          - YYTEXT, YY_EOF, YY_LONG, YY_DOUBLE, YY_STRING, YY_SPECIAL, 
 *                            YY_LETTER, YY_MEMORY_FULL or YY_ERROR 
 *  
 */
static int L_lex()
{
    if(L_YY == 0) return(YY_EOF);
    return(YY_lex(L_YY));
}


/**
 *  Reads the next entry on the open stream L_YY but does not search the token in L_TABLE.
 *  
 *  See http://xon.be/scr4/libs1/libs161.htm for more details on YY_read() function.
 *  
 *  @return     int     next token in the LEC expression: 
 *                          - YY_EOF if L_YY is NULL or if end of file|string is reached
 *                          - YYTEXT, YY_EOF, YY_LONG, YY_DOUBLE, YY_STRING, YY_SPECIAL, 
 *                            YY_LETTER, YY_MEMORY_FULL or YY_ERROR 
 *  
 */
static int L_read()
{   
    if(L_YY == 0) return(YY_EOF);
    return(YY_read(L_YY));
}


/**
 *  Rewinds the stream, i.e. the next L_read() will give the same answer as the previous one.
 *  
 */
static void L_unread()
{
    if(L_YY) 
        YY_unread(L_YY);
}


/**
 *  Reads the next token on L_YY after having read a $-sign. The next token is expected to be a macro name. 
 *  The macro is searched for by the function L_expand().
 *    
 *  If the macro exists, the semi-colons in the macro text are replaced by commas. The resulting contents is finally 
 *  recorded (pushed) on the stream L_YY.
 *  
 *  On error, the global variable L_errno is set to L_MACRO_ERR and returned by the function.
 *  
 *  @return             int     0 if the macro exists 
 *                              L_MACRO_ERR on error: the next token was not a valid macro name, or the macro does not exist.
 *  
 */
static int L_macro()
{
    char    *ptr;

    if(L_read() != YY_WORD) return(L_errno = L_MACRO_ERR);
    ptr = L_expand(LYYTEXT);
    if(ptr == 0) return(L_errno = L_MACRO_ERR);
    /*    YY_record(L_YY, ")"); */  /* JMP 25-09-98 */
    SCR_replace(ptr, ";", ","); /* JMP 25-09-98 */
    YY_record(L_YY, ptr);
    /*    YY_record(L_YY, "("); */  /* JMP 25-09-98 */

    return(0);
}


/**
 *  Reads characters on the L_YY stream until reaching a single quote (') char or EOF.
 *  L_read_string() is called after a single quote is read, indicating the beginning of a string.
 *  The string is saved in L_YY (macro LYYTEXT).
 *    
 *  @return     int     number of read chars
 */
static int L_read_string()
{
    int     i = 0;
    int     ch = 0;

    while(1) {
        ch = YY_getc(L_YY);
        switch(ch) {
            case '\'' :
            case YY_EOF   :
                YY_add_to_text(L_YY, i, 0);
                YY_add_to_text(L_YY, i + 1, 0);
                L_YY->yy_lg = i;
                return(i);

            default    :
                break;
        }

        YY_add_to_text(L_YY, i++, ch);
    }
}


/** 
 * Reads a string and expands its contents if * is part of the string. 
 * Uses K_expand() to expand the string.
 *
 * Like in L_macro(), the semi-colons in the string are replaced by commas. 
 * The result is then recorded (pushed) on the stream L_YY.
 *  
 * @return     int     0 on success, L_MACRO_ERR if error in the expanding process.
 */
static int L_string()
{
    char    *ptr, *K_expand();

    L_read_string();
    ptr = K_expand(K_VAR, NULL, LYYTEXT, '*');
    if(ptr == 0) return(L_errno = L_MACRO_ERR);
    SCR_replace(ptr, ";", ",");
    YY_record(L_YY, ptr);
    SCR_free(ptr);
    return(0);
}


/**
 *  Reads the next char on L_YY.
 *  
 *  @return     int     YY_EOF if the end of string/file is reached or if L_YY is null
 *                      the next char otherwise
 */
static int L_getc()
{
    if(L_YY == 0) return(YY_EOF);
    return(YY_getc(L_YY));
}


/**
 *  Records (pushes) a char on the stream L_YY. The next call the L_getc() will return that character.
 *
 *  @param [in]     int     character to push on L_YY
 */
static void L_ungetc(int ch)
{
    if(L_YY == 0) return;
    YY_ungetc(ch, L_YY);
}


/**
 * Skips the tokens on L_YY until having reached "end of expression" (; by default) or end of file.
 */

static void L_skip()
{
    int     type;

    if(L_YY == 0) return;
    L_unread();
    do type = L_lex();
    while(type != L_EOE && type != YY_EOF);
}


/**
 * Main function to browse a LEC expression token by token in the stream L_YY. 
 *  - skips the spaces 
 *  - reads the next token and returns its group (L_OP, L_FN, L_TFN, L_MTFN, YY_OEF, YY_LONG...)
 *  - stores the keyword in L_TOKEN.tk_def (e.g.: L_LOG)
 *  - stores the real value, if any, in L_TOKEN.tk_real
 * 
 * @return     int     - group the token read belongs to (L_FN, L_OP...) or 
 *                            specific token type (L_VAR, L_DCONST, L_LCONST, L_PERIOD...)
 *                     - L_SYNTAX_ERR if the token is not valid.
 *                     - L_MACRO_ERR if the next token was a incorrect macro or if an expand cannot be done
 */ 
int L_get_token()
{
    int     keyw;
    int     ch;

    while(1) {
        ch = L_getc();
        if(!SCR_is_space(ch)) break; /* JMP 20-10-11 */
    }

    switch(ch) {
        case '$' :
            if(L_macro()) return(L_errno);
            break;

        case '\'' :
            if(L_string()) return(L_errno);
            break;

        default :
            L_ungetc(ch);
            break;
    }

again:
    keyw = L_lex();

    L_TOKEN.tk_def = keyw;
    if(is_op(keyw))   return(L_OP);
    if(is_fn(keyw))   return(L_FN);
    if(is_tfn(keyw))  return(L_TFN);
    if(is_val(keyw))  return(L_VAL);
    if(is_mtfn(keyw))  return(L_MTFN)/* JMP 20-04-98 */;
    if(keyw >= 0)     return(keyw);
    if(LYYTEXT) SCR_strlcpy(L_TOKEN.tk_name, LYYTEXT, L_MAX_NAME);
    switch(keyw) {
        case YY_EOF     :
            keyw = YY_EOF;
            break;
        case YY_WORD    :
            if(L_ISCOEF(LYYTEXT)) keyw = L_COEF;
            else keyw = L_VAR;
            break;
        case YY_LONG    :
            keyw = L_get_int();
            break;
        case YY_DOUBLE  :
            keyw = L_DCONST;
            L_TOKEN.tk_real = LYYDOUBLE;
            break;
        default :
            L_errno = L_SYNTAX_ERR;
            break;
    }

    L_TOKEN.tk_def = keyw;
    return(keyw);
}



/* ==+==== L_INCLUDE ========================
    Check if all elements of a are in b
    Return 1 if YES, 0 if NO
    ========================================= 
static int L_include(a, b)
char    *a, *b;
{
    int     i;

    for(; *a; a++) {
        for(i = 0 ; b[i] ; i++)
            if(*a == b[i]) break;
        if(b[i] == 0) return(0);
    }
    return(1);
}
*/

/** 
 * Reads an integer or a PERIOD on the stream L_YY.
 *
 * After having read a integer on L_YY (stored in LYYLONG), tries to read a PERIOD (yyyyPppp) on the stream L_YY.
 * If the next character is invalid for a PERIOD, unreads it and returns L_LCONST. Otherwise, reads the second part of the PERIOD and
 * return L_PERIOD. 
 *
 * @return     int     L_LCONST: value in L_TOKEN.tk_long
 *                     L_PERIOD: value in L_TOKEN.tk_period
 *                     YY_ERROR if the period in invalid (ex 2010Y3 pr 2021M0)
 *
 * On error L_errno is set to L_PERIOD_ERR.
 */   
static int L_get_int()
{
    int     nb_per,
            ch;
    long    l;

    l = LYYLONG;
    ch = L_getc();
    nb_per = L_pos(L_PERIOD_CH, toupper(ch));
    if(nb_per < 0) {
        L_ungetc(ch);
        L_TOKEN.tk_long = LYYLONG;
        return(L_LCONST);
    }

    if(L_read() != YY_LONG || L_PERIOD_NB[nb_per] < LYYLONG || LYYLONG == 0) {
        L_unread();
        L_errno = L_PERIOD_ERR;
        return(YY_ERROR);
    }

    if(l < 50) l+= 2000;
    else if(l < 200) l+= 1900;
    L_TOKEN.tk_period.p_y = l;
    L_TOKEN.tk_period.p_p = toupper(ch);
    L_TOKEN.tk_period.p_s = LYYLONG;

    return(L_PERIOD);
}


/*
    Reads a period from a opened file (yy)

static int L_get_period(YYFILE* yy, PERIOD* per)
{
    int     l, ch, nb_per;

    if(YY_lex(yy) != YY_LONG) {
        YY_unread(yy);
        return(-1);
    }
    l = yy->yy_long;
    if(l < 50) l+= 2000;
    else if(l < 200) l+= 1900;
    ch = YY_getc(yy);
    nb_per = L_pos(L_PERIOD_CH, toupper(ch));
    if(nb_per < 0) {
        YY_ungetc(ch, yy);
        YY_unread(yy);
        return(-1);
    }
    if(YY_lex(yy) != YY_LONG || L_PERIOD_NB[nb_per] < yy->yy_long) {
        YY_unread(yy);
        return(-1);
    }
    per->p_y = l;
    per->p_p = toupper(ch);
    per->p_s = yy->yy_long;

    return(0);
}
*/