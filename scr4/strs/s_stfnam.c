#include "scr4.h"

/* ====================================================================
Transforme un nom de fichier en format Unix en remplaçant les
BACKSLASH (\) par des SLASH (/).

&EN filename = pointeur vers le nom du fichier

&RT Pointeur vers filename

&SA SCR_std_filename()
=======================================================================*/

U_ch *SCR_unix_filename(filename)
U_ch   *filename;
{
    int     i;

    if(filename == 0) return(filename);
    for(i = 0 ; filename[i] ; i++)
	if(filename[i] == '\\') filename[i] = '/';
    return(filename);
}

/* ====================================================================
Transforme un nom de fichier en fonction du système cible (Unix ou DOS)
en remplaçant les BACKSLASH (\) ou les SLASH (/) par des SLASH (/) ou
des BACKSLASH (\).

La fonction s'adapte au système courant.

&EN filename = pointeur vers le nom du fichier

&RT Pointeur vers filename

&SA SCR_unix_filename()
=======================================================================*/

U_ch *SCR_std_filename(filename)
U_ch   *filename;
{
    int     i;

    if(filename == 0) return(filename);
    for(i = 0 ; filename[i] ; i++)
#ifdef DOS
	if(filename[i] == '/') filename[i] = '\\';
#else
	if(filename[i] == '\\') filename[i] = '/';
#endif
    return(filename);
}

/* ====================================================================
Transforme un nom de fichier en fonction du système cible (Unix ou DOS)
en remplaçant les BACKSLASH (\) ou les SLASH (/) par des SLASH (/) ou
des BACKSLASH (\).

Cette fonction supprime également les doubles // ou \\ sauf en début de nom.

La fonction s'adapte au système courant.

&EN filename = pointeur vers le nom du fichier

&RT Pointeur vers filename

&SA SCR_unix_filename()
=======================================================================*/

U_ch *SCR_std_filename_1(filename)
U_ch   *filename;
{
    int     i, j, ch;
    char    res[1024];

#ifdef DOS
	ch = '\\';
#else
	ch = '/';
#endif

    if(filename == 0) return(filename);
    SCR_std_filename(filename);

    for(i = j = 0 ; filename[i] ; i++) {
	if(ch != filename[i] || j < 2 || res[j - 1] != ch)
	    res[j++] = filename[i];
	}
    res[j] = 0;
    strcpy(filename, res);
    return(filename);
}













