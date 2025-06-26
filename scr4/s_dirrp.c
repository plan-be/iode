#include "s_dir.h"
#include "s_strs.h"

/* ====================================================================
Relative Path : retourne le nom relatif d'un fichier à un directory
quelconque. file contient le nom du fichier, dir celui du directory, res
le résultat.

En cas d'erreur, res est vide et la variable DOS errno indique l'origine
du problème (Drive not ready, etc). La fonction retourne un pointeur nul.
Cette fonction utilise SCR_fullpath().

&RT pointeur vers le nom relatif du fichier (res) ou NULL en cas d'erreur.
&EX
    Soit le directory courant sur c: \usr
			      sur a: \

	    Appel                                 Valeur de res

    SCR_relpath("c:\usr",     "toto",           res)     toto
    SCR_relpath("c:\usr\scr", "toto",           res)     ..\toto
    SCR_relpath("c:\usr\scr", "c:\toto",        res)     ..\..\toto
    SCR_relpath("c:\usr",     "c:\usr\scr\toto",res)     scr\toto
    SCR_relpath("a:\usr",     "toto",           res)     c:\usr\toto
    SCR_relpath("c:\usr",     "c:\dos",         res)     ..\dos\
       ...
&TX
&SA SCR_dir(), SCR_getcwd(), SCR_fullpath()
======================================================================= */

char *SCR_relpath(dir, file, res)
char    *dir, *file, *res;
{
    char    bufdir[512], buffile[512], *dirsep,
	    **vdir, **vfile;
/*    char    **SCR_vtoms(); /* JMP 04-01-98 */
    int     i, j;

    res[0] = 0;
    if(SCR_fullpath(dir,   bufdir)  == 0) return((char *)0);
    if(SCR_fullpath(file,  buffile) == 0) return((char *)0);

#ifdef DOS
    dirsep = "\\";
#if !defined(DOSW32) && !defined(SCRW32)
    SCR_lower(bufdir);
    SCR_lower(buffile);
#endif
#else
    dirsep = "/";
#endif

    vdir  = (char **) SCR_vtoms(bufdir, "\\/");
    vfile = (char **) SCR_vtoms(buffile, "\\/");

    for(i = 0 ; vdir[i] ; i++)
	if(vfile[i] == 0 || strcmp(vdir[i], vfile[i])) break;

    if(i == 0) {
	strcpy(res, buffile);
	return(res);
	}

    if(vdir[i])
	for(j = i; vdir[j] ; j++) {
	    strcat(res, "..");
	    strcat(res, dirsep);
	    }

    for(j = i; vfile[j] ; j++) {
	strcat(res, vfile[j]);
	if(vfile[j + 1]) strcat(res, dirsep);
	}
    SCR_free_tbl((U_ch **)vfile);
    SCR_free_tbl((U_ch **)vdir);
    return(res);
}

