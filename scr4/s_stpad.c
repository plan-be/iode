#include "s_strs.h"
/* ====================================================================
Prolonge un string avec des blancs de telle sorte que la longueur du
string devienne lg. Les blancs commmencent … partir du premier caractŠre
plus petit que ' ' rencontr‚.

Place un '\0' … la fin du string (str[lg] = '\0').

Si la longueur de str est sup‚rieure … lg, str est tronqu‚.

&RT le string transform‚
&EX
    char    buf[20];

    strcpy(buf, "123456789");
    printf("'%s'\n", buf);
    SCR_pad(buf, 10);
    printf("'%s'\n", buf);

    donne

    '123456789'
    '123456789 '
&TX
=======================================================================*/

unsigned char *SCR_pad(str, lg)
unsigned char *str;
int     lg;
{
    int     i;

    for(i = (int)strlen(str) ; i < lg ; i++)
	str[i] = ' ';
    str[lg] = 0;
    return(str);
}
