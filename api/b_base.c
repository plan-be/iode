#include "iode.h"

char    B_SEPS[] = " ,\n\t";
int     B_NBDEC = 2;
int     B_LANG = 0;             /* JMP38 26-09-92 */
int     B_MULTIBAR = 0;         /* GB 30/10/2007 MULTIBAR function*/
int	B_GRAPHDEFAULT = 0;     /* GB 10/03/2011 T_auto function*/
// Correction dans les multibar secondaires ...

char **B_ainit_chk(arg, adef, nb)
char    *arg;
ADEF    *adef;
int     nb;
{
    char    **args;

    args = A_init(arg);
    if(args == 0) return(args);
    if((adef && A_check(args, adef)) ||
       (nb > 0 && SCR_tbl_size(args) != nb) ) {
	B_seterrn(OM_ILL_ARGS);
	A_free(args);
	args = 0;
	}

    return(args);
}

char **B_vtom_chk(arg, nb)
char    *arg;
int     nb;
{
    char    **args;

    args = SCR_vtoms(arg, B_SEPS);
    if(args == 0) return(args);
    if((nb > 0 && SCR_tbl_size(args) != nb)) {
	B_seterrn(OM_ILL_ARGS);
	SCR_free_tbl(args);
	args = 0;
    }

    return(args);
}

B_loop(argv, fn, client)
char    *argv[];
int     (*fn)();
char    *client;
{
    int     i, rc;

    for(i = 0 ; argv[i] ; i++) {
	if (client == NULL) rc = (*fn)(argv[i]);
	else rc = (*fn)(argv[i], client);
	if(rc) return(rc);
    }

    return(0);
}

B_ainit_loop(arg, fn, client)
char    *arg;
int     (*fn)();
char    *client;
{
    char    **argv;
    int     rc;

    if((argv = B_ainit_chk(arg, 0L, 0)) == 0) return(-1);
    rc = B_loop(argv, fn, client);
    A_free(argv);
    return(rc);
}

extern char    **B_ERROR_MSG;
extern int     B_ERROR_NB;

void B_display_last_error()
{
    char    *v = NULL, *buts[2];

    if(B_ERROR_NB == 0) return;
    SCR_add_ptr(&B_ERROR_MSG, &B_ERROR_NB, NULL);

    v = SCR_mtov(B_ERROR_MSG, '\n');
/*    SCR_strfacpy(&(vkp_error_ERROR), v);
    PG_edit_window(vkp_error);
*/
    buts[0] = "Ok";
    buts[1] = 0;
    SCR_MessageBox("E R R O R", v, buts);
    SCR_free(v);

    B_reset_error();
}

void B_print_last_error()
{
    int     i, nb;

    if(B_ERROR_NB == 0) return;
    SCR_add_ptr(&B_ERROR_MSG, &B_ERROR_NB, NULL);

    for(i = 0; i < B_ERROR_NB - 1; i++) W_printf("%s\n", B_ERROR_MSG[i]);

    B_reset_error();
}

void B_clear_last_error()
{
    //if(B_ERROR_NB == 0) return;
    //SCR_add_ptr(&B_ERROR_MSG, &B_ERROR_NB, NULL);
    B_reset_error();
}

void B_reset_error()
{
    int i;
    
    if(B_ERROR_NB == 0) return;
    SCR_add_ptr(&B_ERROR_MSG, &B_ERROR_NB, NULL);
    SCR_free_tbl(B_ERROR_MSG);
    B_ERROR_MSG = 0;
    B_ERROR_NB = 0;
}

B_get_arg0(arg0, arg, lg)
char    *arg0, *arg;
int     lg;
{
    int     i;

    SCR_replace(arg, "\t", " ");
    U_ljust_text(arg);
    for(i = 0; i < lg - 1 && arg[i] ; i++) {
	if(U_is_in(arg[i], B_SEPS)) break;
	arg0[i] = arg[i];
    }
    arg0[i] = 0;
    return(i);
}
/*
char *B_msg(n)
int     n;
{
    static FILE *fd_msg = 0;
    char        *pt, *SCR_search_file();

    if(fd_msg == 0) {
	pt = SCR_search_file("iode.msg");
	if(pt) fd_msg = fopen(pt, "r");
	}
    return(SCR_get_err_txt(fd_msg, n));
}
/* JMP_M 4.21 01-09-95 */

char *B_msg(n)
int     n;
{
    return(SCR_err_txt(n + 1000)); /* JMP_M 4.21 01-09-95 */
}

B_argpos(str, ch)
char    *str;
int     ch;
{
    int     pos;

    ch = SCR_upper_char(ch);
    pos = L_pos(str, ch);
    pos = max(0, pos);
    return(pos);
}

/* retourne -100 si l'argument est faux, sa valeur num‚rique sinon */

B_get1int(arg)
char    *arg;
{
    char    **args;
    int     n;

    args = B_ainit_chk(arg, 0L, 1);
    if(args == 0) return(-100);
    n = atoi(args[0]);
    SCR_free_tbl(args);
    return(n);
}

/* Déplacé de sb_global.c */ 
B_ScrollSet(arg, plong, inf, sup)
char    *arg;
long    *plong;
int     inf, sup;
{
    int     n;

    n = B_get1int(arg);
    if(n <= -100) return(-1);
    n = max(inf, n);
    n = min(sup, n);
    *plong = n;
    return(0);
}



