#if defined(SCRW32) || defined(DOSW32)

#include "scr4w.h"

/* ========================================================================
Installe un service Windows. Ce service est composé d'un programme
exécutable et d'une série de caractéristiques :

&EN char *ProgName : nom complet de l'exécutable
&EN char *ServiceName : nom symbolique de service (ex. "S4issrv")
&EN char *ServiceTitle : nom complet du service (ex. "Serveur ISAM SCR4/AL1")
&EN char *ServiceDescr : description du service (ex. "Serveur de bases et de fichier SCR4/AL1")
&EN char *autostart : 1 pour installer le service avec l'option auto start, 0 pour un démarrage manuel.

&EX
    for(i = 1; i < argc; i++) {
	if(strcmp(argv[i], "-install") == 0)
	    WscrServiceInstall("c:\\scr\\s4issrv.exe", "s4issrv", "Test S4ISSRV",
				"Serveur de Bases de donnees et de fichiers SCR/AL1", 1);
	else if(strcmp(argv[i], "-remove") == 0)
	    WscrServiceRemove("s4issrv");
	else if(strcmp(argv[i], "-start") == 0)
	    WscrServiceStart("s4issrv");
	else if(strcmp(argv[i], "-stop") == 0)
	    WscrServiceStop("s4issrv");
	}
&TX

&SA WscrServiceInstall(), WscrServiceRemove(), WscrServiceStart(), WscrServiceStop(), WscrServiceRun()
================================================================== */

WscrServiceInstall(char *ProgName, char *ServiceName, char *ServiceTitle,
		   char *ServiceDescr, int autostart)
{
    //char         szModulePathname[_MAX_PATH];
    SC_HANDLE    hService, hSCM;
    DWORD        dwStartType;
    int          rc = 0;
    SERVICE_DESCRIPTION sdBuf;


    // Open the SCM on this machine.
    hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if(hSCM == 0) return(-1);

    // GetModuleFileName(NULL, szModulePathname, dimof(szModulePathname));

    // Add this service to the SCM's database.
    if(ServiceTitle == 0) ServiceTitle = ServiceName;
    if(autostart == 1) dwStartType = SERVICE_AUTO_START;
    else               dwStartType = SERVICE_DEMAND_START;

    hService = CreateService(hSCM, ServiceName, ServiceTitle, 0,
      SERVICE_WIN32_OWN_PROCESS, dwStartType, SERVICE_ERROR_IGNORE,
      ProgName, NULL, NULL, NULL, NULL, NULL);

    if(hService == 0) {
	Debug(" [WscrServiceInstall] CreateService error %ld\n",GetLastError());
	rc = -1;
	}
    else
	if(ServiceDescr) {
	    CloseServiceHandle(hService);
	    hService = OpenService(hSCM, ServiceName, SERVICE_CHANGE_CONFIG);

	    sdBuf.lpDescription = ServiceDescr;
	    ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &sdBuf);
	    }

    // Close the service and the SCM
    if(hService) CloseServiceHandle(hService);
    if(hSCM)     CloseServiceHandle(hSCM);

    return(rc);
}

/* ========================================================================
Supprime (désinstalle) un service Windows.

&EN char *ServiceName : nom symbolique de service (ex. "S4issrv")

&EX
    voir WscrServiceInstall().
&TX

&SA WscrServiceInstall(), WscrServiceRemove(), WscrServiceStart(), WscrServiceStop(), WscrServiceRun()
================================================================== */

WscrServiceRemove(char *ServiceName)
{
   // Open the SCM on this machine.
   SC_HANDLE hService, hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);

   // Open this service for DELETE access
   hService = OpenService(hSCM, ServiceName, DELETE);

   // Remove this service from the SCM's database.
   DeleteService(hService);

   // Close the service and the SCM
   CloseServiceHandle(hService);
   CloseServiceHandle(hSCM);
}

/* ========================================================================
Démarre un service Windows.

&EN char *ServiceName : nom symbolique de service (ex. "S4issrv")

&EX
    voir WscrServiceInstall().
&TX

&SA WscrServiceInstall(), WscrServiceRemove(), WscrServiceStart(), WscrServiceStop(), WscrServiceRun()
================================================================== */
WscrServiceStart(char *ServiceName)
{
    SC_HANDLE       hService = 0, hSCM = 0;
    SERVICE_STATUS  ssStatus;
    int             rc = -1;
    DWORD           dwOldCheckPoint, dwStartTickCount, dwWaitTime, dwStatus;

    // Open the SCM on this machine.
    hSCM  = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if(hSCM == 0) return(-1);

    // Open this service for START access
    hService = OpenService(hSCM, ServiceName, SERVICE_START);
    if(hService == 0) goto err;

    // Start this service from the SCM's database.
    if(!StartService(hService, 0, NULL)) goto err;

    if(!QueryServiceStatus(hService, &ssStatus)) goto err;

    // Save the tick count and initial checkpoint.
    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;
    while(ssStatus.dwCurrentState == SERVICE_START_PENDING) {
	// Do not wait longer than the wait hint. A good interval is
	// one tenth the wait hint, but no less than 1 second and no
	// more than 10 seconds.
	dwWaitTime = ssStatus.dwWaitHint / 10;
	if(dwWaitTime < 1000)            dwWaitTime = 1000;
	else if(dwWaitTime > 10000)      dwWaitTime = 10000;
	Sleep( dwWaitTime );

	// Check the status again.
	if(!QueryServiceStatus(hService, &ssStatus)) break;
	if(ssStatus.dwCheckPoint > dwOldCheckPoint) {
	    // The service is making progress.
	    dwStartTickCount = GetTickCount();
	    dwOldCheckPoint = ssStatus.dwCheckPoint;
	    }
	else {
	    if(GetTickCount()-dwStartTickCount > ssStatus.dwWaitHint) break;
	    }
	}

    if(ssStatus.dwCurrentState == SERVICE_RUNNING) {
	Debug("StartService SUCCESS.\n");
	dwStatus = NO_ERROR;
	rc = 0;
	}
    else {
	Debug("\nService not started. \n");
	Debug("  Current State: %d\n", ssStatus.dwCurrentState);
	Debug("  Exit Code: %d\n", ssStatus.dwWin32ExitCode);
	Debug("  Service Specific Exit Code: %d\n",
	    ssStatus.dwServiceSpecificExitCode);
	Debug("  Check Point: %d\n", ssStatus.dwCheckPoint);
	Debug("  Wait Hint: %d\n", ssStatus.dwWaitHint);
	dwStatus = GetLastError();
	}

err:
    if(hService) CloseServiceHandle(hService);
    if(hSCM)     CloseServiceHandle(hSCM);

    return(rc);
}

/* ========================================================================
Stoppe un service Windows.

&EN char *ServiceName : nom symbolique de service (ex. "S4issrv")

&EX
    voir WscrServiceInstall().
&TX

&SA WscrServiceInstall(), WscrServiceRemove(), WscrServiceStart(), WscrServiceStop(), WscrServiceRun()
================================================================== */
WscrServiceStop(char *ServiceName)
{
    SC_HANDLE        hService = 0, hSCM = 0;
    SERVICE_STATUS   ssStatus;
    int              rc = -1;

    // Open the SCM on this machine.
    hSCM  = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if(hSCM == 0) return(-1);

    // Open this service for STOP access
    hService = OpenService(hSCM, ServiceName, SERVICE_STOP);
    if(hService == 0) goto err;

    // Stop this service
    if(!ControlService(hService, SERVICE_CONTROL_STOP, &ssStatus)) goto err;
    rc = 0;

err:
    if(hService) CloseServiceHandle(hService);
    if(hSCM)     CloseServiceHandle(hSCM);

    return(rc);
}

#endif
















