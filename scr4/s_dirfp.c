#include "s_strs.h"
#include "s_dir.h"

#ifdef DOS
#include <dos.h>
#endif

/* ====================================================================
Full Path : retourne le nom complet d'un fichier à partir d'un nom
relatif. name contient le nom du fichier, res le résultat.

En cas d'erreur, res est vide et la variable DOS errno indique l'origine
du problème (Drive not ready, etc).

&RT pointeur vers le nom complet du fichier (res) ou NULL en cas d'erreur.
&EX
    Soit le directory courant sur c: \usr
			      sur a: \

    SCR_fullpath("toto", res)     c:\usr\toto
    SCR_fullpath(".\\toto", res)  c:\usr\toto
    SCR_fullpath("./toto", res)   c:\usr\toto
    SCR_fullpath("../toto", res)  c:\toto
    SCR_fullpath("\\toto", res)   c:\toto
    SCR_fullpath("a:toto", res)   a:\toto
       ...
&TX
&SA SCR_dir(), SCR_getcwd(), SCR_relpath()
======================================================================= */

char *SCR_fullpath(name, res)
char    *name, *res;
{
    int     drive, nvcdir, i, dirch;
    char    cdir[512], **vcdir, **vname, *tmp;
/*    char    **SCR_vtoms(), *SCR_mtov(); /* JMP 04-01-98 */

#ifdef DOS
    dirch = '\\';
    if(strlen(name) > 2 && memcmp(name, "\\\\", 2) == 0) {  /* JMP 31-03-98 */
	strcpy(res, name);
	return(res);
	}                                                   /* JMP 31-03-98 */
    else {
	if(strlen(name) < 2 || name[1] != ':')
	    drive = SCR_dosgetdrive();  /* JMP 22-01-98 */
	else
	    {
	    drive = SCR_upper_char(name[0]) - 'A' + 1;
	    name += 2;
	    }
	if(SCR_getcwd(drive, cdir) == 0) {
	    res[0] = 0;
	    return((char *)0);
	    }
	res[0] = 'a' + drive - 1;
	res[1] = ':';
	res[2] = dirch;
	res[3] = 0;
	}
#else
    dirch = '/';
    getcwd(cdir + 2, 78);
    res[0] = dirch;
    res[1] = 0;
#endif

    vname = (char **) SCR_vtoms(name, "\\/");
    if(name[0] == '/' || name[0] == '\\')
	vcdir = (char **) SCR_vtoms("", "\\/");
    else
	vcdir = (char **) SCR_vtoms(cdir + 2, "\\/");
    nvcdir = SCR_tbl_size(vcdir);
    for(i = 0; vname[i] ; i++) {
	if(strcmp(vname[i], ".") == 0) continue;
	if(strncmp(vname[i], "..", 2) == 0) {
	    if(nvcdir > 0) {
		nvcdir--;
		SCR_free(vcdir[nvcdir]);
		}
	    continue;
	    }
	SCR_add_ptr(&vcdir, &nvcdir, vname[i]);
	}

    SCR_add_ptr(&vcdir, &nvcdir, 0L);
    tmp = SCR_mtov(vcdir, dirch);
    strcat(res, tmp);
    SCR_free(tmp);
    SCR_free_tbl(vcdir);    /* JMP 14-12-96 */
    SCR_free_tbl(vname);    /* JMP 14-12-96 */

#if defined(DOS) && !defined(DOSW32) && !defined(SCRW32) /* JMP 31-03-98 */
    SCR_lower(res);
#endif

    return(res);
}



