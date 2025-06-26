#if defined(SCRW32) || defined(DOSW32) || defined(LINUX)

/************ INTERFACE SOCKET POUR LINUX *******************/
#if defined(UNIX)
#include "scr4.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <netdb.h>
#define WINAPI
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#define SOCKET int
#define closesocket close

/************ INTERFACE SOCKET POUR WINDOWS *******************/
#else

#include "scr4w.h"
#include <winsock.h>

#endif

/************ INTERFACE STANDARDISE ***************************/

extern WSOCK1  WSOCKS[WSOCK_MAX_SOCKS];
extern int     WSOCKS_CURRENT;
extern int WSOCK_DEBUG;
extern int WSOCK_BUFSIZE;


int WSock1Register(int cltsock)
{
    int     i;

    for(i = 0 ; i < WSOCK_MAX_SOCKS ; i++)
	if(WSOCKS[i].cltsock == 0) break;

    if(i == WSOCK_MAX_SOCKS) return(-1);
    memset(WSOCKS + i, 0, sizeof(WSOCK1));
    WSOCKS[i].cltsock = cltsock;
//    Debug("NbSocks=%d\n", WSock1NbSocks());
    return(i);
}

int WSock1UnRegister(int cltsock)
{
    int     i;

    for(i = 0 ; i < WSOCK_MAX_SOCKS ; i++)
	if(WSOCKS[i].cltsock == cltsock) break;

    if(i == WSOCK_MAX_SOCKS) return(-1);
    SCR_free(WSOCKS[i].wsock_buf);
    memset(WSOCKS + i, 0, sizeof(WSOCK1));
//    Debug("NbSocks=%d\n", WSock1NbSocks());
    return(i);
}


int WSock1NbSocks()
{
    int     i, count = 0;

    for(i = 0 ; i < WSOCK_MAX_SOCKS ; i++)
	if(WSOCKS[i].cltsock) count++;

    return(count);
}


int WSock1SetCurrent(int cltsock)
{
    int     i;

    for(i = 0 ; i < WSOCK_MAX_SOCKS ; i++)
	if(WSOCKS[i].cltsock == cltsock) break;

    if(i == WSOCK_MAX_SOCKS) return(-1);
    WSOCKS_CURRENT = i;
    return(i);
}


int WSock1GetTotalR(int cltsock)
{
    int     i;

    for(i = 0 ; i < WSOCK_MAX_SOCKS ; i++)
	if(WSOCKS[i].cltsock == cltsock) break;

    if(i == WSOCK_MAX_SOCKS) return(-1);
    return(WSOCKS[i].wsock_totalr);
}


int WSock1GetTotalW(int cltsock)
{
    int     i;

    for(i = 0 ; i < WSOCK_MAX_SOCKS ; i++)
	if(WSOCKS[i].cltsock == cltsock) break;

    if(i == WSOCK_MAX_SOCKS) return(-1);
    return(WSOCKS[i].wsock_totalw);
}


/*NH*/
WSOCKCLT *WSockCreateWSC(int WinSock, int ClientSock, int (*fn)(int))
{
    WSOCKCLT    *wsc;

    wsc = (WSOCKCLT *)malloc(sizeof(WSOCKCLT));
    wsc->fn = fn;
    wsc->srvsock = (int)WinSock;
    wsc->cltsock = (int)ClientSock;
    return(wsc);
}

/*NH
Fonction appelée par le Thread Client
*/
long WINAPI WSockClient(WSOCKCLT *wsc)
{
    int     rc;
    char    ip[40], name[120];

    if(WSOCK_DEBUG) {
	Debug("\n*** Entering User function (socket %d)\n", wsc->cltsock);
	if(WSockGetPeer(wsc->cltsock, ip, name) == 0)
	    Debug("    Client : %s [%s]\n", name, ip);
	}
    WSOCK_TOTALR = WSOCK_TOTALW = 0;
    rc = (wsc->fn)(wsc->cltsock);
    if(WSOCK_DEBUG) {
	Debug("*** Exiting User function (socket %d). Rc=%d\n", wsc->cltsock, rc);
	Debug("    Bytes sent : %ld. Bytes received : %ld\n", WSOCK_TOTALW, WSOCK_TOTALR);
	}
    shutdown(wsc->cltsock, 0x02);
    closesocket(wsc->cltsock);
    WSock1UnRegister(wsc->cltsock);
    if(WSOCK_DEBUG) Debug("*** Socket %d closed.\n", wsc->cltsock);
    free(wsc);
}

/*
Fonction de lancement de process client avec la commande cmd
*/
long WINAPI WSockClientProc(char *subcmd, int cltsock)
{
#ifdef UNIX
    int                 rc = 0;
    char                cmd[256], ip[40], name[120];

    if(WSOCK_DEBUG) {
	DebugB("\n*** Starting new process '%s'.", cmd);
	if(WSockGetPeer(cltsock, ip, name) == 0)
	    Debug("    Client : %s [%s]\n", name, ip);
	}

    switch(fork()) {
	case -1 :
	    rc = -1;
	    break;

	case 0 : // fils
	    //fn(cltsock);
	    exit(0);

	default :
	   closesocket(cltsock);
	   break;
	}


    WSock1UnRegister(cltsock);
    if(WSOCK_DEBUG) DebugE("Rc = %d.", rc);
    if(rc) return(0);
    return(-1);

#else
// Win32
    STARTUPINFO         si;
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES sa;
    int                 rc, dupsock;
    DWORD               ret;
    char                cmd[256], ip[40], name[120];

    memset(&si, 0, sizeof(si));
    memset(&pi, 0, sizeof(pi));
    memset(&sa, 0, sizeof(sa));
    si.cb = sizeof(si);
    sa.nLength = sizeof(sa);
    // Duplicate handle for win95
    if(WscrGetOSVersion((char *)0) != 2) {
	if(!DuplicateHandle(GetCurrentProcess(),
		(HANDLE)cltsock,
		GetCurrentProcess(),
		(HANDLE*)&dupsock,
		0,
		TRUE, // Inheritable
		DUPLICATE_SAME_ACCESS)) {
		    Debug("Cannot duplicate Socket Handle\n");
		    return(-1);
		    }
	}
    else {
	dupsock = cltsock;
	}

    sprintf(cmd, subcmd, dupsock);

    if(WSOCK_DEBUG) {
	DebugB("\n*** Starting new process '%s'.", cmd);
	if(WSockGetPeer(cltsock, ip, name) == 0)
	    Debug("    Client : %s [%s]\n", name, ip);
	}
    rc = CreateProcess(NULL, cmd, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS,
		     NULL, NULL, &si, &pi);

    WSock1UnRegister(cltsock);
    if(WSOCK_DEBUG) DebugE("Rc = %d.", rc);
    if(rc) return(0);
    return(-1);
#endif
}

/* =================================================================
Fonction serveur.
Démarre un socket côté serveur (STREAM).


&EN port : porte
&EN fn : fonction utilisateur (NULL si multi process)
&EN subcmd (Windows seul) : commande à lancer (multi-process) avec %d à la place
    du numéro de socket ouvert. NULL si pas multi-process
&EN bThread : 0 si single thread server, 1 si multi-thread ou multi-process en Unix
&EN nqueue : taille de la file d'attente maximum sur le socket serveur


EN Unix, les threads ne sont pas implémentés. Seules les versions
multi-process et mono-process sont opérationnelles pour l'instant.

On utilise la même syntaxe en Windows multi-thread et en Unix multi-process :

&CO
   WSockStartServer(port, fn, 0, 1, n);
&TX
Le premier 0 indique pas de commande à lancer, le second multi-thread.
&RT

&EN 0 en cas de succès
&EN -1 : erreur à l'initialisation
&EN -2 : erreur à la création du socket initial
&EN -3 : erreur au bind
&EN -4 : erreur au listen
&EN -5 : erreur à accept
&EN -6 : erreur à la création d'un sous thread
&EN -7 : erreur à la création d'un sous process

&IT Serveur multi-thread
&CO
    FileServer(int sock)
    {
	int     lg;
	char    buf[80];

	while(1) {
	    lg = WSockReadString(sock, buf);
	    if(lg < 0) break;
	    if(strncmp(buf, "GET ", 4) == 0) {
		FileCopy(sock, buf + 4);
		break;
		}
	    else if(buf[0] == 0) break;
	    else WSockPrintf(sock, "Error");
	    }
    }

    main()
    {
	int     rc, port = 5000;

	printf("\n*** Starting Server - pid=%d\n", getpid());
	rc = WSockStartServer(port, FileServer, 0, 1, 4);
	if(rc < 0) printf("Error %d.\n", rc);
	exit(rc);
    }

&TX

&IT Serveur multi-process

Ce programme lancé sans argument démarre le serveur qui attend une
connexion. Lors d'une connexion, il se lance avec comme paramètres -s
et le numéro du socket créé. La fonction FileServer n'est utilisée que
dans ce second cas.

&CO
    FileServer()
    {
    ...
    }

    main(argc, argv)
    int     argc;
    char    *argv[];
    {
	int     rc, port = 5000;
	char    buf[120];

	// Connexion établie, lancement de la fonction FileServer()
	if(argc > 2 && strcmp(argv[1], "-s") == 0) {
	    printf("\n*** Connecting child for socket %d\n", atoi(argv[2]));
	    rc = WSockStartServerProc(atoi(argv[2]), FileServer);
	    exit(rc);
	    }
	// Démarrage du serveur : attente de connexion
	else {

	    printf("\n*** Starting Server - pid=%d\n", getpid());
	    sprintf(buf, "%s -s %%d", argv[0]);
	    rc = WSockStartServer(port, 0, buf, 0, 4);
	    if(rc < 0) {
		printf("Error %d.\n", rc);
		exit(rc);
		}
	    }
    }
&TX

&SA WSockStartServerProc(), WSockConnect()
================================================================= */

int WSockStartServer(int port, int (*fn)(int), char* subcmd, int bThread, int nqueue)
{
    int                 rc, iId;
    SOCKET              WinSocket = -1, // Server socket
			ClientSock;     // Socket for communicating with Client
//    SOCKADDR_IN local_sin,      // Local socket address
//                accept_sin;     // Receives the address of the connecting entity
    struct sockaddr_in  local_sin,      // Local socket address
			accept_sin;     // Receives the address of the connecting entity
    char                ip[40], name[120];

    int         accept_sin_len, first = 1;
    WSOCKCLT    *wsc;
    char        buf[512];

    // Initiate Winsock.
    if(WSockStartup() != 0) return(-1);

    // Create a TCP/IP socket, WinSocket.
    if(WSOCK_DEBUG) DebugB("\n*** Creating server socket (for port %d)...", port);
    if((WinSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	rc = -2;
	if(WSOCK_DEBUG) DebugE("Failed (rc=%d)", WinSocket);
	goto cleanup;
	}
    if(WSOCK_DEBUG) DebugE("Ok (socket=%d)", WinSocket);

    // Associate the local address with WinSocket.
    local_sin.sin_family = AF_INET;
    local_sin.sin_port = htons(port);
    local_sin.sin_addr.s_addr = htonl(INADDR_ANY);
    if(WSOCK_DEBUG) DebugB("Binding (port %d)...", port);
    if((rc = bind(WinSocket,
	    (struct sockaddr *) &local_sin,
	    sizeof (local_sin))) < 0) {
	if(WSOCK_DEBUG) DebugE("Failed (rc=%d)", rc);
	rc = -3;
	goto cleanup;
	}
    if(WSOCK_DEBUG) DebugE("Ok");

    // Establish a socket to listen for incoming connections.
    if(WSOCK_DEBUG) DebugB("Listening (port %d)...", port);
    if((rc = listen(WinSocket, nqueue)) < 0) {
	if(WSOCK_DEBUG) DebugE("Failed (rc=%d)", rc);
	rc = -4;
	goto cleanup;
	}
    if(WSOCK_DEBUG) DebugE("Ok");

#ifdef UNIX
    // Ignore le signal de fin de process des fils (sinon ils restent defunct)
    // On peut aussi installer un handler pour noter pour chaque process sa fin et
    // son code retour
    signal(SIGCHLD, SIG_IGN);
#endif

    while(1) {
	// Accept an incoming connection attempt on WinSocket.
	accept_sin_len = sizeof (accept_sin);
	if(WSOCK_DEBUG) DebugB("Accepting connection (port %d)...", port);
	ClientSock = accept(WinSocket,
			   (struct sockaddr *) &accept_sin,
			   (int *) &accept_sin_len);
	if(ClientSock == INVALID_SOCKET) {
	    if(WSOCK_DEBUG) DebugE("Failed (rc=%d)", ClientSock);
	    rc = -5;
	    goto cleanup;
	    }
	if(WSOCK_DEBUG) DebugE("Ok (socket=%d)", ClientSock);

	// Check for a STOP SERVER request
	rc = recv(ClientSock, buf, 510, MSG_PEEK);
	if(strncmp(buf, "STOP", 4) == 0) {
	    shutdown(ClientSock, 0x02);
	    closesocket(ClientSock);
	    break;
	    }

	WSock1Register(ClientSock);
	WSock1SetCurrent(ClientSock);
	// Fonction client
	if(subcmd) {
	    // Start new process
	    if(WSockClientProc(subcmd, (int)ClientSock)) {
		rc = -7;
		goto cleanup;
		}
	    }
	else {
	    wsc = WSockCreateWSC((int)WinSocket, (int)ClientSock, fn);
	    if(bThread) {
	 #ifndef UNIX
		HANDLE      hThread;

		// Start New Thread
		hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)WSockClient, (long *)wsc,0,&iId);
		if(!hThread) {
		    rc = -6;
		    closesocket(ClientSock);
		    goto cleanup;
		    }
		else CloseHandle(hThread);
	 #else
		if(WSOCK_DEBUG) {
		    DebugB("\n*** Starting new process\n");
		    if(WSockGetPeer(ClientSock, ip, name) == 0)
			Debug("    Client : %s [%s]\n", name, ip);
		    }

		rc = 0;
		switch(fork()) {
		    case -1 : // Erreur
			rc = -1;
			break;

		    case 0 : // fils
			closesocket(WinSocket);
			fn(ClientSock);
			goto cleanup;

		    default : // père
		       closesocket(ClientSock);
		       break;
		    }

		WSock1UnRegister(ClientSock);
		if(WSOCK_DEBUG) DebugE("Rc = %d.", rc);
		if(rc) goto cleanup;

	 #endif
		}
	    else {
		// Single Thread Server
		rc = WSockClient(wsc);
		if(rc) break;
		}
	    }
	}

cleanup :
    if(WSOCK_DEBUG) Debug("*** Closing Server socket %d\n", WinSocket);
    if(WinSocket > 0) closesocket(WinSocket);
    WSock1UnRegister(WinSocket);
    WSockCleanup();
    return(0);
}

/* =================================================================
Fonction serveur.
Branche sur un socket client pour lequel une connexion vient d'être
créée. Un exemple peut être trouvé dans la fonction WSockStartServer()

&EN cltsock : numéro du socket
&EN fn : fonction utilisateur

&RT
&EN -1 : erreur à l'initialisation
&EN valeur de retour de fn sinon

&SA WSockStartServer()
================================================================= */

int WSockStartServerProc(int cltsock, int (*fn)(int))
{
    int         rc;
    char        ip[40], name[120];

    WSock1SetCurrent(cltsock);
    if(WSockStartup() != 0) return(-1);
    if(WSOCK_DEBUG) {
	    Debug("\n*** Entering User function (socket %d)\n", cltsock);
	//if(WSockGetPeer(cltsock, ip, name) == 0)
	//    Debug("    Client : %s [%s]\n", name, ip);
	}

    WSOCK_TOTALR = WSOCK_TOTALW = 0;
    rc = fn(cltsock);
    if(WSOCK_DEBUG) {
	Debug("*** Exiting User function (socket %d). Rc=%d\n", cltsock, rc);
	Debug("    Bytes sent : %ld. Bytes received : %ld\n", WSOCK_TOTALW, WSOCK_TOTALR);
	}
    shutdown(cltsock, 0x02);
    closesocket(cltsock);
    WSock1UnRegister(cltsock);
    WSockCleanup();
    return(rc);
}

#endif