#if !defined(DOSWIN) && !defined(SCRW32)

/*NH*/
WscrDdeServer()
{
    return(-1);
}

/*NH*/
WscrDdeServerEnd()
{
    return(-1);
}

#else

#include "scr4w.h"

/* ============ DDE Server Part ================== */

DWORD   idSrvInst = 0;

char    *WDdeSrvService = NULL;
char    **WDdeSrvTopic = NULL;
int     WDdeSrvNbTopic = 0;
HSZ     hszSrvService;

#ifdef SCRPROTO
char    *(*WDdeRequestFn)(char *, char *);
int     (*WDdePokeFn)(char *, char *, char *);
#else
char    *(*WDdeRequestFn)();
int     (*WDdePokeFn)();
#endif

/*=====================================================================
Fonction d'initialisation d'un serveur DDE. Cette fonction n'est
exploitable que sous Windows 32.

Cette fonction doit ˆtre lanc‚e dŠs que le serveur peut ˆtre appel‚. En
g‚n‚ral, elle est ex‚cut‚e dans WscrMain().

Ses paramŠtres indiquent … toutes les autres applications les services
qui sont rendus disponibles et les fonctions utilisateur … utiliser pour
r‚pondre aux requˆtes.

&EN service = nom du service. En g‚n‚ral, le service a pour nom
    le programme lui-mˆme.

&EN topic = tableau de strings reprenant tous les topics qui peuvent
    ˆtre exploit‚s par le serveur. Ces informations sont rendues
    de la sorte disponibles pour tous les autres applicatifs.

&EN Request = pointeur vers la fonction utilisateur exploit‚e lorsqu'une
    requˆte est faite au serveur

&EN Poke = pointeur vers une fonction utilisateur ex‚cut‚e lorsqu'un applicatif
    veut ex‚cut‚e une requˆte de type "poke", c'est-…-dire passer de
    l'information au serveur en vue par exemple d'un enregistrement par
    celui-ci.

&TI Fonction Request()
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

La fonction Request() r‚pond … une demande: elle renvoie normalement
une valeur au programme appelant. Mais rien n'empˆche de lui faire
ex‚cuter n'importe quelle fonction dans le programme serveur.

La fonction Request() a pour syntaxe :

&CO
    char *Request(char *topic, char *item)
&TX

Le topic est n‚cessairement l'un de ceux pass‚s comme paramŠtre
d'initialisation du serveur (le systŠme filtre les topics inexistants).
L'item est n'importe quel texte pass‚ par le programme appelant.

Le r‚sultat de la fonction doit ˆtre nul est cas d'erreur ou un string
allou‚ en cas de r‚ponse. Ce string est lib‚r‚ par le systŠme et doit
donc imp‚rativement ˆtre allou‚.


&IT Exemple
ÄÄÄÄÄÄÄ

&CO
    char *MyRequest(char *topic, char *item)
    {
	char    *ptr;

	if(strcmp(topic, "CLIENTNAME") == 0) {
	    is_client_NUM = atol(item);
	    if(IS_search(is_client, 0, SCR_EQ, (char *)0) == 0) {
		ptr = SCR_malloc(40);
		strcpy(ptr, is_client_NAME);
		return(ptr);
		}
	    else return((char *)0);
	    }
	....

    }
&TX

&TI Fonction Poke()
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

L'objectif de la fonction Poke est de changer des valeurs dans le
programme serveur, par exemple en ajoutant un record ou en modifiant
un champ.

La fonction Poke() a pour syntaxe :

&CO
    int Poke(char *topic, char *item, char *data)
&TX

Le topic est l'un de ceux pass‚s comme paramŠtre d'initialisation du
serveur. L'item est n'importe quel texte pass‚ par le programme
appelant. data est la valeur modifi‚e ou … fixer.

Le r‚sultat de la fonction doit ˆtre 0 en cas de succŠs, -1 en cas
d'erreur.

&IT Exemple
ÄÄÄÄÄÄÄ

&CO
    int Poke(char *topic, char *item, char *data)
    {
	if(strcmp(topic, "CLIENTNAME") == 0) {
	    is_client_NUM = atol(item);
	    if(IS_search(is_client, 0, SCR_EQ, (char *)0) == 0) {
		SCR_set_text(is_client_name, data);
		return(IS_rewrite(is_client));
		}
	    else return(-1);
	    }
	....

    }
&TX

&SA WscrDdeServerEnd()
==================================================================== */

WscrDdeServer(
char    *service,
char    **topic,
char    *(*Request)(char *, char *),
int     (*Poke)(char *, char *, char *)
)
{
    int i;

    if(service == NULL || topic == NULL) return(-1);
    SCR_strip(service);

    // Initialize for using DDEML Server
    if(idSrvInst == 0) {
	if (DdeInitialize (&idSrvInst, (PFNCALLBACK) &DdeSrvCallback,
			CBF_FAIL_EXECUTES |
			CBF_SKIP_REGISTRATIONS | CBF_SKIP_UNREGISTRATIONS,
			0L))
	    return(-1);
    }

    WDdeSrvService = SCR_stracpy(service);
    hszSrvService = DdeCreateStringHandle (idSrvInst, service, 0) ;
    DdeNameService (idSrvInst, hszSrvService, NULL, DNS_REGISTER) ;

    if(topic != NULL) {
	for(i = 0; topic[i]; i++)
	    SCR_add_ptr((U_ch ***)&WDdeSrvTopic, &WDdeSrvNbTopic, topic[i]);
    }
    SCR_add_ptr((U_ch ***)&WDdeSrvTopic, &WDdeSrvNbTopic, NULL);
    WDdeSrvNbTopic--;


    WDdeRequestFn = Request;
    WDdePokeFn = Poke;
    return(0);
}

/* ===================================================================
Cette fonction termine le service initialis‚ par WscrDdeServer().
Elle est ex‚cut‚e normalement dans le fonction WscrEnd().

&SA WscrDdeServer()
==================================================================== */

WscrDdeServerEnd()
{
    if(idSrvInst == 0) return(-1);

    DdeNameService(idSrvInst, hszSrvService, NULL, DNS_UNREGISTER);
    DdeFreeStringHandle(idSrvInst, hszSrvService) ;
    idSrvInst = 0;

    DdeUninitialize(idSrvInst) ;
    SCR_free(WDdeSrvService);
    SCR_free_tbl((U_ch **)WDdeSrvTopic);
    return(0);
}

/*NH*/
HDDEDATA CALLBACK DdeSrvCallback (UINT iType, UINT iFmt, HCONV hConv,
			       HSZ hsz1, HSZ hsz2, HDDEDATA hData,
			       DWORD dwData1, DWORD dwData2)
{
    char        *szBuffer, szService[256], szTopic[256],szItem[256];
    char        **items, *DdeGetItem();
    int         i, nbitem, lg;
    HDDEDATA    hddeData = 0;

    switch (iType) {
    case XTYP_CONNECT :
	// hsz2  = service
	DdeQueryString(idSrvInst, hsz2, szService, sizeof(szService), 0);
	if(strcmp(szService, WDdeSrvService) != 0) return(FALSE);

	// hsz1  = topic
	DdeQueryString(idSrvInst, hsz1, szTopic, sizeof(szTopic), 0);
	for(i = 0; i < WDdeSrvNbTopic; i++)
	    if(strcmp(szTopic, WDdeSrvTopic[i]) == 0) break;
	if(i == WDdeSrvNbTopic) return(FALSE);

	return((HDDEDATA)TRUE);

    case XTYP_REQUEST :
    case XTYP_ADVREQ  :
	if(WDdeRequestFn == 0) return(DDE_FNOTPROCESSED);
	if(iFmt != CF_TEXT) return(DDE_FNOTPROCESSED);

	// hsz1  = topic
	DdeQueryString(idSrvInst, hsz1, szTopic, sizeof(szTopic), 0);

	// hsz2  = item
	DdeQueryString (idSrvInst, hsz2, szItem, sizeof(szItem), 0);
	szBuffer = (*WDdeRequestFn)(szTopic, szItem);
	if(szBuffer == 0) return(DDE_FNOTPROCESSED);

	hddeData = DdeCreateDataHandle(idSrvInst,
		(unsigned char *) szBuffer, strlen(szBuffer) + 1,
		0, hsz2, CF_TEXT, 0);
	SCR_free(szBuffer);
	return(hddeData);

    case XTYP_POKE :
	if(WDdePokeFn == 0) return(DDE_FNOTPROCESSED);
	if(iFmt != CF_TEXT) return(DDE_FNOTPROCESSED);

	// hsz1  = topic
	DdeQueryString(idSrvInst, hsz1, szTopic, sizeof(szTopic), 0);

	// hsz2  = item
	DdeQueryString (idSrvInst, hsz2, szItem, sizeof(szItem), 0);

	lg = DdeGetData(hData, NULL, 0, 0);
	szBuffer = SCR_malloc(lg + 1);
	DdeGetData(hData, szBuffer, lg, 0);
	(*WDdePokeFn)(szTopic, szItem, szBuffer);
	SCR_free(szBuffer);
	return((HDDEDATA)DDE_FACK);
    }

    return(NULL);
}

#endif



