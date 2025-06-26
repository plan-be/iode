#include "s_strs.h"
/* ====================================================================
Prolonge un string avec des blancs de telle sorte que la longueur du
string devienne lg. Les blancs commmencent à partir du premier caractère
plus petit que ' ' rencontré.

Place un '\0' à la fin du string (str[lg] = '\0').

Si la longueur de str est supérieure à lg, str est tronqué.

&RT le string transformé
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
