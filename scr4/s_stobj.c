#include <stdio.h>
#include "s_strs.h"

/* =================================================================
Lit et alloue un string dans le fichier décrit par fd. Ce fichier a été
préalablement construit à l'aide des fonctions SCR_Dump*().

Si le string lu dans le fichier est de longueur nulle, *txt est fixé
à 0.

&EN fd : file descriptor
&EN txt : pointeur vers le pointeur destiné à contenir le string

&RT -1 en cas de fin de fichier ou d'erreur de lecture, 0 sinon

&SA SCR_Dump*(), SCR_Load*()
==================================================================== */

SCR_LoadString(fd, txt)
FILE            *fd;
unsigned char   **txt;
{
    unsigned short  lg = 0;

    *txt = 0;
    if(fread(&lg, sizeof(short), 1, fd) != 1) return(-1);
    if(lg <= 0) return(0);
    *txt = SCR_malloc(lg + 1);
    if(fread(*txt, lg, 1, fd) != 1) return(-1);
    return(0);
}

/* =================================================================
Lit un string static (ou déjà alloué, la taille n'étant pas vérifiée)
dans le fichier décrit par fd. Ce fichier a été préalablement construit
à l'aide des fonctions SCR_Dump*().

Si le string lu dans le fichier est de longueur nulle, txt[0] est fixé
à 0.

&EN fd : file descriptor
&EN txt : pointeur vers le pointeur destiné à contenir le string

&RT -1 en cas de fin de fichier ou d'erreur de lecture, 0 sinon

&SA SCR_Dump*(), SCR_Load*()
==================================================================== */

SCR_LoadSString(fd, txt)
FILE            *fd;
unsigned char   *txt;
{
    unsigned short  lg = 0;

    txt[0] = 0;
    if(fread(&lg, sizeof(short), 1, fd) != 1) return(-1);
    if(lg <= 0) return(0);
    if(fread(txt, lg, 1, fd) != 1) return(-1);
    txt[lg] = 0;
    return(0);
}

/* =================================================================
Lit un short dans le fichier décrit par fd. Ce fichier a été
préalablement construit à l'aide des fonctions SCR_Dump*().

&EN fd : file descriptor
&EN a : pointeur vers un short destiné à recevoir la valeur

&RT -1 en cas de fin de fichier ou d'erreur de lecture, 0 sinon

&SA SCR_Dump*(), SCR_Load*()
==================================================================== */

SCR_LoadShort(fd, a)
FILE    *fd;
short   *a;
{
    if(fread(a, sizeof(short), 1, fd) != 1) return(-1);
    return(0);
}

/* =================================================================
Sauve le string txt (terminé par 0) dans le fichier décrit par fd.

&EN fd : file descriptor
&EN txt : pointeur vers le string. Peut être nul.

&SA SCR_Dump*(), SCR_Load*()
==================================================================== */

int SCR_DumpString(fd, txt)
FILE            *fd;
unsigned char   *txt;
{
    unsigned short  lg = 0;

    if(txt == 0) fwrite(&lg, sizeof(short), 1, fd);
    else {
	lg = (int)strlen(txt);
	fwrite(&lg, sizeof(short), 1, fd);
	if(lg) fwrite(txt, lg, 1, fd);
	}
    return(0);
}

/* =================================================================
Sauve le short a dans le fichier décrit par fd.

&EN fd : file descriptor
&EN a : valeur à stocker

&SA SCR_Dump*(), SCR_Load*()
==================================================================== */

int SCR_DumpShort(fd, a)
FILE    *fd;
short   a;
{
    fwrite(&a, sizeof(short), 1, fd);
    return(0);
}



