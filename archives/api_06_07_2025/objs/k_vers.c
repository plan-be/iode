/*
 * @header4iode
 *
 * Functions to retrieve the current IODE version.
 *
 *    char *K_LastVersion(): returns in an allocated string the current version of IODE. 
 *    char *K_CurrentVersion(): returns in an allocated string the IODE version of the current executable.
 */
#include "scr4/s_strs.h"

#include "api/constants.h"
#include "api/objs/vers.h"


/********************************************************
Retourne un string alloué contenant le contenu du fichier
 version.txt sur le site iode.plan.be.
*********************************************************/

/**
 *  Returns in an allocated string the current version of IODE. 
 *  That string is retrieved in the file http://iode.plan.be/version.txt which contains "IODE <major>.<minor>" 
 *  with the latest version of IODE.
 *  
 *  @return     char* last version of IODE on the web site.
 *  
 *  @note   The function SCR_HttpGetFile("iode.plan.be", 80, "version.txt") does not work as expected. Therefore, 
 *          the value returned is the current IODE version (K_CusrrentVersion()) as of version 6.xx.
 *  
 *   TODO: specify the version (6.xx) from which the file could no longer be read from the website.
 */
char *K_LastVersion()
{
    //char *vers;

    /* Annulé car le retour http ne fonctionne plus comme avant.
    vers = SCR_HttpGetFile("iode.plan.be", 80, "version.txt");

    //Debug("iode version => '%s'\n", vers);
    if(vers) {
    	if(strlen(vers) > 9) vers[9] = 0;
    	return(vers);
    }
    return(K_CurrentVersion());
    */

    return(K_CurrentVersion());
}


/**
 *  Returns in an allocated string the IODE version of the current executable in the form 
 *      "IODE <major>.<minor>" where minor and major come from iode.ini. 
 *  
 *  @return     char* current version of IODE
 *   
 */

char *K_CurrentVersion()
{
    char buf[30];

    sprintf(buf, "IODE %d.%d", IODE_VERSION_MAJOR, IODE_VERSION_MINOR);
    return(SCR_stracpy(buf));
}

