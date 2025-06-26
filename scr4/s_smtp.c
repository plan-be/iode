#if defined(DOSW32) || defined(SCRW32) || defined(LINUX)
#include <scr4w.h>
#include <time.h>   // JMP 2/5/2013

//#define DEBUG

int     SENDMAIL_SOCKET = 0;
char    SENDMAIL_ERROR[81];
char    SENDMAIL_BUF[1024];
int     SENDMAIL_PORT = 25; /* JMP 02-11-01 */
int     SENDMAIL_DEBUG = 0;
char    SENDMAIL_PASSWD[255];   /* BP_M 18-12-2009 16:28 */
int     SENDMAIL_CONTENTTYPE = 0;   /* JMP 19/2/2015 */
//int     SENDMAIL_CONTENTTYPE;   /* BP_M 22-11-2012 10:37 */
	    // 0: content-type: multipart/mixed ative
	    // 1: content-type: multipart/alternative
	    // 2: content-type: text/html


/*NH============================================================================
Envoie des données (ou une commande) sur le socket ouvert par SendMailConnect().

&EN char    *rec : record à envoyer
&RT
&EN 0 : Ok
&EN -1 : erreur

Le message d'erreur se trouve dans la variable globale SENDMAIL_ERROR.

&SA SendMailConnect(), SendMailData(), SendMailStartData(), SendMailWriteData()
=============================================================================== */

SendMailWrite(rec)
char    *rec;
{
    int     err;

    if(rec == 0 || rec[0] == 0) return(0); /* JMP 03-11-01 */
    err = WSockBWrite(SENDMAIL_SOCKET, rec, strlen(rec));
    if(SENDMAIL_DEBUG) {
	if(strcmp(rec, "\r\n")) printf("W:%s", rec);
	if(rec[0] && rec[strlen(rec) - 1] != '\n') printf("\n");
    }
    if(err == 0) return(0);
    sprintf(SENDMAIL_ERROR, "SendMail Socket error %d", err);
    return(-1);
}

/*NH============================================================================
Lit la réponse du serveur SMTP sur le socket ouvert par SendMailConnect().

&RT
&EN code de retour du serveur
&EN -1 : erreur de transmission

En cas d'erreur de transmission, le message d'erreur se trouve dans la
variable globale SENDMAIL_ERROR.

La variable globale char *SENDMAIL_BUF contient le texte retourné par le
serveur.

&SA SendMailConnect(), SendMailData(), SendMailStartData(), SendMailWriteData()
=============================================================================== */

int SendMailRead()
{
    int     err;

    err = WSockRead(SENDMAIL_SOCKET, SENDMAIL_BUF, 1023);
    if(SENDMAIL_DEBUG) {
	printf("R:%s", SENDMAIL_BUF);
	if(SENDMAIL_BUF[strlen(SENDMAIL_BUF) - 1] != '\n') printf("\n");
    }
    if(err < 0) {
	sprintf(SENDMAIL_ERROR , "SendMail Socket error %d", err);
	return(err);
    }
    return(atoi(SENDMAIL_BUF));
}

/*NH*/
SendMailCmd(cmd)
char    *cmd;
{
    if(SendMailWrite(cmd)) return(-1);
    return(SendMailRead());
}


/*============================================================================
Connecte au serveur SMTP server via le port port. Les paramètres host et
user sont seulement indicatifs et ne sont en général pas traités par le
serveur.

&EN char *server : nom du serveur (ex. "smtp.xon.be")
&EN int port     : numéro du port (ex. : 25)
&EN char *host   : nom du host source (ex. "heaven.be")
&EN char *user   : user source (ex. "god@heaven.be")

&EN Si la variable globale - SENDMAIL_PASSWD - est complètée, le programme enverra
    la commande d'authentification SMTP - AUTH PLAIN - avec le login et le mot de passe
    encodé en base64

&RT
&EN 0 : connexion opérée sans problème
&EN -1 : erreur à l'initialisation
&EN -2 : erreur à la création du socket
&EN -3 : host not found
&EN -4 : erreur au connect
&EN -5 : code retour du serveur SMTP incorrect
&EN -6 : erreur retournée par le serveur SMTP (HELO)
&EN -7 : erreur retournée par le serveur SMTP (MAIL FROM)

Le message d'erreur se trouve dans la variable globale SENDMAIL_ERROR.

&EX
    U_ch    **data;

    if(SendMailConnect("smtp.whitehouse.org", 25, "heaven.ru", "kgb")) return(-1);
    if(SendMailTo("bill@whitehouse.be")) goto err;
    data = SCR_vtom("To: <bill@whitehouse.org>,Subject:La vie est belle,Coucou,c'est moi,..", ',');
    if(SendMailData(data)) goto err;
    SCR_free_tbl(data);
    SendMailClose();
    return(0);
  err:
    printf("Error : %s\n", SENDMAIL_ERROR);
    SendMailClose();
    return(-1);
&TX

&SA SendMailConnect(), SendMailTo(), SendMailStartData(), SendMailWriteData(),
 SendMailFile(), SendMailEndData(), SendMailData(), SendMailClose()
=============================================================================== */

SendMailConnect(server, port, host, user)
char    *server, *host, *user;
int     port;
{
    char    buf[512], buf2[512];
    int     rc, lg;

    SENDMAIL_SOCKET = WSockConnect(server, port);

    if(WSOCK_DEBUG)
	DebugB("SMTP connection : from host '%s', user '%s', to server '%s' port %d ...",
	       host, user, server, port);
    if(SENDMAIL_SOCKET <= 0) {
	switch(SENDMAIL_SOCKET) {
	    case -1 :
		SCR_strlcpy(SENDMAIL_ERROR, "Socket lib initialisation error", 80);
		break;
	    case -2 :
		SCR_strlcpy(SENDMAIL_ERROR, "Socket creation error", 80);
		break;
	    case -3 :
		SCR_strlcpy(SENDMAIL_ERROR, "Destination host not found", 80);
		break;
	    case -4 :
		SCR_strlcpy(SENDMAIL_ERROR, "Connection to host rejected", 80);
		break;
	}
	if(WSOCK_DEBUG) DebugE(" %s",SENDMAIL_ERROR);
	return(SENDMAIL_SOCKET);
    }
    if(WSOCK_DEBUG) DebugE("Ok\n");

    rc = SendMailRead();
    if(rc < 200 || rc > 299) {
	WSockClose(SENDMAIL_SOCKET);
	SENDMAIL_SOCKET = 0;
	SCR_strlcpy(SENDMAIL_ERROR, "Sendmail Socket read error", 80);
	if(WSOCK_DEBUG) DebugE(" %s\n",SENDMAIL_ERROR);
	return(-5);
    }

    sprintf(buf, "HELO %s\r\n", host);
    rc = SendMailCmd(buf);
    if(rc < 200 || rc > 299) {
	SCR_strlcpy(SENDMAIL_ERROR, SENDMAIL_BUF, 80);
	if(WSOCK_DEBUG) DebugE(" %s\n",SENDMAIL_ERROR);
	return(-6);
    }

    /* BP_M 16-12-2009 11:01 */
    if(SENDMAIL_PASSWD[0] > 0) {
	memset(buf, 0, 512);
	memcpy(buf + 1, user, strlen(user));
	memcpy(buf + 2 + strlen(user), SENDMAIL_PASSWD, strlen(SENDMAIL_PASSWD));
	lg = strlen(user) + strlen(SENDMAIL_PASSWD) + 2;
	SCR_code64(buf, lg, buf2);
	sprintf(buf, "AUTH PLAIN %s\r\n", buf2);
	rc = SendMailCmd(buf);
    }

    sprintf(buf, "MAIL FROM:<%s>\r\n", user);
    rc = SendMailCmd(buf);
    if(rc < 200 || rc > 299) {
	SCR_strlcpy(SENDMAIL_ERROR, SENDMAIL_BUF, 80);
	if(WSOCK_DEBUG) DebugE(" %s\n",SENDMAIL_ERROR);
	return(-7);
    }

    return(0);
}

/*============================================================================
Déconnecte du serveur SMTP ouvert par SendMailConnect().

&RT
&EN 0 : déconnexion opérée sans problème
&EN -1 : erreur (connexion effectuée)

Le message d'erreur se trouve dans la variable globale SENDMAIL_ERROR.

&SA SendMailConnect(), SendMailTo(), SendMailStartData(), SendMailWriteData(),
 SendMailFile(), SendMailEndData(), SendMailData(), SendMailClose()
=============================================================================== */

SendMailClose()
{
    int     rc;

    if(SENDMAIL_SOCKET <= 0) return(0);
    rc = SendMailCmd("QUIT \r\n");
    if(rc == 221) rc = 0;
    else rc = -1;
    WSockClose(SENDMAIL_SOCKET);
    SENDMAIL_SOCKET = 0;
    return(rc);
}

/*============================================================================
Ajoute des destinataires pour le message.

&EN char *dest = liste des destinataires séparés par des virgules,
    points-virgules ou blancs.

&RT
&EN 0 : déconnexion opérée sans problème
&EN -1 : erreur (connexion effectuée)

Le message d'erreur se trouve dans la variable globale SENDMAIL_ERROR.

&SA SendMailConnect(), SendMailTo(), SendMailStartData(), SendMailWriteData(),
 SendMailFile(), SendMailEndData(), SendMailData(), SendMailClose()
=============================================================================== */

SendMailTo(dest)
char    *dest;
{
    char    buf[1024];
    U_ch    **tbl = 0;
    int     i;

    tbl = SCR_vtoms(dest, ",; ");
    if(tbl == 0 || SCR_tbl_size(tbl) == 0) {
	strcpy(SENDMAIL_ERROR, "Incorrect list of recipients");
	return(-1);
    }
    for(i = 0 ; tbl[i] ; i++) {
	sprintf(buf, "RCPT TO:<%s>\r\n", tbl[i]);
	if(SendMailCmd(buf) != 250) {
	    SCR_strlcpy(SENDMAIL_ERROR, SENDMAIL_BUF, 80);
	    SCR_free_tbl(tbl);
	    return(-1);
	}
    }

    SCR_free_tbl(tbl);
    return(0);
}

/*============================================================================
Prépare l'envoi d'un message.

&RT
&EN 0 : Ok
&EN -1 : erreur

Le message d'erreur se trouve dans la variable globale SENDMAIL_ERROR.

&SA SendMailConnect(), SendMailTo(), SendMailStartData(), SendMailWriteData(),
 SendMailFile(), SendMailEndData(), SendMailData(), SendMailClose()
=============================================================================== */

SendMailStartData()
{
    if(SendMailCmd("DATA\r\n") == 354) return(0);
    SCR_strlcpy(SENDMAIL_ERROR, SENDMAIL_BUF, 80);
    return(-1);
}

/*============================================================================
Termine l'envoi d'un message.

&RT
&EN 0 : Ok
&EN -1 : erreur

Le message d'erreur se trouve dans la variable globale SENDMAIL_ERROR.

&SA SendMailConnect(), SendMailTo(), SendMailStartData(), SendMailWriteData(),
 SendMailFile(), SendMailEndData(), SendMailData(), SendMailClose()
=============================================================================== */

SendMailEndData()
{
    if(SendMailCmd(".\r\n") == 250) return(0);
    SCR_strlcpy(SENDMAIL_ERROR, SENDMAIL_BUF, 80);
    return(-1);
}

/*============================================================================
Envoie une ligne d'un message.

&RT
&EN 0 : Ok
&EN -1 : erreur

Le message d'erreur se trouve dans la variable globale SENDMAIL_ERROR.

&SA SendMailConnect(), SendMailTo(), SendMailStartData(), SendMailWriteData(),
 SendMailFile(), SendMailEndData(), SendMailData(), SendMailClose()
=============================================================================== */

SendMailWriteData(data)
U_ch    *data;
{
    char    buf[1024];
    int     rc;

    if(data == 0) return(0);
    if(strcmp(data, " ") != 0) {
	if(data[0] == '.') SendMailWrite(".");
	if(rc = SendMailWrite(data)) return(rc);
    }
    return(SendMailWrite("\r\n"));
}

/*============================================================================
Envoie le message. Ce message contiendra les éventuels Subject, To ... qui
seront traités par l'Agent Pop. Cette fonction appelle SendMailStartData(),
SendMailWriteData() et SendMailEndData().

&EN U_ch **data : tableau de strings terminé par un NULL contenant les
    lignes du message.

&RT
&EN 0 : Ok
&EN -1 : erreur

Le message d'erreur se trouve dans la variable globale SENDMAIL_ERROR.

&SA SendMailConnect(), SendMailTo(), SendMailStartData(), SendMailWriteData(),
 SendMailFile(), SendMailEndData(), SendMailData(), SendMailClose()
=============================================================================== */

SendMailData(data)
U_ch    **data;
{
    int     i;

    if(data == 0) return(0);
    if(SendMailStartData()) return(-1);
    for(i = 0 ; data[i] ; i++)
	if(SendMailWriteData(data[i])) return(-1);

    return(SendMailEndData());
}

/*============================================================================
Envoie un fichier dans la zone DATA du message. La zone DATA est ouverte et
fermée par cette fonction (un seul fichier peut donc être envoyé).

&EN char *filename;

&RT
&EN 0 : Ok
&EN -1 : erreur

Le message d'erreur se trouve dans la variable globale SENDMAIL_ERROR.

&SA SendMailConnect(), SendMailTo(), SendMailStartData(), SendMailWriteData(),
 SendMailFile(), SendMailEndData(), SendMailData(), SendMailClose()
=============================================================================== */

SendMailFile(filename)
char    *filename;
{
    FILE    *fd;
    char    buf[1024];
    U_ch    **data;
    int     nl = 0, c, i = 0;

    fd = fopen(filename, "rb");
    if(fd == 0) {
	sprintf(SENDMAIL_ERROR, "SendMail : File %s not found", filename);
	return(-1);
    }
    while(1) {
	c = fgetc(fd);
	if(c == EOF) break;
	if(c == '\n' || c == '\f') {
	    buf[i] = 0;
	    if(i == 0) {
		buf[0] = ' ';
		buf[1] = 0;
	    }
	    if(nl == 0 || nl % 100 == 0)
		data = (U_ch **)SCR_realloc(data, sizeof(char **), nl, nl + 100);
	    data[nl] = SCR_stracpy(buf);
	    nl++;
	    i = 0;
	}
	else {
	    buf[i++] = c;
	    if(i > 1022)
		ungetc('\n', fd);
	}
    }
    if(i > 0) {
	data[nl] = SCR_stracpy(buf);
	nl++;
    }
    data[nl] = 0;
    SendMailData(data);
    SCR_free_tbl(data);
    fclose(fd);
    return(0);
}


char    *SMTP_BOUNDARY = "----------NextPart_1a2z3e4r5t6";

/*============================================================================
Démarre une session SMTP. Crée le socket et envoie les premières
informations jusqu'au début de la section DATA.

&EN char *smtpsrv : serveur SMTP      ("mail.xon.be")
&EN int  port     : port SMTP         (25)
&EN char *fromadr : adresse de l'expéditeur avec le nom de domaine (jmp@xon.be)
&EN char *fromname : nom de l'expéditeur
&EN char *toadr   : adresse(s) destination séparées par des virgules
&EN char *subject : titre du message

&RT
&EN 0 : Ok
&EN -1 : erreur à l'initialisation
&EN -2 : erreur à la création du socket
&EN -3 : host not found
&EN -4 : erreur au connect
&EN -5 : code retour du serveur SMTP incorrect
&EN -6 : erreur retournée par le serveur SMTP (HELO)
&EN -7 : erreur retournée par le serveur SMTP (MAIL FROM)
&EN -8 : erreur en cours de transmission

Le message d'erreur se trouve dans la variable globale SENDMAIL_ERROR.

&EX
    if(SendMailInitEx(argv[1],  // serveur : mail.xon.be
		    25,       // Porte
		    argv[2],  // From address : jmp@xon.be
		    argv[2],  // From user name : Jean-Marc
		    argv[3],  // To : bp@xon.be
		    "Essai"   // subject
		    )) goto err;

    // Si on veut un texte libre comme attach 1 (vu souvent comme body)
    if(SendMailStartAttachTxt()) goto err;
    SendMailAddAttachTxtLine("Début de mon texte", 1);
    SendMailAddAttachTxtLine("------------------", 1);
    SendMailAddAttachTxtLine("Line 1\nLigne 2\nLigne 3", 1);
    SendMailAddAttachTxtLine("---\nFin.", 1);

    // attacher des fichiers avec diff encoding
    if(SendMailAttach("testmail.txt", "text/plain", 0, 1, 1)) goto err;
    if(SendMailAttach("testmail.txt", "text/plain", 64, 1, 1)) goto err;
    if(SendMailAttach("lastnode.gif", "image/gif", 1, 0, 0)) goto err;
    if(SendMailAttach("Euro.doc", "application/octet-stream", 1, 0, 1)) goto err;

    // Fin
    if(SendMailEnd()) goto err;
    printf("Ok\n");
    return(0);

err:
    printf("Error : %s\n", SENDMAIL_ERROR);
    SendMailClose();
    return(-1);
}

&TX

&SA SendMailInit(), SendMailEnd(), SendMailAttach(), SendMailStartAttachTxt(), SendMailAddAttachTxtLine()
=============================================================================== */

SendMailInitEx(char *smtpsrv, int port, char *fromaddr,
	       char *fromname, char *toaddr, char *subject)
{
    char    buf[2560], bufdate[128], buftime[128]; // JMP 5/10/2010
    char    host[64], ip[20];
    int     rc = 0;
    time_t  rawtime;
    struct tm *timeinfo;


    if(WSockGetHost(host, ip))
	strcpy(host, smtpsrv);
	//strcpy(host, "unknown");

    rc = SendMailConnect(smtpsrv, port, host, fromaddr);
    if(rc) return(rc);
    if(SendMailTo(toaddr)) return(-8);
    if(SendMailStartData()) return(-8);
    if(fromname)
	sprintf(buf, "From: \"%s\" <%s>", fromname, fromaddr);
    else
	sprintf(buf, "From: <%s>", fromaddr);
    if(SendMailWriteData(buf)) return(-8);
    if(SendMailWriteData("MIME-Version: 1.0")) return(-8);

    /*SCR_long_to_fdate(_SCR_current_date(), bufdate, "mm/dd/yyyy"); // JMP 5/10/2010
    SCR_long_to_ftime(_SCR_current_time(), buftime, "hh:mm");       // JMP 5/10/2010
    sprintf(buf, "Date: %s %s", bufdate, buftime);                // JMP 5/10/2010
    // JMP 2/5/2013 */

    time(&rawtime);                                 // JMP 2/5/2013
    timeinfo = localtime (&rawtime);                // JMP 2/5/2013
    strcpy(bufdate, asctime(timeinfo));             // JMP 2/5/2013
    if(strlen(bufdate) > 0)
	bufdate[strlen(bufdate) - 1] = 0;           // JMP 2/5/2013
    sprintf(buf, "Date: %s", bufdate);              // JMP 2/5/2013

    if(SendMailWriteData(buf)) return(-8);                        // JMP 5/10/2010

    sprintf(buf, "To: %s", toaddr);
    if(SendMailWriteData(buf)) return(-8);

    if(subject) {
	sprintf(buf, "Subject: %s", subject);
	if(SendMailWriteData(buf)) return(-8);
    }
    switch(SENDMAIL_CONTENTTYPE)  {
	case 0: sprintf(buf, "Content-Type: multipart/mixed; boundary=\"%s\"", SMTP_BOUNDARY);
		break;
	case 1: sprintf(buf, "Content-Type: multipart/alternative; boundary=\"%s\"", SMTP_BOUNDARY);
		break;
	case 2: sprintf(buf, "Content-Type: text/htlm; boundary=\"%s\"", SMTP_BOUNDARY);
		break;
    }
    if(SendMailWriteData(buf)) return(-8);
    if(SendMailWriteData(" ")) return(-8);
    //if(SendMailWriteData("This is a multi-part message in MIME format.")) return(-8); // JMP 21-10-2012

    return(0);
}

/*============================================================================
Démarre une session SMTP. Crée le socket et envoie les premières
informations jusqu'au début de la section DATA.

&EN char *smtpsrv : serveur SMTP      ("mail.xon.be")
&EN char *fromadr : adresse de l'expéditeur avec le nom de domaine (jmp@xon.be)
&EN char *fromname : nom de l'expéditeur
&EN char *toadr   : adresse(s) destination séparées par des virgules
&EN char *subject : titre du message

La porte du serveur SMTP est définie par la variable globale int SENDMAIL_PORT. Par défaut,
celle-ci est fixée à 25.

&RT
&EN 0 : Ok
&EN -1 : erreur à l'initialisation
&EN -2 : erreur à la création du socket
&EN -3 : host not found
&EN -4 : erreur au connect
&EN -5 : code retour du serveur SMTP incorrect
&EN -6 : erreur retournée par le serveur SMTP (HELO)
&EN -7 : erreur retournée par le serveur SMTP (MAIL FROM)
&EN -8 : erreur en cours de transmission

Le message d'erreur se trouve dans la variable globale SENDMAIL_ERROR.

&EX
    if(SendMailInit(argv[1],  // serveur : mail.xon.be
		    argv[2],  // From address : jmp@xon.be
		    argv[2],  // From user name : Jean-Marc
		    argv[3],  // To : bp@xon.be
		    "Essai"   // subject
		    )) goto err;

    // Si on veut un texte libre comme attach 1 (vu souvent comme body)
    if(SendMailStartAttachTxt()) goto err;
    SendMailAddAttachTxtLine("Début de mon texte", 1);
    SendMailAddAttachTxtLine("------------------", 1);
    SendMailAddAttachTxtLine("Line 1\nLigne 2\nLigne 3", 1);
    SendMailAddAttachTxtLine("---\nFin.", 1);

    // attacher des fichiers avec diff encoding
    if(SendMailAttach("testmail.txt", "text/plain", 0, 1)) goto err;
    if(SendMailAttach("testmail.txt", "text/plain", 64, 1)) goto err;
    if(SendMailAttach("lastnode.gif", "image/gif", 1, 0)) goto err;
    if(SendMailAttach("Euro.doc", "application/octet-stream", 1, 0)) goto err;

    // Fin
    if(SendMailEnd()) goto err;
    printf("Ok\n");
    return(0);

err:
    printf("Error : %s\n", SENDMAIL_ERROR);
    SendMailClose();
    return(-1);
}

&TX

&SA SendMailInit(), SendMailEnd(), SendMailAttach(), SendMailStartAttachTxt(), SendMailAddAttachTxtLine()
=============================================================================== */

SendMailInit(char *smtpsrv, char *fromaddr,
	     char *fromname, char *toaddr, char *subject)
{

    return(SendMailInitEx(smtpsrv, SENDMAIL_PORT, fromaddr,
			  fromname, toaddr, subject));
}

/*============================================================================
Termine une session SMTP démarrée par SendMailInit().

&RT
&EN 0 : Ok
&EN -1 : erreur

Le message d'erreur se trouve dans la variable globale SENDMAIL_ERROR.

&EX
    voir SendMailInit()
&TX
&SA SendMailInit(), SendMailEnd(), SendMailAttach(), SendMailStartAttachTxt(), SendMailAddAttachTxtLine()
=============================================================================== */

SendMailEnd()
{
    char    buf[256];
    int     rc;

    sprintf(buf, "--%s--", SMTP_BOUNDARY);
    if(rc = SendMailWriteData(buf)) return(rc);
    if(rc = SendMailEndData()) return(rc);
    if(rc = SendMailClose()) return(rc);
    return(0);
}

/*============================================================================
Démarre un nouvel attachment de type texte. Le contenu de cet attachment
doit être alimenté par la fonction SendMailAddAttachTxtLine().
Il s'agit en général du premier attachment d'un mail qui constituera le corps
du message.

Attention, cette fonction ne peut être utilisée qu'après SendMailInit().


&EN 0 : Ok
&EN -1 : erreur

Le message d'erreur se trouve dans la variable globale SENDMAIL_ERROR.

&EX
    voir SendMailInit()
&TX
&SA SendMailInit(), SendMailEnd(), SendMailAttach(), SendMailStartAttachTxt(), SendMailAddAttachTxtLine()
=============================================================================== */

SendMailStartAttachTxt()
{
    char    buf[256];
    int     rc;

    sprintf(buf, "--%s", SMTP_BOUNDARY);
    if(rc = SendMailWriteData(buf)) return(rc);
    if(rc = SendMailWriteData("Content-Type: text/plain; charset=\"iso-8859-1\"")) return(rc);
    if(rc = SendMailWriteData("Content-Transfer-Encoding: 8bit")) return(rc);
    if(rc = SendMailWriteData(" ")) return(rc);

    return(0);
}

/*============================================================================
Ajoute un ligne à l'attachment courant commencé par l'appel à SendMailStartAttachTxt().

&EN char *txt : texte à envoyer
&EN int convertansi : 1 pour convertir d'Oem à Ansi, 0 sans conversion


&EN 0 : Ok
&EN -1 : erreur

Le message d'erreur se trouve dans la variable globale SENDMAIL_ERROR.

&EX
    voir SendMailInit()
&TX
&SA SendMailInit(), SendMailEnd(), SendMailAttach(), SendMailStartAttachTxt(),
SendMailAddAttachTxtLine()
=============================================================================== */

SendMailAddAttachTxtLine(char *txt, int convertansi)
{
    int     rc;

    if(convertansi) SCR_OemToAnsi(txt, txt);
    if(rc = SendMailWriteData(txt)) return(rc);
    if(convertansi) SCR_AnsiToOem(txt, txt);
    return(0);
}


/*============================================================================
Attache un fichier au mail démarré par SendMailInit(). Un nombre quelconque
de fichiers peuvent être attachés. L'encodage peut se faire selon les trois modes standards :
&EN 8 bits
&EN base64
&EN Quoted Printable

Attention, cette fonction ne peut être utilisée qu'après SendMailInit().


&EN char *filename = nom du fichier à attacher
&EN char *contenttype = type de fichier (mime type)
	(ex. "text/plain" ou "image/gif", "application/octet-stream")
&EN int encoding = codage de l'attachment
&EN2 0 : 8 bits
&EN2 1 ou 64 : base64 (1 pour la compatibilité)
&EN2 2 : Quoted Printable
&EN int convertansi = convertit automatiquement d'Ascii en Ansi
&EN isbody : si 1, n'indique pas "Content-Disposition: attachment", ni le nom du fichier

&RT
&EN 0 : Ok
&EN -1 : erreur

Le message d'erreur se trouve dans la variable globale SENDMAIL_ERROR.

&EX
    voir SendMailInit()
&TX
&SA SendMailInit(), SendMailEnd(), SendMailAttach(), SendMailStartAttachTxt(), SendMailAddAttachTxtLine()
=============================================================================== */

SendMailAttach(char *filename, char *contenttype, int encoding, int convertansi, int isbody)
{
    char    buf[256], file2[512], *content_encoding;
    int     rc;

    strcpy(file2, filename);
    SCR_replace(file2, "\\", "/");
    sprintf(buf, "--%s", SMTP_BOUNDARY);
    if(rc = SendMailWriteData(buf)) return(rc);

    switch(encoding) {
	case 0 :
	    sprintf(buf, "Content-Type: %s; charset=\"iso-8859-1\";", contenttype);
	    if(!isbody) sprintf(buf + strlen(buf), " name=\"%s\"", file2);
	    if(rc = SendMailWriteData(buf)) return(rc);
	    sprintf(buf, "Content-Transfer-Encoding: 8bit");
	    if(rc = SendMailWriteData(buf)) return(rc);
	    if(!isbody) sprintf(buf, "Content-Disposition: attachment; filename=\"%s\"", file2);
	    if(rc = SendMailWriteData(buf)) return(rc);
	    if(rc = SendMailWriteData(" ")) return(rc);
	    if(rc = SendMailFile8(filename, convertansi)) return(rc);
	    break;

	case 1:
	case 64 :
	    sprintf(buf, "Content-Type: %s;", contenttype);
	    if(!isbody) sprintf(buf + strlen(buf), " name=\"%s\"", file2);
	    if(rc = SendMailWriteData(buf)) return(rc);
	    sprintf(buf, "Content-Transfer-Encoding: base64");
	    if(rc = SendMailWriteData(buf)) return(rc);
	    if(!isbody) sprintf(buf, "Content-Disposition: attachment; filename=\"%s\"", file2);
	    if(rc = SendMailWriteData(buf)) return(rc);
	    if(rc = SendMailWriteData(" ")) return(rc);
	    if(rc = SendMailFileCode64(filename, convertansi)) return(rc);
	    break;

	case 2 :
	    sprintf(buf, "Content-Type: %s;", contenttype);
	    if(!isbody) sprintf(buf + strlen(buf), " name=\"%s\"", file2);
	    if(rc = SendMailWriteData(buf)) return(rc);
	    sprintf(buf, "Content-Transfer-Encoding: quoted-printable");
	    if(rc = SendMailWriteData(buf)) return(rc);
	    if(!isbody) sprintf(buf, "Content-Disposition: attachment; filename=\"%s\"", file2);
	    if(rc = SendMailWriteData(buf)) return(rc);
	    if(rc = SendMailWriteData(" ")) return(rc);
	    if(rc = SendMailFileQP(filename, convertansi)) return(rc);
	    break;
    }

    // rc = SendMailWriteData(" "); /* JMP 11-03-03 */
    return(rc);
}


/*NH*/
SendMailFile8(char *filename, int convertansi)
{
    char    buf[256];
    FILE    *fd;
    int     ch, i = 0;

    fd = fopen(filename, "rb");
    if(fd == 0) return(-1);

    while(1) {
	ch = getc(fd);
	if(ch == EOF) {
	    if(i > 0) {
		buf[i] = 0;
		if(convertansi) SCR_OemToAnsi(buf, buf);
		SendMailWriteData(buf);
	    }
	    break;
	}
	if(ch == '\n' || ch == '\f' || i >= 255) {
	    buf[i] = 0;
	    if(convertansi) SCR_OemToAnsi(buf, buf);
	    SendMailWriteData(buf);
	    i = 0;
	}
	if(ch != '\n' && ch != '\f')
	    buf[i++] = ch;
    }

    fclose(fd);
    return(0);
}

/*NH*/
SendMailFileQP(char *filename, int convertansi)
{
    char    buf[256];
    FILE    *fd;
    int     ch, i = 0;

    fd = fopen(filename, "rb");
    if(fd == 0) return(-1);

    while(1) {
	ch = getc(fd);
	if(ch == EOF) {
	    if(i > 0) {
		buf[i] = 0;
		SendMailWriteData(buf);
	    }
	    break;
	}
	if(ch == '\n' || i >= 80) {
	    if(i >= 80) buf[i++] = '=';
	    buf[i] = 0;
	    SendMailWriteData(buf);
	    i = 0;
	}
	if(ch != '\n') {
	    if(convertansi) ch = SCR_OemToAnsiChar(ch);
	    if(ch >= 127 || ch < ' ' || ch == '=') {
		buf[i++] = '=';
		buf[i++] = "0123456789ABCDEF"[ch / 16];
		buf[i++] = "0123456789ABCDEF"[ch % 16];
	    }
	    else
		buf[i++] = ch;
	}
    }

    fclose(fd);
    return(0);
}

static char charCodes[] = {
    'A','B','C','D','E','F','G','H',
    'I','J','K','L','M','N','O','P',
    'Q','R','S','T','U','V','W','X',
    'Y','Z','a','b','c','d','e','f',
    'g','h','i','j','k','l','m','n',
    'o','p','q','r','s','t','u','v',
    'w','x','y','z','0','1','2','3',
    '4','5','6','7','8','9','+','/'
};

/*NH*/
SendMailFileCode64(char *filename, int convertansi)
{
    int             ch, nc = 0, nct = 0, rc;
    unsigned char   bufin[3], bufout[80];
    FILE            *fd;

    fd = fopen(filename, "rb");
    if(fd == 0) return(-1);

    while(1) {
	ch = getc(fd);
	if(ch == EOF) {
	    switch(nc) {
		case 0 :
		    bufout[nct] = 0; /* JMP 31-12-2003 */
		    break;
		case 1 :
		    bufout[nct + 0] = charCodes[((bufin[0] >> 2) & 0x3f)];
		    //bufout[nct + 1] = charCodes[((bufin[0] & 0xf) << 2)];
		    bufout[nct + 1] = charCodes[((bufin[0] & 0x3) << 4)]; /* JMP 28-02-03 */
		    bufout[nct + 2] = '=';
		    bufout[nct + 3] = '=';
		    bufout[nct + 4] = 0;
		    break;
		case 2 :
		    bufout[nct + 0] = charCodes[((bufin[0] >> 2) & 0x3f)];
		    bufout[nct + 1] = charCodes[((bufin[0] & 0x3) << 4)|((bufin[1] >> 4) & 0xf)];
		    //bufout[nct + 2] = charCodes[(bufin[1] & 0xf)];
		    bufout[nct + 2] = charCodes[(bufin[1] & 0xf) << 2]; /* JMP 28-02-03 */
		    bufout[nct + 3] = '=';
		    bufout[nct + 4] = 0;
		    break;

	    }
	    if(rc = SendMailWriteData(bufout)) return(rc);
	    break;
	}
	if(convertansi) ch = SCR_OemToAnsiChar(ch);
	bufin[nc++] = ch;
	if(nc == 3) {
	    bufout[nct + 0] = charCodes[((bufin[0] >> 2) & 0x3f)];
	    bufout[nct + 1] = charCodes[((bufin[0] & 0x3) << 4)|((bufin[1] >> 4) & 0xf)];
	    bufout[nct + 2] = charCodes[((bufin[1] & 0xf) << 2)|((bufin[2] >> 6) & 0x3)];
	    bufout[nct + 3] = charCodes[bufin[2] & 0x3f];
	    bufout[nct + 4] = 0;
	    nct += 4;
	    if(nct >= 72) {
		if(rc = SendMailWriteData(bufout)) return(rc);
		nct = 0;
	    }
	    nc = 0;
	}
    }
    fclose(fd);
    return(0);
}


/*
main(argc, argv)
int     argc;
char    *argv[];
{
    int     i;

    if(argc < 5) {
	printf("Syntax : sendmail host user to filename\n");
	return(1);
	}

    if(SendMailInit(argv[1],  // serveur : mail.xon.be
		    argv[2],  // From address : jmp@xon.be
		    argv[2],  // From user name : Jean-Marc
		    argv[3],  // To : bp@xon.be
		    "Essai"   // subject
		    )) goto err;

    // Si on veut un texte libre comme attach 1 (vu souvent comme body)
    if(SendMailStartAttachTxt()) goto err;
    SendMailAddAttachTxtLine("Début de mon texte", 1);
    SendMailAddAttachTxtLine("------------------", 1);
    SendMailAddAttachTxtLine("Line 1\nLigne 2\nLigne 3", 1);
    SendMailAddAttachTxtLine("---\nFin.", 1);

    // attacher des fichiers avec diff encoding
    if(SendMailAttach("testmail.txt", "text/plain", 0, 1, 1)) goto err;
    if(SendMailAttach("testmail.txt", "text/plain", 64, 1, 1)) goto err;
    if(SendMailAttach("lastnode.gif", "image/gif", 1, 0, 0)) goto err;
    if(SendMailAttach("Euro.doc", "application/octet-stream", 1, 0, 1)) goto err;

    // Fin
    if(SendMailEnd()) goto err;

    printf("Ok\n");
    return(0);

err:
    printf("Error : %s\n", SENDMAIL_ERROR);
    SendMailClose();
    return(-1);
}
*/
/*
// OK
main(argc, argv)
int     argc;
char    *argv[];
{
    if(argc < 5) {
	printf("Syntax : sendmail host user to filename\n");
	return(1);
	}
    if(SendMailConnect(argv[1], 25, argv[1], argv[2])) goto err;
    if(SendMailTo(argv[3])) goto err;
    if(SendMailStartData()) return(-1);
    if(SendMailFile8(argv[4], 0)) goto err;
    SendMailEndData();
    SendMailClose();
    printf("Ok\n");
    return(0);

    printf("Ok\n");
    return(0);

err:
    printf("Error : %s\n", SENDMAIL_ERROR);
    SendMailClose();
    return(-1);
}
*/


/*
main(argc, argv)
int     argc;
char    *argv[];
{
    if(SendMailConnect("mail.xon.be", 25, "xon.be", "jmp@xon.be")) goto err;
    if(SendMailTo("jmp@xon.be")) goto err;
    if(SendMailStartData()) return(-1);
    SendMailWriteData("From: \"jmp@plan.be\" <jmp@plan.be>");
    SendMailWriteData("MIME-Version: 1.0");
    SendMailWriteData("To: <jmp@xon.be>");
    SendMailWriteData("Subject:La vie est belle");
    SendMailWriteData("Content-Type: multipart/mixed;");
    SendMailWriteData("\tboundary=\"Coucou1234\"");
    SendMailWriteData(" ");
    SendMailWriteData("--Coucou1234");
    SendMailWriteData("Content-Type: text/plain;");
    SendMailWriteData("Content-Transfer-Encoding: 8bit");
    SendMailWriteData("Content-Disposition: attachment;");
    SendMailWriteData(" ");
    SendMailWriteData("Envoyé par SCR/AL1, Gni gni gni!");
    SendMailWriteData(" ");
    SendMailWriteData("--Coucou1234");
    SendMailWriteData("Content-Type: text/plain;");
    SendMailWriteData("Content-Transfer-Encoding: 8bit");
    SendMailWriteData("Content-Disposition: attachment;");
    SendMailWriteData(" ");
    SendMailWriteData("Voici mon texte attaché 2222");
    SendMailWriteData(" ");
    SendMailWriteData("--Coucou1234");
    SendMailWriteData("Content-Type: image/gif;");
    SendMailWriteData("\tname=\"lastnode.gif\"");
    SendMailWriteData("Content-Transfer-Encoding: base64");
    SendMailWriteData("Content-Disposition: inline;");
    SendMailWriteData("\tfilename=\"lastnode.gif\"");
    SendMailWriteData(" ");
    SendMailWriteData("R0lGODlhEAAWANMAAAAAAIAAAACAAICAAAAAgIAAgACAgMDAwICAgP8AAAD/AP//AAAA//8A");
    SendMailWriteData("/wD//////yH5BAUAAA8ALAAAAAAQABYAAAQc8MlJq5Xo6n0z/5sHjpRIkln6qGfrvnAsz3Qc");
    SendMailWriteData("AQA7");
    SendMailWriteData(" ");

    SendMailWriteData("--Coucou1234--");



    SendMailEndData();
    SendMailClose();
    printf("Ok\n");
    return(0);
err:
    printf("Error : %s\n", SENDMAIL_ERROR);
    SendMailClose();
    return(-1);
}

*/



#endif

