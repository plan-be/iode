#include <stdlib.h> /* JMP 06-01-98 */
#include <string.h> /* JMP 06-01-98 */
#ifdef UNIX
#include <locale.h> /* JMP 19-02-03 */
#endif

/* ===========================================================
    Modifie sur place un texte contenant un float formatté selon les
    définitions locales des décimales. Nécessaire en Unix/Fr. En effet, la
    fonction atof("1.12") retourne 1.0 en version française. A noter que,
    dans ce cas, sprintf(toto, "%lf", 20.5) donne "20,5"

    Ne fait rien en Windows ou DOS.
========================================================== */

SCR_localisestring(char *txt)
{
#ifdef UNIX
    int     i, sep = '.', csep = '.';
    struct lconv *loc = localeconv();

    csep = loc->decimal_point[0];
    if(csep == '.') sep = ',';
    else            sep = '.';

    for(i = 0 ; txt[i] ; i++) if(txt[i] == sep) txt[i] = csep;
#endif
    return(0);
}

/* ===========================================================
    Pour des raisons de portabilités avec UnixWare
    il faut controler si dans le texte passer à atof il n'y à pas une
    virgule car avec unixware et Linux en Fr
	sprintf(toto, "%lf", 20.5)
	donne
	20,5
    ========================================================== */

double SCR_atof(txt)        /* BP_M 24-07-95 */
char    *txt;
{
    char    buf[255];

    strcpy(buf, txt);
    SCR_localisestring(buf);
    return(atof(buf));
}





