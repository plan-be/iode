#include "scr4w.h"


char    SCRFTP_ERROR[81];
char    SCRFTP_BUF[1024];
int     SCRFTP_DEBUG = 0;

ScrFtpWrite(int socket, char *rec)
{
    int     err;

    if(rec == 0 || rec[0] == 0)
	return(0);

    err = WSockBWrite(socket, rec, strlen(rec));
    if(SCRFTP_DEBUG) {
	if(strcmp(rec, "\r\n")) printf("W:%s", rec);
	if(rec[0] && rec[strlen(rec) - 1] != '\n') printf("\n");
    }
    if(err == 0)
	return(0);

    sprintf(SCRFTP_ERROR, "Ftp Socket error %d", err);
    return(-1);
}


ScrFtpRead(int socket)
{
    int     err;

    err = WSockRead(socket, SCRFTP_BUF, 1023);
    if(SCRFTP_DEBUG) {
	printf("R:%s", SCRFTP_BUF);
	if(SCRFTP_BUF[strlen(SCRFTP_BUF) - 1] != '\n') printf("\n");
    }
    if(err < 0) {
	sprintf(SCRFTP_ERROR , "Ftp Socket error %d", err);
	return(err);
    }
    return(atoi(SCRFTP_BUF));
}

extern int WSOCK_FORCE_IOMODE;

/* =======================================================================
    Se connecte au serveur ftp 'srv' et retourne le socket de connexion
    srv : IN : Serveur
    post : IN : port TCP/IP. Si le port est 0, la valeur par défaut est 21
    user : IN : Login de l'utilisateur
    pass : IN : Mot de passe de l'utilisateur

    RET : socket de connexion
========================================================================== */

ScrFtpConnect(char *srv, int port, char *user, char *pass)
{
    int     socket, rc;
    char    buf[255];

    WSOCK_FORCE_IOMODE = 1;
    socket = WSockConnect(srv, port);
    WSOCK_FORCE_IOMODE = 0;

    if(WSOCK_DEBUG)
	DebugB("FTP connection : user '%s', to server '%s' port %d ...",
	       user, srv, port);
    if(socket <= 0) {
	switch(socket) {
	case -1 :
	    SCR_strlcpy(SCRFTP_ERROR, "Socket lib initialisation error", 80);
	    break;
	case -2 :
	    SCR_strlcpy(SCRFTP_ERROR, "Socket creation error", 80);
	    break;
	case -3 :
	    SCR_strlcpy(SCRFTP_ERROR, "Destination host not found", 80);
	    break;
	case -4 :
	    SCR_strlcpy(SCRFTP_ERROR, "Connection to host rejected", 80);
	    break;
	}
	if(WSOCK_DEBUG) DebugE(" %s",SCRFTP_ERROR);
	return(socket);
    }
    if(WSOCK_DEBUG) DebugE("Ok\n");

    rc = ScrFtpRead(socket);

    if(rc <= 0)
	return(rc);

    sprintf(buf, "USER %s\r\n", user);
    rc = ScrFtpWrite(socket, buf);
    if(rc != 0) {
	WSockClose(socket);
	return(0);
    }
    rc = ScrFtpRead(socket);
    if(rc < 300) {
	WSockClose(socket);
	return(0);
    }
    sprintf(buf, "PASS %s\r\n", pass);
    rc = ScrFtpWrite(socket, buf);
    if(rc != 0) {
	WSockClose(socket);
	return(0);
    }
    rc = ScrFtpRead(socket);
    if(rc < 200) {
	WSockClose(socket);
	return(0);
    }

    return(socket);
}

/* ===============================================================
    Ferme une connexion TCP/IP sur un serveur FTP
    socket : IN : Socket de connexion
    return : 0 : si ok
	     -1 : si la commande QUIT n'est pas acceptée
	     -2 : si la valeur de retour de QUIT est une erreur
================================================================== */

ScrFtpClose(int socket)
{
    char    buf[255];
    int     rc;

    strcpy(buf, "QUIT\r\n");
    rc = ScrFtpWrite(socket, buf);
    if(rc != 0)
	return(-1);

    rc = ScrFtpRead(socket);
    if(rc < 220)
	return(-2);
    WSockClose(socket);

    return(0);
}

/* ==========================================================================
    Change le répertoire courant sur le serveur FTP
    socket : IN : socket de connexion
    dir    : IN : répertoire de destination
    return : 0 : si ok
	     -1 : Si la commande de changement de répertoire n'est pas valide ou
		  si le repertoire de destination n'existe pas
============================================================================= */

ScrFtpChdir(int socket, char *dir)
{
    int     rc;
    char    buf[255];

    sprintf(buf, "CWD %s\r\n", dir);
    rc = ScrFtpWrite(socket, buf);
    if(rc != 0) {
	return(-1);
    }
    rc = ScrFtpRead(socket);
    if(rc == 250) return(0);
    return(-1);
}

/* ==========================================================================
    Crée le sous-repertoire dir sur le serveur FTP
    socket : IN : socket de connexion
    dir    : IN : sous-répertoire à créer
    return : 0 : si ok
	     -1 : Si le répertoire n'a pu Ûtre créé
============================================================================= */

ScrFtpMkdir(int socket, char *dir)
{
    int     rc;
    char    buf[255];

    sprintf(buf, "MKD %s\r\n", dir);
    rc = ScrFtpWrite(socket, buf);
    if(rc != 0) {
	return(-1);
    }
    rc = ScrFtpRead(socket);
    if(rc == 257) return(0);
    return(-1);
}


/* ================================================================================
   Envoie un fichier 'file' sur le socket ftp avec le 'to'
   si to est nul, file est utilisé comme nom
   Renvoie une valeur négative si le transfert a échoué
   Renvoie le nombre de bytes envoyés si le le transfert c'est déroulé correctement
=================================================================================== */

ScrFtpPutFile(int socket, char *file, char *to)
{
    int     rc, i, len = 0, c, send_sock = 0, ports, tot_len = 0;
    char    buf[512], msg[1024];
    FILE    *fd = 0;
    U_ch    **tbl;

    msg[0] = 0;
    // Ouvre de fichier à envoyer   /* BP_M 11-01-2011 12:06 */
    fd = ISC_fopen(file, "rb");
    if(fd == 0) {
	rc = -11;
	goto ended;
    }

    // Passe en mode Binaire
    sprintf(buf, "TYPE I\r\n");
    rc = ScrFtpWrite(socket, buf);
    if(rc != 0) {
	rc = -1;
	goto ended;
    }
    rc = ScrFtpRead(socket);
    if(rc < 200) {
	rc = -2;
	goto ended;
    }

    // Demande au serveur de passer en mode passif
    sprintf(buf, "PASV\r\n");
    rc = ScrFtpWrite(socket, buf);
    if(rc != 0) {
	rc = -3;
	goto ended;
    }
    rc = ScrFtpRead(socket);
    if(rc != 227) {
	sprintf(msg, "Error %.20s ", SCRFTP_BUF);
	rc = -4;
	goto ended;
    }
    // Le message reçu du serveur contient l'adresse IP et le port TCP/IP
    // où se connecter et envoyer les données
    for(i = 20 ; SCRFTP_BUF[i] ; i++)
	if(SCRFTP_BUF[i] == '(') break;
    if(SCRFTP_BUF[i] == 0) {
	rc = -5;
	goto ended;
    }
    i++;
    strcpy(buf, SCRFTP_BUF + i);
    for(i = 0 ; buf[i] ; i++)
	if(buf[i] == ')') break;
    if(buf[i] == 0) {
	rc = -6;
	goto ended;
    }
    buf[i] = 0;
    tbl = SCR_vtoms2(buf, ",", 0);
    if(SCR_tbl_size(tbl) < 6) {
	SCR_free_tbl(tbl);
	rc = -7;
	goto ended;
    }

    ports = atoi(tbl[4]) * 256 + atoi(tbl[5]);
    sprintf(buf, "%s.%s.%s.%s", tbl[0], tbl[1],  tbl[2],  tbl[3]);
    SCR_free_tbl(tbl);
    // Force le connect à ne pas faire d'ioctl
    WSOCK_FORCE_IOMODE = 1;
    // Se connecte au serveur de données
    send_sock = WSockConnect(buf, ports);
    WSOCK_FORCE_IOMODE = 0;

    if(send_sock <= 0) {
	rc = -8;
	goto ended;
    }

    // Envoi au serveur FTP l'ordre d'attendre les données sur le canal de réception
    // Et crée un fichier du nom 'file'
    if(to == 0) to = file;
    sprintf(buf, "STOR %s\r\n", to);
    rc = ScrFtpWrite(socket, buf);
    if(rc != 0) {
	rc = -9;
	goto ended;
    }
    rc = ScrFtpRead(socket);
    if(rc < 100) {
	rc = -10;
	goto ended;
    }

    rc = len = 0;
    while(!ISC_feof(fd)) {
	c = ISC_getc(fd);
	if(c == EOF || c == ISC_EOF) break;
	buf[len++] = c;
	if(len == 512) {
	    rc = WSockWrite(send_sock, buf, len);
	    if(rc != 0)
		break;
	    tot_len += len;
	    len = 0;
	}
    }
    ISC_fclose(fd);
    fd = 0;
    if(rc) {
	rc = -12;
	goto ended;
    }
    if(len > 0) {
	rc = WSockWrite(send_sock, buf, len);
	tot_len += len;
    }
    WSockFlush(send_sock);

ended:
    if(fd) ISC_fclose(fd);
    if(send_sock) {
	WSockClose(send_sock);
	rc = ScrFtpRead(socket);
	if(rc < 200) rc = -13;
    }
    Debug("ScrFtpPutFile(%d, %s, %s) return(len=%d, %d);\n", socket, file, to, tot_len, rc);
    if(rc < 0) {
	return(rc);
    }
    return(tot_len);
}

/* ================================================================================
   Copie file dans to sur le serveur ftp ouvert (socket).
   Commence par changer de directory si le premier caractère de to est / ou \.
   Essaie de créer les dir intermédiaires qui n'existent pas.

   Renvoie une valeur négative si le transfert a échoué
   Renvoie le nombre de bytes envoyés si le le transfert c'est déroulé correctement,
    un code < 0 sinon
=================================================================================== */

ScrFtpPutFileInDir(int socknb, char *file, char *to)
{
    int     i, rc = 0;
    U_ch    **tbl;

    // explose en répertoires
    tbl = SCR_vtoms(to, "/\\");
    rc = ScrFtpChdir(socknb, "/");
    if(rc) return(rc);

    for(i = 0 ; tbl[i + 1] ; i++)  {
	rc = ScrFtpChdir(socknb, tbl[i]);
	if(rc)  {
	    rc = ScrFtpMkdir(socknb, tbl[i]);
	    if(rc) break;
	    rc = ScrFtpChdir(socknb, tbl[i]);
	    if(rc) break;
	}
    }

    if(rc == 0)
	rc = ScrFtpPutFile(socknb, file, tbl[i]);

    SCR_free_tbl(tbl);
    return(rc);
}

/* =========================================================================
   Lit un répertoire sur le serveur ftp et revoit une structure SCRSTAT *
   le nom, la taille et le type de fichier est indiqué
   Retourne 0 si erreur
   Retourne un pointeur vers un tableau de pointeur SCRSTAT
============================================================================ */
SCRSTAT **ScrFtpDir(int socket, char *dir)
{
    int     rc, i, j, mode, c, recv_sock = 0, ports, nbl, len, totlen;
    char    buf[512], bufr[512], *tmp;
    FILE    *fd;
    U_ch    **tbl, **tbl2;
    SCRSTAT **ss;

    // Passe en mode ASCII

    sprintf(buf, "TYPE A\r\n");
    rc = ScrFtpWrite(socket, buf);
    if(rc != 0) {
	sprintf(buf, "BINARY\r\n");
	rc = ScrFtpWrite(socket, buf);
	if(rc != 0) {
	    rc = -1;
	    goto ended;
	}
    }
    rc = ScrFtpRead(socket);
    if(rc < 200) {
	rc = -2;
	goto ended;
    }

    // Demande au serveur de passer en mode passif
    sprintf(buf, "PASV\r\n");
    rc = ScrFtpWrite(socket, buf);
    if(rc != 0) {
	rc = -3;
	goto ended;
    }
    rc = ScrFtpRead(socket);
    if(rc != 227) {
	rc = -4;
	goto ended;
    }

    // Le message reçu du serveur contient l'adresse IP et le port TCP/IP
    // où se connecter et envoyer les données
    for(i = 20 ; SCRFTP_BUF[i] ; i++)
	if(SCRFTP_BUF[i] == '(') break;
    if(SCRFTP_BUF[i] == 0) {
	rc = -5;
	goto ended;
    }
    i++;
    strcpy(buf, SCRFTP_BUF + i);
    for(i = 0 ; buf[i] ; i++)
	if(buf[i] == ')') break;
    if(buf[i] == 0) {
	rc = -6;
	goto ended;
    }
    buf[i] = 0;
    tbl = SCR_vtoms2(buf, ",", 0);
    if(SCR_tbl_size(tbl) < 6) {
	SCR_free_tbl(tbl);
	rc = -7;
	goto ended;
    }

    ports = atoi(tbl[4]) * 256 + atoi(tbl[5]);
    sprintf(buf, "%s.%s.%s.%s", tbl[0], tbl[1],  tbl[2],  tbl[3]);
    SCR_free_tbl(tbl);
    // Force le connect à ne pas faire d'ioctl
    WSOCK_FORCE_IOMODE = 1;
    // Se connecte au serveur de données
    recv_sock = WSockConnect(buf, ports);
    WSOCK_FORCE_IOMODE = 0;

    if(recv_sock <= 0) {
	rc = -8;
	goto ended;
    }

    // Envoi au serveur FTP l'ordre de renvoyer sur le canal de données la liste des fichiers dir répertoire
    sprintf(buf, "LIST %s\r\n", dir);
    rc = ScrFtpWrite(socket, buf);
    if(rc != 0) {
	rc = -9;
	goto ended;
    }
    rc = ScrFtpRead(socket);
    if(rc < 100) {
	rc = -10;
	goto ended;
    }

    tmp = SCR_malloc(10000);
    totlen = 10000;
    len = 0;

    while(1) {
	rc = WSockRead(recv_sock, buf, 512);
	if(rc <= 0) break;
	if(len + rc > totlen) {
	    tmp = SCR_realloc(tmp, sizeof(U_ch *), totlen, totlen + 10000);
	    totlen += 10000;
	}
	memcpy(tmp + len, buf, rc);
	len += rc;
    }
ended:
    if(recv_sock) {
	WSockClose(recv_sock);
	rc = ScrFtpRead(socket);
	if(len <= 0 || rc == 450)
	    rc = -14;
	else if(rc < 226)
	    rc = -13;
    }
    if(rc < 0) return((SCRSTAT **)0);

    SCR_replace(tmp, "\r\n", "\n");
    if(tmp[strlen(tmp) - 1] == '\n') tmp[strlen(tmp) -1] = 0;
    tbl = SCR_vtoms2(tmp, "\n", 0);
    len = SCR_tbl_size(tbl);
    if(len == 0) return((SCRSTAT **)0);

    ss = (SCRSTAT **)SCR_malloc(sizeof(SCRSTAT *) * (len + 1));
    for(i = 0 ; tbl[i] ; i++) {
	tbl2 = SCR_vtoms2(tbl[i], " \t", 1);
	for(j = 0 ; tbl2[j] ; j++) SCR_strip(tbl2[j]);
	len = SCR_tbl_size(tbl2);
	ss[i] = (SCRSTAT *)SCR_malloc(sizeof(SCRSTAT));
	strcpy(ss[i]->ss_name, tbl2[len - 1]);
	j = atol(tbl2[len - 5]);
	if(j >= 0)
	    ss[i]->ss_size = j;

	mode = 6;
	if(tbl[i][0] == 'd' || strcmp(".", tbl2[len - 1]) == 0 || strcmp("..", tbl2[len - 1]) == 0) {
	    mode |= 1;
	    mode |= SCR_FTYPE_DIR;
	    ss[i]->ss_size = 0;
	}
	ss[i]->ss_mode = mode;
    }
    return(ss);
}

/* ==============================================================
    Lit un fichier 'file' sur le socket ftp et le crée dans to
    si act = 1 : le fichier est éffacé du serveur
================================================================= */
ScrFtpGetFile(int socket, char *file, char *to, int act)
{
    int     rc, i, len, c, recv_sock = 0, ports, tot_len = 0;
    char    buf[512];
    FILE    *fd;
    U_ch    **tbl;

    // Passe en mode Binaire
    sprintf(buf, "TYPE I\r\n");
    rc = ScrFtpWrite(socket, buf);
    if(rc != 0) {
	rc = -1;
	goto ended;
    }
    rc = ScrFtpRead(socket);
    if(rc < 200) {
	rc = -2;
	goto ended;
    }

    // Demande au serveur de passer en mode passif
    sprintf(buf, "PASV\r\n");
    rc = ScrFtpWrite(socket, buf);
    if(rc != 0) {
	rc = -3;
	goto ended;
    }
    rc = ScrFtpRead(socket);
    if(rc != 227) {
	rc = -4;
	goto ended;
    }

    // Le message reçu du serveur contient l'adresse IP et le port TCP/IP
    // où se connecter et envoyer les données
    for(i = 20 ; SCRFTP_BUF[i] ; i++)
	if(SCRFTP_BUF[i] == '(') break;
    if(SCRFTP_BUF[i] == 0) {
	rc = -5;
	goto ended;
    }
    i++;
    strcpy(buf, SCRFTP_BUF + i);
    for(i = 0 ; buf[i] ; i++)
	if(buf[i] == ')') break;
    if(buf[i] == 0) {
	rc = -6;
	goto ended;
    }
    buf[i] = 0;
    tbl = SCR_vtoms2(buf, ",", 0);
    if(SCR_tbl_size(tbl) < 6) {
	SCR_free_tbl(tbl);
	rc = -7;
	goto ended;
    }

    ports = atoi(tbl[4]) * 256 + atoi(tbl[5]);
    sprintf(buf, "%s.%s.%s.%s", tbl[0], tbl[1],  tbl[2],  tbl[3]);
    SCR_free_tbl(tbl);
    // Force le connect à ne pas faire d'ioctl
    WSOCK_FORCE_IOMODE = 1;
    // Se connecte au serveur de données
    recv_sock = WSockConnect(buf, ports);
    WSOCK_FORCE_IOMODE = 0;

    if(recv_sock <= 0) {
	rc = -8;
	goto ended;
    }

    // Envoi au serveur FTP l'ordre d'attendre les données sur le canal de réception
    // Et crée un fichier du nom 'file'
    sprintf(buf, "RETR %s\r\n", file);
    rc = ScrFtpWrite(socket, buf);
    if(rc != 0) {
	rc = -9;
	goto ended;
    }
    rc = ScrFtpRead(socket);
    if(rc < 100) {
	rc = -10;
	goto ended;
    }
    if(rc == 550) {
	rc = -15;
	goto ended;
    }

    // Ouvre de fichier à créer
    if(to == 0) to = file;
    fd = ISC_fopen(to, "wb+");
    if(fd == 0) {
	rc = -11;
	goto ended;
    }
    rc = tot_len = 0;

    while(1) {
	i = WSockRead(recv_sock, buf, 512);
	if(i <= 0) break;
	ISC_fwrite(buf, i, 1, fd);
	tot_len += i;
    }
    ISC_fclose(fd);

ended:
    if(recv_sock) {
	WSockClose(recv_sock);
	if(rc < 0) return(rc);
	rc = ScrFtpRead(socket);
	if(rc < 200) rc = -13;
	if(act) {
	    sprintf(buf, "DELE %s\r\n", file);
	    rc = ScrFtpWrite(socket, buf);
	    if(rc != 0) {
		rc = -9;
		goto ended;
	    }
	    rc = ScrFtpRead(socket);
	    if(rc < 200) {
		rc = -14;
		goto ended;
	    }
	}
    }
    if(rc < 0) return(rc);

    return(tot_len);
}


/* ==============================================================
    Efface un fichier 'file' sur le socket ftp
    socket : IN : socket de connexion
    file   : IN : fichier à effacer
    return : 0 : si ok
	     -1 : Si l'envoi de la commande à échoué
	     -2 : Si le fichier n'existe pas
================================================================= */
ScrFtpDelFile(int socket, char *file)
{
    int     rc;
    char    buf[255];

    sprintf(buf, "DELE %s\r\n", file);
    rc = ScrFtpWrite(socket, buf);
    if(rc != 0)
	return(-1);
    rc = ScrFtpRead(socket);
    if(rc < 200)
	return(-2);

    return(0);
}

/* ==============================================================
    Retoure le répertoire courant sur le serveur
    socket : IN : socket de connexion
    res    : OUT: répertoire courant du serveur FTP
		  si res est nul, la valeur retournée est allouée
		  sinon res doit être assez long pour contenir le résultat
    return : 0 : si ok
	     -1 : Si l'envoi de la commande à échoué
	     -2 : Si le fichier n'existe pas
================================================================= */
char *ScrFtpPwd(int socket, char *res)
{
    int     rc, i, j;
    char    *pwd, buf[255];

    strcpy(buf, "XPWD\r\n");
    rc = ScrFtpWrite(socket, buf);
    if(rc != 0)
	return((char *)0);
    rc = ScrFtpRead(socket);
    if(rc < 200)
	return((char *)0);

    i = U_pos('"', SCRFTP_BUF);
    if(i < 0) return((char *)0);
    j = U_pos('"', SCRFTP_BUF + i + 1);
    if(j < 0) return((char *)0);
    if(!res) {
	pwd = SCR_malloc(j - i + 1);
	res = pwd;
    }
    memcpy(res, SCRFTP_BUF + i + 1, j);
    res[j] = 0;

    return(res);
}

/* ==============================================================
    Renomme un fichier 'filein' par 'fileout' sur le socket ftp
    socket : IN : socket de connexion
    filein : IN : Nom d'origine
    fileout: IN : Nouveau nom
    return : 0 : si ok
	     -1 : Si l'envoi de la commande à échoué
	     -2 : Si le fichier n'existe pas
================================================================= */
ScrFtpRenameFile(int socket, char *filein, char *fileout)
{
    int     rc;
    char    buf[255];

    sprintf(buf, "RNFR %s\r\n", filein);
    rc = ScrFtpWrite(socket, buf);
    if(rc != 0)
	return(-1);
    rc = ScrFtpRead(socket);
    if(rc < 350)
	return(-2);

    sprintf(buf, "RNTO %s\r\n", fileout);
    rc = ScrFtpWrite(socket, buf);
    if(rc != 0)
	return(-1);
    rc = ScrFtpRead(socket);
    if(rc < 250)
	return(-2);

    return(0);
}














