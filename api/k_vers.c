#include "iode.h"

/********************************************************
Retourne un string alloué contenant le contenu du fichier
 version.txt sur le site iode.plan.be.
*********************************************************/

char *K_LastVersion()
{
	char *vers;

	/* Annulé car le retour http ne fonctionne plus comme avant.
    vers = SCR_HttpGetFile("iode.plan.be", 80, "version.txt");
    
    Debug("iode version => '%s'\n", vers);
	if(vers) {
		if(strlen(vers) > 9) vers[9] = 0;
		return(vers);
	}
	return(K_CurrentVersion());
    */
    
    return(K_CurrentVersion());
}

/********************************************************
Retourne un string alloué contenant la version courante de IODE
"IODE 6.45" par exemple.
*********************************************************/

char *K_CurrentVersion()
{
	char buf[30];

	sprintf(buf, "IODE %d.%d", IODE_VERSION_MAJOR, IODE_VERSION_MINOR);
	return(SCR_stracpy(buf));
}

