/**
 * @header4iode
 * 
 * Helper functions for reading and writing IODE ascii files.
 *
 *    IODE_REAL K_read_real(YYFILE *yy)
 *    long K_read_long(YYFILE* yy)
 *    char* K_read_str(YYFILE* yy)
 *    PERIOD *K_read_per(YYFILE* yy)
 *    SAMPLE *K_read_smpl(YYFILE* yy)
 *    int K_read_align(YYFILE* yy)
 *  
 *    void K_stracpy(char** to, char* from)
 *    int K_wrdef(FILE* fd, YYKEYS* table, int def)
 *    int K_compare(YYKEYS* yy1, YYKEYS* yy2)
 *    char *K_wrap(char *in, int lg)
 *
 */

#include "iode.h"

/**
 *  Reads a token on the YY stream and interprets the token as a IODE_REAL (double) if possible.
 *  If not, returns L_NAN and rewinds the YY stream.
 *  
 *  The token can be a double, a long or the string "na". Other values are rejected.
 *  
 *  @param [in, out]    yy  YYFILE*     YY stream
 *  @return                 IODE_REAL   value of the next token or L_NAN.
 *  
 */
IODE_REAL K_read_real(YYFILE *yy)
{
    IODE_REAL    val;
    int     minus = 1;

ag:
    switch(YY_lex(yy)) {
        case YY_DOUBLE :
            val = minus * yy->yy_double;
            break;
        case YY_LONG   :
            val = minus * yy->yy_long;
            break;
        case KT_BREAK :
        case YY_SPECIAL:
            if(yy->yy_text[0] == '-') {
                minus = -1;
                goto ag;
            }
            //  YY_unread(yy); // Annul‚ version 6.21

            if(yy->yy_text[0] != '/' &&
                    yy->yy_text[0] != '.')  /* JMP 03-01-2013 */
                YY_unread(yy);             /* GB / = na  ??? */
            return(L_NAN);

        case YY_WORD   :
            if(strcmp("na", yy->yy_text) != 0)
                YY_unread(yy);
            return(L_NAN);
        default :
            YY_unread(yy);
            return(L_NAN);
    }

    return(val);
}

/**
 *  Reads the next token on the YY stream and returns a long. If the token is not a long, returns 0 and rewinds the stream.
 *  
 *  @param [in, out]    yy  YYFILE*     YY stream
 *  @return                 long        value of the next token or 0.
 *  
 */
long K_read_long(YYFILE* yy)
{
    long    val;
    int     minus = 1;

ag:
    switch(YY_lex(yy)) {
        case YY_LONG   :
            val = minus * yy->yy_long;
            break;
        case YY_SPECIAL:
            if(yy->yy_text[0] == '-') {
                minus = -1;
                goto ag;
            }
            else if(yy->yy_text[0] == '+') { /* JMP 16-05-00 */
                minus = 1;
                goto ag;
            }
            YY_unread(yy);
            return(0);

        default :
            YY_unread(yy);
            return(0);
    }

    return(val);
}

/**
 *  Reads the next token on the YY stream. If it is a string, returns an allocated copy of the string. If not, returns NULL.
 *  
 *  @param [in, out]    yy  YYFILE*     YY stream
 *  @return                 char*       allocated copy of the string or NULL if the read token is not a string.
 *  
 */
char* K_read_str(YYFILE* yy)
{
    char    *ptr = NULL;

    if(YY_lex(yy) != YY_STRING) return(NULL);
    ptr = SCR_stracpy(yy->yy_text);
    return(ptr);
}

/**
 *  Reads the next tokens on the YY stream and tries to interpret them as a PERIOD definition (<long><char><long>).
 *  If it is not possible, returns NULL.
 *  
 *  @param [in, out]    yy  YYFILE*     YY stream
 *  @return                 PERIOD*     NULL or pointer to an allocated PERIOD 
 *  
 */

PERIOD  *K_read_per(YYFILE* yy)
{
    char    buf[30], buf1[2];
    PERIOD  *tmp = NULL;

    buf[0] = 0;
    if(YY_lex(yy) != YY_LONG) goto err;
    strcpy(buf, yy->yy_text);
    buf1[0] = YY_getc(yy);
    buf1[1] = 0;
    strcat(buf, buf1);
    if(YY_lex(yy) != YY_LONG) goto err;
    strcat(buf, yy->yy_text);

    tmp = PER_atoper(buf);
    return(tmp);

err:
    return(NULL);
}


/**
 *  Reads the next tokens on the YY stream and tries to interpret them as a SAMPLE (2 x PERIOD) definition (<long><char><long> <long><char><long>).
 *  If it is not possible, returns NULL.
 *  
 *  @param [in, out]    yy  YYFILE*     YY stream
 *  @return                 PERIOD*     NULL or pointer to an allocated PERIOD 
 *  
 *  TODO: improve (correct) if the syntax is incorrect on YY stream.
 */

SAMPLE  *K_read_smpl(YYFILE* yy)
{
    int     nb;
    SAMPLE  *smpl = NULL;
    PERIOD  *one, *two;

    one = K_read_per(yy);
    two = K_read_per(yy);
    if(one == 0 || two == 0 || (nb = PER_diff_per(two, one)) < 0)
        kerror(0, "%s incorrect period", YY_error(yy));
    else {
        smpl = (SAMPLE *) SW_nalloc(sizeof(SAMPLE));
        memcpy(&(smpl->s_p1), one, sizeof(PERIOD));
        memcpy(&(smpl->s_p2), two, sizeof(PERIOD));
        smpl->s_nb = nb + 1;
    }

    SW_nfree(one);
    SW_nfree(two);
    return(smpl);
}


/**
 *  Reads the next token on the YY stream: LEFT, RIGHT or CENTER.
 *  If the token is not one of them, returns LEFT and rewinds yy.
 *  
 *  @param [in, out]    yy  YYFILE*     YY stream
 *  @return                 int         1 for CENTER, 2 for RIGHT, 0 otherwise
 *  
 */
int K_read_align(YYFILE* yy)
{
    int keyw;

    keyw = YY_lex(yy);
    switch(keyw) {
        case KT_LEFT :
            return(0);
        case KT_CENTER:
            return(1);
        case KT_RIGHT :
            return(2);
        default       :
            YY_unread(yy);
            return(0);
    }
}


/**
 *  Alloc and copy a string. 
 *  
 *  @param [in, out] to   char**     placeholder for the new allocated pointer to from
 *  @param [in]      from char*      pointer to the string to cbe copied (cannot be NULL)
 *  @return          void       
 *  
 *  TODO: replace by SCR_stracpy ?
 */

void K_stracpy(char** to, char* from)
{
    *to = SW_nalloc(strlen(from) + 1);
    strcpy(*to, from);
}


/**
 *  Searches the position of a integer in a table of YYKEYS and writes the corresponding token onto fd.
 *
 *  @see http://www.xon.be/scr4/libs1/libs157.htm for more information on YYKEYS.
 *  
 *  @param [in] fd      FILE*     file pointer
 *  @param [in] table   YYKEYS*   pointer to a table of YYKEYS
 *  @param [in] def     int       numerical value of a token   
 *  @return             int       0
 */

int K_wrdef(FILE* fd, YYKEYS* table, int def)
{
    YYKEYS  *yk;

    for(yk = table ; yk->yk_word != 0 ; yk++)
        if(yk->yk_def == def) {
            fprintf(fd, "%s ", yk->yk_word);
            return(0);
        }
    fprintf(fd, "/* UNDEFINED */ ");
    return(0);
}

/**
 *  Helper function passed as parameter to qsort to sort a table of YYKEYS.
 *  
 *  @param [in] yy1     YYKEYS* pointer to a YYKEYS struct
 *  @param [in] yy2     YYKEYS* pointer to a YYKEYS struct
 *  @return             int     -1 if yy1 < yy2, 0 if yy1 == yy2, 1 if yy1 > yy2
 *  
 */
int K_compare(YYKEYS* yy1, YYKEYS* yy2)
{
    return(strcmp(yy1->yk_word, yy2->yk_word));
}

/**
 *  Wraps a string (by inserting \n) to limit each line to lg characters. Splits on spaces or one of ";,*+-/)". 
 *  If the string contains a \n, doesn't modify the string (but allocates a copy).
 *  
 *  @param [in] in char*    input string.
 *  @param [in] lg int      max length of a line after  transformation.
 *  @return        char*    allocated string of max lg chars between \n.
 *  
 */
char *K_wrap(char *in, int lg)
{
    unsigned char  *txt = NULL,
                    **tbl = NULL;

    if(in == 0) return(txt);
    if(U_is_in('\n', in))  return(SCR_stracpy(in));

    tbl = SCR_text(in, ";,*+-/) ", lg); /* JMP 09-03-95 */
    if(tbl == 0) return(txt);
    txt = SCR_mtov(tbl, '\n');
    SCR_free_tbl(tbl);
    return(txt);
}
