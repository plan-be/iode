#include "s_strs.h"
#include <stdio.h>

extern FILE     *PC_FD;

/* ====================================================================
Recherche dans le fichier filename la définition de l'item prname et
exécute les commandes associés en remplaçant les paramètres par les
valeurs arg1, arg2 et arg3.

Le fichier est décrit dans la fonction PC_list().

&RT 0 en cas de succès, -1 si l'item n'est pas trouvé
&EX
    PC_exec("printcap", "ps401", "toto.ps", NULL, NULL);
&TX
&SA PC_infos(), PC_list(), SCR_free_tbl();
=======================================================================*/

PC_exec(filename, prname, arg1, arg2, arg3)
char    *filename, *prname, *arg1, *arg2, *arg3;
{
    char    buf[200];
    int     type;

    if(PC_open(filename)) return(-1);

    while(1) {
	type = PC_getline(prname, buf);
	if(type < 0) break;
	if(type > 0) continue;
	while(1) {
	    if(PC_getline("", buf) != 2) {
		fclose(PC_FD);
		return(0);
		}
	    if(buf[0] == '#') continue;
	    if(arg1) SCR_replace(buf, "%1", arg1);
	    if(arg2) SCR_replace(buf, "%2", arg2);
	    if(arg3) SCR_replace(buf, "%3", arg3);

	    system(buf);
	    }
	}
    fclose(PC_FD);
    return(-1);
}

