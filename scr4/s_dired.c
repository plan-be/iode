#include "scr.h"

/*NH*/
SCR_dir_edit_fn(text, dirname, ss)
char    *text, *dirname;
SCRSTAT *ss;
{
    char    dt[15], tm[15];

    SCR_mode_fmt(ss->ss_mode, text);
    SCR_long_to_fdate(ss->ss_mdate, dt, "dd/mm/yy");
    SCR_long_to_ftime(ss->ss_mtime, tm, "hh:mm");
    sprintf(text + 10, "%9ld %s %s %s", ss->ss_size, dt, tm, ss->ss_name);
    return(0);
}

/* ======================================================================
Lit les fichiers correspondant au masque path et les affiche dans un menu.
La sélection des fichiers découle de la valeur de add_dir:

&EN 0 pour une sélection normale (directory et fichiers)
&EN 1 pour une sélection de tous les directories et des seuls fichiers
    correspondant à path
&EN 2 pour une sélection des seuls fichiers, sans les directory
&EN 3 pour une sélection des seuls directory

Les variables dir et file contiennent en output le nom absolu du directory
scanné et le masque de recherche. Elles doivent avoir une taille suffisante.

Le MENU est placé en ligne line, colonne col, sur un nombre de lignes nl.
L'attribut de fond est attr.

fn est un pointeur de fonction qui permet de filtrer les fichiers trouvés :
cette fonction a la syntaxe suivante :

&CO
    int fn(char *text, char * dirname, SCRSTAT *ss);
&TX

où
&EN dirname est le nom du directory scanné (IN) dérivé de path,
&EN SCRSTAT le pointeur vers une structure contenant les informations sur le
fichier courant
&EN text (OUT) le texte à placer dans le menu.

&RT pointeur vers une structure SCRSTAT statique avec le fichier
    sélectionné. NULL si pas de sélection (sortie par ESCAPE).

&SA SCR_dir_edit()
------------------------------------------------------------------------- */
SCRSTAT *SCR_dir_edit_1(path, add_dir, dir, file, line, col, nl, attr, fn)
char    *path, *dir, *file;
int     add_dir, line, col, nl, attr;
int     (*fn)();
{
    MENU            *mn;
    SCRSTAT         **stbl;
    static SCRSTAT  ss;
    int             i = 0;
    //char            buf[132];
    char            buf[1024];
    long            tot = 0L;

//    stbl = SCR_dir(path, add_dir, dir, file);
    stbl = ISC_dir(path, add_dir, dir, file); /* JMP 17-06-01 */
    if(stbl == 0) {
	PG_display_error("Path '%s' : file not found", path);
	return((SCRSTAT *)0);
	}

    mn = MN_create_default(line, col, attr, nl);
    MN_set_title(mn, "Dir %s", dir);

    if(fn == 0) fn = SCR_dir_edit_fn;
    for(i = 0 ; stbl[i] ; i++) {
	(*fn)(buf, dir, stbl[i]);
	MN_add_set_option_text(mn, "%s", buf);
	mn->mn_opts[mn->mn_nb_opts - 1].opt_letter = stbl[i]->ss_name[0];
	tot += stbl[i]->ss_size;
	}
    if(i != mn->mn_nb_opts)
	MN_set_option_text(mn, mn->mn_nb_opts - 1, "...LOW MEMORY...");
    MN_set_footnote(mn, "%d files (%ld bytes)", i, tot);
    MN_end(mn);
    i = MN_edit(mn);
    if(i > 0) memcpy(&ss, stbl[i - 1], sizeof(SCRSTAT));
    MN_free(mn);
    SCR_free_tbl(stbl);
    if(i > 0) return(&ss);
    else return((SCRSTAT *)0);
}

/* ======================================================================
Lit les fichiers correspondant au masque path et les affiche dans un menu, à
l'aide de la fonction SCR_dir_edit_1().

Si le fichier sélectionné est un directory, ce directory est édité à son
tour, à nouveau par un appel à SCR_dir_edit_1().

Les variables dir et file contiennent en output le nom absolu du dernier
directory scanné et le masque de recherche. Elles doivent avoir une taille
suffisante.

Le MENU est placé en ligne line, colonne col, sur un nombre de lignes nl.
L'attribut de fond est attr.

fn est un pointeur de fonction qui permet de filtrer les fichiers trouvés :
cette fonction a la syntaxe suivante :

&CO
    int fn(char *text, char * dirname, SCRSTAT *ss);
&TX

où
&EN dirname est le nom du directory scanné (IN) dérivé de path,
&EN SCRSTAT le pointeur vers une structure contenant les informations sur le
fichier courant
&EN text (OUT) le texte à placer dans le menu.


Cette fonction est utilisée dans les champs de type DIR.

&RT pointeur vers une structure SCRSTAT statique avec le fichier
    sélectionné. NULL si pas de sélection (sortie par ESCAPE).

&SA SCR_dir_edit_1(), SCR_dir(), ...
------------------------------------------------------------------------- */

SCRSTAT *SCR_dir_edit(path, dir, file, line, col, nl, attr, fn)
char    *path, *dir, *file;
int     line, col, nl, attr;
int     (*fn)();
{
    SCRSTAT *ss;
    //char    c_path[2 * SCR_FILE_LG + 1], tmp[2 * SCR_FILE_LG + 1]; /* JMP 19-04-98 */
    char    c_path[SCR_STAT_NAME_LG + 1], tmp[SCR_STAT_NAME_LG + 1]; /* JMP 03-11-06 */

    SCR_strlcpy(c_path, path, SCR_STAT_NAME_LG);
    while(1) {
	ss = SCR_dir_edit_1(c_path, 1, dir, file, line, col, nl, attr, fn);
	if(ss == 0 || (ss->ss_mode & SCR_FTYPE_DIR) == 0) return(ss);
	if(strcmp("..", ss->ss_name) == 0) SCR_split_dir(dir, tmp);
	else SCR_dir_join(dir, dir, ss->ss_name);
	SCR_dir_join(c_path, dir, file);
	}
}

/* ======================================================================
Lit les fichiers correspondant au masque path et les affiche dans un menu, à
l'aide de la fonction SCR_dir_edit().

Le nom absolu du fichier choisi est retourné dans res qui doit être de
taille suffisante.

Le MENU est placé en ligne line, colonne col, sur un nombre de lignes nl.
L'attribut de fond est attr.

fn est un pointeur de fonction qui permet de filtrer les fichiers trouvés :
cette fonction a la syntaxe suivante :

&CO
    int fn(char *text, char * dirname, SCRSTAT *ss);
&TX

où
&EN dirname est le nom du directory scanné (IN) dérivé de path,
&EN SCRSTAT le pointeur vers une structure contenant les informations sur le
fichier courant
&EN text (OUT) le texte à placer dans le menu.

Cette fonction est utilisée dans les champs de type DIR.

&RT 0 si un fichier est sélectionné, -1 sinon (et res[0] = 0)

&SA SCR_dir_edit(), SCR_dir(), ...
------------------------------------------------------------------------- */

SCR_dir_select(path, res, line, col, nl, attr, fn)
char    *path, *res;
int     line, col, nl, attr;
int     (*fn)();
{
    SCRSTAT *ss;
//    char    dir[2 * SCR_FILE_LG + 1], file[2 * SCR_FILE_LG + 1]; /* JMP 19-04-98 */
    char    dir[SCR_STAT_NAME_LG + 1], file[SCR_STAT_NAME_LG + 1]; /* JMP 03-11-06 */

    ss = SCR_dir_edit(path, dir, file, line, col, nl, attr, fn);
    if(ss == 0) {
	res[0] = 0;
	return(-1);
	}
    SCR_dir_join(res, dir, ss->ss_name);
    return(0);
}

/*NH
DIR_relative(res, cwd, dir)
unsigned char   *res,
		*cwd,
		*dir;
{
    unsigned char **cwdt,
		  **dirt;
    int           i, k;

    cwdt = SCR_vtom(cwd, DIR_CHAR);
    dirt = SCR_vtom(dir, DIR_CHAR);

    for(i = 0 ; cwdt[i] != 0 && dirt[i] != 0 ; i++)
	if(strcmp(dirt[i], cwdt[i]) != 0) break;
    res[0] = 0;
    for(k = i ; cwdt[k] != 0 ; k++) {
	strcat(res, "..");
	strcat(res, DIR_SEPAR);
	}
    for(k = i ; dirt[k] != 0 ; k++) {
	strcat(res, dirt[k]);
	strcat(res, DIR_SEPAR);
	}
    if(dir[strlen(dir) - 1] != DIR_CHAR) res[strlen(res) - 1] = 0;
    SCR_free_tbl(dirt);
    SCR_free_tbl(cwdt);
}
*/



