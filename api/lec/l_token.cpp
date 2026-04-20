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
 *      static int L_get_int()                      Reads an integer or a Period on the stream L_YY.
 *  
 */
#include "api/objs/grep.h"
#include "api/lec/lec.h"

/* WARNING !!! : IF YOU MODIFY L_TABLE (below), DON'T FORGET TO CHANGE
		    L_PRIOR,
		    L_MAX_FARGS,  L_MIN_FARGS,
		    L_MAX_TARGS,  L_MIN_TARGS 
            L_MAX_MTARGS, L_MIN_MTARGS 
*/


// --- FUNCTIONS ---

/**
 * @brief Get total number of registred tokens.
 * 
 * @return int 
 */
int L_nb_tokens()
{
    return (int) (sizeof(L_TABLE) / sizeof(YYKEYS));
}


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
    static int sorted = 0;

    YY_CASE_SENSITIVE = 1;
    if(sorted == 0) 
    {
        qsort(L_TABLE, sizeof(L_TABLE) / sizeof(YYKEYS), sizeof(YYKEYS), YY_compare);
        sorted = 1;
    }

    L_YY = YY_open(filename, L_TABLE, sizeof(L_TABLE) / sizeof(YYKEYS), type);
    if(L_YY == 0) 
        return -1;
    return 0;
}


/**
 *  Close the stream L_YY.
 */
void L_close()
{
    if(L_YY == 0) 
        return;
    
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
    if(L_YY == 0) 
        return YY_EOF;
    
    int res = YY_lex(L_YY);
    return res;
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
    if(L_YY == 0) 
        return YY_EOF;
    
    int res = YY_read(L_YY);
    return res;
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
 *  TODO: BUG: the replacement of ; by , acts on the list itself which is an error! Must be changed in a tmp buffer 
 *             before recording
 */
static int L_macro()
{
    char* ptr;

    if(L_read() != YY_WORD)
    {
        L_errno = L_MACRO_ERR;
        return L_errno;
    }

    ptr = L_expand((char*) LYYTEXT);
    if(!ptr) 
        return(L_errno = L_MACRO_ERR);
    
    SCR_replace((unsigned char*) ptr, (unsigned char*) ";", (unsigned char*) ",");
    YY_record(L_YY, (unsigned char*) ptr);

    return 0;
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
    int i = 0;
    int ch = 0;

    while(1) 
    {
        ch = YY_getc(L_YY);
        switch(ch) 
        {
            case '\'' :
            case YY_EOF :
                YY_add_to_text(L_YY, i, 0);
                YY_add_to_text(L_YY, i + 1, 0);
                L_YY->yy_lg = i;
                return i;
            default :
                break;
        }

        YY_add_to_text(L_YY, i++, ch);
    }
}

/**
 *  Reads the next char on L_YY.
 *  
 *  @return     int     YY_EOF if the end of string/file is reached or if L_YY is null
 *                      the next char otherwise
 */
static int L_getc()
{
    if(L_YY == 0) 
        return YY_EOF;
    
    int ch = YY_getc(L_YY);
    return ch;
}


/**
 *  Records (pushes) a char on the stream L_YY. The next call the L_getc() will return that character.
 *
 *  @param [in]     int     character to push on L_YY
 */
static void L_ungetc(int ch)
{
    if(L_YY == 0) 
        return;
    
    YY_ungetc(ch, L_YY);
}


/** 
 * Reads an integer or a Period on the stream L_YY.
 *
 * After having read a integer on L_YY (stored in LYYLONG), tries to read a Period (yyyyPppp) on the stream L_YY.
 * If the next character is invalid for a Period, unreads it and returns L_LCONST. Otherwise, reads the second part of the Period and
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
    long l = LYYLONG;
    int ch = L_getc();
    int nb_per = get_pos_in_char_array((char*) periodicities.c_str(), toupper(ch));
    if(nb_per < 0) 
    {
        L_ungetc(ch);
        L_TOKEN.tk_long = LYYLONG;
        return L_LCONST;
    }

    if(L_read() != YY_LONG || PERIOD_NB[nb_per] < LYYLONG || LYYLONG == 0) 
    {
        L_unread();
        L_errno = L_PERIOD_ERR;
        return YY_ERROR;
    }

    if(l < 50) 
        l+= 2000;
    else if(l < 200) 
        l+= 1900;

    L_TOKEN.tk_period.year = l;
    L_TOKEN.tk_period.periodicity = toupper(ch);
    L_TOKEN.tk_period.step = LYYLONG;

    return L_PERIOD;
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
    L_read_string();

    char* ptr = K_expand(VARIABLES, NULL, (char*) LYYTEXT, '*');
    if(!ptr)
    {
        L_errno = L_MACRO_ERR;
        return L_errno;
    }

    SCR_replace((unsigned char*) ptr, (unsigned char*) ";", (unsigned char*) ",");
    YY_record(L_YY, (unsigned char*) ptr);
    SCR_free(ptr);
    return 0;
}



/**
 * Skips the tokens on L_YY until having reached "end of expression" (; by default) or end of file.
 */

static void L_skip()
{
    if(L_YY == 0) 
        return;
    
    L_unread();

    int type;
    do 
        type = L_lex();
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
    int type;
    int ch;

    while(1) 
    {
        ch = L_getc();
        if(!SCR_is_space(ch)) 
            break;
    }

    switch(ch) 
    {
        case '$' :
            if(L_macro()) 
                return L_errno;
            break;

        case '\'' :
            if(L_string()) 
                return L_errno;
            break;

        default :
            L_ungetc(ch);
            break;
    }

    type = L_lex();

    L_TOKEN.tk_def = type;

    if(is_op(type))   
        return L_OP;
    
    if(is_fn(type))   
        return L_FN;
    
    if(is_tfn(type))  
        return L_TFN;
    
    if(is_val(type))  
        return L_VAL;
    
    if(is_mtfn(type))  
        return L_MTFN;
    
    if(type >= 0)     
        return type;
    
    if(LYYTEXT) 
        SCR_strlcpy((unsigned char*) L_TOKEN.tk_name, LYYTEXT, L_MAX_NAME);
    
    switch(type) 
    {
        case YY_EOF :
            type = YY_EOF;
            break;
        case YY_WORD :
            if(is_coefficient((char*) LYYTEXT)) 
                type = L_COEF;
            else 
                type = L_VAR;
            break;
        case YY_LONG :
            type = L_get_int();
            break;
        case YY_DOUBLE :
            type = L_DCONST;
            L_TOKEN.tk_real = (float) LYYDOUBLE;
            break;
        default :
            L_errno = L_SYNTAX_ERR;
            break;
    }

    L_TOKEN.tk_def = type;
    return type;
}
