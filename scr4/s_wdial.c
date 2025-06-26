#if defined(SCRW32) || defined(DOSW32)
#include <windows.h>
#include <ras.h>

/******************** RAS ERROR CODES ***************************
   600     An operation is pending.
   601     The port handle is invalid.
   602     The port is already open.
   603     Caller's buffer is too small.
   604     Wrong information specified.
   605     Cannot set port information.
   606     The port is not connected.
   607     The event is invalid.
   608     The device does not exist.
   609     The device type does not exist.
   610     The buffer is invalid.
   611     The route is not available.
   612     The route is not allocated.
   613     Invalid compression specified.
   614     Out of buffers.
   615     The port was not found.
   616     An asynchronous request is pending.
   617     The port or device is already disconnecting.
   618     The port is not open.
   619     The port is disconnected.
   620     There are no endpoints.
   621     Cannot open the phone book file.
   622     Cannot load the phone book file.
   623     Cannot find the phone book entry.
   624     Cannot write the phone book file.
   625     Invalid information found in the phone book.
   626     Cannot load a string.
   627     Cannot find key.
   628     The port was disconnected.
   629     The port was disconnected by the remote machine.
   630     The port was disconnected due to hardware failure.
   631     The port was disconnected by the user.
   632     The structure size is incorrect.
   633     The port is already in use or is not configured for Remote
	   Access dialout.
   634     Cannot register your computer on the remote network.
   635     Unknown error.
   636     The wrong device is attached to the port.
   637     The string could not be converted.
   638     The request has timed out.
   639     No asynchronous net available.
   640     A NetBIOS error has occurred.
   641     The server cannot allocate NetBIOS resources needed to support
	   the client.
   642     One of your NetBIOS names is already registered on the remote
	   network.
   643     A network adapter at the server failed.
   644     You will not receive network message popups.
   645     Internal authentication error.
   646     The account is not permitted to log on at this time of day.
   647     The account is disabled.
   648     The password has expired.
   649     The account does not have Remote Access permission.
   650     The Remote Access server is not responding.
   651     Your modem (or other connecting device) has reported an error.
   652     Unrecognized response from the device.
   653     A macro required by the device was not found in the device .INF
	   file section.
   654     A command or response in the device .INF file section refers to
	   an undefined macro
   655     The <message> macro was not found in the device .INF file
	   section.
   656     The <defaultoff> macro in the device .INF file section contains
	   an undefined macro
   657     The device .INF file could not be opened.
   658     The device name in the device .INF or media .INI file is too
	   long.
   659     The media .INI file refers to an unknown device name.
   660     The device .INF file contains no responses for the command.
   661     The device .INF file is missing a command.
   662     Attempted to set a macro not listed in device .INF file section.
   663     The media .INI file refers to an unknown device type.
   664     Cannot allocate memory.
   665     The port is not configured for Remote Access.
   666     Your modem (or other connecting device) is not functioning.
   667     Cannot read the media .INI file.
   668     The connection dropped.
   669     The usage parameter in the media .INI file is invalid.
   670     Cannot read the section name from the media .INI file.
   671     Cannot read the device type from the media .INI file.
   672     Cannot read the device name from the media .INI file.
   673     Cannot read the usage from the media .INI file.
   674     Cannot read the maximum connection BPS rate from the media .INI
	   file.
   675     Cannot read the maximum carrier BPS rate from the media .INI
	   file.
   676     The line is busy.
   677     A person answered instead of a modem.
   678     There is no answer.
   679     Cannot detect carrier.
   680     There is no dial tone.
   681     General error reported by device.
   682     ERROR WRITING SECTIONNAME
   683     ERROR WRITING DEVICETYPE
   684     ERROR WRITING DEVICENAME
   685     ERROR WRITING MAXCONNECTBPS
   686     ERROR WRITING MAXCARRIERBPS
   687     ERROR WRITING USAGE
   688     ERROR WRITING DEFAULTOFF
   689     ERROR READING DEFAULTOFF
   690     ERROR EMPTY INI FILE
   691     Access denied because username and/or password is invalid on the
	   domain.
   692     Hardware failure in port or attached device.
   693     ERROR NOT BINARY MACRO
   694     ERROR DCB NOT FOUND
   695     ERROR STATE MACHINES NOT STARTED
   696     ERROR STATE MACHINES ALREADY STARTED
   697     ERROR PARTIAL RESPONSE LOOPING
   698     A response keyname in the device .INF file is not in the
	   expected format.
   699     The device response caused buffer overflow.
   700     The expanded command in the device .INF file is too long.
   701     The device moved to a BPS rate not supported by the COM driver.
   702     Device response received when none expected.
   703     ERROR INTERACTIVE MODE
   704     ERROR BAD CALLBACK NUMBER
   705     ERROR INVALID AUTH STATE
   706     ERROR WRITING INITBPS
   707     X.25 diagnostic indication.
   708     The account has expired.
   709     Error changing password on domain.
   710     Serial overrun errors were detected while communicating with
	   your modem.
   711     RasMan initialization failure. Check the event log.
   712     Biplex port is initializing. Wait a few seconds and redial.
   713     No active ISDN lines are available.
   714     Not enough ISDN channels are available to make the call.
   715     Too many errors occurred because of poor phone line quality.
   716     The Remote Access IP configuration is unusable.
   717     No IP addresses are available in the static pool of Remote
	   Access IP addresses.
   718     PPP timeout.
   719     PPP terminated by remote machine.
   720     No PPP control protocols configured.
   721     Remote PPP peer is not responding.
   722     The PPP packet is invalid.
   723     The phone number, including prefix and suffix, is too long.
   724     The IPX protocol cannot dial-out on the port because the
	   computer is an IPX router.
   725     The IPX protocol cannot dial-in on the port because the IPX
	   router is not installed..
   726     The IPX protocol cannot be used for dial-out on more than one
	   port at a time.
   727     Cannot access TCPCFG.DLL.
   728     Cannot find an IP adapter bound to Remote Access.
   729     SLIP cannot be used unless the IP protocol is installed.
   730     Computer registration is not complete.
   731     The protocol is not configured.
   732     The PPP negotiation is not converging.
   733     The PPP control protocol for this network protocol is not
	   available on the server.
   734     The PPP link control protocol terminated..
   735     The requested address was rejected by the server..
   736     The remote computer terminated the control protocol.
   737     Loopback detected..
   738     The server did not assign an address.
   739     The remote server cannot use the Windows NT encrypted password.
   740     The TAPI devices configured for Remote Access failed to
	   initialize or were not installed correctly.
   741     The local computer does not support encryption.
   742     The remote server does not support encryption.
   743     The remote server requires encryption.
   744     Cannot use the IPX net number assigned by the remote server.
	   Check the event log.
   752     A syntax error was encountered while processing a script.

******************** RAS ERROR CODES ***************************/

HRASCONN      WSCR_RASCONN = 0;
int           WSCR_RASERROR = 0;

DWORD (APIENTRY *fnRasDial) ( LPRASDIALEXTENSIONS, LPSTR, LPRASDIALPARAMSA, DWORD, LPVOID, LPHRASCONN );
DWORD (APIENTRY *fnRasGetConnectStatus) ( HRASCONN, LPRASCONNSTATUSA );
DWORD (APIENTRY *fnRasGetErrorString) ( UINT, LPSTR, DWORD );
DWORD (APIENTRY *fnRasHangUp) ( HRASCONN );
DWORD (APIENTRY *fnRasEnumConnections) (LPRASCONN, LPDWORD, LPDWORD);

/*NH*/
WscrRasLoadDll()
{
    static  int done = 0;
    HINSTANCE   inst;

    if(done) return(0);
    inst = LoadLibrary("rasapi32.dll");
    if(inst < 0) return(-1);

    fnRasDial =             GetProcAddress(inst, "RasDialA");
    fnRasGetConnectStatus = GetProcAddress(inst, "RasGetConnectStatusA");
    fnRasGetErrorString   = GetProcAddress(inst, "RasGetErrorStringA");
    fnRasHangUp           = GetProcAddress(inst, "RasHangUpA");
    fnRasEnumConnections  = GetProcAddress(inst, "RasEnumConnectionsA");

    if(fnRasDial == 0 || fnRasGetConnectStatus == 0
	|| fnRasGetErrorString == 0 || fnRasHangUp == 0
	|| fnRasEnumConnections == 0) return(-1);

    done = 1;
    WSCR_RASERROR = 0;
    return(0);
}

/*NH*/
VOID WINAPI WscrRasDialEvents(
  HRASCONN hrasconn,  // handle to RAS connection
  UINT unMsg,         // type of event that has occurred
  RASCONNSTATE rascs, // connection state about to be entered
  DWORD dwError,      // error that may have occurred
  DWORD dwExtendedError
		      // extended error information for some errors
)
{
    printf("Ras Event...\n");
}

/*================================================================
Connect to an ISP. The connection is done with the first available modem.
The modem is hung up before trying the new connection.

&EN char *tel : phone number prefixed by .. or connexion name
	       (from default ras phonebook)
&EN user : username for this connection
&EN passwd : password for user

&RT 0 if successfull, error code if error (RAS error code)

&EX
    main(argc, argv)
    int     argc;
    char    *argv[];
    {
	int     rc, State;
	char    DeviceType[120], DeviceName[120];

	if(argc < 4) {
	    printf("IConnect phone user passwd\n");
	    exit(1);
	    }
	rc = WscrRasConnect(argv[1], argv[2], argv[3]);
	if(rc) {
	    printf("Error %d\n", rc);
	    WscrRasHangUp();
	    exit(rc);
	    }
	if(WscrRasStatus(&State, DeviceType, DeviceName)) {
	    printf("Can't access State of the device\n");
	    WscrRasHangUp();
	    exit(2);
	    }
	else {
	    printf("State      = %d\n", State);
	    printf("DeviceType = %s\n", DeviceType );
	    printf("DeviceName = %s\n", DeviceName );
	    }

	printf("Pressez ENTER pour raccrocher");
	getchar();
	WscrRasHangUp();
	exit(0);
    }

    Use :
	myras ..022134567 moi monpwd   (w/o phonebook)
	myras euronet moi monpwd       (from phonebook)
&TX
&SA WscrRasStatus(), WscrRasHangUp(), WscrNetUse()
================================================================ */

WscrRasConnect(tel, user, pwd)
char    *tel, *user, *pwd;
{
    RASDIALPARAMS rdp;
    int os;

    if(WscrRasLoadDll()) return(-1);
    WscrRasHangUp();

    Debug("Paramètres de WscrRasConnect: %s - %s - %s\n", tel, user, pwd);

    memset(&rdp, 0, sizeof(RASDIALPARAMS));

    rdp.dwSize = sizeof(RASDIALPARAMS);

    os = WscrGetOSVersion(NULL);
    Debug("OS VERSION = %d\n", os);
    if(os != 2)
	rdp.dwSize -= (sizeof(DWORD) + sizeof(ULONG_PTR));


    if(strncmp(tel, "..", 2) == 0) {
	Debug("Numéro de téléphone avec .. : %s\n", tel);
	strcpy(rdp.szPhoneNumber, tel + 2);
	rdp.szEntryName[0] = 0;
    }
    else {
	Debug("Entrée dans le phonebook : %s\n", tel);
	strcpy(rdp.szEntryName, tel);
    }

    strcpy(rdp.szUserName, user);
    strcpy(rdp.szPassword, pwd);

    Debug("Valeur dans la structure RASDIALPARAMS\n");
    Debug("\tszPhoneNumber : %s\n", rdp.szPhoneNumber);
    Debug("\tszEntryName   : %s\n", rdp.szEntryName);
    Debug("\tszUserName    : %s\n", rdp.szUserName);
    Debug("\tszPassword    : %s\n", rdp.szPassword);
    Debug("\tdwSize        : %d\n", rdp.dwSize);


//    WSCR_RASERROR = fnRasDial(NULL, NULL, &rdp, 1, WscrRasDialEvents, &WSCR_RASCONN);
    WSCR_RASERROR = fnRasDial(NULL, NULL, &rdp, 0, 0, &WSCR_RASCONN);
    if(WSCR_RASERROR) WscrRasHangUp();
    return(WSCR_RASERROR);
}


/*================================================================
Returns the last RAS error message. The message is stored in a static buffer
local to the function.

&SA WscrRasStatus(), WscrRasHangUp(), WscrNetUse()
================================================================ */

char *WscrRasErrorMsg()
{
    static char buf[122];

    if(WscrRasLoadDll()) strcpy(buf, "Library rasapi32.dll not found");
    else if(WSCR_RASERROR) fnRasGetErrorString(WSCR_RASERROR, buf, 120);
    else if(WSCR_RASCONN == 0) strcpy(buf, "No connection");
    else strcpy(buf, "Success");
    return(buf);
}

/*================================================================
Retrieve information on a connection opened by WscrRasConnect().

&EN int *State : current state of the connexion
&EN char *DeviceType
&EN char $DeviceName

&RT 0 if successfull, error code if error

&EX
    main(argc, argv)
    int     argc;
    char    *argv[];
    {
	int     rc, State;
	char    DeviceType[120], DeviceName[120];

	if(argc < 4) {
	    printf("IConnect phone user passwd\n");
	    exit(1);
	    }
	rc = WscrRasConnect(argv[1], argv[2], argv[3]);
	if(rc) {
	    printf("Error %d\n", rc);
	    WscrRasHangUp();
	    exit(rc);
	    }
	if(WscrRasStatus(&State, DeviceType, DeviceName)) {
	    printf("Can't access State of the device\n");
	    WscrRasHangUp();
	    exit(2);
	    }
	else {
	    printf("State      = %d\n", State);
	    printf("DeviceType = %s\n", DeviceType );
	    printf("DeviceName = %s\n", DeviceName );
	    }

	printf("Pressez ENTER pour raccrocher");
	getchar();
	WscrRasHangUp();
	exit(0);
    }
&TX
&SA WscrRasStatus(), WscrRasHangUp(), WscrNetUse()
================================================================ */

WscrRasStatus(int *State, char *DeviceType, char *DeviceName)
{

    return(WscrRasStatus1(WSCR_RASCONN, State, DeviceType, DeviceName));
}

/*NH*/
WscrRasStatus1(HRASCONN hConn, int *State, char *DeviceType, char *DeviceName)
{
    RASCONNSTATUS   RasConnStatus;
    int             rc2, os = WscrGetOSVersion(NULL);

    if(WscrRasLoadDll()) return(-1);
    if(hConn == 0) return(-1);
    memset(&RasConnStatus, 0, sizeof(RASCONNSTATUS));
    RasConnStatus.dwSize = sizeof(RASCONNSTATUS);
    if(os != 2)
	RasConnStatus.dwSize -= RAS_MaxPhoneNumber + 1;

    rc2 = fnRasGetConnectStatus(hConn, &RasConnStatus);
    if(rc2 == 0) {
	*State = RasConnStatus.rasconnstate;
	strcpy(DeviceType, RasConnStatus.szDeviceType);
	strcpy(DeviceName, RasConnStatus.szDeviceName);
	}
    return(rc2);
}

/*================================================================
Retrieve information on a connection opened by WscrRasConnect().
================================================================= */
WscrRasStatusConnection(char *EntryName, int *State, char *DeviceType, char *DeviceName)
{
    RASCONN     *lpRasConns;
    int         rc, i;
    DWORD       Size, Nb;

    if(WscrRasLoadDll()) return(-1);
    Size = 20 * sizeof(RASCONN);
    lpRasConns = (RASCONN *) SCR_malloc(Size);
    lpRasConns->dwSize = sizeof(RASCONN);
    rc = fnRasEnumConnections(lpRasConns, &Size, &Nb);
    if(rc == 0) {
	for(i = 0; i < Nb; i++) {
	    if(EntryName == 0 || SCR_cstrcmp(EntryName, lpRasConns[i].szEntryName) == 0)
	    rc = WscrRasStatus1(lpRasConns[i].hrasconn, State, DeviceType, DeviceName);
	    }
	}
    SCR_free(lpRasConns);
    return(rc);
}

/*================================================================
Retrieve all current connections.
================================================================= */
char **WscrRasGetConnections()
{
    RASCONN     *lpRasConns;
    int         rc, i, nb = 0;
    DWORD       Size, Nb;
    char        **tbl = 0;

    if(WscrRasLoadDll()) return(tbl);
    Size = 20 * sizeof(RASCONN);
    lpRasConns = (RASCONN *) SCR_malloc(Size);
    lpRasConns->dwSize = sizeof(RASCONN);
    rc = fnRasEnumConnections(lpRasConns, &Size, &Nb);
    if(rc != 0) {
	SCR_free(lpRasConns);
	return(tbl);
	}

    for(i = 0; i < Nb; i++) {
	SCR_add_ptr(&tbl, &nb, lpRasConns[i].szEntryName);
	}

    SCR_free(lpRasConns);
    SCR_add_ptr(&tbl, &nb, (char *)0);
    return(tbl);
}

/*================================================================
Stop a RAS connection initialized by WscrRasConnect().

&RT 0 if successfull, error code if error.

&EX
    main(argc, argv)
    int     argc;
    char    *argv[];
    {
	int     rc, State;
	char    DeviceType[120], DeviceName[120];

	if(argc < 4) {
	    printf("IConnect phone user passwd\n");
	    exit(1);
	    }
	rc = WscrRasConnect(argv[1], argv[2], argv[3]);
	if(rc) {
	    printf("Error %d\n", rc);
	    WscrRasHangUp();
	    exit(rc);
	    }
	if(WscrRasStatus(&State, DeviceType, DeviceName)) {
	    printf("Can't access State of the device\n");
	    WscrRasHangUp();
	    exit(2);
	    }
	else {
	    printf("State      = %d\n", State);
	    printf("DeviceType = %s\n", DeviceType );
	    printf("DeviceName = %s\n", DeviceName );
	    }

	printf("Pressez ENTER pour raccrocher");
	getchar();
	WscrRasHangUp();
	exit(0);
    }
&TX
&SA WscrRasStatus(), WscrRasHangUp(), WscrNetUse()
================================================================ */

WscrRasHangUp()
{
    int     rc;

    rc = WscrRasHangUp1(WSCR_RASCONN);
    WSCR_RASCONN = 0;
    return(rc);
}

/*NH*/
WscrRasHangUp1(HRASCONN hConn)
{
    int     rc2, rc = -1, State, os = WscrGetOSVersion(NULL);
    char    DeviceType[120], DeviceName[80];

    if(WscrRasLoadDll()) return(-1);
    if(hConn) {
	rc = fnRasHangUp(hConn);
	if(os != 2) return(rc);
	while(1) {
	    rc2 = WscrRasStatus1(hConn, &State, DeviceType, DeviceName);
	    if(rc2 < 0 || rc2 == ERROR_INVALID_HANDLE) break;
	    Sleep(10);
	    }
	Sleep(1500);
	}
    return(rc);
}

/*================================================================
Hangs up a specified connexion.
================================================================= */
WscrRasHangUpConnection(char *EntryName)
{
    RASCONN     *lpRasConns;
    int         rc, i;
    DWORD       Size, Nb;

    if(WscrRasLoadDll()) return(-1);
    Size = 20 * sizeof(RASCONN);
    lpRasConns = (RASCONN *) SCR_malloc(Size);
    lpRasConns->dwSize = sizeof(RASCONN);
    rc = fnRasEnumConnections(lpRasConns, &Size, &Nb);
    if(rc == 0) {
	for(i = 0; i < Nb; i++) {
	    if(EntryName == 0 || SCR_cstrcmp(EntryName, lpRasConns[i].szEntryName) == 0)
	    rc = WscrRasHangUp1(lpRasConns[i].hrasconn);
	    }
	}
    SCR_free(lpRasConns);
    return(rc);
}

/*
main(argc, argv)
int     argc;
char    *argv[];
{
    int     rc, State, i;
    char    DeviceType[120], DeviceName[120];
    char    **tbl;

    tbl = WscrRasGetConnections();
    if(tbl) {
	printf("Current connections\n");
	for(i = 0 ; tbl[i] ; i++) {
	    printf("  %d : '%s'\n", i, tbl[i]);
	    WscrRasStatusConnection(tbl[i], &State, DeviceType, DeviceName);
	    printf("    Status : %d, DeviceType : %s, DeviceName : %s\n", State, DeviceType, DeviceName);
	    }
	SCR_free_tbl(tbl);
	}
    else
	printf("No current connections\n");

    if(argc < 4) {
	printf("DialR phone user passwd\n");
	exit(1);
	}
    rc = WscrRasConnect(argv[1], argv[2], argv[3]);
    if(rc) {
	printf("Error %d - %s\n", rc, WscrRasErrorMsg());
	WscrRasHangUp();
	exit(rc);
	}
    rc = WscrRasStatus(&State, DeviceType, DeviceName);

    if(rc) {
	WSCR_RASERROR = rc;
	printf("Msg : %s\n", WscrRasErrorMsg());
	printf("Can't access State of the device\n");
	WscrRasHangUp();
	exit(2);
	}
    else {
	printf("State      = %d\n", State);
	printf("DeviceType = %s\n", DeviceType );
	printf("DeviceName = %s\n", DeviceName );
	}

    printf("Msg : %s\n", WscrRasErrorMsg());
    printf("Pressez ENTER pour raccrocher");
    getchar();
    WscrRasHangUp();
    printf("Msg : %s\n", WscrRasErrorMsg());
    exit(0);
}

Syntax()
{
    printf("s4dial : initiate, display status and close a modem connection.\n");
    printf("Syntax : \n");
    printf("    s4dial -c connection_name [-login login -pwd password] : establish a connection based on the phonebook definition\n");
    printf("    s4dial -d [connection_name] : disconnect [one or] all connections\n");
    printf("    s4dial -s [connection_name] : display the current status [of a specific connection]\n");
    printf("        connection_name = entry as defined in the default phonebook\n");
    printf("                          or ..phonenb (first modem available is used\n");
}
*/
#endif
