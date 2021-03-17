#include <stdlib.h>
#include "s_strs.h"
#include "s_dir.h"

/*NH*/
#ifndef GENPROTO
/*NH*/
SCR_dircmp(pd1, pd2)
char    *pd1, *pd2;
{
    SCRSTAT **d1 = (SCRSTAT **)pd1,
	    **d2 = (SCRSTAT **)pd2;

    return(strcmp((char *)*d1, (char *)*d2));
}
#endif

/* ======================================================================
Retourne une table de pointeurs vers des structures SCRSTAT correspondant
aux sp‚cifications donn‚es dans path (par ex. *c*). Les noms sont class‚s
par ordre alphab‚tique, mais les directories sont tous plac‚s au d‚but. La
table est termin‚e par un pointeur nul.

Le paramŠtre add_dir indique la s‚lection … effectuer sur les directory :

&EN 0 pour une s‚lection normale (directory et fichiers)
&EN 1 pour une s‚lection de tous les directory et des seuls fichiers
    correspondant … path
&EN 2 pour une s‚lection des seuls fichiers, sans les directory
&EN 3 pour une s‚lection des seuls directory

Les variables dir et filename contiennent en output le nom absolu du
directory scann‚ et le masque de recherche. Elles doivent avoir une taille
suffisante.

La valeur de crc d‚termine le fait de calculer ou non le crc32 de chaque
fichier (voir SCR_stat_ex()).

La table est allou‚ … l'aide de SCR_add_ptr(). L'espace occup‚ est
lib‚r‚ par SCR_free_tbl().

&EX
    SCRSTAT     **ss;
    int         i;
    char        dir[SCR_FILE_LG + 1];
    char        file[SCR_FILE_LG + 1];

    ss = SCR_dir_ex("c*c", 0, dir, file, 1);
    if(ss == 0) return;
    printf("Directory of %s\n", dir);
    for(i = 0 ; ss[i] ; i++)
	printf("Name : %s - crc32 : %u\n", ss[i]->ss_name, ss[i]->ssx_crc32);
    SCR_free_tbl(ss);
&TX
&RT une table de SCRSTAT * contenant les fichiers s‚lectionn‚s et termin‚
    par un pointeur nul. Si aucun fichier n'est trouv‚, retourne NULL

&SA SCR_dir()
------------------------------------------------------------------------- */

#ifdef SCRCPP
SCRSTAT **SCR_dir_ex_fn(
char    *path,
int     add_dir,
char    *dir,
char    *filename,
int     crc,
int     *fn()
)
#else
SCRSTAT **SCR_dir_ex_fn(path, add_dir, dir, filename, crc, fn)
char    *path;
int     add_dir;
char    *dir, *filename;
int     crc;
int     (*fn)();
#endif

{
    SCRSTAT   **fss = 0, **dss = 0, *ss;
    int         nf = 0, nd = 0;
    SCRDIR      *sdir;
    char        buf[SCR_STAT_NAME_LG + 1]; /* JMP 03-11-06 */
/*    extern  char *SCR_malloc(), *SCR_realloc(); /* JMP 04-01-98 */

    if(SCR_split_name(path, dir, filename)) return((SCRSTAT **) 0);
    sdir = SCR_opendir(dir);
    if(sdir == 0) return((SCRSTAT **) 0);

    while(1) {
	ss = SCR_readdir_ex(sdir, 1, crc);
	if(ss == 0) break;
	if(strcmp(".", ss->ss_name) == 0) continue;
	if(add_dir == 2 && (ss->ss_mode & SCR_FTYPE_DIR)) continue;
	if(add_dir == 3 && !(ss->ss_mode & SCR_FTYPE_DIR)) continue;
	if(add_dir == 1 && (ss->ss_mode & SCR_FTYPE_DIR)) {
	    if(nd % 100 == 0)
		dss = (SCRSTAT **)SCR_realloc(dss, sizeof(SCRSTAT *), nd, nd + 100);
	    dss[nd] = (SCRSTAT *)SCR_malloc(sizeof(SCRSTAT));
	    memcpy(dss[nd], ss, sizeof(SCRSTAT));
	    if(fn != 0) (*fn)(ss);
	    nd++;
	    }

	/* ELSE CHECK NAME AND STORE IN fflist */
	else {
	    //SCR_strlcpy(buf, ss->ss_name, SCR_FILE_LG);
	    SCR_strlcpy(buf, ss->ss_name, SCR_STAT_NAME_LG); /* JMP 03-11-06 */
	    if(SCR_grep(filename, buf, 0)) {
		if(U_is_in('.', buf)) continue;
		strcat(buf, ".");
		if(SCR_grep(filename, buf, 0)) continue;
		}
	    if(nf % 100 == 0)
		fss = (SCRSTAT **)SCR_realloc(fss, sizeof(SCRSTAT *), nf, nf + 100);
	    fss[nf] = (SCRSTAT *)SCR_malloc(sizeof(SCRSTAT));
	    memcpy(fss[nf], ss, sizeof(SCRSTAT));
	    nf++;
	    if(fn != 0) (*fn)(ss);
	    }
	}

    SCR_closedir(sdir);

    /* IF NOTHING FOUND, RETURN */
    if(nf + nd == 0) return((SCRSTAT **) 0);

    /* SORT NAMES */
    if(nf > 1) qsort((char *)fss, nf, sizeof(SCRSTAT *), SCR_dircmp);
    if(nd > 1) qsort((char *)dss, nd, sizeof(SCRSTAT *), SCR_dircmp);

    /* APPEND DIRLIST TO FFLIST AND ADD NULL NAME (LAST) */
    dss = (SCRSTAT **)
		  SCR_realloc(dss, sizeof(SCRSTAT *), nd, nf + nd + 1);
    if(nf > 0) memcpy(dss + nd, fss, sizeof(SCRSTAT *) * nf);
    dss[nd + nf] = 0;

    SCR_free(fss);
    return(dss);
}

#ifdef SCRCPP
SCRSTAT **SCR_dir_ex(
char    *path,
int     add_dir,
char    *dir,
char    *filename,
int     crc
)
#else
SCRSTAT **SCR_dir_ex(path, add_dir, dir, filename, crc)
char    *path;
int     add_dir;
char    *dir, *filename;
int     crc;
#endif

{
    return(SCR_dir_ex_fn(path, add_dir, dir, filename, crc, 0));
}

/* ======================================================================
Retourne une table de pointeurs vers des structures SCRSTAT correspondant
aux sp‚cifications donn‚es dans path (par ex. *c*). Les noms sont class‚s
par ordre alphab‚tique, mais les directories sont tous plac‚s au d‚but. La
table est termin‚e par un pointeur nul.

Le paramŠtre add_dir indique la s‚lection … effectuer sur les directory :

&EN 0 pour une s‚lection normale (directory et fichiers)
&EN 1 pour une s‚lection de tous les directory et des seuls fichiers
    correspondant … path
&EN 2 pour une s‚lection des seuls fichiers, sans les directory
&EN 3 pour une s‚lection des seuls directory

Les variables dir et filename contiennent en output le nom absolu du
directory scann‚ et le masque de recherche. Elles doivent avoir une taille
suffisante.

La table est allou‚ … l'aide de SCR_add_ptr(). L'espace occup‚ est
lib‚r‚ par SCR_free_tbl().

&EX
    SCRSTAT     **ss;
    int         i;
    char        dir[SCR_STAT_NAME_LG + 1];
    char        file[SCR_STAT_NAME_LG + 1];

    ss = SCR_dir("c*c", 0, dir, file);
    if(ss == 0) return;
    printf("Directory of %s\n", dir);
    for(i = 0 ; ss[i] ; i++)
	printf("Name : %s - size : %ld\n", ss[i]->ss_name, ss[i]->ss_size);
    SCR_free_tbl(ss);
&TX
&RT une table de SCRSTAT * contenant les fichiers s‚lectionn‚s et termin‚
    par un pointeur nul. Si aucun fichier n'est trouv‚, retourne NULL
------------------------------------------------------------------------- */

#ifdef SCRCPP
SCRSTAT **SCR_dir(
char    *path,
int     add_dir,
char    *dir,
char    *filename
)
#else
SCRSTAT **SCR_dir(path, add_dir, dir, filename)
char    *path;
int     add_dir;
char    *dir, *filename;
#endif

{
    return(SCR_dir_ex(path, add_dir, dir, filename, 0));
}























