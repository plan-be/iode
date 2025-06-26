#if defined(SCRW32) || defined(DOSW32)
#include "scr4w.h"

int WDDE_TIMEOUT = 4000;
int WDDE_NB_TRIES = 5;

/* ============ DDE Client Part ================== */

DWORD   idInst = 0;

/* ===================================================================
Cette fonction détermine le schéma de gestion des TIMEOUT lors de
requêtes au serveur DDE.

&EN ~cnbsecs~C : durée du TIMEOUT en millisecondes
&EN ~cnbtries~C : nombre d'essais

Les essais seront cumulatifs : premier essai avec ~cnbsecs~C
millisecondes d'attente, deuxième avec 2 * ~cnbsecs~C ms d'attente, etc.

Par défaut, ces valeurs sont respectivement de 4000 msecs et 5 essais.

&SA WscrDdeConnect(), WscrDdeGet(), WscrDdeSet(), WscrDdeExecute()
==================================================================== */

WscrDdeSetTimeOut(int msecs, int nbtries)
{
    WDDE_TIMEOUT = msecs;
    WDDE_NB_TRIES = nbtries;
    return(0);
}

/* ===================================================================
Cette fonction établit une conversation avec un serveur DDE et exécute
des fonctions de ce serveur. Elle permet par exemple de lancer Word ou
Excel et de charger un fichier, de l'imprimer, puis de quitter le
programme.

&EN Service = identification du serveur
&EN Topic = sujet
&EN Cmd = commande à exécuter

&RT 0 en cas de succès, -1 en cas d'erreur de connexion au serveur
    ou de retour erronné du serveur.

&EX
    PrintWordFile(char *filename)
    {
	char    buf[256];

	sprintf(buf, "[FileOpen \"%s\"]", filename);
	WscrDdeExecute("winword", "system", buf);
	sprintf(buf, "[FilePrintSetup .Printer = \"%s\"]", txt);
	WscrDdeExecute("winword", "system", buf);
	sprintf(buf, "[FilePrint .FileName = \"%s\"]", filename);
	WscrDdeExecute("winword", "system", buf);
	WscrDdeExecute("winword", filename, "[FileClose 2]");
    }
&TX

&SA WscrDdeConnect(), WscrDdeGet(), WscrDdeSet()
==================================================================== */

WscrDdeExecute(Service, Topic, Cmd)
char *Service, *Topic;
unsigned char *Cmd;
{
    HCONV           hConv;
    int             rc;                    /* JMP 25-10-97 */

    hConv = WscrDdeConnect(Service, Topic);
    if(hConv == NULL) return(-1);
    rc = WscrDdeExecuteCmd(hConv, Cmd);    /* JMP 25-10-97 */
    WscrDdeEnd(hConv) ;
    return(rc);                            /* JMP 25-10-97 */
}

/*NH*/
WscrDdeExecuteCmd(hConv, cmd)
HCONV   hConv;
unsigned char* cmd;
{
    DWORD       dwResult;
    int         i;

    if(hConv == NULL || cmd == NULL) return(-1);
    SCR_strip(cmd);

    /* Execute */
    for(i = 0 ; i < WDDE_NB_TRIES ; i++) {
	DdeClientTransaction(cmd, (DWORD) (strlen(cmd) + 1), hConv, NULL, CF_TEXT,
	    XTYP_EXECUTE, (i + 1) * WDDE_TIMEOUT, &dwResult);
	if(DdeGetLastError(idInst) == DMLERR_DATAACKTIMEOUT) continue;
	else break;
	}

    return(0);                               /* JMP 25-10-97 */
}

/* ===================================================================
Cette fonction termine une conversation démarrée par WscrDdeConnect().

&SA WscrDdeConnect(), WscrDdeGetItem() et WscrDdeSetItem()
==================================================================== */

WscrDdeEnd(hConv)
HCONV   hConv;
{
    DdeDisconnect(hConv);
    DdeUninitialize(idInst) ;
    idInst = 0;
    return(0);
}

/* ===================================================================
Cette fonction interroge un serveur préalablement connecté par
WscrDdeConnect() et demande l'information décrite dans szItem.

&EN hConv = handle de conversation obtenu par WscrDdeConnect()
&EN szItem = spécification des données demandées.

&RT La valeur de retour est un pointeur alloué contenant la valeur
    demandée ou NULL en cas d'erreur.

&SA WscrDdeConnect(), WscrDdeSetItem(), WscrDdeEnd(), WscrDdeGet()
==================================================================== */

unsigned char* WscrDdeGetItem(hConv, szItem)
HCONV   hConv;
char    *szItem;
{
    unsigned char   *ptr = NULL;

    HSZ         hszItem;
    DWORD       dwResult, dwLength;
    HDDEDATA    hDDEData;
    BYTE FAR*   pData;
    int         ntr;

    // Request notification
    if(hConv == NULL || szItem == NULL) return(NULL);
    SCR_strip(szItem);

    hszItem = DdeCreateStringHandle (idInst, szItem, 0) ;

    for(ntr = 0 ; ntr < WDDE_NB_TRIES ; ntr++) {
	hDDEData = DdeClientTransaction (
			NULL, 0, hConv, hszItem, CF_TEXT,
			XTYP_REQUEST, (1 + ntr) * WDDE_TIMEOUT, &dwResult);
	if(DdeGetLastError(idInst) == DMLERR_DATAACKTIMEOUT) continue;
	else break;
	}

    DdeFreeStringHandle (idInst, hszItem) ;

    if(hDDEData) {
	pData = DdeAccessData(hDDEData, &dwLength);
	ptr = SCR_malloc((int) dwLength);
	strcpy(ptr, pData);
	DdeUnaccessData(hDDEData);
	DdeFreeDataHandle(hDDEData);
    }


    return(ptr);
}

/* ===================================================================
Cette fonction envoie une donnée vers un serveur préalablement connecté
par WscrDdeConnect().

&EN hConv = handle de conversation obtenu par WscrDdeConnect()
&EN szItem = spécification des données à fixer par le serveur
&EN ptr = pointeur vers le string à envoyer vers le serveur

&RT 0 en cas de succès, -1 en cas d'erreur

&SA WscrDdeConnect(), WscrDdeGetItem(), WscrDdeEnd(), WscrDdeSet()
==================================================================== */

WscrDdeSetItem(hConv, szItem, ptr)
HCONV   hConv;
char    *szItem;
unsigned char* ptr;
{
    HSZ         hszItem;
    DWORD       dwResult;
    int         i;

    if(hConv == NULL || szItem == NULL || ptr == NULL) return(-1);
    SCR_strip(szItem);
    SCR_strip(ptr);

    // Request notification
    hszItem = DdeCreateStringHandle (idInst, szItem, 0) ;
    for(i = 0 ; i < WDDE_NB_TRIES ; i++) {
	DdeClientTransaction (ptr, (DWORD)(strlen(ptr) + 1), hConv, hszItem,
	    CF_TEXT, XTYP_POKE, (i + 1) * WDDE_TIMEOUT, &dwResult) ;
	if(DdeGetLastError(idInst) == DMLERR_DATAACKTIMEOUT) continue;
	else break;
	}

    DdeFreeStringHandle (idInst, hszItem) ;

    return(0);
}

/* ===================================================================
Cette fonction établit une conversation avec un serveur, effectue une
requête et coupe la conversation.

&EN szService = identification du serveur
&EN szTopic = sujet
&EN szItem = spécification des données

&RT pointeur alloué vers le résultat de la requête ou NULL en cas d'erreur

&EX
    char *cells;

    cells = WscrDdeGet("EXCEL", "Sheet2", "R2C2:R3C5");
    ...

    SCR_free(cells);
&TX

&SA WscrDdeConnect(), WscrDdeSet()
==================================================================== */

unsigned char* WscrDdeGet(szService, szTopic, szItem)
char *szService, *szTopic, *szItem;
{
    HCONV           hConv;
    unsigned char   *ptr;

    hConv = WscrDdeConnect(szService, szTopic);
    if(hConv == NULL) return(NULL);
    ptr = WscrDdeGetItem(hConv, szItem);
    WscrDdeEnd(hConv) ;
    return(ptr);
}

/* ===================================================================
Cette fonction établit une conversation avec un serveur, envoie des
données au serveur et coupe la conversation.

&EN szService = identification du serveur
&EN szTopic = sujet
&EN szItem = spécification des données à fixer
&EN ptr = valeur des données

&RT 0 en cas de succès, -1 en cas d'erreur

&EX
    char *cells;

    cells = WscrDdeSet("EXCEL", "Sheet2", "R2C2:R2C2", "1.23");
    ...

    SCR_free(cells);
&TX

&SA WscrDdeConnect(), WscrDdeGet()
==================================================================== */

WscrDdeSet(szService, szTopic, szItem, ptr)
unsigned char *ptr;
char *szService, *szTopic, *szItem;
{
    HCONV           hConv;

    hConv = WscrDdeConnect(szService, szTopic);
    if(hConv == NULL) return(0);
    WscrDdeSetItem(hConv, szItem, ptr);
    WscrDdeEnd(hConv) ;
    return(0);
}

/* ===================================================================
Cette fonction démarre une conversation avec un serveur DDE. Elle
indique le service et le topic pour lesquels une ou des requêtes vont
être effectuées.

Cette fonction doit être utilisée si plusieurs requêtes consécutives
doivent être lancées. Si ce n'est pas le cas, on préfèrera les fonctions
WscrDdeGet() et WscrDdeSet().

&RT Handle de conversation qui devra être passé aux fonctions
    WscrDdeEnd(), WscrDdeGetItem() et WscrDdeSetItem().


&SA WscrDdeEnd(), WscrDdeGetItem() et WscrDdeSetItem()
==================================================================== */

HCONV WscrDdeConnect(szService, szTopic)
char *szService, *szTopic;
{
    HCONV   hConv = NULL;
    HSZ     hszService, hszTopic;
    char    *ptr, buf[255], *getenv();    /* BP_MODIF 18-12-98 */
    UINT    ScrDdeError;

    if(szService == NULL || szTopic == NULL) return(NULL);
    SCR_strip(szService);
    SCR_strip(szTopic);

    // Initialize for using DDEML
    // if(idInst == NULL) {
    if(idInst == 0) {
	if (DdeInitialize (&idInst, (PFNCALLBACK) &DdeCallback,
			APPCLASS_STANDARD | APPCMD_CLIENTONLY, 0L))
	 return(hConv) ;
    }

    // Try connecting
    hszService = DdeCreateStringHandle (idInst, szService, 0) ;
    hszTopic   = DdeCreateStringHandle (idInst, szTopic,   0) ;

    hConv = DdeConnect (idInst, hszService, hszTopic, NULL) ;
    // If that doesn't work, load server
    if (hConv == NULL) {
	/* BP_MODIF 18-12-98 */
	ptr = getenv("AEROWINWORD");
	if(ptr != 0) strcpy(buf, ptr);
	else         strcpy(buf, szService);
	WinExec (buf, SW_SHOWMINNOACTIVE) ;
	hConv = DdeConnect (idInst, hszService, hszTopic, NULL) ;
    }
    if(hConv == NULL)
	ScrDdeError = DdeGetLastError(idInst);

    // Free the string handles
    DdeFreeStringHandle (idInst, hszService) ;
    DdeFreeStringHandle (idInst, hszTopic) ;

    // If still not connected, display message box
    return(hConv);
}

/*NH
HCONV WscrDdeConnect(szService, szTopic)
char *szService, *szTopic;
{
    HCONV   hConv = NULL;
    HSZ     hszService, hszTopic;
    int     nbtest = 0;
    char    *ptr, buf[255];

    if(szService == NULL || szTopic == NULL) return(NULL);
    SCR_strip(szService);
    SCR_strip(szTopic);

    // Initialize for using DDEML
    if(idInst == 0) {
	if (DdeInitialize (&idInst, (PFNCALLBACK) &DdeCallback,
			APPCLASS_STANDARD | APPCMD_CLIENTONLY, 0L))
	 return(hConv) ;
    }

    // Try connecting
    hszService = DdeCreateStringHandle (idInst, szService, 0) ;
    hszTopic   = DdeCreateStringHandle (idInst, szTopic,   0) ;

    while(1) {
	hConv = DdeConnect (idInst, hszService, hszTopic, NULL) ;
	// If that doesn't work, load server
	if (hConv == NULL) {
	    if(nbtest == 3) {
		/* BP_M 27-10-97
		ptr = getenv("AEROWINWORD");
		if(ptr != 0) strcpy(buf, ptr);
		else         strcpy(buf, szService);
		/* BP_M 27-10-97

		WinExec (buf, SW_SHOWMINNOACTIVE) ;
		Sleep(1000);
		hConv = DdeConnect (idInst, hszService, hszTopic, NULL) ;
		if(hConv == NULL) {
		    nbtest = 0;
		    continue;
		}
		break;
	    }
	    nbtest++;
	    Sleep(1000);
	    continue;
	}
	else
	    break;

    }

    // Free the string handles
    DdeFreeStringHandle (idInst, hszService) ;
    DdeFreeStringHandle (idInst, hszTopic) ;

    // If still not connected, display message box
    return(hConv);
}

*/

/*NH*/
HDDEDATA CALLBACK DdeCallback (UINT iType, UINT iFmt, HCONV hConv,
			       HSZ hsz1, HSZ hsz2, HDDEDATA hData,
			       DWORD dwData1, DWORD dwData2)
{
     switch (iType)
	  {
	  case XTYP_ADVDATA :       // hsz1  = topic
				    // hsz2  = item
				    // hData = data

	       return (HDDEDATA) DDE_FACK ;

	  case XTYP_DISCONNECT :
	       hConv = NULL ;
//             PG_display_error("The server has disconnected"); /* BP_MODIF 23-12-98 */
	       return NULL ;
	  }

     return NULL ;
}

/*NH*/
/*
WscrDdeAddHotItem(hConv, szItem)
HCONV   hConv;
char    *szItem;
{
    HSZ        hszItem;
    int         i;

    // Request notification
    hszItem = DdeCreateStringHandle (idInst, szItem, 0) ;

    for(i = 0 ; i < WDDE_NB_TRIES ; i++) {
	DdeClientTransaction (NULL, 0, hConv, hszItem, CF_TEXT,
		XTYP_ADVSTART | XTYPF_ACKREQ,
		(i + 1) * WDDE_TIMEOUT, NULL) ;
	if(DdeGetLastError(idInst) == DMLERR_DATAACKTIMEOUT) continue;
	else break;
	}

    DdeFreeStringHandle (idInst, hszItem) ;
}

WscrDdeDeleteHotItem(hConv, szItem)
HCONV   hConv;
char *szItem;
{
    HSZ        hszItem;
    int         i;

    hszItem = DdeCreateStringHandle (idInst, szItem, 0) ;

    for(i = 0 ; i < WDDE_NB_TRIES ; i++) {
	DdeClientTransaction(NULL, 0, hConv, hszItem, CF_TEXT,
	    XTYP_ADVSTOP, (i + 1) * WDDE_TIMEOUT, NULL) ;
	if(DdeGetLastError(idInst) == DMLERR_DATAACKTIMEOUT) continue;
	else break;
	}

    DdeFreeStringHandle(idInst, hszItem) ;
}
*/


#else

WscrDdeSetTimeOut(int msecs, int nbtries)
{
    return(0);
}

WscrDdeExecute(Service, Topic, Cmd)
char *Service, *Topic;
unsigned char *Cmd;
{
    return(-1);
}

WscrDdeExecuteCmd(hConv, cmd)
long  hConv;
unsigned char* cmd;
{
    return(-1);
}

WscrDdeEnd(hConv)
long   hConv;
{
    return(-1);
}

unsigned char* WscrDdeGetItem(hConv, szItem)
long  hConv;
char    *szItem;
{
    return(0);
}

WscrDdeSetItem(hConv, szItem, ptr)
long   hConv;
char    *szItem;
unsigned char* ptr;
{
    return(-1);
}

unsigned char* WscrDdeGet(szService, szTopic, szItem)
char *szService, *szTopic, *szItem;
{
    return(0);
}

WscrDdeSet(szService, szTopic, szItem, ptr)
unsigned char *ptr;
char *szService, *szTopic, *szItem;
{
    return(0);
}

#endif
