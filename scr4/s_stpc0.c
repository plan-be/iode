#include "s_strs.h"
#include <stdio.h>

FILE    *PC_FD;
char    PC_SEPS[] = ":";
char    PC_BEG[] = "+";

/*NH*/
PC_open(filename)
char    *filename;
{
    char    *pt;
/*    char    *SCR_search_file(); /* JMP 04-01-98 */

    PC_FD = 0;
    pt = SCR_search_file(filename);
    if(pt == 0) return(-1);
    PC_FD = fopen(pt, "r");
    if(PC_FD == 0) return(-1);
    return(0);
}

/*NH
Lit la ligne suivante et retourne :
    -1 : fin de fichier
    0  : correspond au prname
    1  : ligne de définition != prname
    2  : ligne de commande
*/

PC_getline(prname, buf)
char    *prname, *buf;
{
    int     beglg = strlen(PC_BEG);

    if(SCR_read_line(PC_FD, buf, 130) < 0) return(-1);
    if(memcmp(buf, PC_BEG, beglg)) return(2);
    if(memcmp(buf + beglg, prname, strlen(prname))) return(1);
    return(0);
}

