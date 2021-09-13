#include "s_strs.h"
#include <stdio.h>


#ifdef DOS
#include <io.h>
char    SCR_BASE_PATH[64] = ".;\\;c:\\;c:\\scr";
#define PATH_SEP    ';'
#define DIR_SEP     '\\'
#else
char    SCR_BASE_PATH[64] = ".:/usr/lib/scr:/usr/scr";
#define PATH_SEP    ':'
#define DIR_SEP     '/'
#endif

/* ====================================================================
Recherche un fichier sur base d'un "PATH" donn�. Le path est un string
contenant des noms de directories s�par�s par des ; en DOS et des : en
UNIX. D�s que le fichier est trouv� dans un des directories, le nom
complet est retourn� dans un buffer statique qui contient la derni�re
valeur.

Si le nom du fichier commence par "." ou "/" ou "\", la recherche n'a
pas lieu : le nom de fichier est consid�r� comme compl�tement d�termin�.

&RT une pointeur vers un buffer statique si le fichier est trouv�. NULL
sinon.
&EX
    char    *pt;

    pt = SCR_search_path(".;\\;c:\\;c:\\scr", file);
    if(pt == NULL) {
	fprintf(stderr, "File %s not found in PATH\n", file);
	return(-1);
	}
    else {
	printf("Premier fichier trouv� : %s\n", pt);
	return(0);
	}
&TX
&SA SCR_search_file()
=======================================================================*/

char    *SCR_search_path(path, file)
char    *path;
char    *file;
{
    static U_ch    pt[256]; // JMP 3/2017
    U_ch           *txt, c;
    char    fdrv[3], pdrv[3];
    int     i = 0;

    if(strlen(file) < 1) return((char *)0);

    txt = SCR_stracpy(file);
    fdrv[0] = 0;
#ifdef DOS
    if(txt[1] == ':') {
	memcpy(fdrv, txt, 2);
	fdrv[2] = 0;
	strcpy(txt, file + 2);
    }
#endif
    if(txt[0] == DIR_SEP || txt[0] == '.' || fdrv[0] != 0) {
	SCR_free(txt);
	if(access(file, 0) == 0) {
	    strcpy(pt, file);
	    return((char *)pt);
	}
	return((char *)0);
    }

    SCR_free(txt);          /* JMP 24-06-96 */
    while(*path != 0) {
	i = 0;
	if(*path == PATH_SEP) path++;
	while(*path != PATH_SEP && *path != 0) {
	    pt[i] = *path;
	    path++; i++;
	}
	if(i == 0 || pt[i - 1] != DIR_SEP) pt[i++] = DIR_SEP; /* JMP 10-02-96 */
	pt[i++] = 0;
	strcat(pt, file);
	if(access(pt, 0) == 0)
	    return((char *)pt);
    }
    return((char *)0);
}

/* ====================================================================
Recherche un fichier sur base du "PATH" de SCR/AL1. Le path est d�fini
par la variable globale SCR_PATH. Si cette variable est vide ou que le
fichier n'est pas trouv� dans les directory d�finis, la variable
d'environnement PATH est utilis�e. Si PATH ne contient pas le fichier,
la variable C SCR_BASE_PATH du syst�me est utilis�e.
La valeur par d�faut est :

&CO
    en DOS  : ".;\\;c:\\;c:\\scr";
    en UNIX : ".:/usr/lib/scr";
&TX

La fonction SCR_search_path() est utilis�e pour rechercher le fichier.

&RT une pointeur vers un buffer statique si le fichier est trouv�. NULL
sinon.
&EX
    char    *pt;

    pt = SCR_search_file(file);
    if(pt == NULL) {
	fprintf(stderr, "File %s not found\n", file);
	return(-1);
	}
    else {
	printf("Premier fichier trouv� : %s\n", pt);
	return(0);
	}
&TX
&SA SCR_search_path()
=======================================================================*/

char *SCR_search_file(file)
char    *file;
{
    char    *pt = 0, *path;
/*    char    *getenv(); /* JMP 04-01-98 */

    if(file[0] == DIR_SEP || file[0] == '.') return(file);

    path = getenv("SCR_PATH");
    if(path != 0) pt = SCR_search_path(path, file);
    if(pt) return(pt);
    pt = SCR_search_path(SCR_BASE_PATH, file);          /* JMP 04-03-92 */
    if(pt) return(pt);
    path = getenv("PATH");                              /* JMP 26-02-92 */
    if(path != 0) pt = SCR_search_path(path, file);     /* JMP 26-02-92 */
    return(pt);
}







