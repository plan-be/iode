#include "iode.h"

IODE_REAL K_read_real(yy)
YYFILE  *yy;
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

long K_read_long(yy)
YYFILE  *yy;
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

char    *K_read_str(yy)
YYFILE  *yy;
{
    char    *ptr = NULL;

    if(YY_lex(yy) != YY_STRING) return(NULL);
    ptr = SCR_stracpy(yy->yy_text);
    return(ptr);
}

PERIOD  *K_read_per(yy)
YYFILE  *yy;
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

SAMPLE  *K_read_smpl(yy)
YYFILE  *yy;
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


void K_stracpy(to, from)
char    **to;
char    *from;
{
    *to = SW_nalloc(strlen(from) + 1);
    strcpy(*to, from);
}


/* === K_wrdef ============================================
    Write a defined keyword search in table
	table   : IN :
	def     : IN : define corresponding
    ========================================================= */

K_wrdef(fd, table, def)
FILE    *fd;
YYKEYS  *table;
int     def;
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

K_compare(yy1, yy2)
YYKEYS  *yy1, *yy2;
{
    return(strcmp(yy1->yk_word, yy2->yk_word));
}

char    *K_wrap(in, lg)
char    *in;
int     lg;
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
