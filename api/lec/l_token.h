#pragma once

#include "scr4/s_yy.h"          // YYKEYS

#include "api/lec/l_common.h"
#include "api/lec/l_exec_fns.h"
#include "api/lec/l_exec_ops.h"
#include "api/lec/l_exec_tfn.h"
#include "api/lec/l_exec_val.h"
#include "api/lec/l_exec_mtfn.h"


#define L_MAX_NAME  K_MAX_NAME

struct TOKEN 
{
    int     type;
    float   real_value;
    long    long_value;
    Period  period;
    char    name[L_MAX_NAME + 1];
};

// --- LEC tokens ---
// TODO: update YYFILE and the related functions to use std::map<std::string, int> instead of YYKEYS C array
inline YYKEYS L_TABLE[] = 
{
    (unsigned char*) "!",            L_NOT,
    (unsigned char*) "!=",           L_NE,
    (unsigned char*) "(",            L_OPENP,
    (unsigned char*) "()",           L_OCPAR,
    (unsigned char*) ")",            L_CLOSEP,
    (unsigned char*) "*",            L_TIMES,
    (unsigned char*) "**",           L_EXP,
    (unsigned char*) "+",            L_PLUS,
    (unsigned char*) "+ ",           L_UPLUS,
    (unsigned char*) ",",            L_COMMA,
    (unsigned char*) "/",            L_DIVIDE,
    (unsigned char*) ":",            L_COLON,
    (unsigned char*) ";",            L_EOE,
    (unsigned char*) "<",            L_LT,
    (unsigned char*) ">=",           L_GE,
    (unsigned char*) "<=",           L_LE,
    (unsigned char*) "<>",           L_NE,
    (unsigned char*) "=",            L_EQ,
    (unsigned char*) ">",            L_GT,
    (unsigned char*) "-",            L_MINUS,
    (unsigned char*) "- ",           L_UMINUS,
    (unsigned char*) "abs",          L_ABS,
    (unsigned char*) "acf",          L_ACF,
    (unsigned char*) "acos",         L_ACOS,
    (unsigned char*) "and",          L_AND,
    (unsigned char*) "app",          L_APP,
    (unsigned char*) "appdif",       L_DAPP,
    (unsigned char*) "asin",         L_ASIN,
    (unsigned char*) "atan",         L_ATAN,
    (unsigned char*) "ceil",         L_CEIL,
    (unsigned char*) "cos",          L_COS,
    (unsigned char*) "cosh",         L_COSH,
    (unsigned char*) "corr",         L_CORR,
    (unsigned char*) "covar",        L_COVAR,
    (unsigned char*) "covar0",       L_COVAR0,
    (unsigned char*) "d",            L_DIFF,
    (unsigned char*) "dapp",         L_DAPP,
    (unsigned char*) "dlag",         L_DLAG,
    (unsigned char*) "dln",          L_DLN,
    (unsigned char*) "e",            L_E,
    (unsigned char*) "euro",         L_EURO,
    (unsigned char*) "exp",          L_EXPN,
    (unsigned char*) "gamma",        L_GAMMA,
    (unsigned char*) "div0",         L_DIV0,
    (unsigned char*) "grandom",      L_GRANDOM,
    (unsigned char*) "grt",          L_GRT,
    (unsigned char*) "hp",           L_HP,
    (unsigned char*) "hpstd",        L_HPSTD,
    (unsigned char*) "if",           L_IF,
    (unsigned char*) "index",        L_INDEX,
    (unsigned char*) "int",          L_INT,
    (unsigned char*) "interpol",     L_INTERPOL,
    (unsigned char*) "isan",         L_FNISAN,
    (unsigned char*) "floor",        L_FLOOR,
    (unsigned char*) "l",            L_LAG,
    (unsigned char*) "lastobs",      L_LASTOBS,
    (unsigned char*) "ln",           L_LN,
    (unsigned char*) "lcount",       L_LCOUNT,
    (unsigned char*) "lmean",        L_LMEAN,
    (unsigned char*) "log",          L_LOG,
    (unsigned char*) "lprod",        L_LPROD,
    (unsigned char*) "lstderr",      L_LSTDERR,
    (unsigned char*) "lsum",         L_LSUM,
    (unsigned char*) "ma",           L_MAVG,
    (unsigned char*) "mavg",         L_MAVG,
    (unsigned char*) "max",          L_MAX,
    (unsigned char*) "mean",         L_MEAN,
    (unsigned char*) "min",          L_MIN,
    (unsigned char*) "not",          L_NOT,
    (unsigned char*) "or",           L_OR,
    (unsigned char*) "pi",           L_PI,
    (unsigned char*) "prod",         L_PROD,
    (unsigned char*) "r",            L_RAPP,
    (unsigned char*) "rad",          L_RAD,
    (unsigned char*) "random",       L_RANDOM,
    (unsigned char*) "round",        L_ROUND,
    (unsigned char*) "sign",         L_SIGN,
    (unsigned char*) "sin",          L_SIN,
    (unsigned char*) "sinh",         L_SINH,
    (unsigned char*) "sqrt",         L_SQRT,
    (unsigned char*) "stddev",       L_STDDEV,
    (unsigned char*) "stderr",       L_STDERR,
    (unsigned char*) "sum",          L_SUM,
    (unsigned char*) "t",            L_TIME,
    (unsigned char*) "tan",          L_TAN,
    (unsigned char*) "tanh",         L_TANH,
    (unsigned char*) "urandom",      L_URANDOM,
    (unsigned char*) "var",          L_VARIANCE,
    (unsigned char*) "vmax",         L_VMAX,
    (unsigned char*) "vmin",         L_VMIN,
    (unsigned char*) "[",            L_OPENB,
    (unsigned char*) "]",            L_CLOSEB,
    (unsigned char*) "^",            L_EXP,
    (unsigned char*) "i",            L_I
};

/**
 * @brief Get total number of registered tokens.
 * 
 * @return int 
 */
inline int nb_tokens()
{
    return (int) (sizeof(L_TABLE) / sizeof(YYKEYS));
}

/*---------------- CLASS LECPARSER -------------------*/

struct LecParser
{
private:
    // Static helper function for sorting
    static int yy_compare(const void *a, const void *b)
    {
        return YY_strcmp((const char*) a, (const char*) b);
    }

    // Private helper methods
    int lex();
    int read();
    void unread();
    int macro();
    int read_string();
    int getc();
    void ungetc(int ch);
    int get_int(TOKEN& token);
    int string();
    void skip();

public:
    /**
     * Opens a file or a string for reading and assigns the open stream to L_YY.
     * 
     * @param [in] filename  char*   pointer to the filename or to the string to be opened
     * @param [in] type      int     YY_MEM if the LEC expression is in the string pointed to by filename
     *                               YY_FILE if filename is the name of the file containing the LEC expression
     * @return               int     0 on success, -1 if the file cannot be opened
     */
    int open_all(char* filename, int type);

    /**
     * Close the current stream.
     */
    void close();

    /**
     * Main function to browse a LEC expression token by token.
     * 
     * @param[in, out]  group   - group the token read belongs to (L_FN, L_OP...) or specific token type
     *                          - L_SYNTAX_ERR if the token is not valid
     *                          - L_MACRO_ERR if the next token was an incorrect macro
     * @return          TOKEN   the token read from the stream
     */
    TOKEN read_next_token(int& group);

    /**
     * Convenience method to open a file for reading.
     * @param [in] filename  char*  name of the file to open
     * @return               int    0 on success, -1 on error
     */
    int open_file(char* filename)
    {
        return open_all(filename, YY_FILE);
    }

    /**
     * Convenience method to open a string for reading.
     * @param [in] text  char*  string to parse
     * @return           int    0 on success, -1 on error
     */
    int open_string(char* text)
    {
        return open_all(text, YY_MEM);
    }

    /**
     * Convenience method to open stdin for reading.
     * @return  int  0 on success, -1 on error
     */
    int open_stdin()
    {
        return open_all((char*) 0, YY_STDIN);
    }
};
