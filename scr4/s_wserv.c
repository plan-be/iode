#if defined(SCRW32) || defined(DOSW32)
#include "scr4w.h"

SERVICE_STATUS_HANDLE   SRV_STAT_HD = {0};
SERVICE_STATUS          SRV_STAT = {0};
int                     SRV_RUNNING = 0, SRV_FNRUNNING = 1;
int                     (*SRV_FN)();
int                     SRV_DEBUG = 0;
char                    SRV_NAME[80];

/*NH*************************************************************************
 This routine is the callback from the SCManager to handle specific
 service control requests. It MUST call SetServiceStatus before it
 returns, regardless of whether the status has changed.

 Inputs: service control requested

 Outputs: none
 ****************************************************************************/

void WINAPI WscrControlHandler(DWORD Opcode)
{
    int         i;

    if(SRV_DEBUG) Debug("[%s] Entering ControlHandler(%d)\n", SRV_NAME, Opcode);

    switch(Opcode) {
	case SERVICE_CONTROL_PAUSE:
	    // Do whatever it takes to pause here.
	    SRV_STAT.dwCurrentState = SERVICE_PAUSED;
	    break;

	case SERVICE_CONTROL_CONTINUE:
	    // Do whatever it takes to continue here.
	    SRV_STAT.dwCurrentState = SERVICE_RUNNING;
	    break;

	case SERVICE_CONTROL_SHUTDOWN:
	case SERVICE_CONTROL_STOP :
	    // Do whatever it takes to stop here.
	    SRV_STAT.dwWin32ExitCode = 0;
	    SRV_STAT.dwCurrentState = SERVICE_STOP_PENDING;
	    SRV_STAT.dwCheckPoint    = 0;
	    SRV_STAT.dwWaitHint      = 2000;
	    if(!SetServiceStatus(SRV_STAT_HD, &SRV_STAT))
		Debug(" [WscrControlhandler] SetServiceStatus error %ld\n", GetLastError());

	    SRV_RUNNING = i = 0;
	    while(SRV_FNRUNNING && i++ < 100 ) Sleep(50);
	    if(SRV_DEBUG) Debug("[%s] Stopping Service\n", SRV_NAME);
	    //SRV_STAT.dwCurrentState = SERVICE_STOPPED;
	    return;

	case SERVICE_CONTROL_INTERROGATE:
	    // Fall through to send current status.
	    break;

	default:
	    Debug(" [WscrControHandler] Unrecognized opcode %ld\n", Opcode);
	    break;
	}

    // Send current status.
    if(!SetServiceStatus(SRV_STAT_HD, &SRV_STAT)) {
	Debug(" [WscrControlHandler] SetServiceStatus error %ld\n",GetLastError());
	}

/*    if(Opcode == SERVICE_CONTROL_STOP || Opcode== SERVICE_CONTROL_SHUTDOWN) {
	SRV_STAT.dwWin32ExitCode = 0;
	SRV_STAT.dwCurrentState  = SERVICE_STOPPED;
	SRV_STAT.dwCheckPoint    = 0;
	SRV_STAT.dwWaitHint      = 0;
	if(!SetServiceStatus(SRV_STAT_HD, &SRV_STAT)) {
	    Debug(" [MY_SERVICE] SetServiceStatus error %ld\n",GetLastError());
	    }
	Debug(" [MY_SERVICE] Leaving MyService \n");
	}
*/
    return;
}

/*NH*/
void WINAPI WscrServiceMain(DWORD dwNumServiceArgs, LPTSTR * lpServiceArgs)
{
    DWORD rc;

    if(SRV_DEBUG) Debug("[%s] Entering ServiceMain\n", SRV_NAME);
    SRV_STAT_HD = RegisterServiceCtrlHandler(SRV_NAME, WscrControlHandler);
    if(SRV_STAT_HD == 0) {
	Debug("Error RegisterServiceCtrlHandler : %d\n", GetLastError());
	return;
	}

    /* Let the SCManager know that we are running. */
    SRV_STAT.dwServiceType              = SERVICE_WIN32_OWN_PROCESS;
    SRV_STAT.dwCurrentState             = SERVICE_RUNNING;
    SRV_STAT.dwControlsAccepted         = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    SRV_STAT.dwWin32ExitCode            = NO_ERROR;
    SRV_STAT.dwServiceSpecificExitCode  = NO_ERROR;
    SRV_STAT.dwCheckPoint               = 0;
    SRV_STAT.dwWaitHint                 = 0;

    if(!SetServiceStatus(SRV_STAT_HD, &SRV_STAT)) {
	Debug(" [WscrServiceMain] SetServiceStatus error %ld\n",GetLastError());
	return;
	}

    /* Call user fn */
    SRV_RUNNING = 1;
    if(SRV_DEBUG) Debug("[%s] Entering User Function\n", SRV_NAME);
    rc = (*SRV_FN)();
    if(SRV_DEBUG) Debug("[%s] User Function ended\n", SRV_NAME);

    /* Set service status */
    SRV_STAT.dwWin32ExitCode = rc;
    SRV_STAT.dwCurrentState = SERVICE_STOPPED;
    if(!SetServiceStatus(SRV_STAT_HD, &SRV_STAT))
	Debug(" [WscrServiceMain] SetServiceStatus error %ld\n",GetLastError());

    if(SRV_DEBUG) Debug("[%s] Service Stopped\n", SRV_NAME);
}
/* ========================================================================
Lancement d'un service. Il s'agit de la fonction qui exécute le service (et
pas de celle qui démarre le service). Le service doit être installé.

Cette fonction doit être lancée par le programme principal du service.


&EN char *ServiceName : nom symbolique de service (ex. "S4issrv") qui doi
&EN int  (*fn)() : pointeur vers la fonction utilisateur qui exécute le service lui-même.

&IT Fonction utilisateur
────────────────────────
La fonction utilisateur est la fonction principale du service. Elle ne prend
pas d'argument. Sa valeur de retour sera le code retour du service.

Le service étant géré par le Service Control Manager (SCM), il faut un
minimum de collaboration entre la fonction utilisateur et le SCM.

Pour un maximum de simplicité, cette collaboration est établie (dans
SCR/AL1) via deux variables partagées :

&EN int SRV_RUNNING gérée par le SCM qui indique l'état du service (démarré ou
    stoppé). Au moment de l'appel de la fonction utilisateur, SRV_RUNNING
    vaut toujours 1.

&EN int SRV_FNRUNNING gérée par la fonction utilisateur qui indique si la
    fonction est toujours en cours.

Ainsi,

&EN la fonction utilisateur sait si elle doit s'arrêter (dès que
    SRV_RUNNING devient 0, ce qui indique que le service demande l'arrêt)

&EN le SCM attend que SRV_FNRUNNING soit nulle (ce qui indique que la
    fonction s'est terminée correctement)

&EX
int MyServiceFunction()
{
    long        i = 0L;

    SRV_FNRUNNING = 1;
    while(1) {
	Debug("Passage %d\n", ++i);
	Sleep(500);
	if(SRV_RUNNING == 0) break; // SCM veut stopper le service
	}

    SRV_FNRUNNING = 0; // SCM peut s'arrêter
    Debug("End of MyServiceFunction\n");
    return(0);
}

&TX

&SA WscrServiceInstall(), WscrServiceRemove(), WscrServiceStart(), WscrServiceStop()
================================================================== */

WscrServiceRun(char ServiceName[], int (*fn)())
{
    SRV_FN = fn;
    strcpy(SRV_NAME, ServiceName);

	{
	SERVICE_TABLE_ENTRY ServiceTable[] = {
	    {SRV_NAME, WscrServiceMain},
	    {NULL,        NULL }            // End of list
	    };

	if(SRV_DEBUG) Debug("[%s] Starting Dispatcher\n", SRV_NAME);
	StartServiceCtrlDispatcher(ServiceTable);
	}
    return(0);
}

/*NH

MyServiceFunction()
{
    long        i = 0L;

    SRV_FNRUNNING = 1;
    DebugActif = 1;
    Debug("Entering MyServiceFunction\n");
    while(1) {
	Debug("Passage %d\n", ++i);
	Sleep(1000);
	if(i > 20 || SRV_RUNNING == 0) break;
	}

    SRV_FNRUNNING = 0;
    Debug("End of MyServiceFunction\n");
    return(0);
}

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hinstExePrev, LPSTR pszCmdLine, int nCmdShow)
{
    int  argc   = __argc;
    char **argv = __argv, *szAppName = "s_wserv";
    int  i;
    char        ProgName[256];

    GetModuleFileName(NULL, ProgName, sizeof(ProgName));

    for(i = 1; i < argc; i++) {
	if(argv[i][0] == '-') {
	    // Command line switch
		 if(strcmp(argv[i], "-install") == 0)
		    WscrServiceInstall(ProgName, szAppName, "Test S4ISSRV", "Serveur de Bases de donnees et de fichiers SCR/AL1", 0);
	    else if(strcmp(argv[i], "-remove") == 0)
		    WscrServiceRemove(szAppName);
	    else if(strcmp(argv[i], "-start") == 0)
		    WscrServiceStart(szAppName);
	    else if(strcmp(argv[i], "-stop") == 0)
		    WscrServiceStop(szAppName);
	    else if(strcmp(argv[i], "-debug") == 0)
		    SRV_DEBUG = 1;
	    }
	}
    if(argc < 2) WscrServiceRun(szAppName, MyServiceFunction);
    return(0);
}
*/

#endif



























