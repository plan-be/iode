#include "s_dir.h"
#include "s_strs.h"

/* ====================================================================
Divise un nom de fichier en directory et fichier en étudiant
le nom du path fourni. Retourne dans dir le nom du directory et
dans file les "wildcards" correspondant aux fichiers spécifiés.

&NO Cette fonction ne vérifie pas l'existence du fichier ou du directory.

&RT 0 en cas de succès, -1 en cas d'erreur (directory contenant * ou ?,
    directory supérieur n'existant pas, drive incorrect, ...)
&EX
    Si le directory \usr\ode existe :
      SCR_split_name("\usr\ode\toto", dir, file)
      dir = "\usr\ode" et file = "toto" si toto n'est pas un directory
      dir = "\usr\ode\toto" et file = "*" si toto est un directory

      SCR_split_name("\usr\ode\*.c", dir, file)
      dir = "\usr\ode" et file = "*.c"

    Si le directory \usr\ode n'existe pas :
      SCR_split_name("\usr\ode\toto", dir, file) retourne -1
      SCR_split_name("\usr\ode\*.c", dir, file)  retourne -1

    Dans tous les cas :
      SCR_split_name("\usr\*\*.c", dir, file)  retourne -1
      SCR_split_name("\usr\*\toto", dir, file) retourne -1

&TX

&SA SCR_dir_std(), SCR_split_dir()
======================================================================= */

SCR_split_name(path, dir, file)
char    *path, *dir, *file;
{
    char    buf[SCR_STAT_NAME_LG + 1];

    SCR_strlcpy(buf, path, SCR_STAT_NAME_LG); /* JMP 03-11-06 */
    /* SCR_sqz(buf);/* JMP 21-04-98 */
    U_ljust_text(buf); /* JMP 21-04-98 */
    SCR_strip(buf); /* JMP 21-04-98 */
    if(SCR_fullpath(buf, dir) == 0) return(-1); /* JMP 22-09-99 */
    strcpy(buf, dir);
    SCR_split_dir(dir, file);
    if(U_is_in('*', dir) || U_is_in('?', dir)) return(-1);
    if(U_is_in('*', file) || U_is_in('?', file)) {
	if(SCR_is_dir(dir)) return(0);
	return(-1);
	}
    strcpy(file, "*");
    strcpy(dir, buf);
    if(SCR_is_dir(dir)) return(0);
    SCR_split_dir(dir, file);
    if(!SCR_is_dir(dir)) return(-1);
    if(U_is_in('*', file) || U_is_in('?', file)) return(0);
/*    strcpy(file, "*"); /* JMP 23-10-02 */
    return(0);
}





