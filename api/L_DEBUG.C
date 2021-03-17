#include <stdio.h>

L_debug(txt, a1, a2, a3, a4, a5, a6)
char    *txt, *a1, *a2, a3, *a4, *a5, *a6;
{
    FILE *fd = 0;

    fd = fopen("simul.dbg", "a+");
    if(fd) fprintf(fd, txt, a1, a2, a3, a4, a5, a6);
    fclose(fd);
}

