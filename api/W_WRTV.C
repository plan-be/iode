#include <scr.h>  /* JMP 19-02-98 */
#include <s_a2m.h>  /* JMP 19-02-98 */

extern A2MFILE  *W_af;

extern int W_cont, W_cancel;

W_printfEx(int dup, char *c1, char *c2, char *c3, char *c4, char *c5, char *c6, char *c7, char *c8, char *c9, char *c10, char *c11, char *c12)
{
    char    buf[10500];  /* JMP 16-01-02 */

    if(W_cancel) return(0);

    sprintf(buf, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12);
    if(W_cont == 0 && W_isempty(buf)) return(0);

    if(W_open()) return(-1);

    W_cont = 1;
    if(dup) SCR_replace(buf, "\\", "\\\\");
    A2mMemRecord(W_af, buf);
    return(0);
}

W_printf(char *c1, char *c2, char *c3, char *c4, char *c5, char *c6, char *c7, char *c8, char *c9, char *c10, char *c11, char *c12)
{
    return(W_printfEx(0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12));
}
