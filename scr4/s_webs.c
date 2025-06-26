/*NH JMP 2/8/2014

TODO
    !!! UTF-8 !!! Message => fichiers SCR en UTF-8 ou codage des échanges en UTF-8 ?
	WebSocket connection to 'ws://localhost:12346/' failed:
	Could not decode a text frame as UTF-8.
  X disconnect
    binary data
    ping / pong
    link scr act_functions => exécutable spécifique
  X cleanup code
  X sha1 -> scr + doc
    struct / client instead of globals (for threads)
    replacer WSock* par fns de base
    intégrer dans s4issrv avec primitives JS  :
	load objs, init_database, link pg -> html, ...
*/

/*
WcrWebSockets : server WebSockets
══════════════════════════════════
Ce groupe de fonctions implémente en C une version limitée des "WebSockets".
Il s'agit de définir, du coté serveur, les fonctions minimales permettant de gérer de manière suffisante un serveur websocket.

&TI Intérêt des WebSockets
══════════════════════════════════
Contrairement à Ajax, les WebSockets permettent de créer une connexion full duplex entre un client et un serveur. Dès que la connexion est établie, le contexte coté serveur reste actif en permanence et la performance de ce type de serveur, en tout cas du point de vue du client, est beaucoup plus grande que ce qui peut être imaginé à travers l'implémentation du protocole stateless comme http qui nécessite une reconnexion à chaque nouvelle requête.

Dans le cadre de SCR/AL1,  il est parfaitement concevable de démarrer côté serveur une application qui charge en mémoire l'ensemble des objets d'une application, utilise toutes les fonctionnalités d'SCR/AL1, notamment les actions, pour alimenter une page ou un isam, et d'exporter les résultats à l'intérieur même du browser.

&TI Implémentation
═════════════════
Le programmeur peut se contenter d'appeler une fonction d'initialisation du serveur, fonction à laquelle il devra passer un pointeur vers la fonction qui implémente réellement les fonctionnalités du côté du serveur. Cette fonction reçoit deux arguments :
&EN char * qui représente ce qui était envoyé par le client au serveur et
&EN char ** qui contient le résultat alloué de la commande.

Deux cas doivent être distingués : serveur multiprocess, serveur multithread.

Dans les deux cas une même fonction va être appelée. Cette fonction, WscrWebSocketServerInit(), aura comme paramètres :

&EN la fonction utilisateur principale
&EN le port d'écoute
&EN le nom du process à lancer s'il s'agit d'un serveur multi-process, NULL pour le cas des serveurs multi-thread.

Il faut bien noter qu'une bonne partie des variables définies dans le cadre des websockets sont des variables globales. Il est donc souvent plus approprié de travailler avec un serveur multiprocess qu'avec un serveur multithread.

Dans le cas du serveur multiprocesseur, une fonction supplémentaire sera appelée, mais uniquement dans le cas où le socket est déjà créé par l'appel d'un client au serveur. Cette fonction, WscrWebSocketInstanceInit(), créer une instance du process qui sera à l'écoute et qui répondra à toutes les requêtes du client. Elle prend comme paramètre le socket ouvert et le pointeur de fonction serveur.

&EN int WscrWebSocketServerInit(int (*fn)(U_ch *, U_ch **), int port, char *progname)
&EN int WscrWebSocketInstanceInit(int sock, int (*fn)(U_ch *, U_ch **))

&TI Fonction serveur
═════════════════
La fonction serveur prend de paramètres et retourne un entier :
&EN  le premier paramètre est un pointeur vers le string envoyé par les clients aux serveurs
&EN le second paramètre est un charrette l'étoile allouée par la fonction et qui sera renvoyé comme réponse au client.  Ce pointeur est automatiquement libéré par la fonction appelante.

Lorsque cette fonction retourne une valeur négative le serveur est fermé. Dans tous les autres cas la boucle se poursuit.

&IT Exemple
═════════════════
l'exemple qui suit propose une implémentation d'un serveur fonctionnant selon le choix de l'utilisateur en mode multiprocess ou en mode multi-thread.

&CO
main(argc, argv)
int     argc;
char    *argv[];
{
    int     rc, port = 5557, sock = -1, i,
	    TOrP = 1; // Thread of Process ?
    char    buf[120];

    for(i = 1 ; i < argc ; i++) {
	     if(strcmp(argv[i], "-p") == 0 && argc > i + 1) port = atoi(argv[i + 1]);
	else if(strcmp(argv[i], "-s") == 0 && argc > i + 1) sock = atoi(argv[i + 1]);
	else if(strcmp(argv[i], "-t") == 0) TOrP = 0;
	else if(strcmp(argv[i], "-h") == 0) Syntax();
    }

    if(sock < 0) {
	// Démarrage du serveur : attente de connexion avant fork ou thread
	rc = WscrWebSocketServerInit(MyWebSocketFn, port, (TOrP ? argv[0] : NULL));
    }
    else {
	// Connexion établie, lancement de la fonction WebSocket()
	Debug("\n*** Starting server instance - pid=%d - sock = %d\n", getpid(), sock);
	rc = WscrWebSocketInstanceInit(sock, MyWebSocketFn);
    }

    exit(rc);
}

&TX

&TI Implémentation côté client
═════════════════
Le protocole websocket est défini à peu près dans tous les browsers.  L'exemple qui suit montre comment se connecter à un serveur, puis envoyer des commandes et recevoir des réponses du serveur.  Il est théoriquement possible de créer un client websocket penser. L'intérêt d'une telle fonctionnalité semble limité dans la mesure où le but principal est précisément de pouvoir utiliser des fonctions écrites en C ou les autres langages directement à travers un browser.


&CO
<script>
var socket;

function init(){
  var host = "ws://localhost:12346";

  try{
    if(socket) quit();
    socket = new WebSocket(host);
    log('WebSocket - status '+socket.readyState);
    socket.onopen    = function(msg){ log("Welcome - status "+this.readyState); };
    socket.onmessage = function(msg){ log("Received: "+msg.data); };
    socket.onclose   = function(msg){ log("Disconnected - status "+this.readyState); };
  }
  catch(ex){ log(ex); }
  $("msg").focus();
}

function send(){
  var txt,msg;
  txt = $("msg");
  msg = txt.value;
  if(!msg){ alert("Message can not be empty"); return; }
  txt.value="";
  txt.focus();
  try{ socket.send(msg); log('Sent: '+msg); } catch(ex){ log(ex); }
}
function quit(){
  log("Goodbye!");
  socket.close();
  socket=null;
}

// Utilities
function $(id){ return document.getElementById(id); }
function log(msg){ $("log").innerHTML+="<br>"+msg; }
function myclear(){ $("log").innerHTML = ""; }
function onkey(event){ if(event.keyCode==13){ send(); } }
</script>

&TX



*/


#include "scr4w.h"
#include <winsock.h>

#define WSH_FIN      0x80      // 0x80 = final message if set
#define WSH_RSV      0x00      // 0x70 = zero unless extension negotiated
#define WSH_OPCODE   0x0f      // 0x0f = opcode:
#define WSH_opCONT   0x00      //  continuation
#define WSH_opTEXT   0x01      //  text
#define WSH_opBIN    0x02      //  binary
				  //  0x03..0x07 reserved
#define WSH_opCLOSE  0x08      //  connection close
#define WSH_opPING   0x09      //  ping
#define WSH_opPONG   0x0a      //  pong
				    //  0x0b..0x0f reserved
    // 2nd byte:
#define WSH_MASK     0x80      // MASK bit (masked if set, 4 mask bytes follow length)
#define WSH_PAYLEN   0x7f      // payload length 0..125
				    // if 126, then 2 following bytes= 16-bit length
				    // if 127, then 8 following bytes= 63-bit length (bit63=0)
// Close frame codes
//
#define CLOSE_NORMAL         1000     // RFC 6455 defies more status codes
#define CLOSE_ENDPOINT       1001     // but we only use these
#define CLOSE_PROTOCOL_ERR   1002
#define CLOSE_DATA_ERR       1003

#define TIMEOUT_SECS         20         // timeout for listen, ping is sent to all clients at this rate
#define MAX_PAYLOAD_LEN     (32*1024*1024)  // maximum size of payload, default= 32MB (!)

int     (*WEBS_FN)(U_ch *, U_ch **);
char    *WEBS_PROTOCOL = 0,
	*WEBS_ORIGIN = 0,
	*WEBS_KEY = 0,
	*WEBS_MAGIC = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
U_ch    *WEBS_READMSG = 0, *WEBS_BUFFER = 0, *WEBS_RESPONSE = 0;
int     WEBS_VERSION = 0,
	WEBS_APPEND = 0,
	WEBS_COMPLETE = 0,
	WEBS_LASTMSG = 0;



/* =================================================================
Read, assemble and translate one message on socket sock.
&EN sock : open socket

The result si saved in U_ch *WEBS_READMSG
which is allocated, but desallocated during the next call.

&RT 0 = ok, -1 : error
------------------------------------------------------------------- */

int WscrWebSocketReadMsg(int sock)
{
    U_ch    txt[4096];

    SCR_free(WEBS_READMSG);
    SCR_free(WEBS_BUFFER);
    WEBS_READMSG = WEBS_BUFFER = 0;
    WEBS_APPEND = WEBS_COMPLETE = 0;

    while(1) {
	if(WSockRead(sock, txt, sizeof(txt) - 1) < 0) return(-1);
	//Debug("Read : '%s'\n", txt);
	if(WscrWebSocketReadMsg_1(sock, txt)) return(-1);
	if(WEBS_COMPLETE) return(0);
    }
}

/*NH =================================================================
Sub-fonction to read, assemble and translate messages on socket sock.
Operate on a sub message.

Result in U_ch *WEBS_READMSG (allocated, but desallocated during the next call)

&RT 0 = ok, -1 : error
------------------------------------------------------------------- */

int WscrWebSocketReadMsg_1(int sock, U_ch *txt)
{
    int     msglen, datapos, paylen, i, status, rc;
    U_ch    byte1, byte2, mask[4], *reason;

    // If waiting for more (WEBS_APPEND true), concat, else replace
    if(WEBS_APPEND) WEBS_BUFFER = SCR_strafcat(WEBS_BUFFER, txt);
    else            SCR_strfacpy(&WEBS_BUFFER, txt);

    WEBS_APPEND = WEBS_COMPLETE = 0;
    msglen= strlen(WEBS_BUFFER);
    if(msglen < 6)  return(-1); // must have at least 2-byte header + 4 mask bytes

    datapos= 2;              // default index of payload
    byte1= WEBS_BUFFER[0];
    byte2= WEBS_BUFFER[1];

    if(byte1 & WSH_FIN) WEBS_LASTMSG = 1;
    if((byte2 & WSH_MASK) == 0) return(-1); // note: a client ALWAYS masks, a server NEVER masks

    paylen= byte2 & WSH_PAYLEN;
    if(msglen < paylen )  return(-1);   // simple trap for length bytes in header
    if(paylen == 126) {
	paylen= WEBS_BUFFER[2] * 256 + WEBS_BUFFER[3];
	datapos += 2;
    }
    else if(paylen == 127 ) {
	paylen=0;
	for(i = 0; i < 8; i++)
	    paylen = paylen * 256 + WEBS_BUFFER[i + 2];
	datapos += 8;
    }

    // get 32-bit mask
    memcpy(mask, WEBS_BUFFER + datapos, 4);
    datapos += 4;  // 4 bytes of mask

    if(paylen > MAX_PAYLOAD_LEN) return(-1); // limit max. size
    if(msglen < paylen + datapos) {
	//Debug("ReadMsg : Needs more data");
	WEBS_APPEND = 1;
	return(0);
    }

    // Unmask
    for(i = 0; i < paylen; i++)
	WEBS_BUFFER[i] = WEBS_BUFFER[datapos + i] ^ mask[i % 4];
    WEBS_BUFFER[paylen] = 0;

    switch(byte1 & WSH_OPCODE) {
	case 0x00:  // continuation
	    WEBS_READMSG = SCR_strafcat(WEBS_READMSG, WEBS_BUFFER);
	    WEBS_COMPLETE = ((byte1 & WSH_FIN) != 0);
	    break;

	case 0x01:  // text
	case 0x02:  // binary
	    WEBS_READMSG = SCR_stracpy(WEBS_BUFFER);
	    WEBS_COMPLETE = ((byte1 & WSH_FIN) != 0);
	    break;
	case 0x08:  // connection close
	    status=0;
	    reason="";
	    if(paylen >= 2 )
		status= WEBS_BUFFER[0] * 256 + WEBS_BUFFER[1];
	    if(paylen > 2)
		reason= WEBS_BUFFER + 2;
	    Debug( "Connection close: status=%d, reason=\"$s\"", status, reason);
	    WscrWebServerCloseConnection(sock, CLOSE_NORMAL);
	    return(0);

	case 0x09:  // ping
	    WscrWebSocketMsg("pong", WSH_opPONG);
	    rc = WSockWrite(sock, WEBS_RESPONSE, strlen(WEBS_RESPONSE));
	    break;
	case 0x0a:  // pong ??
	    Debug("Received: pong='%s'", WEBS_BUFFER);
	    break;
	default:
	    return(-1);
    }

    return(0);
}

/* =================================================================
Check if current platform is LittleEndian.
&RT 1 if little endian, 0 otherwise
------------------------------------------------------------------- */

int SCR_isLittleEndian()
{
    char buf[2] = { 1, 0 };

    if(*(short *)buf == 1) return(1);
    return(0);
}

/* =================================================================
Swap bytes of a long integer.
------------------------------------------------------------------- */
int SCR_LongSwap (long i)
{
    unsigned char b1, b2, b3, b4;

    b1 = i & 255;
    b2 = ( i >> 8 ) & 255;
    b3 = ( i>>16 ) & 255;
    b4 = ( i>>24 ) & 255;

    return ((int)b1 << 24) + ((int)b2 << 16) + ((int)b3 << 8) + b4;
}

/* =================================================================
Translate msg of opcode type into WebSocket segment.
Result saved in allocated global U_ch *WEBS_RESPONSE.

&RT len of response
------------------------------------------------------------------- */

int WscrWebSocketMsg(U_ch *msg, int opcode)
{
    int     i, nbytes, flag = 0, len2;
    long    len;
    U_ch    lenbytes[10];

    // Alloue la taille pour le résultat
    len = strlen(msg);
    SCR_free(WEBS_RESPONSE);
    WEBS_RESPONSE = SCR_malloc(len + 10);

    // opCode
    WEBS_RESPONSE[0] = WSH_FIN + opcode;

    // Size
    memset(lenbytes, 0, sizeof(lenbytes)); //pad to 64-bit
    len2 = len;
    if(SCR_isLittleEndian())
	len2 = SCR_LongSwap(len);

    memcpy(lenbytes + 4, &len2, 4);          //lower 32bits

    nbytes = 1;                     //default= 1byte length
    if(len > 125) {
	if(len <= 0xffff){
	    flag = 126;    //flag
	    nbytes= 2;       //+2 byte length
	}else{
	    flag  = 127;    //flag
	    nbytes = 8;      //+8 byte length
	}
    }

    if(flag == 0) WEBS_RESPONSE[1] = len;
    else {
	WEBS_RESPONSE[1] = flag;
	for(i = 0; i < nbytes; i++)
	    WEBS_RESPONSE[i + 2] = lenbytes[(8 - nbytes) + i];
    }

    strcat(WEBS_RESPONSE, msg);
    return(strlen(WEBS_RESPONSE));
}
/* =================================================================
Translate text msg into WebSocket segment.
Result saved in allocated global U_ch *WEBS_RESPONSE.

&RT len of response
------------------------------------------------------------------- */

int WebSocketText(U_ch *msg)
{
    return(WscrWebSocketMsg(msg, WSH_opTEXT));
}

/*NH =================================================================
Protocol handshake.
Based on HTTP upgrade command.
&EN read data sent by client
&EN compute key and response headers
&EN send response to client

&RT 0 if ok, -1 else
------------------------------------------------------------------- */

WscrWebSocketHandshake(int sock)
{
    U_ch    **tbl, hash[512], sha1[256], code64sha1[512], response[1024], txt[4096];
    int     i, ntbl, http = 0, upgrade = 0, rc;

    if(WSockRead(sock, txt, sizeof(txt) - 1) < 0) return(-1);

    SCR_replace(txt, "\r\n", "\n");
    Debug("Received : '%s'\n", txt);
    tbl = SCR_vtom(txt, '\n');
    ntbl = SCR_tbl_size(tbl);
    for(i = 0 ; i < ntbl ; i++) {
	     if(SCR_grep("GET * HTTP*",  tbl[i], 1) == 0)               http = 1;
	else if(SCR_grep("Upgrade: websocket*",  tbl[i], 1) == 0)       upgrade = 1;
	else if(SCR_grep("Origin: *",  tbl[i], 1) == 0)                 WEBS_ORIGIN =   SCR_stracpy(tbl[i] + 8);
	else if(SCR_grep("Sec-WebSocket-Version: *",  tbl[i], 1) == 0)  WEBS_VERSION = atol(tbl[i] + 22);
	else if(SCR_grep("Sec-WebSocket-Protocol: *",  tbl[i], 1) == 0) WEBS_PROTOCOL = SCR_stracpy(tbl[i] + 24);
	else if(SCR_grep("Sec-WebSocket-Key: *",  tbl[i], 1) == 0)      WEBS_KEY =      SCR_stracpy(tbl[i] + 19);
    }
    SCR_free_tbl(tbl);

    // Check all required headers are present
    if(!(http && upgrade && WEBS_VERSION && WEBS_ORIGIN != 0 && WEBS_KEY != 0)) {
	Debug("Missing headers in client request\n");
	return(-1);
    }

    // Compose response key
    sprintf(hash, "%s%s", WEBS_KEY, WEBS_MAGIC);
    SCR_sha1(hash, sha1);
    SCR_code64(sha1, strlen(sha1), code64sha1);
    sprintf(response, "HTTP/1.1 101 Switching Protocols\r\n"
	      "Upgrade: WebSocket\r\n"
	      "Connection: Upgrade\r\n"
	      "Sec-WebSocket-Accept: %s\r\n"
	      , code64sha1);
    if(WEBS_PROTOCOL)
	sprintf(response + strlen(response), "Sec-WebSocket-Protocol: %s\r\n", WEBS_PROTOCOL);
    strcat(response, "\r\n");

    Debug("Response = '%s'\n", response);
    //rc = send(sock, response, strlen(response), 0);
    rc = WSockWrite(sock, response, strlen(response));
    if(rc < 0) {
	Debug("response error = %d\n", rc);
	return(-1);
	}
    return(0);
}

/*NH =================================================================
Close 1 connection :
&EN sends Close frame
&EN closes socket
------------------------------------------------------------------- */

int WscrWebServerCloseConnection(int sock, int reasonCode)
{
    U_ch    reason[64];

    strcpy(reason, "  Connection Closed");
    reason[0]= reasonCode / 256;
    reason[1]= reasonCode % 256;
    WscrWebSocketMsg(reason, WSH_opCLOSE);
    //$arrOpt = array('l_onoff' => 1, 'l_linger' => 1);
    //socket_set_option($client->socket, SOL_SOCKET, SO_LINGER, $arrOpt);
    //socket_write($client->socket,$bytes,$nbytes);
    WSockWrite(sock, WEBS_RESPONSE, strlen(WEBS_RESPONSE));
    return(WSockClose(sock));
}

/*NH =================================================================
Starts loop on 1 socket :

&EN handshaking
&EN recv & translate msg
&EN call callback function
&EN translate and send result

&RT ??
------------------------------------------------------------------- */

int WscrWebSocketLoop(int sock) {
    int     cursock, rc;
    U_ch    *response = 0;

    //Debug("In WscrWebSocketLoop()\n");
    cursock = WSock1Register(sock); // A LA SCR

    // First : handshake
    if(WscrWebSocketHandshake(sock)) return(-1);

    // If ok, read / wri
    while(1) {
	if(WscrWebSocketReadMsg(sock) < 0) return(-1);
	rc = (*WEBS_FN)(WEBS_READMSG, &response);
	if(rc < 0) break;
	if(response) rc = WscrWebSocketMsg(response, WSH_opTEXT);
	SCR_free(response);
	response = 0;
	// Send coded response
	rc = WSockWrite(sock, WEBS_RESPONSE, strlen(WEBS_RESPONSE));
	if(rc < 0) {
	    Debug("WscrWebSocketLoop : response error = %d\n", rc);
	    break;
	}
    }
    WscrWebServerCloseConnection(sock, CLOSE_NORMAL);
    return(rc);
}

/* =================================================================================
Fonction principale de lancement d'un serveur WebSocket.
&EN attend une connexion sur un port (dft 5557)
&EN démarre un nouveau process ou un nouveau thread selon le choix.

&EN int (*fn)(U_ch *, U_ch **) = function called at each incoming request from the client
&EN port = connection port
&EN progname = executable name (for multi process, eg argv[0])
    or null for multithread
------------------------------------------------------------------------------------ */

int WscrWebSocketServerInit(int (*fn)(U_ch *, U_ch **), int port, char *progname)
{
    int     rc;
    char    buf[512];

    if(port <= 0) port = 5557;

    // Démarrage du serveur : attente de connexion
    Debug("\n*** Starting Server - pid=%d\n", getpid());

    if(progname) {
	// multi process
	sprintf(buf, "%s -s %%d -d %d", progname, DebugActif);
	rc = WSockStartServer(port, 0, buf, 0, 4);
    }
    else {
	// multi thread
	WEBS_FN = fn;
	rc = WSockStartServer(port, WscrWebSocketLoop, 0, 1, 4);
    }

    Debug("End of connexion. rc=%d.\n", rc);
    return(rc);
}

/* =================================================================================
Version multi-process de WscrWebServer.

Démarre une instance d'un sous process WebSocket lancé par WscrWebSocketServerInit().
La connexion a déjà été établie et ce process prend en charge,
à travers la fonction fn(), la gestion des requests du client.

Cette fonction doit être appelée dans le main() lorsque le paramètre -s socknb est passé.

&EN fn(U_ch *, U_ch **) = function called at each request from client
&EN sock = socket connecté
------------------------------------------------------------------------------------ */

int WscrWebSocketInstanceInit(int sock, int (*fn)(U_ch *, U_ch **))
{
    WEBS_FN = fn;
    return(WSockStartServerProc(sock, WscrWebSocketLoop));
}


// More info on frames

	/*
	 *  04 logical framing from the spec (all this is masked when incoming
	 *  and has to be unmasked)
	 *
	 * We ignore the possibility of extension data because we don't
	 * negotiate any extensions at the moment.
	 *
	 *    0                   1                   2                   3
	 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 *   +-+-+-+-+-------+-+-------------+-------------------------------+
	 *   |F|R|R|R| opcode|R| Payload len |    Extended payload length    |
	 *   |I|S|S|S|  (4)  |S|     (7)     |             (16/63)           |
	 *   |N|V|V|V|       |V|             |   (if payload len==126/127)   |
	 *   | |1|2|3|       |4|             |                               |
	 *   +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
	 *   |     Extended payload length continued, if payload len == 127  |
	 *   + - - - - - - - - - - - - - - - +-------------------------------+
	 *   |                               |         Extension data        |
	 *   +-------------------------------+ - - - - - - - - - - - - - - - +
	 *   :                                                               :
	 *   +---------------------------------------------------------------+
	 *   :                       Application data                        :
	 *   +---------------------------------------------------------------+
	 *
	 *  We pass payload through to userland as soon as we get it, ignoring
	 *  FIN.  It's up to userland to buffer it up if it wants to see a
	 *  whole unfragmented block of the original size (which may be up to
	 *  2^63 long!)
	 */


