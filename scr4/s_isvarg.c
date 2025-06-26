#define SCRSUPPROTO
#include "scr4.h"

/* ====================================================================
Ouvre une liste d'ISAM. Cette liste doit se terminer par un pointeur
NULL. Lorsqu'un des fichiers ne peut être ouvert, un message est affiché
à l'écran et les fichiers déjà ouverts sont refermés.

La fonction fait pour chaque fichier l'appel suivant :
&CO
	IS_open(is, SCR_UPDATE, SCR_NO_LOCK);
&TX
&RT 0 en cas de succès, -1 sinon.
&EX
    if(IS_open_files(is_clients, is_produits, 0L) != 0)
	return(-1);
&TX
&SA IS_open(), IS_close_files()
=======================================================================*/

IS_open_files(is1, is2, is3, is4, is5, is6, is7, is8, is9, is10)
ISAM    *is1, *is2, *is3, *is4, *is5, *is6, *is7, *is8, *is9, *is10;
{
    ISAM    *is[10];
    int     i;

    is[0] = is1; is[1] = is2; is[2] = is3; is[3] = is4; is[4] = is5;
    is[5] = is6; is[6] = is7; is[7] = is8; is[8] = is9; is[9] = is10;

    for(i = 9 ; i > 0 ; i--)
	if(is[i] == 0) break;
    if(i == 0) {
	PG_display_error(SCR_err_txt(108));
	return(-1);
	}

    for(i = 0 ; is[i] != 0 ; i++) {
	if(IS_open(is[i], SCR_UPDATE, SCR_NO_LOCK)) {
	    PG_display_error(SCR_err_txt(109), is[i]->is_filename, IS_error_text());
	    break;
	    }
	}
    if(is[i] == 0) return(0);

    i--;
    for( ; i >= 0 ; i--)
	IS_close(is[i]);

    return(-1);
}

/* ====================================================================
Ferme une liste de fichiers. La liste de pointeurs vers ISAM doit se
terminer par un pointeur nul.

La fonction fait appel à IS_close() pour chaque fichier.

&EX
    IS_close_files(is_clients, is_produits, 0L);
&TX
&SA IS_close(), IS_open_files()
=======================================================================*/

IS_close_files(is1, is2, is3, is4, is5, is6, is7, is8, is9, is10)
ISAM    *is1, *is2, *is3, *is4, *is5, *is6, *is7, *is8, *is9, *is10;
{
    ISAM    *is[10];
    int     i;

    is[0] = is1; is[1] = is2; is[2] = is3; is[3] = is4; is[4] = is5;
    is[5] = is6; is[6] = is7; is[7] = is8; is[8] = is9; is[9] = is10;

    for(i = 9 ; i > 0 ; i--)
	if(is[i] == 0) break;
    if(i == 0) {
	PG_display_error(SCR_err_txt(110));
	return(-1);
	}

    for(i = 0 ; is[i] != 0 ; i++)
	IS_close(is[i]);
    return(0);
}

/* ====================================================================
Vérifie que des ISAMS peuvent être ouverts en mise à jour. Tous les
fichiers de la liste sont ouverts, puis fermés. Les fichiers qui ne
peuvent être ouverts sont indiqués par un message.

La fonction fait appel à
&CO
	IS_open(is, SCR_UPDATE, SCR_NO_LOCK)
&TX
pour ouvrir le fichier, puis à
&CO
	IS_close(is[i]);
&TX
&RT 0 en cas de succès, -1 sinon.
&EX
    if(IS_check_open(is1, is2, 0L)) return(-1);
&TX
&SA IS_open_files(), IS_close_files(), IS_open(), IS_close()
=======================================================================*/

IS_check_open(is1, is2, is3, is4, is5, is6, is7, is8, is9, is10)
ISAM    *is1, *is2, *is3, *is4, *is5, *is6, *is7, *is8, *is9, *is10;
{
    ISAM    *is[10];
    int     i, flag = 0;

    is[0] = is1; is[1] = is2; is[2] = is3; is[3] = is4; is[4] = is5;
    is[5] = is6; is[6] = is7; is[7] = is8; is[8] = is9; is[9] = is10;

    for(i = 9 ; i > 0 ; i--)
	if(is[i] == 0) break;
    if(i == 0) {
	PG_display_error(SCR_err_txt(111));
	return(-1);
	}

    for(i = 0 ; is[i] != 0 ; i++) {
	if(IS_open(is[i], SCR_UPDATE, SCR_NO_LOCK)) {
	    PG_display_error(SCR_err_txt(112), is[i]->is_filename);
	    flag = 1;
	    continue;
	    }
	IS_close(is[i]);
	}
    if(flag)
	return(-1);
    else
	return(0);
}

