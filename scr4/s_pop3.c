#if defined(DOSW32) || defined(SCRW32) || defined(LINUX)

#include "scr4w.h"

//#define DEBUG
#define POP3_STDBUFSIZE 16384
char    POP3_ERROR[80];
int     POP3_DEBUG = 0;
int     POP3_CURBUFSIZE = 0;

/*NH*/
Pop3Cmd(sock, cmd, rec)
int     sock;
char    *cmd, *rec;
{
    if(Pop3Write(sock, cmd)) return(-1);
    if(Pop3Read(sock, rec, 1)) return(-1);
    //printf("R:%s\n", rec);
    return(0);
}

/*NH*/
char *Pop3CmdDot(sock, cmd)
int     sock;
char    *cmd;
{
    char    *buf = 0, rec[POP3_STDBUFSIZE + 1];
    int     pos;

    if(Pop3Write(sock, cmd)) return(buf);
    if(Pop3Read(sock, rec, 1)) return(buf);
    while(1) {
	buf = SCR_strafcat(buf, rec);
	//printf("R:%s\n", rec);
	pos = U_index(buf, "\r\n.\r");
	if(pos >= 0) {
	    buf[pos] = 0;
	    break;
	    }
	if(Pop3Read(sock, rec, 0)) break;
	}

    return(buf);
}

/*NH
char *Pop3CmdDotReadMsg(sock, cmd, size)
int     sock;
char    *cmd;
int     size;
{
    char    *buf = 0, rec[POP3_STDBUFSIZE + 1];
    int     pos, lg = 0, lgrec;

    if(Pop3Write(sock, cmd)) return(buf);
    if(Pop3Read(sock, rec, 1)) return(buf);
    buf = SCR_malloc(size + POP3_STDBUFSIZE);
    while(1) {
	lgrec = strlen(rec);
	if(lg + lgrec >= size + POP3_STDBUFSIZE) {
	    size += 4096;
	    buf = SCR_realloc(buf, 1, size, size + POP3_STDBUFSIZE);
	    }
	strcat(buf, rec);
	lg += lgrec;
	pos = U_index(buf, "\r\n.\r");
	if(pos >= 0) {
	    buf[pos] = 0;
	    break;
	    }
	if(Pop3Read(sock, rec, 0)) break;
	}

    return(buf);
}
*/
/*NH Nouvelle version */
char *Pop3CmdDotReadMsg(sock, cmd, size)
int     sock;
char    *cmd;
int     size;
{
    char    *buf = 0, rec[1 + POP3_STDBUFSIZE];
    int     pos, posf, lg = 0, lgrec;

    if(Pop3Write(sock, cmd)) return(buf);
    if(Pop3Read(sock, rec, 1)) return(buf);
    buf = SCR_malloc(size + POP3_STDBUFSIZE);
    while(1) {
	lgrec = strlen(rec);
	if(lg + lgrec >= size + POP3_STDBUFSIZE) {
	    size += POP3_STDBUFSIZE;
	    buf = SCR_realloc(buf, 1, size, size + POP3_STDBUFSIZE);
	    }

/*** ANCIENNE VERSION ERRONEE INCIDENTS SILMM \r\n.\r coupé sur 2 records
	pos = U_index(rec, "\r\n.\r");
	if(pos >= 0) {
	    rec[pos] = 0;
	    lgrec = strlen(rec);
	    }
	memcpy(buf + lg, rec, lgrec + 1);
	lg += lgrec;
****/

/* DEBUT Correction */
	memcpy(buf + lg, rec, lgrec + 1);       /* JMP 21/9/2006 */
	lg += lgrec;                            /* JMP 21/9/2006 */
	posf = _max_(0, lg - lgrec - 10);          /* JMP 22-09-06 */
	pos = U_index(buf + posf, "\r\n.\r");    /* JMP 21/9/2006 */
	if(pos >= 0) buf[posf + pos] = 0;       /* JMP 25/9/2006 */
/* FIN Correction */

	if(pos >= 0) break;
	if(Pop3Read(sock, rec, 0)) break;
	}

    return(buf);
}


/*NH*/
Pop3Write(sock, rec)
int     sock;
char    *rec;
{
    char    buf[80];
    int     err;

    err = WSockWrite(sock, rec, strlen(rec));

    if(POP3_DEBUG) printf("W:%s", rec);
    if(err < 0) {
	sprintf(buf, "Socket error %d", err);
	SCR_strlcpy(POP3_ERROR, buf, 80);
	return(-1);
	}
    return(0);
}

/*NH*/
Pop3Read(sock, rec, chk)
int     sock;
char    *rec;
int     chk;
{
    char    buf[80];
    int     err;
    int     cpu = WscrGetMS();
    static  double prev = 0.0;
    static  int prevbuf = 0;
    double  new;
    int     newbuf;

    if(POP3_CURBUFSIZE == 0) {
	POP3_CURBUFSIZE = 512; /* JMP 10-10-06 */
	Debug("*** TCP/IP read bufSize : %d\n", POP3_CURBUFSIZE);
	}
    err = WSockRead(sock, rec, POP3_CURBUFSIZE);
    rec[POP3_CURBUFSIZE] = 0;
    if(POP3_DEBUG) printf("R:%s", rec);
    if(err < 0) {
	sprintf(buf, "Socket error %d", err);
	SCR_strlcpy(POP3_ERROR, buf, 80);
	return(-1);
	}

    // Changement dynamique de la taille du packet si délai > MAXDELAY ou < 1sec
    cpu = WscrGetMS() - cpu;
    if(err > 128 && cpu > 0) { // Seulement si plus de 128 bytes lus
	if(prev <= 0.0) {
	    prev = (1.0 * err) / (1.0 * cpu);
	    prevbuf = POP3_CURBUFSIZE;
	    }
	new = (1.0 * err) / (1.0 * cpu);
	newbuf = POP3_CURBUFSIZE;
	if(new > prev * 1.1) {
	    Debug("Bufsize : %d -> %.2lf KB/sec.", POP3_CURBUFSIZE, new);
	    // if(POP3_CURBUFSIZE <= POP3_MAXBUFSIZE - 128) POP3_CURBUFSIZE += 128;
	    if(newbuf >= prevbuf) POP3_CURBUFSIZE += 128;
	    else                 POP3_CURBUFSIZE -= 128;
	    if(POP3_CURBUFSIZE > POP3_MAXBUFSIZE) POP3_CURBUFSIZE = POP3_MAXBUFSIZE;
	    if(POP3_CURBUFSIZE < 128) POP3_CURBUFSIZE = 128;
	    Debug(" Attempting %d\n", POP3_CURBUFSIZE);
	    }
	if(prev > new * 1.1) {
	    Debug("Bufsize : %d -> %.2lf KB/sec.", POP3_CURBUFSIZE, new);
	    //if(POP3_CURBUFSIZE >= 256) POP3_CURBUFSIZE -= 128;
	    if(newbuf >= prevbuf) POP3_CURBUFSIZE -= 128;
	    else                 POP3_CURBUFSIZE += 128;
	    if(POP3_CURBUFSIZE > POP3_MAXBUFSIZE) POP3_CURBUFSIZE = POP3_MAXBUFSIZE;
	    if(POP3_CURBUFSIZE < 128) POP3_CURBUFSIZE = 128;
	    Debug(" Attempting %d\n", POP3_CURBUFSIZE);
	    }
	prev = new;
	prevbuf = newbuf;
	}
/*
	if(cpu > POP3_MAXDELAY && POP3_CURBUFSIZE > 128) {
	    POP3_CURBUFSIZE /= 2;
	    Debug("*** Delay : %dms --> TCP/IP read bufSize : %d\n", cpu, POP3_CURBUFSIZE);
	    }
	else if(cpu < 30 && POP3_CURBUFSIZE <= 4096) {
	    POP3_CURBUFSIZE *= 2;
	    Debug("*** Delay : %dms --> TCP/IP read bufSize : %d\n", cpu, POP3_CURBUFSIZE);
	    }
	}
*/

    // Check OK si demandé
    if(chk && memcmp(rec, "+OK", 3)) {
	SCR_strlcpy(POP3_ERROR, rec, 80);
	return(-2);
	}
    return(0);
}

/*============================================================================
Connecte au POP3 serveur sur la porte port (par défaut 110) sous le login
user avec le mot de passe pass.

&EN char *server : nom du serveur (ex. "pop3.plan.be")
&EN char *user   : nom de login   (ex. "jmp")
&EN char *pass   : mot de passe   (ex. "123456")
&EN int port     : porte POP3. Si <= 0, fixée à 110.

&RT
&EN si > 0 : entier indiquant le numéro de socket ouvert
&EN -1 : connexion au serveur impossible
&EN -2 : erreur de transmission
&EN -3 : erreur retournée par le serveur

La variable POP3_ERROR contient le message d'erreur éventuel.

&SA Pop3Connect(), Pop3Close(), Pop3Nb(), Pop3GetHeaders(), Pop3GetMsgs(),
    Pop3GetHeader(), Pop3GetMsg(), Pop3FreeMsg(), Pop3Session(), Pop3Detach()
=============================================================================== */

Pop3Connect(server, user, pass, port)
char    *server, *user, *pass;
int     port;
{
    int     sock, rc;
    char    buf[POP3_STDBUFSIZE + 1];

    if(POP3_CURBUFSIZE == 0) {
	POP3_CURBUFSIZE = 512; /* JMP 10-10-06 */
	Debug("*** TCP/IP read bufSize : %d\n", POP3_CURBUFSIZE);
	}
    if(port <= 0) port = 110;
    sock = WSockConnect(server, port);
    if(sock < 0) {
	SCR_strlcpy(POP3_ERROR, "Connection rejected", 80);
	return(-1);
	}

    if(WSockRead(sock, buf, POP3_CURBUFSIZE) < 0) {
	SCR_strlcpy(POP3_ERROR, "Socket read error", 80);
	return(-2);
	}
    if(memcmp(buf, "+OK", 3)) {
	SCR_strlcpy(POP3_ERROR, buf, 80);
	return(-3);
	}

    sprintf(buf, "USER %s\r\n", user);
    rc = Pop3Cmd(sock, buf, buf);
    if(rc) return(rc);

    sprintf(buf, "PASS %s\r\n", pass);
    rc = Pop3Cmd(sock, buf, buf);
    if(rc) return(rc);

    return(sock);
}

/*============================================================================
Termine un session POP3.

&EN int sock : socket retourné par Pop3Connect()

&RT
&EN 0 : Ok
&EN -1 : erreur à la fermeture ou socket non ouvert.

La variable POP3_ERROR contient  le message d'erreur éventuel.

&SA Pop3Connect(), Pop3Close(), Pop3Nb(), Pop3GetHeaders(), Pop3GetMsgs(),
    Pop3GetHeader(), Pop3GetMsg(), Pop3FreeMsg(), Pop3Session(), Pop3Detach()
=============================================================================== */

Pop3Close(sock)
int     sock;
{
    int     rc;
    char    buf[POP3_STDBUFSIZE + 1];

    if(sock <= 0) return(-1);
    sprintf(buf, "QUIT\r\n"); /* JMP 15-02-05 */
    rc = Pop3Cmd(sock, buf, buf);
    WSockClose(sock);
    return(rc);
}

/*============================================================================
Retourne le nombre de message en attente sur le socket sock.

&EN int sock : socket retourné par Pop3Connect()

&RT
&EN entier >= 0 : nbre de messages sur le serveur pour l'utilisateur donné
&EN -1 : erreur retournée par le serveur

La variable POP3_ERROR contient  le message d'erreur éventuel.

&SA Pop3Connect(), Pop3Close(), Pop3Nb(), Pop3GetHeaders(), Pop3GetMsgs(),
    Pop3GetHeader(), Pop3GetMsg(), Pop3FreeMsg(), Pop3Session(), Pop3Detach()
=============================================================================== */

Pop3Nb(sock)
int     sock;
{
    int     nb;
    char    buf[1024], *rep;
    U_ch    **tbl;

    rep = Pop3CmdDot(sock, "LIST\r\n");
    if(rep == 0) return(-1);
    /* JMP 24-10-2003
    nb = atoi(rep + 4);
    */
    SCR_replace(rep, "\r\n", "\n");     /* JMP 24-10-2003 */
    tbl = SCR_vtoms(rep, "\n");         /* JMP 24-10-2003 */
    nb = SCR_tbl_size(tbl) - 1;         /* JMP 24-10-2003 */
    SCR_free_tbl(tbl);                  /* JMP 24-10-2003 */
    SCR_free(rep);
    return(nb);
}


/*NH*/
Pop3ReadHeader(sock, i, msg)
int     sock, i;
POP3MSG *msg;
{
    char    buf[80], buf2[POP3_STDBUFSIZE + 1], *txt;
    U_ch    **tbl;
    int     j;

    /* Taille du message */
    sprintf(buf, "LIST %d\r\n", i);
    if(Pop3Cmd(sock, buf, buf2) < 0) return(-1);
    j = U_pos(' ', buf2 + 4);
    msg->size = atoi(buf2 + j + 5);

    /* Texte du message */
    sprintf(buf, "TOP %d 10\r\n", i);
    txt = Pop3CmdDot(sock, buf);
    if(txt == 0) return(-1);
    SCR_replace(txt, "\r\n..", "\r\n.");
    SCR_replace(txt, "\r\n", "\n");
    tbl = SCR_vtoms2(txt, "\n", 0);
    SCR_free(txt);

    Pop3ReadBeg(msg, tbl);
    SCR_free_tbl(tbl);

    return(0);
}

/*NH
Retourne le nbre de lignes des headers non compris la dernière ligne blanche
*/

Pop3ReadBeg(POP3MSG *msg, U_ch **tbl)
{
    int     j, pos, lgtmp = 0;
    char    boundary[512], *tmp = 0;

    boundary[0] = 0;
    for(j = 0 ; tbl[j] ; j++) {
	if(tbl[j] == 0 || tbl[j][0] == 0) { // Fin des headers
	    msg->headers = (char **) SCR_malloc((j + 1) * sizeof(char *));
	    memcpy(msg->headers, tbl, j * sizeof(char *));
	    msg->headers[j] = 0;
	    SCR_free(tmp);
	    return(j);
	    }

	// met en minuscule les lignes de début /* JMP 23-03-2004 */
	if(strlen(tbl[j]) >= lgtmp) {
	    SCR_free(tmp);
	    lgtmp = strlen(tbl[j]) + 512;
	    tmp = SCR_malloc(lgtmp);
	    }
	strcpy(tmp, tbl[j]);
	SCR_lower(tmp);

	pos = U_index(tmp, "boundary=\"");
	if(pos > 0) {
	    boundary[0] = boundary[1] = '-';
	    strcpy(boundary + 2, tbl[j] + pos + 10);
	    pos = U_index(boundary, "\"");
	    if(pos > 0) boundary[pos] = 0;
	    msg->boundary = SCR_stracpy(boundary);
	    }
	pos = U_index(tmp, "filename=\"");
	if(pos > 0) {
	    msg->filename = SCR_stracpy(tbl[j] + pos + 10);
	    pos = U_index(msg->filename, "\"");
	    if(pos > 0) msg->filename[pos] = 0;
	    U_ljust_text(msg->filename); /* JMP 10-02-2004 */
	    SCR_strip(msg->filename);      /* JMP 10-02-2004 */
	    }
	if(msg->from == 0 && SCR_cstrncmp(tbl[j], "FROM:", 5) == 0) {
	    msg->from = SCR_stracpy(tbl[j] + 6);
	    continue;
	    }
	if(msg->to      == 0 && SCR_cstrncmp(tbl[j], "TO:", 3) == 0) {
	    msg->to = SCR_stracpy(tbl[j] + 4);
	    continue;
	    }
	if(msg->subject == 0 && SCR_cstrncmp(tbl[j], "SUBJECT:", 8) == 0) {
	    msg->subject = SCR_stracpy(tbl[j] + 9);
	    continue;
	    }
	if(msg->date    == 0 && SCR_cstrncmp(tbl[j], "DATE:", 5) == 0) {
	    msg->date = SCR_stracpy(tbl[j] + 6);
	    continue;
	    }
	if(msg->encoding == 0 && SCR_cstrncmp(tbl[j], "Content-Transfer-Encoding:", 26) == 0) {
	    msg->encoding = SCR_stracpy(tbl[j] + 27);
	    U_ljust_text(msg->encoding); /* JMP 10-02-2004 */
	    SCR_sqz(msg->encoding);      /* JMP 10-02-2004 */
	    continue;
	    }
	if(msg->disposition == 0 && SCR_cstrncmp(tbl[j], "Content-Disposition:", 20) == 0) {
	    msg->disposition = SCR_stracpy(tbl[j] + 21);
	    U_ljust_text(msg->disposition); /* JMP 10-02-2004 */
	    SCR_strip(msg->disposition);      /* JMP 10-02-2004 */
	    continue;
	    }
	if(msg->description == 0 && SCR_cstrncmp(tbl[j], "Content-Description:", 20) == 0) {
	    msg->description = SCR_stracpy(tbl[j] + 21);
	    U_ljust_text(msg->description); /* JMP 10-02-2004 */
	    SCR_strip(msg->description);      /* JMP 10-02-2004 */
	    continue;
	    }
	if(msg->type == 0    && SCR_cstrncmp(tbl[j], "Content-Type:", 13) == 0) {
	    msg->type= SCR_stracpy(tbl[j] + 14);
	    U_ljust_text(msg->type); /* JMP 10-02-2004 */
	    SCR_strip(msg->type);      /* JMP 10-02-2004 */
	    continue;
	    }
	}
}

/*NH*/
Pop3ReadBody(POP3MSG *msg, U_ch **tbl)
{
    int     j;

    for(j = 0 ; tbl[j] ; j++) {
	if(tbl[j] == 0) break;
	if(msg->boundary && strncmp(tbl[j], msg->boundary, strlen(msg->boundary)) == 0)
	    break;
	}

    msg->body = (char **) SCR_malloc((j + 1) * sizeof(char *));
    memcpy(msg->body, tbl, j * sizeof(char *));
    msg->body[j] = 0;
    return(j);
}

/*NH*/
Pop3Findttach(POP3MSG *msg, U_ch **tbl)
{
    int     j;

    if(msg->boundary == 0) return(-1);
    /* Cherche le début */
    for(j = 0 ; tbl[j] ; j++) {
	if(strncmp(tbl[j], msg->boundary, strlen(msg->boundary)) == 0) return(j);
	}
    return(-1);
}

/*NH*/
Pop3AnalyseMsg(POP3MSG *msg, U_ch **tbl)
{
    U_ch    **tbl2, **tbl3;
    int     nlh, nlb, starta, enda;
    POP3MSG *amsg;

    /* Headers */
    nlh = Pop3ReadBeg(msg, tbl);
    tbl2 = tbl + nlh + 1;

    /* Body Eventuel */
    nlb = Pop3ReadBody(msg, tbl2);
    tbl2 = tbl2 + nlb;

    /* Attachments Eventuels */
    while(1) {
	starta = Pop3Findttach(msg, tbl2);
	if(starta < 0) break;
	enda = Pop3Findttach(msg, tbl2 + starta + 1);
	if(enda < 0) break;
	msg->attachs = (POP3MSG **) SCR_realloc(msg->attachs, sizeof(POP3MSG *),
						msg->nbattachs, msg->nbattachs + 1);
	amsg = msg->attachs[msg->nbattachs] = (POP3MSG *) SCR_malloc(sizeof(POP3MSG));
	tbl3 = (U_ch **) SCR_malloc((enda + 1) * sizeof(char *));
	memcpy(tbl3, tbl2 + starta + 1 , enda * sizeof(char *));
	tbl3[enda] = 0;
	Pop3AnalyseMsg(amsg, tbl3);
	SCR_free(tbl3);
	msg->nbattachs++;
	tbl2 += enda + starta + 1;
	}

    return(0);
}


/*NH*/
Pop3ReadMsg(sock, i, msg)
int     sock, i;
POP3MSG *msg;
{
    char    buf[80], buf2[POP3_STDBUFSIZE + 1], *txt;
    U_ch    **tbl;
    int     j;

    /* Taille du message */
    DebugB("Lecture message ...");
    sprintf(buf, "LIST %d\r\n", i);
    if(Pop3Cmd(sock, buf, buf2) < 0) return(-1);;
    j = U_pos(' ', buf2 + 4);
    msg->size = atoi(buf2 + j + 5);

    /* Lecture complète du texte du message */
    sprintf(buf, "RETR %d\r\n", i);
    txt = Pop3CmdDotReadMsg(sock, buf, msg->size);  /* JMP 03-10-2003 */
    DebugE("Fini ...");
    if(txt == 0) return(-1);
    DebugB("Replace ...");
    SCR_replace(txt, "\r\n..", "\r\n.");
    SCR_replace(txt, "\r\n", "\n");
    DebugE("Fini ...");
    DebugB("Vtom ...");
    tbl = SCR_vtoms2A(txt, "\n", 0);
    msg->txt = (char **)tbl;
    SCR_free(txt);
    DebugE("Fini ...");

    /* Analyse */
    DebugB("Analyse ...");
    Pop3AnalyseMsg(msg, tbl);
    DebugE("Fini ...");
    return(0);
}

/*============================================================================
Libère une structure POP3MSG et toutes les sous-structures.

&EN msg = un message alloué par Pop3GetMsg(), Pop3GetMsgs(), Pop3GetHeader()
    ou Pop3GetHeaders().

&RT 0

&SA Pop3Connect(), Pop3Close(), Pop3Nb(), Pop3GetHeaders(), Pop3GetMsgs(),
    Pop3GetHeader(), Pop3GetMsg(), Pop3FreeMsg(), Pop3Session(), Pop3Detach()
=============================================================================== */
Pop3FreeMsg(POP3MSG *msg)
{
    int     i;

    if(msg == 0) return(0);
    SCR_free(msg->from);
    SCR_free(msg->date);
    SCR_free(msg->to);
    SCR_free(msg->subject);
    SCR_free(msg->reply);
    SCR_free(msg->type);
    SCR_free(msg->encoding);
    SCR_free(msg->disposition);
    SCR_free(msg->filename);
    SCR_free(msg->boundary);
    SCR_free(msg->body);
    SCR_free(msg->headers);
    SCR_free_tbl((U_ch **)msg->txt);
    for(i = 0 ; i < msg->nbattachs ; i++)
	Pop3FreeMsg(msg->attachs[i]);
    SCR_free(msg->attachs);
    SCR_free(msg);
    return(0);
}

/*============================================================================
Libère un tableau de structures POP3MSG.

&EN msgs = tableau de pointeur retourné par une des fonctions de
Pop3GetHeaders() ou Pop3GetMsgs().

&RT 0

&SA Pop3Connect(), Pop3Close(), Pop3Nb(), Pop3GetHeaders(), Pop3GetMsgs(),
    Pop3GetHeader(), Pop3GetMsg(), Pop3FreeMsg(), Pop3Session(), Pop3Detach()
=============================================================================== */
Pop3FreeMsgs(POP3MSG **msgs)
{
    int     i;

    if(msgs == 0) return(0);
    for(i = 0 ; msgs[i] ; i++)
	Pop3FreeMsg(msgs[i]);

    SCR_free(msgs);
    return(0);
}

/*============================================================================
Lit le header du message msgnb sur le socket sock ouvert via Pop3Connect().

&EN int msgnb : numéro d'ordre du message. Le premier message est le numéro 1 (et pas 0!).
&EN int sock : socket retourné par Pop3Connect()

&RT
&EN 0 : Erreur (voir POP3_ERROR)
&EN structure POP3MSG allouée

La variable POP3_ERROR contient  le message d'erreur éventuel.

&SA Pop3Connect(), Pop3Close(), Pop3Nb(), Pop3GetHeaders(), Pop3GetMsgs(),
    Pop3GetHeader(), Pop3GetMsg(), Pop3FreeMsg(), Pop3Session(), Pop3Detach()
=============================================================================== */

POP3MSG *Pop3GetHeader(int sock, int msgnb)
{
    POP3MSG     *msg;
    char        buf[POP3_STDBUFSIZE + 1], *txt;
    int         i, n;

    POP3_ERROR[0] = 0;
    msg = (POP3MSG *)SCR_malloc(sizeof(POP3MSG));
    if(Pop3ReadHeader(sock, msgnb , msg)) {
	Pop3FreeMsg(msg);
	msg = 0;
	}
    return(msg);
}

/*============================================================================
Lit les headers de tous les messages sur le socket ouvert via Pop3Connect().

&EN int sock : socket retourné par Pop3Connect()

&RT
&EN 0 : Erreur (voir POP3_ERROR)
&EN Tableau pointeurs vers des structures POP3MSG terminée par un NULL

La variable POP3_ERROR contient  le message d'erreur éventuel.

&SA Pop3Connect(), Pop3Close(), Pop3Nb(), Pop3GetHeaders(), Pop3GetMsgs(),
    Pop3GetHeader(), Pop3GetMsg(), Pop3FreeMsg(), Pop3Session(), Pop3Detach()
=============================================================================== */

POP3MSG **Pop3GetHeaders(int sock)
{
    POP3MSG     **msgs = 0;
    int         i, n;

    POP3_ERROR[0] = 0;
    n = Pop3Nb(sock);
    if(n <= 0) return(msgs);
    msgs = (POP3MSG **)SCR_malloc(sizeof(POP3MSG *) * (n + 1));
    for(i = 0 ; i < n ; i++)
	msgs[i] = Pop3GetHeader(sock, i + 1);

    return(msgs);
}

/*============================================================================
Lit le message msgnb sur la session pop3 sock ouverte par Pop3Connect().

&EN int sock : socket retourné par Pop3Connect()
&EN int msgnb : numéro du message (commençant à 1)

&RT

&EN 0 : Erreur (voir POP3_ERROR)
&EN structure POP3MSG allouée

La variable POP3_ERROR contient  le message d'erreur éventuel.

&SA Pop3Connect(), Pop3Close(), Pop3Nb(), Pop3GetHeaders(), Pop3GetMsgs(),
    Pop3GetHeader(), Pop3GetMsg(), Pop3FreeMsg(), Pop3Session(), Pop3Detach()
=============================================================================== */

POP3MSG *Pop3GetMsg(int sock, int msgnb)
{
    POP3MSG     *msg;

    POP3_ERROR[0] = 0;
    msg = (POP3MSG  *)SCR_malloc(sizeof(POP3MSG));
    if(Pop3ReadMsg(sock, msgnb, msg)) {
	Pop3FreeMsg(msg);
	msg = 0;
	}

    return(msg);
}

/*============================================================================
Lit les messages sur le socket ouvert via Pop3Connect().

&EN int sock : socket retourné par Pop3Connect()

&RT
&EN 0 : Erreur (voir POP3_ERROR)
&EN Tableau de pointeurs vers des structures POP3MSG terminé par un NULL

La variable POP3_ERROR contient  le message d'erreur éventuel.

&SA Pop3Connect(), Pop3Close(), Pop3Nb(), Pop3GetHeaders(), Pop3GetMsgs(),
    Pop3GetHeader(), Pop3GetMsg(), Pop3FreeMsg(), Pop3Session(), Pop3Detach()
=============================================================================== */

POP3MSG **Pop3GetMsgs(int sock)
{
    POP3MSG     **msgs = 0;
    int         i, n;

    POP3_ERROR[0] = 0;
    n = Pop3Nb(sock);
    if(n <= 0) return(msgs);
    msgs = (POP3MSG **)SCR_malloc(sizeof(POP3MSG *) * (n + 1));
    for(i = 0 ; i < n ; i++)
	msgs[i] = Pop3GetMsg(sock, i + 1);

    return(msgs);
}

/*============================================================================
Detruit le message msgnb sur le serveur. Il ne sera réellement détruit que
lors de la fin normale de la session.

&EN int sock : socket retourné par Pop3Connect()
&EN int msgnb : numéro du message (commençant à 1)

&RT
&EN -1 : Erreur (voir POP3_ERROR)
&EN 0 : Ok

La variable POP3_ERROR contient  le message d'erreur éventuel.

&SA Pop3Connect(), Pop3Close(), Pop3Nb(), Pop3GetHeaders(), Pop3GetMsgs(),
    Pop3GetHeader(), Pop3GetMsg(), Pop3FreeMsg(), Pop3Session(), Pop3Detach()
=============================================================================== */

Pop3DeleteMsg(int sock, int msgnb)
{
    char    buf[POP3_STDBUFSIZE + 1];

    sprintf(buf, "DELE %d\r\n", msgnb);
    return(Pop3Cmd(sock, buf, buf));
}

/*NH*/
Pop3HexToDec(U_ch *line)
{
    int     ch;
    char    buf[3];

    memcpy(buf, line, 2);
    buf[2] = 0;
    SCR_upper(buf);
    ch = 16 * U_pos(line[0], "0123456789ABCDEF") + U_pos(line[1], "0123456789ABCDEF");
    return(ch);
}


/*============================================================================
Détache le body d'un message et le sauve dans un fichier.

&EN attach : POP3MSG * lu par Pop3ReadMsg() ou sous message (attach)
&EN char *filename : nom du fichier résultat ou 0 ou string vide. Si 0 ou
    string vide, la pièce est sauvée sous le nom qu'elle avait à l'envoi du
    message mais dans le répertoire courant. Si elle n'avait pas de nom,
    elle n'est pas sauvée.

Le type de décodage est basé sur le contenu de l'attachment. Sont gérés :
&EN base64, mais pose problème avec Outlook et les fichiers .doc et autres qui contiennent
    de l'information "cachée" et pas seulement le fichier .doc attaché
&EN quoted-printable
&EN 7bit (pas de décodage)
&EN 8bit (pas de décodage)

&RT
&EN 0 si Ok
&EN -1 si message non défini
&EN -2 si filename est nul et pas de nom de fichier dans le message
&EN -3 si l'attachment est vide
&EN -4 si le fichier cible ne peut être ouvert

&SA Pop3Connect(), Pop3Close(), Pop3Nb(), Pop3GetHeaders(), Pop3GetMsgs(),
    Pop3GetHeader(), Pop3GetMsg(), Pop3FreeMsg(), Pop3Session(), Pop3Detach()
=============================================================================== */
Pop3DetachBody(POP3MSG *attach, char *filename)
{
    FILE        *fd;
    int         i, j, skip, ch, lg, nc = 0;
    char        *txt, *txt2;
    U_ch        *line;

    if(attach == 0) return(-1);
    if(filename == 0 || filename[0] == 0) filename = attach->filename;
    if(filename == 0 || filename[0] == 0) {
	filename = attach->description;
	if(filename) SCR_replace(filename, "/", "_");
	}
    if(filename == 0 || filename[0] == 0) return(-2);
    if(attach->body == 0) return(-3);

    fd = fopen(filename, "wb+");
    if(fd == 0) return(-4);
    if(attach->encoding && SCR_cstrncmp(attach->encoding, "base64", 6) == 0) {
	txt = SCR_mtovs((U_ch **)attach->body, "");
	SCR_strip(txt);
	lg = strlen(txt);
	if(txt[lg - 1] == '=') nc++;
	if(txt[lg - 2] == '=') nc++;
	SCR_pstrip(txt, "=");
	lg = 3 * (lg / 4) - nc;
	txt2 = SCR_malloc(lg + 1);
	SCR_decode64(txt, lg, txt2);
	fwrite(txt2, 1, lg, fd);
	SCR_free(txt);
	SCR_free(txt2);
	}
    else if(attach->encoding && SCR_cstrncmp(attach->encoding, "quoted-printable", 6) == 0) {
	for(i = 0 ; attach->body[i] ; i++) {
	    line = attach->body[i];
	    lg = strlen(line);
	    skip = 0;
	    for(j = 0 ; j < lg ; j++) {
		ch = line[j];
		if(ch == '=') {
		    if(j + 2 < lg) {
			ch = Pop3HexToDec(line + j + 1);
			j += 2;
			putc(ch, fd);
			}
		    else // saut de ligne skippé
			skip = 1;
		    }
		else
		    putc(ch, fd);
		}
	    if(skip == 0 && attach->body[i + 1]) putc('\n', fd); /* JMP 10-03-03 */
	    }
	}
    else {
	for(i = 0 ; attach->body[i] ; i++)
	    fprintf(fd, "%s\n", attach->body[i]);
	}

    fclose(fd);
    return(0);
}

/*============================================================================
Détache une pièce jointe et la sauve dans un fichier.

&EN msg : POP3MSG * lu par Pop3ReadMsg();
&EN int attachnb : numéro de l'attachment ou -1 s'il faut détacher le body du message
&EN char *filename : nom du fichier résultat ou 0 ou string vide. Si 0 ou
    string vide, la pièce est sauvée sous le nom qu'elle avait à l'envoi du
    message mais dans le répertoire courant. Si elle n'avait pas de nom,
    elle n'est pas sauvée.

Le type de décodage est basé sur le contenu de l'attachment. Sont gérés :
&EN base64, mais pose problème avec Outlook et les fichiers .doc et autres qui contiennent
    de l'information "cachée" et pas seulement le fichier .doc attaché
&EN quoted-printable
&EN 7bit (pas de décodage)
&EN 8bit (pas de décodage)

&RT
&EN 0 si Ok
&EN -1 si message non défini ou pas de pièces jointes
&EN -2 si filename est nul et pas de nom de fichier dans le message
&EN -3 si l'attachment est vide
&EN -4 si le fichier cible ne peut être ouvert

&SA Pop3Connect(), Pop3Close(), Pop3Nb(), Pop3GetHeaders(), Pop3GetMsgs(),
    Pop3GetHeader(), Pop3GetMsg(), Pop3FreeMsg(), Pop3Session(), Pop3Detach()
=============================================================================== */
Pop3Detach(POP3MSG *msg, int attachnb, char *filename)
{
    POP3MSG     *attach;
    FILE        *fd;
    int         i, j, skip, ch, lg, nc = 0;
    char        *txt, *txt2;
    U_ch        *line;

    if(msg == 0) return(-1);
    if(attachnb >= 0) {
	if(msg->nbattachs <= attachnb) return(-1);
	attach = msg->attachs[attachnb];
	}
    else {
	attach = msg;
	}

    if(filename == 0 || filename[0] == 0) filename = attach->filename;
    if(filename == 0 || filename[0] == 0) {
	filename = attach->description;
	if(filename) SCR_replace(filename, "/", "_");
	}
    if(filename == 0 || filename[0] == 0) return(-2);
    if(attach->body == 0) return(-3);

    fd = fopen(filename, "wb+");
    if(fd == 0) return(-4);
    if(attach->encoding && SCR_cstrncmp(attach->encoding, "base64", 6) == 0) {
	txt = SCR_mtovs((U_ch **)attach->body, "");
	SCR_strip(txt);
	lg = strlen(txt);
	if(txt[lg - 1] == '=') nc++;
	if(txt[lg - 2] == '=') nc++;
	SCR_pstrip(txt, "=");
	lg = 3 * (lg / 4) - nc;
	txt2 = SCR_malloc(lg + 1);
	SCR_decode64(txt, lg, txt2);
	fwrite(txt2, 1, lg, fd);
	SCR_free(txt);
	SCR_free(txt2);
	}
    else if(attach->encoding && SCR_cstrncmp(attach->encoding, "quoted-printable", 6) == 0) {
	for(i = 0 ; attach->body[i] ; i++) {
	    line = attach->body[i];
	    lg = strlen(line);
	    skip = 0;
	    for(j = 0 ; j < lg ; j++) {
		ch = line[j];
		if(ch == '=') {
		    if(j + 2 < lg) {
			ch = Pop3HexToDec(line + j + 1);
			j += 2;
			putc(ch, fd);
			}
		    else // saut de ligne skippé
			skip = 1;
		    }
		else
		    putc(ch, fd);
		}
	    if(skip == 0 && attach->body[i + 1]) putc('\n', fd); /* JMP 10-03-03 */
	    }
	}
    else {
	for(i = 0 ; attach->body[i] ; i++)
	    fprintf(fd, "%s\n", attach->body[i]);
	}

    fclose(fd);
    return(0);
}


/*============================================================================
Session complète de lecture de message sur un serveur POP3.
Ouvre la session, lit les messages et ferme la session.

&EN char *server : nom du serveur (ex. "pop3.plan.be")
&EN char *user   : nom de login   (ex. "jmp")
&EN char *pass   : mot de passe   (ex. "123456")
&EN int port     : porte POP3. Si <= 0, fixée à 110.

&RT

&EN 0 : Erreur (voir POP3_ERROR)
&EN Tableau de structures POP3MSG

La variable POP3_ERROR contient  le message d'erreur éventuel.

&EX
    main(argc, argv)
    int     argc;
    char    *argv[];
    {
	POP3MSG **msgs;
	int     i, j;
	char    filename[60];

	if(argc < 4) {
	    printf("Syntax : Pop3 pop3host user passwd\n");
	    exit(1);
	    }
	msgs = Pop3Session(argv[1], argv[2], argv[3], 110);
	if(msgs == 0) {
	    printf("%s\n", POP3_ERROR);
	    exit(1);
	    }
	for(i = 0 ; msgs[i] ; i++) {
	    printf("%d. From '%s' Subject '%s'\n", i, msgs[i]->from, msgs[i]->subject);
	    for(j = 0 ; j < msgs[i]->nbattachs ; j++) {
		sprintf(filename, "file%d_%d", i, j);
		if(Pop3Detach(msgs[i], j, filename) == 0)
		    printf(" Attach %d saved in %s\n", j + 1, filename);
		else
		    printf(" Attach %d not saved\n", j + 1);
		}
	    }

	Pop3FreeMsgs(msgs);
	exit(0);
    }


&TX

&SA Pop3Connect(), Pop3Close(), Pop3Nb(), Pop3GetHeaders(), Pop3GetMsgs(),
    Pop3GetHeader(), Pop3GetMsg(), Pop3FreeMsg(), Pop3Session(), Pop3Detach()
=============================================================================== */

POP3MSG **Pop3Session(server, user, passwd, port)
char    *server, *user, *passwd;
int     port;
{
    POP3MSG     **msgs = 0;
    char        buf[1024], *txt;
    int         i, n, sock;

    POP3_ERROR[0] = 0;
    sock = Pop3Connect(server, user, passwd, port);
    switch(sock) {
	case -1 : return(msgs);
	case -2 :
	case -3 : Pop3Close(sock); return(msgs);
	default : break;
	}
    msgs = Pop3GetMsgs(sock);
    Pop3Close(sock);
    return(msgs);
}

/*NH*/
Pop3PrintMsg(POP3MSG *msg)
{
    int     i, j, k;

    printf("\nMessage\n-------\n");

    printf(" From   :%s\n", msg->from);
    printf(" To     :%s\n", msg->to);
    printf(" Subject:%s\n", msg->subject);
    printf(" Size   :%d\n", msg->size);
    printf(" Date   :%s\n", msg->date);
    printf(" Type   :%s\n", msg->type);
    printf(" Encode :%s\n", msg->encoding);
    printf(" Disp.  :%s\n", msg->disposition);
    printf(" File   :%s\n", msg->filename);
    printf(" Attach :%d\n", msg->nbattachs);
    if(msg->body) {
	printf(" Body\n ----\n");
	for(j = 0 ; msg->body[j] ; j++)
	    printf("  >%s\n", msg->body[j]);
	}

    for(j = 0 ; j < msg->nbattachs ; j++) {
	printf("  Attachment %d\n", j);
	Pop3PrintMsg(msg->attachs[j]);
	}
}

Pop3SaveMsg(POP3MSG *msg, char *filename)
{
    int     j;
    char    attachm[256];
    FILE    *fd;

    fd = fopen(filename, "w+");
    if(fd == 0) {
	printf("s4pop3 : cannot open %s for writing\n", filename);
	return(-1);
	}
    if(msg->headers) {
	for(j = 0 ; msg->headers[j] ; j++)
	    fprintf(fd, "%s\n", msg->headers[j]);
	}

    fclose(fd);

    // Body
    sprintf(attachm, "%s_body", filename);
    if(msg->body) {
	if(Pop3DetachBody(msg, attachm) == 0)
	    printf(" Body saved in %s\n", attachm);
	else
	    printf(" Body not saved\n");

	//for(j = 0 ; msg->body[j] ; j++)
	//    fprintf(fd, "%s\n", msg->body[j]);
	}

    for(j = 0 ; j < msg->nbattachs ; j++) {
	DebugB("Begin detach....");
	sprintf(attachm, "%s_%d", filename, j);
	if(Pop3Detach(msg, j, attachm) == 0)
	    printf(" Attach %d saved in %s\n", j + 1, attachm);
	else
	    printf(" Attach %d not saved\n", j + 1);
	DebugE("End");
	}

    return(0);
}

/*
main(argc, argv)
int     argc;
char    *argv[];
{
    POP3MSG **msgs;
    int     i, j;
    char    filename[60];

    if(argc < 4) {
	printf("Syntax : Pop3 pop3host user passwd\n");
	exit(1);
	}
    msgs = Pop3Session(argv[1], argv[2], argv[3], 110);
    if(msgs == 0) {
	printf("%s\n", POP3_ERROR);
	exit(1);
	}
    for(i = 0 ; msgs[i] ; i++) {
	Pop3PrintMsg(msgs[i]);
	}
    for(i = 0 ; msgs[i] ; i++) {
	printf("%d. From '%s' Subject '%s'\n", i, msgs[i]->from, msgs[i]->subject);
	for(j = 0 ; j < msgs[i]->nbattachs ; j++) {
	    sprintf(filename, "file%d_%d", i, j);
	    if(Pop3Detach(msgs[i], j, filename) == 0)
		printf(" Attach %d saved in %s\n", j + 1, filename);
	    else
		printf(" Attach %d not saved\n", j + 1);
	    }
	}

    Pop3FreeMsgs(msgs);
    exit(0);
}
*/

#endif
