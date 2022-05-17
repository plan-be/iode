#include <stdio.h>
#include <string.h>

/* ====================================================================
Cette fonction est … exploiter en connexion avec le programme scr4_app.
Elle positionne le pointeur d‚crit par le FILE descriptor fd au d‚but
des donn‚es concat‚n‚es en fin de fichier.

fd d‚crit le fichier (qui doit ˆtre ouvert en lecture) et n est le
nombre de fichiers concat‚n‚s qui doivent ˆtre saut‚s.

Ainsi, si on a ajout‚ 3 fichiers au fichier data, soient f1 f2 et f3, il
faut, pour trouver la position de f1 utiliser la fonction avec le
paramŠtre 3, pour f2, avec le paramŠtre 2, pour f3 avec le paramŠtre 1.

Au retour de la fonction, fd est positionn‚ sur le d‚but des donn‚es.

Si n est nul, le fichier est repositionn‚ au d‚but et la fonction
retourne 0.

&RT position (long) de l'ensemble n de donn‚es dans le fichier, ou
    -1 si fd est nul ou que le "magic number" n'est pas trouv‚
    en tˆte du fichier concat‚n‚.

&EX
    1. Cr‚ation des donn‚es
    -----------------------
    scr4_app myprog.exe data1
    scr4_app myprog.exe data2

    2. Utilisation en C
    -------------------
    fd = fopen("myprog.exe", "rb");
    if(fd == 0) exit(1);
    SCR_get_app_pos(fd, 2);
    fread(bufdata1, sizeof(bufdata1), 1, fd);
    SCR_get_app_pos(fd, 1);
    fread(bufdata2, sizeof(bufdata2), 1, fd);
    fclose(fd);
&TX
&SA programme scr4_app, SCR_get_app_size()
=======================================================================*/

long SCR_get_app_pos(fd, n)
FILE    *fd;
int     n;
{
    long    pos = 0L, opos;     // JMP 17/05/2022
    char    buf[5];

    if(fd == 0) return(-1L);    // JMP 17/05/2022
    opos = ftell(fd);           // JMP 17/05/2022

    if(n <= 0) goto fin;
    fseek(fd, -(long)(2 * sizeof(long)), 2);                /* JMP_M 4.19 08-05-95 */
    while(1) {
        fread(&pos, sizeof(long), 1, fd);
        fread(buf, 4, 1, fd);                               /* JMP_M 4.19 08-05-95 */
        if(memcmp(buf, "\002\003\004\005", 4)) {
            fseek(fd, opos, 0);                         /* JMP 09-11-97 */
            return(-1);                                 /* JMP_M 4.19 08-05-95 */
        }
        n--;
        if(n <= 0) break;
        fseek(fd, pos - (long)(2 * sizeof(long)), 0);       /* JMP_M 4.19 08-05-95 */
    }
fin:
    fseek(fd, pos, 0);
    return(pos);
}

/* ====================================================================
Cette fonction est … exploiter en connexion avec le programme scr4_app.
Elle calcule la taille d'un fichier concat‚n‚.

fd d‚crit le fichier (qui doit ˆtre ouvert en lecture) et n est le
nombre de fichiers concat‚n‚s qui doivent ˆtre saut‚s.

Ainsi, si on a ajout‚ 3 fichiers au fichier data, soient f1 f2 et f3, il
faut, pour trouver la taille, de f1 utiliser la fonction avec le
paramŠtre 3, pour f2, avec le paramŠtre 2, pour f3 avec le paramŠtre 1.

Au retour de la fonction, fd est positionn‚ au d‚but des donn‚es
correspondant au fichier n.

Si n est nul, le fichier est repositionn‚ au d‚but et la fonction
retourne 0.

&RT taille (long) de l'ensemble n de donn‚es dans le fichier, ou
    -1 si fd est nul ou que le "magic number" n'est pas trouv‚
    en tˆte du fichier concat‚n‚.

&EX
    1. Cr‚ation des donn‚es
    -----------------------
    scr4_app myprog.exe data1
    scr4_app myprog.exe data2

    2. Utilisation en C
    -------------------
    fdin = fopen("myprog.exe", "rb");
    if(fdin == 0) exit(1);
    fdout = fopen("out.dat", "wb+");
    if(fdout == 0) exit(1);
    size = SCR_get_app_size(fdin, 2);
    for(i = 0 ; i < size ; i++)
	putc(fgetc(fdin), fdout);
    fclose(fdin);
    fclose(fdout);
&TX
&SA programme scr4_app, SCR_get_app_pos()
=======================================================================*/

long SCR_get_app_size(fd, n)
FILE    *fd;
int     n;
{
    long    pos = 0L, epos = 0L, opos;
    char    buf[5];

    if(fd == 0) return(-1);
    opos = ftell(fd);           // JMP 17/05/2022
    if(n <= 0) goto fin;
    fseek(fd, -(long)(2 * sizeof(long)), 2);
    while(1) {
        epos = ftell(fd);
        fread(&pos, sizeof(long), 1, fd);
        fread(buf, 4, 1, fd);
        if(memcmp(buf, "\002\003\004\005", 4)) {
            fseek(fd, opos, 0);
            return(-1L);
        }
        n--;
        if(n <= 0) break;
        fseek(fd, pos - (long)(2 * sizeof(long)), 0);
    }
fin:
    fseek(fd, pos, 0);
    return(epos - pos);
}





