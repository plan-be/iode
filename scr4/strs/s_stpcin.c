#include "s_strs.h"
#include <stdio.h>

extern FILE     *PC_FD;

/* ====================================================================
Recherche dans le fichier filename la définition de l'item prname et
retourne un tableau contenant tous les champs de définition de
l'item trouvé.

Le fichier est décrit dans la fonction PC_list().

&RT un pointeur vers le tableau alloué, NULL en cas d'erreur
&EX
    char    **def;

    def = PC_infos("printcap", "ps401")
    switch(def[1][0]) {
	case 'P' : Transscript(filename); break;
	case 'A' : break;
	case 'H' : TransHP(filename); break;
	}
    ...
    SCR_free_tbl(def);
&TX
&SA PC_exec(), PC_list(), SCR_free_tbl();
=======================================================================*/

char **PC_infos(filename, prname)
char    *filename;
char    *prname;
{
    char    **tbl = 0, buf[132];
    int     type;

    if(PC_open(filename)) return(tbl);

    while(1) {
	type = PC_getline(prname, buf);
	if(type < 0) break;
	if(type == 0) {
	    tbl = (char **) SCR_vtoms(buf + strlen(PC_BEG), PC_SEPS);
	    break;
	    }
	}
    fclose(PC_FD);
    return(tbl);
}

