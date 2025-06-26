#if defined(SCRW32) || defined(DOSW32) || defined(LINUX)

/************ INTERFACE SOCKET POUR LINUX *******************/

#if defined(UNIX)
#include "scr4.h"
#include <sys/types.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define WINAPI
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#define SOCKET int
#define closesocket close


WSockGetLastError()
{
    return(errno);
}


WSockStartup()
{
    return(0);
}

WSockCleanup()
{
    return(0);
}


/************ INTERFACE SOCKET POUR WINDOWS *******************/
#else

#include "scr4w.h"
#include <winsock.h>

WSockGetLastError()
{
    return(WSAGetLastError());
}

WSockStartup()
{
    WSADATA     WSAData;

    // Initiate Winsock.
    if(WSAStartup(MAKEWORD(1,1), &WSAData) != 0) return(-1);
    return(0);
}

WSockCleanup()
{
    WSACleanup();
    return(0);
}

#endif

#include "scr4_srv.h"

/************ INTERFACE STANDARDISE ***************************/

extern int WSOCK_DEBUG;
extern int WSOCK_BUFSIZE;

long     WSOCK_CRYPTKEY = 0;
int      WSOCK_FORCE_IOMODE = 0;

/* =================================================================
Fonction client.
Connecte à un serveur (STREAM).

&EN servername: nom du host
&EN port : porte

&RT
&EN >= 0 = identifiant (numéro) du socket créé
&EN -1 : erreur à l'initialisation
&EN -2 : erreur à la création du socket
&EN -3 : host not found
&EN -4 : erreur au connect
================================================================= */

int WSockConnect(servername, port)
char    *servername;
int     port;
{
    struct sockaddr_in  in;
    struct hostent     *hp;
#if !defined(DOSW32) && !defined(SCRW32)
    struct hostent     *gethostbyname();
#endif
    int                 WinSocket = 0, rc = 0, cursock;
    unsigned long       imode = 1;

    // Initiate Winsock.
    if(WSockStartup() != 0) return(-1);

    // Create a TCP/IP socket, WinSocket.
    if(WSOCK_DEBUG) DebugB("\n*** Creating client socket...");
    if((WinSocket = (int)socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {   /* BP_M 17-02-2008 */
        if(WSOCK_DEBUG) DebugE("Failed (rc=%d)", WinSocket);
        rc = -2;
        goto cleanup;
    }


    if(WSOCK_DEBUG) DebugE("Ok (socket=%d)", WinSocket);
    WSock1Register(WinSocket);
    cursock = WSock1SetCurrent(WinSocket);
    WSOCKS[cursock].wsock_totalr = WSOCKS[cursock].wsock_totalw = 0;

    hp = gethostbyname(servername);
    if(hp == 0) {
        rc = -3;
        goto cleanup;
    }

    // Connect to host:port
    memset(&in, sizeof(in), 0);
    in.sin_family = AF_INET;
    memcpy((char *) &in.sin_addr, (char *) hp->h_addr, hp->h_length);
    in.sin_port = htons(port);

    if(WSOCK_DEBUG) DebugB("*** Connecting client to %s:%d...", servername, port);
    if((rc = connect(WinSocket, (struct sockaddr *) &in, sizeof(in))) < 0) {
        if(WSOCK_DEBUG) DebugE("Failed (rc=%d)", rc);
        rc = -4;
        goto cleanup;
    }
    if(WSOCK_DEBUG) DebugE("OK: socket %d - rc = %d", WinSocket, rc);

#if defined(SCRW32) || defined(DOSW32)
    if(!WSOCK_FORCE_IOMODE)
        rc = ioctlsocket(WinSocket, FIONBIO, &imode);

    if(rc)
        Debug("S4: Ioctlsocket - Error\n");
    else
        Debug("S4: iotclsocket - force to no wait\n");

    if(WSOCK_DEBUG) DebugE("Ok");


#endif
    return(WinSocket);


cleanup:
    if(WinSocket > 0) closesocket(WinSocket);
    WSock1UnRegister(WinSocket);
    WSockCleanup();
    return(rc);
}

/* =================================================================
Fonction client. Ferme le socket client cltsock et désalloue
l'environnement Windows Socket.

&EN cltsock : numéro du socket client

&NO Ne pas utiliser dans le cas d'un serveur : le serveur se charge des
opérations de début et de fin.

=================================================================*/

WSockClose(int cltsock)
{
    int     rc, cursock;

    cursock = WSock1SetCurrent(cltsock);
    rc = WSockFlush(cltsock);
    if(WSOCK_DEBUG) {
        Debug("*** Closing socket %d\n", cltsock);
        Debug("    Bytes sent : %ld. Bytes received : %ld\n", WSOCKS[cursock].wsock_totalw, WSOCKS[cursock].wsock_totalr);
    }
    if(cltsock > 0) closesocket(cltsock);
    WSock1UnRegister(cltsock);
    //WSockCleanup();  // JMP 2/8/2014 => closes all sockets !!
    return(rc);
}

/* =================================================================
Fonction client et serveur. Retourne l'adresse IP et le nom de la machine partenaire dans la communication en cours.

&EN cltsock : numéro du socket client
&EN ip      : adresse IP formattée ("193.190.190.10")
&EN name    : nom de la machine ("JMP") ou string vide

&RT 0 en cas de succès, code d'erreur sinon
=================================================================*/

WSockGetPeer(int cltsock, char *ip, char *name)
{
    struct sockaddr pname;
    //struct hostent  *host;
    int             rc, plen = sizeof(pname);
    U_ch            *ipn;

    rc = getpeername((SOCKET) cltsock,  &pname, &plen);
    if(rc) return(rc);
    ipn= pname.sa_data + 2;
    sprintf(ip, "%d.%d.%d.%d", ipn[0], ipn[1], ipn[2], ipn[3]);
    name[0] = 0;
    /*    host = gethostbyaddr(ipn, 4, AF_INET);
        if(host && host->h_name) strcpy(name, host->h_name);
    TROP LENT JMP 23-10-02 */
    return(0);
}

/* =================================================================
Fonction serveur et client.
Vide le buffer d'écriture.

&EN sock : numéro du socket

La variable WSOCK_BUFSIZE définit la taille d'un buffer.

&RT
&EN -1 : erreur
&EN 0 : succès
================================================================= */

WSockFlush(int sock)
{
    int     rc, lg, cursock;

    cursock = WSock1SetCurrent(sock);
    if(cursock < 0) return(-1);
    lg = WSOCKS[cursock].wsock_buflen;

    if(WSOCKS[cursock].wsock_buflen == 0) return(0);
    WSOCKS[cursock].wsock_buflen = 0;
    rc = WSockWrite(sock, WSOCKS[cursock].wsock_buf, lg);
    return(rc);
}

/* =================================================================
Fonction serveur et client.
Bufferise les écritures. WSockFlush() vide le buffer.

&EN sock : numéro du socket
&EN txt : buffer à écrire
&EN lg : nombre de bytes à écrire

La variable WSOCK_BUFSIZE permet de bufferiser les écritures.

&RT
&EN -1 : erreur
&EN 0 : succès
================================================================= */

WSockBWrite(int sock, char *txt, int lg)
{
    int     rc, cursock = WSock1SetCurrent(sock);

    if(cursock < 0) return(-1);

    if(WSOCK_BUFSIZE && WSOCKS[cursock].wsock_buf == 0) {
        WSOCKS[cursock].wsock_buf = SCR_malloc(WSOCK_BUFSIZE);
        WSOCKS[cursock].wsock_buflen = 0;
    }

    if(txt == 0) txt = "";
//    if(txt[0] == 0) return(0);  /* JMP 05-11-01 */
    if(lg <= 0) lg = (int)strlen(txt) + 1;     // ???!!!??? Problème pour les strings NULLS

    if(WSOCKS[cursock].wsock_buflen + lg > WSOCK_BUFSIZE) {
        rc = WSockFlush(sock);
        if(rc) return(rc);
    }

    if(lg > WSOCK_BUFSIZE) {
        rc = WSockWrite(sock, txt, lg);
        WSOCKS[cursock].wsock_buflen = 0;
        return(rc);
    }

    memcpy(WSOCKS[cursock].wsock_buf + WSOCKS[cursock].wsock_buflen, txt, lg);
    WSOCKS[cursock].wsock_buflen += lg;
    return(0);
}

int WSockDebugBuf(unsigned char *buf, int lg)
{
    int     j;

    Debug("'");
    for(j = 0 ; j < lg ; j++) {
        if(buf[j] >= ' ' && buf[j] < 128) Debug("%c", buf[j]);
        else                              Debug("\\%02x", buf[j]);
    }
    Debug("'");
    return(0);
}

/* ==================================================================
Fonction serveur et client.
Ecrit sur le socket décrit par cltsock.

&EN cltsock : numéro du socket
&EN txt : buffer à écrire
&EN lg : nombre de bytes à écrire

La variable WSOCK_BUFSIZE permet de bufferiser les écritures.

&RT
&EN -1 : erreur
&EN 0 : succès
===================================================================== */

WSockWrite(int cltsock, char *txt, int lg)
{
    int         rc, cursock;
    static U_ch *res = 0;
    static int  reslg = 0;

    cursock = WSock1SetCurrent(cltsock);
    rc = WSockFlush(cltsock);
    if(rc) return(rc);

    if(txt == 0) txt = "";
    if(lg <= 0) lg = (int)strlen(txt) + 1;

    if(WSOCK_DEBUG) DebugB("W(%3.3d) : %5.5d bytes ...", cltsock, lg);
    if(reslg < lg) {
        if(res) SCR_free(res);  /* BP_M 14-01-2008 */
        res = SCR_malloc(lg + 1);
        reslg = lg;
    }
    WSockCrypt(res, txt, lg); /* JMP 22-10-02 */
    rc = send(cltsock, res, lg, 0);
    // WSockDecrypt(txt, lg); /* JMP 22-10-02 */
    if(rc == SOCKET_ERROR) {
        if(WSOCK_DEBUG) DebugE("Failed (rc=%2.2d)", rc);
        return(-1); /* BP_M 05-12-2009 15:13 */
    }
    WSOCKS[cursock].wsock_totalw += lg;
    if(WSOCK_DEBUG > 1) WSockDebugBuf(txt, lg);
    if(WSOCK_DEBUG)     DebugE("Ok (rc=%2.2d)", rc);
    return(0);
}


/* =================================================================
Fonction serveur et client.
Bufferise un record précédé de sa longueur sur le socket cltsock.
La longueur est d'abord envoyée dans les 4 premiers bytes, puis le
record lui-même.

&EN cltsock : numéro du socket
&EN txt : record
&EN lg  : longueur du record

&RT
&EN < 0 : erreur
&EN 0 : ok
================================================================= */

WSockBWriteRecord(int cltsock, char *txt, int lg)
{
    int     rc;
    //long    llg;
    U_sh    slg; /* JMP 11-04-00 */

    if(txt == 0) lg = 0;
    slg = lg;
    rc = WSockBWrite(cltsock, (char *)&slg, sizeof(short));
//    llg = lg;
//    rc = WSockBWrite(cltsock, (char *)&llg, sizeof(long));
    if(rc) return(rc);
    if(lg == 0) return(0);
    rc = WSockBWrite(cltsock, txt, lg);
    return(rc);
}

/* =================================================================
Fonction serveur et client.
Ecrit un record sur le socket cltsock.
La longueur est d'abord envoyée dans les 4 premiers bytes, puis le
record lui-même.

&EN cltsock : numéro du socket
&EN txt : record
&EN lg  : longueur du record

&RT
&EN < 0 : erreur
&EN 0 : ok
================================================================= */

WSockWriteRecord(int cltsock, char *txt, int lg)
{
    int     rc;

    rc = WSockBWriteRecord(cltsock, txt, lg);
    if(rc) return(rc);
    return(WSockFlush(cltsock));
}

/* =================================================================
Fonction serveur et client.
Formatte et bufferise sur le socket décrit par cltsock.

&EN cltsock : numéro du socket
&EN format : format d'envoi (printf)
&EN a1, a2, ... : paramètres du format

&RT
&EN -1 : erreur à l'initialisation
&EN 0 : succès
================================================================= */

WSockBPrintf(cltsock, format, a1, a2, a3, a4, a5, a6, a7, a8, a9)
int     cltsock;
char    *format;
char    *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, *a9;
{
    char    buf[2048];

    if(format == 0) return(0);
    sprintf(buf, format, a1, a2, a3, a4, a5, a6, a7, a8, a9);
    return(WSockBWrite(cltsock, buf, (int)strlen(buf)));
}

/* =================================================================
Fonction serveur et client.
Ecrit sur le socket décrit par cltsock.

&EN cltsock : numéro du socket
&EN format : format d'envoi (printf)
&EN a1, a2, ... : paramètres du format

&RT
&EN -1 : erreur à l'initialisation
&EN 0 : succès
================================================================= */

WSockPrintf(cltsock, format, a1, a2, a3, a4, a5, a6, a7, a8, a9)
int     cltsock;
char    *format;
char    *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, *a9;
{
    int     rc;

    rc = WSockBPrintf(cltsock, format, a1, a2, a3, a4, a5, a6, a7, a8, a9);
    if(rc) return(rc);
    return(WSockFlush(cltsock));
}

int CHECK_READ = 0;
extern int ISC_SOCKET, ISC_NB_CONN;
extern S4C **ISC_S4C;

ISC_check_read()
{
    char    buf[512];
    int     i, t_socket = ISC_SOCKET;
    static int in_use;
    int     t_cr = CHECK_READ;

    if(ISC_NB_CONN == 0) return(0);
    if(in_use) return(0);
    in_use = 1;
    CHECK_READ = 2;
    for(i = 0 ; i < ISC_NB_CONN ; i++)
        WSockRead(ISC_S4C[i]->socket, buf, 512);
    CHECK_READ = t_cr;
    in_use = 0;
    return(0);
}

/* =================================================================
Fonction serveur et client.
Lit maximum lg bytes sur le socket cltsock.
Si le nombre de bytes lus est < lg, un 0 est ajouté après le dernier byte
lu.

&EN cltsock : numéro du socket
&EN txt : buffer
&EN lg  : nombre max. de bytes à lire

&RT
&EN < 0 : erreur
&EN > 0 : nombre de bytes lus
================================================================= */

WSockRead(int cltsock, char *txt, int lg)
{
    int     rc, cursock, nb_try = 0, moderecv, nb_try2 = 0;
    int     tmp_check_read = -1;
    unsigned long imode = 1;
    char    *msg_txt = 0;
    static int use1 = 1, msg, set_ioctl = -1;

    cursock = WSock1SetCurrent(cltsock);

    rc = WSockFlush(cltsock);
    if(rc) {
        Debug("WSockRead : WSockFlush = %d\n", rc);
        return(rc);
    }

    if(CHECK_READ)
        imode = 1L;
    else
        imode = 0L;

#if defined (SCRW32) || defined(DOSW32)
//  if(imode != set_ioctl) {
    rc = ioctlsocket(cltsock, FIONBIO, &imode);
//      Debug("ioctlsocket( imode=%d ) return(%d);\n", imode, rc);
    switch(rc) {
    case WSANOTINITIALISED:
        break;
    case WSAENETDOWN:
        Debug("WSockRead : ioctlsocket() = WSAENETDOWN : return(%d);\n", rc);
        return(-1);
    case WSAEINPROGRESS:
        break;
    default:
//              Debug("WSockRead: ioctlsocket() = default : return(%d)\n", rc);
        break;
    }
//      set_ioctl = imode;
//  }

#endif
    while(1) {
        moderecv = 0;
#ifdef LINUX
        if(imode)
            moderecv = MSG_DONTWAIT;
#endif
        rc = recv(cltsock, txt, lg, moderecv);
        if(rc > 0 && rc == lg) goto next;   /* BP_M 25-03-2011 13:13 */
        if(tmp_check_read >= 0 && rc == -1) rc = 0;

        switch(rc) {
#if defined(SCRW32) || defined(DOSW32)
        case WSAENETDOWN:

            nb_try2++;
            if(tmp_check_read < 0)
                tmp_check_read = CHECK_READ;
            CHECK_READ = 1;
            if(nb_try2 < 1000) continue;
            SCR_sleep(1);
            if(nb_try2 < 10000) continue;

            Debug("WSockRead : recv() = WSAENETDOWN : return(-1);\n");
            return(-1);
        case WSAEWOULDBLOCK:
            SCR_sleep(10);
            continue;
        case SOCKET_ERROR:
#endif
#ifdef LINUX
        case EAGAIN:
        case -1:
#endif
            if(CHECK_READ == 2) { // Test clavier
                SCR_sleep(1);
                return(0);
            }
            if(CHECK_READ == 1 && nb_try < 10000) {  // Délai total de max 1.5s
                if(nb_try > 9000)
                    SCR_sleep(10);
                nb_try ++;
                continue;
            }
            return(-2);

        case 0:
            /*
            		Debug("WSockRead : recv = 0 - Test de reconnection\n");
            		SCR_sleep(500);
            		ISC_SOCKET = cltsock;
            		rc = ISC_printf("%d", ISS_LAST_CMD);
            		if(rc < 0) {
            		    Debug("WSockRead : ISC_printf(ISS_LAST_CMD) = %d\n", rc);
            		    goto endconn;
            		}
            		rc = ISC_read_rc();
            		Debug("WSockRead : recv = 0 - Test de reconnection - ok\n");
            		CHECK_READ = tmp_check_read;
            		return(0);
            
endconn:
            
            		if(!CHECK_READ && rc == 0 && nb_try < 1000) {
            		    SCR_sleep(1);
            		    nb_try ++;
            		    continue;
            		}
            */
            Debug("WSockRead : recv = 0 - Connection closed by peer. return(-1);\n");
            WSock1UnRegister(cltsock);
            return(-1);
        default:
            if(rc > 0) {
                if(rc < lg)
                    txt[rc] = 0; /* JMP 12-01-00 */
                goto next;
            }
            else {
                Debug("WSockRead() : recv = %d - return(-1)\n", rc);
                return(-1);
            }
            break;
        }
    }
next:
    WSockDecrypt(txt, txt, rc); /* JMP 22-10-02 */
    WSOCKS[cursock].wsock_totalr += rc;

    if(CHECK_READ) {
        if(rc == 10 && memcmp(txt, "S4", 2) == 0 && memcmp(txt + 8, "S4", 2) == 0) {
            Debug("S4C: Demande du serveur - vérification de la connexion OK - %.10s\n", txt);
            WSockWrite(cltsock, txt, 10);
        }
        if(rc == 8 && memcmp(txt, "S4STOPS4", 8) == 0) {
            Debug("S4C: Demande du d'arret du serveur OK\n");
            WSockWrite(cltsock, txt, 8);
            //SCR_end(); /* JMP 31-10-2012 pour ne pas tirer tout SCR dans le link */
            exit(0);
        }
        if(rc == 7 && memcmp(txt, "S4MSGS4", 7) == 0) {
            WSockWrite(cltsock, "S4MSGS4OK", 9);
            Debug("S4C: Réception d'un message du serveur\n");
            msg_txt = SCR_malloc(4096);
            WSockReadRecord(cltsock, msg_txt);
            if(!msg)
                PG_display_error(msg_txt);
            msg = 1;
        }
    }

    if(WSOCK_DEBUG > 1) {
        //Debug("Avant debugbuf\n");
        WSockDebugBuf(txt, rc);
        //Debug("Après debugbuf\n");
    }
//    if(WSOCK_DEBUG) DebugE("Ok (rc=%2.2d)", rc);
//  DebugE("Ok (rc=%2.2d)", rc);

    return(rc);
}

/* =================================================================
Fonction serveur et client.
Lit maximum lg sur le socket cltsock mais ne "consomme" pas les bytes. La prochaine
lecture sur le socket contiendra toujours les bytes lus.

&EN cltsock : numéro du socket
&EN txt : buffer
&EN lg  : nombre max. de bytes à lire

&RT
&EN < 0 : erreur
&EN >= 0 : nombre de bytes lus
================================================================= */

WSockPeek(int cltsock, char *txt, int lg)
{
    int     rc;

    rc = WSockFlush(cltsock);
    if(rc) return(rc);
    if(WSOCK_DEBUG) DebugB("P(%3.3d) : %5.5d bytes ...", cltsock, lg);
    rc = recv(cltsock, txt, lg, MSG_PEEK);
    if(rc == SOCKET_ERROR) {
        if(WSOCK_DEBUG) DebugE("Failed (rc=%2.2d)", rc);
    }
    else {
        if(WSOCK_DEBUG) DebugE("Ok (rc=%2.2d)", rc);
    }

    if(rc > 0) WSockDecrypt(txt, txt, rc); /* JMP 06-11-2003 */
    return(rc);
}

/* =================================================================
Fonction serveur et client.
Lit exactement lg bytes sur le socket cltsock.

&EN cltsock : numéro du socket
&EN txt : buffer
&EN lg  : nombre exact de bytes à lire

&RT
&EN < 0 : erreur
&EN 0 : succès
================================================================= */

WSockReadLgBytes(int cltsock, char *txt, int lg)
{
    int     rc, nb = 0;

    if(lg == 0) return(0);
    while(1) {
        rc = WSockRead(cltsock, txt, lg);
        if(rc < 0) {
//          Debug("WSockReadLgBytes : WSockRead = %d\n", rc);
            return(rc);
        }
        if(rc < lg) {
            lg -= rc;
            txt += rc;
        }
        else return(0);
    }
}

/* =================================================================
Fonction serveur et client.
Lit un record dont la longueur est contenue dans les 4 premiers bytes
sur socket cltsock.

&EN cltsock : numéro du socket
&EN txt : buffer
&EN lg  : nombre de bytes lus

&RT
&EN < 0 : erreur
&EN >= 0 : nombre de bytes du record
================================================================= */

WSockReadRecord(int cltsock, char *txt)
{
    int     rc;
    //long    lg;
    U_sh    slg; /* JMP 11-04-00 */

//    rc = WSockReadLgBytes(cltsock, (char *)&lg, sizeof(long));
    rc = WSockReadLgBytes(cltsock, (char *)&slg, sizeof(short));
    if(rc) return(rc);
//    if(lg == 0) return(0);
    if(slg == 0) return(0);
    rc = WSockReadLgBytes(cltsock, txt, (int)slg);
    return(rc);
}

/* =================================================================
Fonction serveur et client.
Lit et alloue un record dont la longueur est contenue dans les 4 premiers bytes
sur socket cltsock.

&EN cltsock : numéro du socket
&EN txt : buffer
&EN lg  : nombre de bytes lus

&RT
&EN < 0 : erreur
&EN 0 : nombre de bytes du record
================================================================= */

WSockReadRecordAlloc(int cltsock, char **txt)
{
    int     rc;
    //long    lg;
    U_sh    slg; /* JMP 11-04-00 */

    *txt = 0;
//    rc = WSockReadLgBytes(cltsock, (char *)&lg, sizeof(long));
    rc = WSockReadLgBytes(cltsock, (char *)&slg, sizeof(short));
    if(rc) return(rc);
//    if(lg == 0) return(0);
    if(slg == 0) return(0);
    *txt = SCR_malloc((int)slg);
    rc = WSockReadLgBytes(cltsock, *txt, (int)slg);
    return(rc);
}

/* =================================================================
Fonction serveur et client.
Lit un Zstring dans txt. Txt doit être de taille suffisante (en tout
cas, min 20 bytes). Le string ne peut excéder 2048 bytes.

&EN cltsock : numéro du socket
&EN txt : buffer

&RT
&EN < 0 : erreur
&EN >= 0 : nombre de bytes lus
================================================================= */

WSockReadString(int cltsock, char *txt)
{
    int     rc, lg = 0, i;

    while(1) {
        if(lg < 2037) lg += 256; /* JMP 08-11-2003 */
        rc = WSockPeek(cltsock, txt, lg);
        if(rc < 0) return(rc);
        for(i = 0 ; i < rc ; i++) {
            if(txt[i] == 0 || (i == rc - 1 && rc < lg)) {   /* JMP 06-11-2003 */
                rc = WSockReadLgBytes(cltsock, txt, i + 1);
                txt[i + 1] = 0; /* JMP 06-11-2003 */
                return(rc);
            }
        }
    }
}

/* =================================================================
Retourne l'adresse IP d'un host.

&EN servername: nom du host
&EN int ip[4] : adresse ip du host

&RT
&EN 0 : 0k
&EN -1 : erreur à l'initialisation de winsock
&EN -3 : host not found
================================================================= */

int WSockGetIp(char *servername, int *ip)
{
    struct hostent     *hp;
#if !defined(DOSW32) && !defined(SCRW32)
    struct hostent     *gethostbyname();
#endif
    int                 i, rc = 0;

    // Initiate Winsock.
    if(WSockStartup() != 0) return(-1);

    hp = gethostbyname(servername);
    if(hp == 0) {
        rc = -3;
        goto cleanup;
    }

    for(i = 0 ; i < 4 ; i++)
        ip[i] = ((U_ch *)hp->h_addr)[i];

cleanup:
    WSockCleanup();
    return(rc);
}
/* =================================================================
Retourne le nom de la machine courante ainsi que son adresse IP.

&EN name: nom du host (64 bytes min)
&EN int ip[4] : adresse ip du host

&RT
&EN 0 : 0k
&EN -1 : erreur à l'initialisation de winsock
&EN -3 : host not found
================================================================= */

int WSockGetHost(char *name, int *ip)
{
    //struct hostent     *hp;
    int                 rc = 0;

    // Initiate Winsock.
    if(WSockStartup() != 0) return(-1);

    if(gethostname(name, 64)) {
        rc = -3;
        goto cleanup;
    }

    rc = WSockGetIp(name, ip);

cleanup:
    WSockCleanup();
    return(rc);
}

/************************* CRYPTAGE ****************************************

Fonctions de cryptage-décryptage dépendant :
    1. de la position dans le stream des échanges (le codage de 'a' change)
    2. de la seconde de démarrage de la connexion (le codage change dans le temps)
    3. du caractère précédent                     (le codage change selon ce qui précède)
    4. de tables de cryptage aléatoires
    5. une clé d'encryptage
****************************************************************************/

unsigned char *WSOCK_CRYPTBUF = "abcedfghijklm123sdfezjfsd123x'{(éàç";

static unsigned char WSOCK_CRTB[][256] = {
    81, 129, 218, 119,  23, 141, 192, 131, 186,  46, 249, 174,  84, 245, 187, 164,
    253, 150, 236,  29,  32, 188,  72, 117, 172, 111,  12, 204, 163,  98, 181, 199,
    21,  33,  86, 219,  37, 169, 213,  31, 251,  76, 214, 212,  28, 161, 190, 157,
    211, 113,  99,  83, 104,  49, 200, 148, 166,  25,  88,  34,   4,  96,  60,  73,
    59, 185, 138, 154, 108,   1, 142, 159, 106, 147, 227, 252, 102,  57,  95, 121,
    64, 183,  42,  90, 205, 193, 242, 208, 177,  89,  65,  63, 221, 195, 124, 136,
    77, 231,   3, 247, 224, 207,  50,  71, 122, 105, 235,   9, 180, 209, 210, 101,
    38, 167, 233, 156, 149, 184, 134, 246, 160, 112, 182, 171, 237,  61, 250, 170,
    241,  44,  51, 107,  20,  10,  48,  35, 194, 230, 128, 254, 196,  17,  67, 243,
    11, 137, 158, 198,   8,  19, 135,  47,  53, 226, 203, 223,  40, 145,  82,  93,
    27, 238,  55,  87, 109, 110,  91,  79,   2, 103, 165, 228, 225, 176,  14, 162,
    54,  36, 125,   7,  26, 120,  94,  56, 232,   6,  66, 155,  75, 189, 234,  68,
    220,  69,  22,  13,  18, 116,  43, 206, 201, 248, 244, 140, 255,  80, 222, 100,
    52, 144, 191,  16, 173,  97,  45, 151,  85, 215, 175, 123, 127,  92, 202, 130,
    217,   0, 114,  58, 146, 168,  62, 152,  24,  15, 216,  78, 179, 143, 132,  39,
    240,  70, 133,   5, 239,  74, 197, 178, 118,  30, 229, 126, 153, 139,  41, 115,

    232,  49,  22,   5,  61, 198,  37, 206,   1,  82, 160,  55, 187, 107, 109, 165,
    218, 106, 190, 142,  16, 203, 118, 209, 147,  98,  56, 143,  72, 202, 125, 235,
    100, 224, 251,  73,  66, 213,  46, 245,  30, 186,  44, 243, 114,  52, 104, 207,
    101,  81, 226, 231, 115, 240,  12,  14, 239, 112, 223, 182, 180, 227,  10, 185,
    229, 108,  93, 113, 173, 136, 117, 139, 234,  23, 150, 189, 162, 205,   8, 130,
    242, 140,  67,   2, 138, 194,  92, 174, 238, 166,  39, 237, 148,  87, 254, 252,
    124, 191, 176,  15,  29, 196, 116, 151,  65, 248, 110, 170, 181, 200, 171,  91,
    201, 212, 210, 214,   3, 129,  89,  62,  20,  74, 216, 163,  18, 128,  17,  19,
    127, 188, 204, 123, 217,  42,  31,  43, 137,  47,  60, 250,  99,  28,  57, 193,
    78, 146, 184,  96, 159, 169, 155, 126, 154,  13,  27, 105,  58, 145, 131, 156,
    45, 158,  38,  51,  24, 103, 152, 241, 253, 208, 161,  85, 102,  79, 132, 177,
    25, 175, 244, 153, 255, 179, 246, 122,  70,  86,  36, 111,  59,  32, 178, 249,
    33,  83,  41,  80,  94, 183,  34, 220,  21,   9, 215,  63, 197,  76,  95, 236,
    48, 144, 157, 199,  40, 211,  26, 121, 149, 172,  53, 135, 141, 168,  75,   4,
    88,   6, 133,  71, 167,  68, 134, 119,  97,  77, 164, 219,   7, 221,   0,  84,
    247,  64, 120,  69,  54, 192, 225, 228, 222, 233,  35, 230,  50, 195,  11,  90,

    243,  10,  46, 208, 110, 220, 231, 114, 181, 210, 103,   5,  87, 223,  20, 206,
    57,  31, 116, 138, 254, 203,  51, 253, 235, 196, 102, 168, 241,  52, 132, 204,
    122, 128,  59,  77,  45,  73,  82, 152,  97,   3,  67, 162,  53, 209,  84, 131,
    36, 228, 212,  68, 227,  78, 140,  56, 134, 219, 105, 171, 240, 154,  34,  28,
    236,  40,  24, 221,  39,  47, 142, 190,  79,  41,  14, 214, 180,  15,  32, 211,
    23, 101, 178, 167,  63,  86, 192, 123,  72, 217, 207, 164, 250, 115, 188,  30,
    149, 160,  92,  71, 233, 163,  98,   7,  74, 229, 200,  11, 216,  76, 166, 224,
    137, 124, 184, 150, 121,  33,  29, 234, 151,  42, 197, 255, 156, 159,  25, 249,
    107,  54,   0, 248,  75,  43,  80, 213,  94,  70,  22, 199,  65, 109,  81, 225,
    252, 194, 176,  88, 111, 139,  17,  21,  48, 130,  62,   1,  91, 145, 112, 183,
    118, 222, 182, 175,   9, 205,   8, 135,  66, 117,  16,  85, 247, 218, 232, 244,
    136,  18,  60, 155, 238, 195,  99,  58, 170, 143,  38, 125,  55,  96, 169,  12,
    35, 198, 191,  61, 193,  64,  69, 245,   4, 202, 113, 148,  44, 161,   2, 127,
    95, 141, 179,  27, 226, 165, 126, 144, 106, 100, 158,  37, 120, 104,  26,   6,
    49,  90,  13,  50, 215, 108, 187,  93, 246, 237, 133, 172, 239, 230, 201, 186,
    147, 153, 129, 189, 177, 157, 146, 173,  19, 251, 174, 185,  89, 242, 119,  83,

    240,  52, 118, 210,  40, 254, 111,  50, 181,  30,  28, 171,  41, 133,  96,  26,
    245,  31, 117,  21, 170,  51,  53,  55, 191,   8, 148, 169,  68, 250,   1,  37,
    89, 102, 192, 109, 134, 131, 235, 143,   2,   3, 184, 253,  38,  91,  29,  11,
    19, 150, 138,   0, 202, 183, 166,  15, 221,  27,  65,  54,  59, 189, 225, 107,
    242, 172, 255, 244, 190,  20,  99, 129,   4, 201, 252, 121,  58,  48, 106, 145,
    9, 197, 161,   5, 176,  80, 224, 135,  78,  10, 126, 239, 153, 147, 217, 227,
    152,  76,  87, 157, 128, 142, 195,  61,  72,  93,  49, 206, 160, 137,  42,  69,
    16,  14,  13, 144, 247, 123, 208, 213, 159, 241, 204, 105, 238, 193, 139, 234,
    18, 219, 185, 164,   7, 112,   6, 182, 101, 174, 243, 156,  63, 127, 218, 232,
    81,  84,  57,  35, 114, 231, 187,  60,  43, 196, 188,  33, 223,  36, 246,  67,
    24, 124, 141, 115,  95, 212,  90,  97, 220, 113, 110,  74, 103, 209, 122,  56,
    248, 165, 100, 216, 154, 198,  83, 199, 233, 211,  23,  77,  92, 175,  34,  44,
    66,  82,  39,  47, 108, 177, 140, 236, 205, 178,  79, 200,  64,  88, 120, 167,
    168,  86, 151, 132, 104,  45,  73, 207, 203, 149, 146,  17,  32, 155, 230,  98,
    130, 226, 119,  25, 158, 215,  75,  46,  22,  85,  12,  62, 163, 222, 237,  94,
    214, 194, 180, 186, 251, 162, 228, 125, 249, 136, 116,  70, 179,  71, 173, 229
};

static unsigned char WSOCK_DECRTB[][256] = {

    225,  69, 168,  98,  60, 243, 185, 179, 148, 107, 133, 144,  26, 195, 174, 233,
    211, 141, 196, 149, 132,  32, 194,   4, 232,  57, 180, 160,  44,  19, 249,  39,
    20,  33,  59, 135, 177,  36, 112, 239, 156, 254,  82, 198, 129, 214,   9, 151,
    134,  53, 102, 130, 208, 152, 176, 162, 183,  77, 227,  64,  62, 125, 230,  91,
    80,  90, 186, 142, 191, 193, 241, 103,  22,  63, 245, 188,  41,  96, 235, 167,
    205,   0, 158,  51,  12, 216,  34, 163,  58,  89,  83, 166, 221, 159, 182,  78,
    61, 213,  29,  50, 207, 111,  76, 169,  52, 105,  72, 131,  68, 164, 165,  25,
    121,  49, 226, 255, 197,  23, 248,   3, 181,  79, 104, 219,  94, 178, 251, 220,
    138,   1, 223,   7, 238, 242, 118, 150,  95, 145,  66, 253, 203,   5,  70, 237,
    209, 157, 228,  73,  55, 116,  17, 215, 231, 252,  67, 187, 115,  47, 146,  71,
    120,  45, 175,  28,  15, 170,  56, 113, 229,  37, 127, 123,  24, 212,  11, 218,
    173,  88, 247, 236, 108,  30, 122,  81, 117,  65,   8,  14,  21, 189,  46, 210,
    6,  85, 136,  93, 140, 246, 147,  31,  54, 200, 222, 154,  27,  84, 199, 101,
    87, 109, 110,  48,  43,  38,  42, 217, 234, 224,   2,  35, 192,  92, 206, 155,
    100, 172, 153,  74, 171, 250, 137,  97, 184, 114, 190, 106,  18, 124, 161, 244,
    240, 128,  86, 143, 202,  13, 119,  99, 201,  10, 126,  40,  75,  16, 139, 204,

    238,   8,  83, 116, 223,   3, 225, 236,  78, 201,  62, 254,  54, 153,  55,  99,
    20, 126, 124, 127, 120, 200,   2,  73, 164, 176, 214, 154, 141, 100,  40, 134,
    189, 192, 198, 250, 186,   6, 162,  90, 212, 194, 133, 135,  42, 160,  38, 137,
    208,   1, 252, 163,  45, 218, 244,  11,  26, 142, 156, 188, 138,   4, 119, 203,
    241, 104,  36,  82, 229, 243, 184, 227,  28,  35, 121, 222, 205, 233, 144, 173,
    195,  49,   9, 193, 239, 171, 185,  93, 224, 118, 255, 111,  86,  66, 196, 206,
    147, 232,  25, 140,  32,  48, 172, 165,  46, 155,  17,  13,  65,  14, 106, 187,
    57,  67,  44,  52, 102,  70,  22, 231, 242, 215, 183, 131,  96,  30, 151, 128,
    125, 117,  79, 158, 174, 226, 230, 219,  69, 136,  84,  71,  81, 220,  19,  27,
    209, 157, 145,  24,  92, 216,  74, 103, 166, 179, 152, 150, 159, 210, 161, 148,
    10, 170,  76, 123, 234,  15,  89, 228, 221, 149, 107, 110, 217,  68,  87, 177,
    98, 175, 190, 181,  60, 108,  59, 197, 146,  63,  41,  12, 129,  75,  18,  97,
    245, 143,  85, 253, 101, 204,   5, 211, 109, 112,  29,  21, 130,  77,   7,  47,
    169,  23, 114, 213, 113,  37, 115, 202, 122, 132,  16, 235, 199, 237, 248,  58,
    33, 246,  50,  61, 247,  64, 251,  51,   0, 249,  72,  31, 207,  91,  88,  56,
    53, 167,  80,  43, 178,  39, 182, 240, 105, 191, 139,  34,  95, 168,  94, 180,

    130, 155, 206,  41, 200,  11, 223, 103, 166, 164,   1, 107, 191, 226,  74,  77,
    170, 150, 177, 248,  14, 151, 138,  80,  66, 126, 222, 211,  63, 118,  95,  17,
    78, 117,  62, 192,  48, 219, 186,  68,  65,  73, 121, 133, 204,  36,   2,  69,
    152, 224, 227,  22,  29,  44, 129, 188,  55,  16, 183,  34, 178, 195, 154,  84,
    197, 140, 168,  42,  51, 198, 137,  99,  88,  37, 104, 132, 109,  35,  53,  72,
    134, 142,  38, 255,  46, 171,  85,  12, 147, 252, 225, 156,  98, 231, 136, 208,
    189,  40, 102, 182, 217,  81,  26,  10, 221,  58, 216, 128, 229, 141,   4, 148,
    158, 202,   7,  93,  18, 169, 160, 254, 220, 116,  32,  87, 113, 187, 214, 207,
    33, 242, 153,  47,  30, 234,  56, 167, 176, 112,  19, 149,  54, 209,  70, 185,
    215, 157, 246, 240, 203,  96, 115, 120,  39, 241,  61, 179, 124, 245, 218, 125,
    97, 205,  43, 101,  91, 213, 110,  83,  27, 190, 184,  59, 235, 247, 250, 163,
    146, 244,  82, 210,  76,   8, 162, 159, 114, 251, 239, 230,  94, 243,  71, 194,
    86, 196, 145, 181,  25, 122, 193, 139, 106, 238, 201,  21,  31, 165,  15,  90,
    3,  45,   9,  79,  50, 135,  75, 228, 108,  89, 173,  57,   5,  67, 161,  13,
    111, 143, 212,  52,  49, 105, 237,   6, 174, 100, 119,  24,  64, 233, 180, 236,
    60,  28, 253,   0, 175, 199, 232, 172, 131, 127,  92, 249, 144,  23,  20, 123,

    51,  30,  40,  41,  72,  83, 134, 132,  25,  80,  89,  47, 234, 114, 113,  55,
    112, 219, 128,  48,  69,  19, 232, 186, 160, 227,  15,  57,  10,  46,   9,  17,
    220, 155, 190, 147, 157,  31,  44, 194,   4,  12, 110, 152, 191, 213, 231, 195,
    77, 106,   7,  21,   1,  22,  59,  23, 175, 146,  76,  60, 151, 103, 235, 140,
    204,  58, 192, 159,  28, 111, 251, 253, 104, 214, 171, 230,  97, 187,  88, 202,
    85, 144, 193, 182, 145, 233, 209,  98, 205,  32, 166,  45, 188, 105, 239, 164,
    14, 167, 223,  70, 178, 136,  33, 172, 212, 123,  78,  63, 196,  35, 170,   6,
    133, 169, 148, 163, 250,  18,   2, 226, 206,  75, 174, 117, 161, 247,  90, 141,
    100,  71, 224,  37, 211,  13,  36,  87, 249, 109,  50, 126, 198, 162, 101,  39,
    115,  79, 218,  93,  26, 217,  49, 210,  96,  92, 180, 221, 139,  99, 228, 120,
    108,  82, 245, 236, 131, 177,  54, 207, 208,  27,  20,  11,  65, 254, 137, 189,
    84, 197, 201, 252, 242,   8, 135,  53,  42, 130, 243, 150, 154,  61,  68,  24,
    34, 125, 241, 102, 153,  81, 181, 183, 203,  73,  52, 216, 122, 200, 107, 215,
    118, 173,   3, 185, 165, 119, 240, 229, 179,  94, 142, 129, 168,  56, 237, 156,
    86,  62, 225,  95, 246, 255, 222, 149, 143, 184, 127,  38, 199, 238, 124,  91,
    0, 121,  64, 138,  67,  16, 158, 116, 176, 248,  29, 244,  74,  43,   5,  66
};


/*NH*/
WSockCryptEx(U_ch *res, U_ch *buf, int lg, int setpos)
{
    int             i, cpos;
    unsigned char   b0, b1, b2, cprec, byte;
    static int      pos = 0;
    static unsigned char prec = 100;
    unsigned long   c0;

//    Debug("Crypt[%d] : Pos=%d : lg=%03d ", WSOCK_CRYPTKEY, pos, lg);
//    for(i = 0 ; i < lg ; i++) Debug("%02x", buf[i]);
//    Debug("\n");

    if(WSOCK_CRYPTKEY <= 0) {
        memcpy(res, buf, lg);
    }
    else {
        if(setpos <= 0) {
            cpos = pos;
            cprec = prec;
        }
        else {
            cpos = setpos;
            cprec = 123;
        }

        for(i = 0 ; i < lg ; i++) {
            c0 = WSOCK_CRYPTKEY >> (8 * (cpos % 4));
            b0 = 0xFF & c0;
            b1 = b0 ^ cpos;
            byte = buf[i];
            b2 = buf[i] ^ b1;
            res[i] = WSOCK_CRTB[cprec % 4][b2];
            cpos++;
            cprec = byte;
        }

    }
    if(setpos <= 0) {
        pos += lg;
        prec = cprec;
    }
    return(0);
}

/*NH*/
WSockDecryptEx(U_ch *res, U_ch *buf, int lg, int setpos)
{
    int             i, cpos;
    unsigned char   b0, b1, b2, cprec;
    static int      pos = 0;
    static unsigned char prec = 100;
    unsigned long   c0;

    if(WSOCK_CRYPTKEY <= 0) {
        memcpy(res, buf, lg);
    }
    else {
        if(setpos <= 0) {
            cpos = pos;
            cprec = prec;
        }
        else {
            cpos = setpos;
            cprec = 123;
        }

        for(i = 0 ; i < lg ; i++) {
            c0 = WSOCK_CRYPTKEY >> (8 * (cpos % 4));
            b0 = 0xFF & c0;
            b1 = b0 ^ cpos;
            b2 = WSOCK_DECRTB[cprec % 4][buf[i]];
            res[i] = b2 ^ b1;
            cpos++;
            cprec = res[i];
        }
    }
//    Debug("DeCrypt[%d] : Pos=%d : lg=%03d ", WSOCK_CRYPTKEY, pos, lg);
//    for(i = 0 ; i < lg ; i++) Debug("%02x", res[i]);
//    Debug("\n");
    if(setpos <= 0) {
        pos += lg;
        prec = cprec;
    }
    return(0);
}

/*NH*/
WSockCrypt(U_ch *res, U_ch *buf, int lg)
{
    return(WSockCryptEx(res, buf, lg, 0));
}

/*NH*/
WSockDecrypt(U_ch *res, U_ch *buf, int lg)
{
    return(WSockDecryptEx(res, buf, lg, 0));
}


#endif

















