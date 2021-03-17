#include "iode.h"

/* WARNING !!! : IF YOU MODIFY L_TABLE, DON'T FORGET TO CHANGE
		    L_PRIOR,
		    L_MAX_FARGS, L_MIN_FARGS,
		    L_MAX_TARGS, L_MIN_TARGS */

/* PRIORITY OF OPERATORS  and functions args : SEE LEC.H FOR POSITIONS */
int L_PRIOR[13] = {2, 3, 4, 4, 4, 4, 4, 4, 5, 5, 6, 6, 7};
int L_MIN_FARGS[] = {1, 1, 1, 1, 1, 1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 2};
int L_MAX_FARGS[] = {1, 1, 2, 1, 1, 2, 255, 255, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 255, 255, 1, 255, 255, 1, 255, 1, 1, 1, 2, 1, 2, 1, 2};
int L_MAX_TARGS[]  = {2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 0, 3}; /* JMP 17-04-98 */
int L_MIN_TARGS[]  = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1}; /* JMP 17-04-98 */
int L_MAX_MTARGS[]  = {4, 4, 4, 3, 3, 4, 4, 1, 2, 4, 2, 4}; /* GB 14-11-00 */ // JMP 12/4/2019 
int L_MIN_MTARGS[]  = {2, 2, 2, 1, 1, 2, 2, 1, 2, 2, 2, 2}; /* GB 14-11-00 */ // JMP 12/4/2019 


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

TOKEN   L_TOKEN;

/*NH*/
/*
    Opens file or string for reading by L_get_token
    return YYFILE * in L_YY (global)
*/


L_open_all(filename, type)
char    *filename;
int     type;
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

/*NH*/
/*
    Close the stream L_YY.
*/

void L_close()
{
    if(L_YY == 0) return;
    YY_close(L_YY);
    L_YY = 0;
}

/*NH*/
/* === L_LEX =========================================
    Get next keyw in L_YY
   =================================================== */

L_lex()
{
    if(L_YY == 0) return(YY_EOF);
    return(YY_lex(L_YY));
}
/*NH*/
/* === L_READ =========================================
    Get next entry in L_YY
   =================================================== */

L_read()
{
    if(L_YY == 0) return(YY_EOF);
    return(YY_read(L_YY));
}
/*NH*/
/* === L_UNREAD =========================================
    UnGet next entry in L_YY
   =================================================== */

L_unread()
{
    if(L_YY == 0) return(YY_EOF);
    return(YY_unread(L_YY));
}
/*NH*/
/* === L_MACRO =========================================
    Reads a macro
   =================================================== */

L_macro()
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
/*NH*/
/* === L_STRING =========================================
    Reads a string and expand
   =================================================== */

L_string()
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

L_read_string()
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

/*NH*/
/* === L_getc =========================================
    Get next char in L_YY
   =================================================== */

L_getc()
{
    if(L_YY == 0) return(YY_EOF);
    return(YY_getc(L_YY));
}
/*NH*/
/* === L_UNgetc =========================================
    UnGet next char  in L_YY
   =================================================== */

void L_ungetc(ch)
{
    if(L_YY == 0) return;
    YY_ungetc(ch, L_YY);
}

/*NH*/
/* === L_SKIP =========================================
    Skip to end of expression or end of file
   =================================================== */

void L_skip()
{
    int     type;

    if(L_YY == 0) return;
    L_unread();
    do type = L_lex();
    while(type != L_EOE && type != YY_EOF);
}

/*NH*/
/* === L_GET_TOKEN ==============================
    Read the next token on the stream and
    return his type. Complete information
    types : L_OPENP, L_CLOSEP, L_EOE, L_OP, L_FN
	    L_VAR, L_COEF, ...
    saved in L_TOKEN
   ============================================== */

L_get_token()
{
    int     keyw;
    int     ch;

    while(1) {
        ch = L_getc();
        /*        if(!isspace(ch) && ch != '"') break; */
//        if(!isspace(ch)) break; /* JMP 20-10-11 */
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


/*NH*/
/* ==+==== L_INCLUDE ========================
    Check if all elements of a are in b
    Return 1 if YES, 0 if NO
    ========================================= */
L_include(a, b)
char    *a, *b;
{
    int     i;

    for( ; *a; a++) {
        for(i = 0 ; b[i] ; i++)
            if(*a == b[i]) break;
        if(b[i] == 0) return(0);
    }
    return(1);
}

/*NH*/
/* === L_GET_INT ================================
    Read an int or a period on the stream
   ============================================== */

L_get_int()
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

/*NH*/
/* === WR_define ============================================
    Write a defined keyword search in L_TABLE
	def : IN : define corresponding
    ========================================================= */

#if 0

WR_define(def) // clashes with SCR
int     def;
{
    YYKEYS  *yk;

    for(yk = L_TABLE ; yk->yk_word != 0 ; yk++)
        if(yk->yk_def == def) {
            printf("%s ", yk->yk_word);
            return;
        }
    printf("/* UNDEFINED */ ");
}
#endif

/*
    Reads a period from a opened file (yy)
*/
L_get_period(yy, per)
YYFILE  *yy;
PERIOD  *per;
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
