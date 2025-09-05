#include "scr4w.h"
#include "scr4_srv.h"

#define ISC_FGETBUFSIZE 32000
//#define ISC_FGETBUFSIZE 28000
//#define ISC_FGETBUFSIZE 16384
#define ISC_ISLOCAL(fd)    ((unsigned long)(fd) > 21)

extern S4C **ISC_S4C;
extern int ISC_NB_CONN;

extern char    SCR_ISC_SEPS[2];
extern long    ISC_SRV_TIME;
extern int     ISC_SRV_ALGO;

extern char     ISC_CWD[SCR_MAX_FILE_LG_MAX + 1];   /* BP_M 09-10-2012 11:20 */

static int _tmpg_DebugActif          = -1000;
static int _tmpg_SCR_GZIP_LEVEL      = -1000;
static int _tmpg_ISC_COMPRESS        = -1000;

/* ========================================================================
Initialise une communication avec le serveur s4issrv (crée un socket et se
connecte au serveur s4issrv). Cette fonction suffit s'il n'y a pas d'accès
base de données. Sinon, il y a lieu d'utiliser ISC_init_database().

Le nom du serveur et de la porte sont déterminés de la façon suivante :

&EN Si la variable SCR_ISC_SERVER (char *) est définie et non vide, elle contient
    le nom de la machine serveur sous forme d'une adresse IP ou d'un nom de
    host. Si elle n'est pas définie, la variable d'environnement
    SCR_ISC_SERVER est utilisée. Si celle-ci n'est pas non plus définie, le
    host local est utilisé ("localhost").

&EN Un raisonnement identique est valable pour la variable SCR_ISC_PORT
    (int) qui indique le numéro de la porte à ouvrir. Par défaut, cette
    valeur est fixée à 5000.

Les variables globales DebugActif, ISC_COMPRESS et SCR_GZIP_LEVEL sont passées
au serveur par cette fonction.

En sortie, la variable ISC_SERVER_VERS (int) contient la version du serveur s4issrv.

D'autre part, si la variable
&CO
    char *SCR_ISC_LOGIN
&TX
n'est pas nulle, elle est envoyée au serveur pour authentification de
l'utilisateur. Cette variable a le format :
&CO
    "login/key" où key est un entier long positif
&TX

Le login et la clé sont définis sur le serveur dans le fichier scr4_iss.ini.

En sortie, la variable SCR_ISC_ERRNO contient une des valeurs suivantes :

&EN 0 : Ok
&EN -1 : connexion impossible
&EN -2 : user inconnu
&EN -3 : clé d'accès incorrecte

&RT
&EN 0 en cas de succès,
&EN -1 : connexion impossible
&EN -2 : user inconnu
&EN -3 : clé d'accès incorrecte

&SA ISC_end(), ISC_init_database()
================================================================== */

ISC_init()
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(0);
#else
    int         rc;
    char        *getenv(), *ptr;

//    WSOCK_DEBUG = 0;

    SCR_ISC_ERRNO = 0;
    if(SCR_ISC_SERVER == 0 || SCR_ISC_SERVER[0] == 0) {
	ptr = getenv("SCR_ISC_SERVER");
	if(ptr)
	    SCR_ISC_SERVER = SCR_stracpy(ptr);
	else
	    SCR_ISC_SERVER = SCR_stracpy("localhost");
	}

    if(SCR_ISC_PORT == 0) {
	ptr = getenv("SCR_ISC_PORT");
	if(ptr)
	    SCR_ISC_PORT = atoi(ptr);
	else {
	    SCR_ISC_PORT = 5000;
	    if(SCR_ISC_VERSION >= 403) SCR_ISC_PORT = 6000 + SCR_ISC_VERSION; /* JMP 18-02-07 */
	    }
	}

    rc = ISC_init2(SCR_ISC_SERVER, SCR_ISC_PORT);
    if(rc >= 0)
	ISC_SOCKET = rc;
    return(rc);
#endif
}

ISC_init2(char *srv, int port)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(0);
#else
    int         rc, sock, sock2;
    char        *getenv(), *ptr;
    S4C         *s4c;

//    WSOCK_DEBUG = 0;

    SCR_ISC_ERRNO = 0;

    rc = ISC_check_exist(srv, port);
    if(rc >= 0) return(ISC_S4C[rc]->socket);


    Debug("Connecting to : %s:%d\n", srv, port);
    sock = WSockConnect(srv, port);
    if(sock <= 0) return(SCR_ISC_ERRNO = -1);

    ISC_add_srv(sock, srv, port);
    rc = ISC_check_exist(srv, port);
    ISC_SERVER_VERS = ISC_cmd("%d %d", ISS_GETVERS, SCR_ISC_VERSION);
    if(ISC_SERVER_VERS < 0) ISC_SERVER_VERS = 100;
    Debug("S4ISSRV Version : %d\n", ISC_SERVER_VERS);

    s4c = ISC_S4C[rc];  /* BP_M 09-10-2012 11:43 */
    if(s4c->cwd[0] > ' ') {
	Debug("ISC_init2: setcwd: '%s'\n", s4c->cwd);
	ISC_setcwd(s4c->cwd);
    }

    ISC_set_vars_gnl(1, sock);
    SCR_ISC_ERRNO = rc = ISC_set_user(sock);
    if(rc == 0)
	return(sock);

    return(SCR_ISC_ERRNO);
#endif
}

/* ========================================================================
Termine une communication initialisée par ISC_init() avec le serveur s4issrv.

&SA ISC_init(), ISC_end_database()
================================================================== */

ISC_end()
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(0);
#else
    int     rc, i;

    for(i = 0 ; i < ISC_NB_CONN ; i++)  /* BP_M 07-12-2009 19:13 */
	IS_end_database_1(i);

    ISC_SOCKET = 0;
    ISC_NB_CONN = 0;    /* BP_M 07-12-2009 19:12 */
    _tmpg_DebugActif          = -1000;
    _tmpg_ISC_COMPRESS        = -1000;
    _tmpg_SCR_GZIP_LEVEL      = -1000;
    return(rc);
#endif
}

/* ========================================================================
Envoie vers le serveur les variables globales. Doit être utilisée après ume
modification d'une de celles-ci.

Les variables globales transférées sont : DebugActif, ISC_COMPRESS et SCR_GZIP_LEVEL.

&RT 0 en cas de succès, -1 en cas d'erreur.

&SA ISC_init(), ISC_init_database()
================================================================== */

ISC_set_vars_gnl(int force, int sock)
{
    int     rc ,tmp_sock;

#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(0);
#else
    if(!force && _tmpg_DebugActif          == DebugActif              &&
       _tmpg_ISC_COMPRESS        == ISC_COMPRESS            &&
       _tmpg_SCR_GZIP_LEVEL      == SCR_GZIP_LEVEL)
	    return(0);

    tmp_sock = ISC_SOCKET;
    ISC_SOCKET = sock;

    if(ISC_printf("%d", ISS_SETVARSGNL) < 0) goto ended;
    if(ISC_send_rc(DebugActif) < 0) goto ended;
    if(ISC_send_rc(ISC_COMPRESS) < 0) goto ended;           /* JMP 04-06-00 */
    if(ISC_send_rc(SCR_GZIP_LEVEL) < 0) goto ended;         /* JMP 04-06-00 */


    _tmpg_DebugActif          = DebugActif             ;
    _tmpg_ISC_COMPRESS        = ISC_COMPRESS           ;
    _tmpg_SCR_GZIP_LEVEL      = SCR_GZIP_LEVEL         ;

    rc = ISC_read_rc();
    ISC_SOCKET = tmp_sock;
    return(rc);
ended:

    ISC_SOCKET = tmp_sock;
    return(-1);
#endif
}

extern unsigned char *WSOCK_CRYPTBUF;

/*NH ========================================================================
Envoie le login vers le serveur. Fixe en conséquence la clé d'encryptage.
Cette fonction est utilisé par ISC_init().

Le login est défini par la variable globale
&CO
    char *SCR_ISC_LOGIN;
&TX
Si cette variable est nulle ou vide, la variable d'environnement SCR_ISC_LOGIN est lue et
est utilisée si elle est définie pour l'envoi du login.

&RT
&EN 0 en cas de succès,
&EN -1 en cas d'erreur.
&EN -3 en cas de clé incorrecte
&EN -100 en cas d'accès interdit

&SA ISC_init(), ISC_init_database()
================================================================== */

ISC_set_user(int sock)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(0);
#else
    char            *login;
    int             rc, pos, tmp_sock;
    unsigned char   buf[80];

//    Debug("CL:login='%s'\n", login);
    if(SCR_ISC_LOGIN == 0 || SCR_ISC_LOGIN[0] == 0) {
	login = getenv("SCR_ISC_LOGIN");
	if(login)
	    SCR_ISC_LOGIN = SCR_stracpy(login);
	else
	    return(0);
	}

    tmp_sock = ISC_SOCKET;
    ISC_SOCKET = sock;

    login = SCR_ISC_LOGIN;
    pos = U_pos('/', login);
    if(pos > 0) login[pos] = 0;
    rc = ISC_printf("%d %s", ISS_SETUSER, login);
    login[pos] = '/';
//    Debug("Login=%s;CRYPTKEY=%d\n", login, WSOCK_CRYPTKEY);
    if(rc < 0) goto ended;
    rc = ISC_read_rc();
    switch(rc) {
	case -100 : return(-100); // login incorrect
	case 1 : return(0); // Pas de fichier .ini sur le serveur
	default :
	    /* Lit l'heure du serveur */
	    ISC_SRV_TIME = ISC_read_long();
	    /* Lit l'algo du serveur */
	    ISC_SRV_ALGO = ISC_read_rc();
	    /* Lit et vérifie le buffer encrypté */
	    ISC_read_rec(buf);
	    WSOCK_CRYPTKEY = ISC_SRV_TIME ^ atol(login + pos + 1);
	    WSockDecryptEx(buf, buf, strlen(WSOCK_CRYPTBUF), 10);
	    if(memcmp(buf, WSOCK_CRYPTBUF, strlen(WSOCK_CRYPTBUF))) {
		// Debug("CRYPTKEY %d incorrecte\n", WSOCK_CRYPTKEY);
		SCR_sleep(1000);
		// Ferme le socket
		WSockClose(ISC_SOCKET);
		ISC_SOCKET = 0;
		_tmpg_DebugActif          = -1000;
		_tmpg_ISC_COMPRESS        = -1000;
		_tmpg_SCR_GZIP_LEVEL      = -1000;
		rc = -1;
		break;
		}
	    // Debug("CRYPTKEY=%d\n", WSOCK_CRYPTKEY);
	    rc = 0;
	    break;
	}
ended:
    ISC_SOCKET = tmp_sock;
    return(rc);
#endif
}

extern U_ch    *ISC_BUFS[21];
extern int     ISC_BUFS_LG[21];
extern int     ISC_BUFS_POS[21];
extern int     ISC_BUFS_MODE[21];
extern int     ISC_BUFS_UNGETC[21];
extern int     ISC_BUFS_MAXLG[21];
extern int     ISC_BUFS_SOCKET[21];
extern int     ISC_BUFS_REEL[21];

/*NH*/
ISC_filedn(char *srvfile, char *localfile)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(-1);
#else
    FILE    *fd;
    char    buf[1024];
    int     rc, rbytes;

    // Connecte au serveur
    if(ISC_switch_server(srvfile)) return(-1);
    if(ISC_switch_server(localfile)) return(-1);

    rc = ISC_printf("%d %s", ISS_FILEDN, srvfile);
    if(rc < 0) return(rc);
    rc = ISC_read_rc();
    if(rc < 0) return(rc);

    fd = fopen(localfile, "w+b");

    while(1) {
	rc = ISC_inbuf_read_lg_bytes((char *)&rbytes, sizeof(int));
	if(rc < 0) break;

	if(rbytes == 0) break; // fin de fichier (rc = 0)
	if(rbytes < 0) { // Erreur
	    rc = -1;
	    break;
	    }
	rc = ISC_inbuf_read_lg_bytes(buf, rbytes);
	if(rc < 0) break;
	rc = fwrite(buf, rbytes, 1, fd);
	if(rc < 0) break;
	}

    fclose(fd);
    return(rc);
#endif
}

/* ========================================================================
ISC_access() vérifie si le processus serait autorisé à lire, écrire, exécuter, ou tester
l'existence d'un fichier (ou d'un autre objet appartenant au système de fichiers),
dont le nom est filename.

&EN filename : nom du fichier à tester
&EN mode : masque constitué d'un ou plusieurs arguments liés par un OU binaire ( | )
R_OK (4), W_OK (2), X_OK (1).Les requêtes R_OK, W_OK et X_OK servent respectivement
à tester la lecture, l'écriture, et l'exécution du fichier.

Si filename est préfixé par un nom d'hôte suivi d'une porte, suivi du
séparateur ! en DOS, @@ en Unix, l'opération est effectuée via le serveur
s4issrv (scr4_iss en Linux) tournant sur l'hôte.

Par exemple :
&CO
	192.168.2.1:6000!c:\usr\myfile
&TX
pour opérer sur le fichier distant sur la machine 192.168.2.1 sur laquelle s4issrv
est lancé sur la porte 6000.

S'il n'y a pas de nom de machine, l'opération a lieu sur la machine locale.

&NO Sous Unix, le test est effectué avec les User-ID et Group-ID réels du processus,
plutôt qu'avec les IDs effectifs qui sont utilisés lorsque l'on tente l'opération.
Ceci permet aux programmes Set-UID de déterminer les autorisations de l'utilisateur
ayant invoqué le programme.


&NO Sous Linux, un fichier DOS (partition montée) peut être considéré comme exécutable,
alors que l'appel système execve(2) échouera évidemment.

&RT 0 en cas de succès, -1 en cas d'erreur (voir errno dans ce cas)
================================================================== */

ISC_access(char *filename, int mode)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(access(filename, mode));
#else
    int     rc = 0;
    int     port;
    char    server[80], srvfile[256];

    // Connecte au serveur
    if(ISC_switch_server(filename)) return(-1);

    // Détermine le serveur et la porte (
    ISC_split_filename(filename, server, &port, srvfile);
    if(server[0] == 0) {
	return(access(srvfile, mode));
	}

    if(srvfile[0] == 0 || strcmp(srvfile, "/") == 0 ||
			  strcmp(srvfile, "\\") == 0) return(0);
    rc = ISC_printf("%d %s %d", ISS_ACCESS, srvfile, mode);
    if(rc < 0) return(rc);
    return(ISC_read_rc());
#endif
}

/* ========================================================================
Change le nom d'un fichier sur une machine.

&EN oldname : nom du fichier (avec éventuellement serveur:port!)
&EN newname : nouveau nom du fichier (sans serveur:port!)

Si oldname ou newname est préfixé par un nom d'hôte suivi d'une porte, suivi du
séparateur ! en DOS, @@ en Unix, l'opération est effectuée via le serveur
s4issrv (scr4_iss en Linux) tournant sur l'hôte.

Par exemple :
&CO
	192.168.2.1:6000!c:\usr\myfile
&TX
pour opérer sur le fichier distant sur la machine 192.168.2.1 sur laquelle s4issrv
est lancé sur la porte 6000.

S'il n'y a pas de nom de machine, l'opération a lieu sur la machine locale.

&RT 0 en cas de succès, -1 en cas d'erreur.

=========================================================================== */
int ISC_rename(char *oldname, char *newname)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(rename(oldname, newname));
#else
    int     rc;
    int     port;
    char    oserver[80], osrvfile[256];
    char    nserver[80], nsrvfile[256];

    // Connecte au serveur
    if(ISC_switch_server(oldname)) return(-1);

    // Détermine le serveur et la porte (
    ISC_split_filename(oldname, oserver, &port, osrvfile);
    ISC_split_filename(newname, nserver, &port, nsrvfile);
    if(oserver[0] == 0) {
	return(rename(osrvfile, nsrvfile));
	}

    rc = ISC_printf("%d %s;%s", ISS_RENAME, osrvfile, nsrvfile);
    if(rc < 0) return(rc);
    return(ISC_read_rc());
#endif
}

/* ========================================================================
Local copy : copie le fichier from dans to. Ces fichiers doivent se trouver sur la même machine.

&EN from : nom du fichier (avec éventuellement serveur:port!)
&EN to : nouveau nom du fichier (sans serveur:port!)

Si from est préfixé par un nom d'hôte suivi d'une porte, suivi du
séparateur ! en DOS, @@ en Unix, l'opération est effectuée via le serveur
s4issrv (scr4_iss en Linux) tournant sur l'hôte.

Par exemple :
&CO
	192.168.2.1:6000!c:\usr\myfile
&TX
pour opérer sur le fichier distant sur la machine 192.168.2.1 sur laquelle s4issrv
est lancé sur la porte 6000.

S'il n'y a pas de nom de machine, l'opération a lieu sur la machine locale.

&RT 0 en cas de succès, -1 en cas d'erreur.
=========================================================================== */

ISC_local_copy(char *from, char *to)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(SCR_copy_file(from, to));
#else
    int     rc;
    int     port;
    char    oserver[80], osrvfile[256];
    char    nserver[80], nsrvfile[256];

    // Connecte au serveur
    if(ISC_switch_server(from)) return(-1);

    /* Détermine le serveur et la porte */
    ISC_split_filename(from, oserver, &port, osrvfile);
    ISC_split_filename(to,   nserver, &port, nsrvfile);

    if(oserver[0] == 0)
	return(SCR_copy_file(osrvfile, nsrvfile));

    rc = ISC_printf("%d %s;%s", ISS_LOCALCOPY, osrvfile, nsrvfile);
    if(rc < 0) return(rc);
    return(ISC_read_rc());
#endif
}

/* ========================================================================
Détruit un fichier.

Si filename est préfixé par un nom d'hôte suivi d'une porte, suivi du
séparateur ! en DOS, @@ en Unix, l'opération est effectuée via le serveur
s4issrv (scr4_iss en Linux) tournant sur l'hôte.

Par exemple :
&CO
	192.168.2.1:6000!c:\usr\myfile
&TX
pour opérer sur le fichier distant sur la machine 192.168.2.1 sur laquelle s4issrv
est lancé sur la porte 6000.

S'il n'y a pas de nom de machine, l'opération a lieu sur la machine locale.


&RT 0 en cas de succès, -1 en cas d'erreur.
================================================================== */

int ISC_unlink(char *filename)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(unlink(filename));
#else
    int     rc;
    int     port;
    char    server[80], srvfile[256];

    // Connecte au serveur
    if(ISC_switch_server(filename)) return(-1);

    // Détermine le serveur et la porte (
    ISC_split_filename(filename, server, &port, srvfile);
    if(server[0] == 0) {
	return(unlink(srvfile));
	}

    rc = ISC_printf("%d %s", ISS_UNLINK, srvfile);
    if(rc < 0) return(rc);
    return(ISC_read_rc());
#endif
}

/* ========================================================================
Retourne les informations sur le fichier filename. Fonction remplacée par ISC_stat_ex()
dans la version 4.58.

Voir SCR_stat().

Si filename est préfixé par un nom d'hôte suivi d'une porte, suivi du
séparateur ! en DOS, @@ en Unix, l'opération est effectuée via le serveur
s4issrv (scr4_iss en Linux) tournant sur l'hôte.

Par exemple :
&CO
	192.168.2.1:6000!c:\usr\myfile
&TX
pour opérer sur le fichier distant sur la machine 192.168.2.1 sur laquelle s4issrv
est lancé sur la porte 6000.

S'il n'y a pas de nom de machine, l'opération a lieu sur la machine locale.

&RT 0 en cas de succès, -1 en cas d'erreur.
&SA SCR_stat()
================================================================== */
ISC_stat(char *filename, SCRSTAT *ss)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(SCR_stat(filename, ss));
#else
    int     rc;
    int     port;
    char    server[80], srvfile[256];
    SCRSTATPRE458 ss458;

    // Annule les éléments de la structure
    memset(&ss458, 0, sizeof(SCRSTATPRE458));
    ss->ss_mode = 0;
    ss->ss_size = 0;
    ss->ss_mdate = ss->ss_mtime = 0;
    ss->ss_cdate = ss->ss_ctime = 0;
    ss->ss_adate = ss->ss_atime = 0;
    ss->ss_gid = ss->ss_uid = ss->ss_nlink = 0;
    ss->ssx_mtime = ss->ssx_atime = ss->ssx_ctime = 0;
    ss->ssx_crc32 = 0;

    // Connecte au serveur
    if(ISC_switch_server(filename)) return(-1);

    // Détermine le serveur et la porte (
    ISC_split_filename(filename, server, &port, srvfile);
    if(server[0] == 0) {
	return(SCR_stat(srvfile, ss));
	}

    rc = ISC_printf("%d %s", ISS_STAT, srvfile);
    if(rc < 0) return(rc);
    rc = ISC_read_rc();
    if(rc) return(rc);
    memset(ss, 0, sizeof(SCRSTAT));
    if(ISC_SERVER_VERS <= 401) {
	rc = ISC_read_rec(&ss458);
	SCR_statdirPRE458_2_last(&ss458, ss);
    }
    else
	rc = ISC_read_rec((char *)ss);
    if(rc < 0) return(rc);
    return(0);
#endif
}

SCR_statdirPRE458_2_last(SCRSTATPRE458 *ss458, SCRSTAT *ss)
{
//  memcpy(ss->ss_name, ss458->ss_name, 64);
    memcpy(&(ss->ss_size), &(ss458->ss_size), sizeof(SCRSTATPRE458) - 64);
}

SCR_statdir401_2_last(SCRSTAT401 *ss401, SCRSTAT *ss)
{
//  memcpy(ss->ss_name, ss401->ss_name, 128);
    memcpy(&(ss->ss_size), &(ss401->ss_size), sizeof(SCRSTAT401) - 128);
}


/* ========================================================================
Retourne les informations sur le fichier filename. Voir SCR_stat_ex().

Si filename est préfixé par un nom d'hôte suivi d'une porte, suivi du
séparateur ! en DOS, @@ en Unix, l'opération est effectuée via le serveur
s4issrv (scr4_iss en Linux) tournant sur l'hôte.

Par exemple :
&CO
	192.168.2.1:6000!c:\usr\myfile
&TX
pour opérer sur le fichier distant sur la machine 192.168.2.1 sur laquelle s4issrv
est lancé sur la porte 6000.

S'il n'y a pas de nom de machine, l'opération a lieu sur la machine locale.

&RT 0 en cas de succès, -1 en cas d'erreur.
&SA SCR_stat_ex(), ISC_stat()
================================================================== */
ISC_stat_ex(char *filename, SCRSTAT *ss, int crc)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(SCR_stat_ex(filename, ss, crc));
#else
    int     rc;
    int     port;
    char    server[80], srvfile[256];
    SCRSTAT401 ss401;

    // Annule les éléments de la structure
    memset(&ss401, 0, sizeof(SCRSTAT401));
    ss->ss_mode = 0;
    ss->ss_size = 0;
    ss->ss_mdate = ss->ss_mtime = 0;
    ss->ss_cdate = ss->ss_ctime = 0;
    ss->ss_adate = ss->ss_atime = 0;
    ss->ss_gid = ss->ss_uid = ss->ss_nlink = 0;
    ss->ssx_mtime = ss->ssx_atime = ss->ssx_ctime = 0;
    ss->ssx_crc32 = 0;

    // Connecte au serveur
    if(ISC_switch_server(filename)) return(-1);

    // Détermine le serveur et la porte (
    ISC_split_filename(filename, server, &port, srvfile);
    if(server[0] == 0) {
	return(SCR_stat_ex(srvfile, ss, crc));  /* JMP 26-02-03 */
	}

    rc = ISC_printf("%d %s %d", ISS_STATEX, srvfile, crc);
    if(rc < 0) return(rc);
    rc = ISC_read_rc();
    if(rc) return(rc);
    if(ISC_SERVER_VERS <= 401) {
	rc = ISC_read_rec(&ss401);
	SCR_statdir401_2_last(&ss401, ss);
    }
    else
	rc = ISC_read_rec((char *)ss);
//    rc = ISC_read_rec((char *)ss);
    if(rc < 0) return(rc);
    return(0);
#endif
}

/* ========================================================================
Complète les structures ~cSCRSTAT ss~C dont le seul nom (ss_name) est passé
comme argument. Si ~ccrc~C vaut ~c1~C, le CRC32 est calculé pour chaque fichier.

Le premier nom de la table ss est utilisé pour déterminer le serveur et la
porte si les fichiers ne sont pas locaux. Tous les fichiers doivent donc se
trouver sur le même serveur.

Si le premier ss_name est préfixé par un nom d'hôte suivi d'une porte, suivi du
séparateur ! en DOS, @@ en Unix, l'opération est effectuée via le serveur
s4issrv (scr4_iss en Linux) tournant sur l'hôte.

Par exemple :
&CO
	192.168.2.1:6000!c:\usr\myfile
&TX
pour opérer sur le fichier distant sur la machine 192.168.2.1 sur laquelle s4issrv
est lancé sur la porte 6000.

S'il n'y a pas de nom de machine, l'opération a lieu sur la machine locale.

&RT 0 en cas de succès, -1 en cas d'erreur.
&SA SCR_stat_ex(), ISC_stat()
================================================================== */
ISC_mstat_ex_fn(SCRSTAT **ss, int crc, int *fn())
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(SCR_mstat_ex(ss, crc));
#else
    int     rc = 0, port, nb = 0, i, j, pos, rstatsize, lstatsize, namesize;
    char    server[80], srvfile[256], *buf;


    for(i = 0 ; ss[i] ; i++)
	if(ss[i]->ss_name[0] > 0)
	    nb++;

//    nb = SCR_tbl_size(ss);
    if(nb <= 0) return(0);

    // Annule les éléments de la structure
    for(i = 0 ; ss[i] ; i++) {
	ss[i]->ss_mode = 0;
	ss[i]->ss_size = 0;
	ss[i]->ss_mdate = ss[i]->ss_mtime = 0;
	ss[i]->ss_cdate = ss[i]->ss_ctime = 0;
	ss[i]->ss_adate = ss[i]->ss_atime = 0;
	ss[i]->ss_gid   = ss[i]->ss_uid = ss[i]->ss_nlink = 0;
	ss[i]->ssx_mtime = ss[i]->ssx_atime = ss[i]->ssx_ctime = 0;
	ss[i]->ssx_crc32 = 0;
	}

    // Connecte au serveur
    /* BP_M 14-12-2009 11:55 */
    for(i = 0 ; ss[i] ; i++)
	if(ss[i]->ss_name[0] > 0) break;
    if(ss[i] == 0) return(0);
    if(ISC_switch_server(ss[i]->ss_name)) return(-1);

    // Détermine le serveur et la porte (premier élément ss_name)
    ISC_split_filename(ss[i]->ss_name, server, &port, srvfile);
    /* BP_M 14-12-2009 11:56 */
    if(server[0] == 0) {
	return(SCR_mstat_ex_fn(ss, crc, fn)); /* JMP 25-07-07 */
	}

    // Envoi commande
    namesize = sizeof(ss[0]->ss_name);
    rc = ISC_printf("%d %d %d %d", ISS_MSTATEX, nb, namesize, crc);
    if(rc < 0) return(rc);

    // Créer buffer et stripper les noms (pas de serveur, port)
    buf = SCR_malloc(namesize * nb);
    for(i = 0, j = 0 ; ss[i] ; i++) {
	if(ss[i]->ss_name[0] == 0) continue;
	pos = U_pos(SCR_ISC_SEPS[1], ss[i]->ss_name);
	memcpy(buf + j * namesize, ss[i]->ss_name + pos + 1, namesize - (pos + 1));
	buf[(j + 1) * namesize - 1] = 0;
	j++;
	}

    // Envoyer les noms
    rc = ISC_write_rec(buf, nb * namesize);
    SCR_free(buf);
    if(rc < 0) return(rc);

    // Lire taille du buffer de retour (à partir de ss_size, dépend du serveur)
    rstatsize = ISC_read_rc();
    lstatsize = sizeof(SCRSTAT) - namesize;

// Debug("rstatsize=%d, lstatsize=%d", rstatsize, lstatsize);

    // Allouer et lire retour
    buf = SCR_malloc(rstatsize * nb);
    rc = ISC_read_rec(buf);
    if(rc < 0) {SCR_free(buf); return(rc);}

    // Copier le retour dans ss
    for(i = 0, j = 0 ; ss[i] ; i++) {
	if(ss[i]->ss_name[0] == 0) continue;
	memcpy(&(ss[i]->ss_size), buf + j * rstatsize, _min_(rstatsize, lstatsize));
	j++;

// Debug("%s : %d\n", ss[i]->ss_name, ss[i]->ss_size);
	}
    SCR_free(buf);  /* BP_M 15-12-2009 12:27 */
    return(0);
#endif
}

ISC_mstat_ex(SCRSTAT **ss, int crc)
{
    return(ISC_mstat_ex_fn(ss, crc, 0));
}

/* ========================================================================
Calcule le CRC32 d'un fichier local ou distant sur les premiers lg bytes.
Si lg est <= 0, retourne le crc32 de tout le fichier.

Si filename est préfixé par un nom d'hôte suivi d'une porte, suivi du
séparateur ! en DOS, @@ en Unix, l'opération est effectuée via le serveur
s4issrv (scr4_iss en Linux) tournant sur l'hôte.

Par exemple :
&CO
	192.168.2.1:6000!c:\usr\myfile
&TX
pour opérer sur le fichier distant sur la machine 192.168.2.1 sur laquelle s4issrv
est lancé sur la porte 6000.

S'il n'y a pas de nom de machine, l'opération a lieu sur la machine locale.



&RT crc32 du fichier en cas de succès, -1 en cas d'erreur de transmission, 0
    en cas d'erreur serveur (fichier non trouvé).
================================================================== */
unsigned long ISC_getcrc32_lg(char *filename, long lg)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(ScrFileCrc32(filename));
#else
    int     rc, port;
    char    server[80], srvfile[256];
    unsigned long crc = -1L;

    // Connecte au serveur
    if(ISC_switch_server(filename)) return(crc);

    // Détermine le serveur et la porte (
    ISC_split_filename(filename, server, &port, srvfile);
    if(server[0] == 0) {
	return(ScrFileCrc32Lg(srvfile, lg));
	}

    rc = ISC_printf("%d %s %ld", ISS_GETCRC32LG, srvfile, lg);
    if(rc < 0) return(crc);
    crc = (unsigned long) ISC_read_long();
    return(crc);
#endif
}

/* ========================================================================
Calcule le CRC32 d'un fichier local ou distant.

Si filename est préfixé par un nom d'hôte suivi d'une porte, suivi du
séparateur ! en DOS, @@ en Unix, l'opération est effectuée via le serveur
s4issrv (scr4_iss en Linux) tournant sur l'hôte.

Par exemple :
&CO
	192.168.2.1:6000!c:\usr\myfile
&TX
pour opérer sur le fichier distant sur la machine 192.168.2.1 sur laquelle s4issrv
est lancé sur la porte 6000.

S'il n'y a pas de nom de machine, l'opération a lieu sur la machine locale.


&RT crc32 du fichier en cas de succès, -1 en cas d'erreur de transmission, 0
    en cas d'erreur serveur (fichier non trouvé).
================================================================== */

unsigned long ISC_getcrc32(char *filename)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(ScrFileCrc32(filename));
#else
    int     rc, port;
    char    server[80], srvfile[256];
    unsigned long crc = -1L;

    // Connecte au serveur
//Debug("1\n");
    if(ISC_switch_server(filename)) return(crc);
//Debug("2\n");

    // Détermine le serveur et la porte (
    ISC_split_filename(filename, server, &port, srvfile);
    if(server[0] == 0) {
	return(ScrFileCrc32(srvfile));
	}

//Debug("3\n");
    rc = ISC_printf("%d %s", ISS_GETCRC32, srvfile);
//Debug("4 - %d\n", rc);
    if(rc < 0) return(crc);
    crc = (unsigned long) ISC_read_long();
//Debug("5 - %u\n", crc);
    return(crc);
#endif
}

/* ========================================================================
ISC_fopen() ouvre le fichier filename et lui associe un flux, exactement comme fopen()

Si filename est préfixé par un nom d'hôte suivi d'une porte, suivi du
séparateur ! en DOS, @@ en Unix, l'opération est effectuée via le serveur
s4issrv (scr4_iss en Linux) tournant sur l'hôte. On reçoit alors un numéro de flux virtuel
qui a son correspondant sur le process serveur.

Par exemple :
&CO
	192.168.2.1:6000!c:\usr\myfile
&TX
pour opérer sur le fichier distant sur la machine 192.168.2.1 sur laquelle s4issrv
est lancé sur la porte 6000.

S'il n'y a pas de nom de machine, l'opération a lieu sur la machine locale.

L'argument mode pointe vers une chaîne commençant par l'une des séquences suivantes (d'autres caractères peuvent suivre la séquence) :

&EN r : Ouvre le fichier en lecture. Le pointeur de flux est placé au début du fichier.
&EN r+: Ouvre le fichier en lecture et écriture. Le pointeur de flux est placé au début du fichier.
&EN w : Ouvre le fichier en écriture. Le fichier est créé s'il n'existait pas. S'il existait déjà, sa longueur est ramenée à 0. Le pointeur de flux est placé au début du fichier.
&EN w+ :Ouvre le fichier en lecture et écriture. Le fichier est créé s'il n'existait pas. S'il existait deja, sa longueur est ramenée à 0. Le pointeur de flux est placé au début du fichier.
&EN a : Ouvre le fichier en écriture. Le fichier est créé s'il n'existait pas. Le pointeur de flux est placé à la fin du fichier.
&EN a+ : Ouvre le fichier en lecture et écriture. Le fichier est créé s'il n'existait pas. Le pointeur de flux est placé à la fin du fichier.

La chaîne mode peut également inclure la lettre ``b'' comme 3ème caractere,
ou même entre les deux caractères d'une des séquences à 2 lettres vues ci-dessus.
Ce mode sert uniquement à assurer la compatibilité avec DOS/Windows.

&RT
&EN 0 : erreur : la variable SCR_ISC_ERRNO contient le code de l'erreur
&EN2 -2 : trop de fichiers ouverts sur le serveur
&EN2 -3 : fichier non trouvé
&EN2 -100 : accès rejeté
&EN valeur positive : pointeur (fichier local) ou pseudo-pointeur (fichier distant) vers le
flux associé au fichier
&SA ISC_fopen(), ISC_fflush(), ISC_fclose(), ISC_fread(),
	ISC_getc(), ISC_putc(), ISC_ftell(), ISC_fseek()
================================================================== */

FILE *ISC_fopen(char *filename, char *mode)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(fopen(filename, mode));
#else
    FILE    *fd = 0;
    int     rc;
    int     port, sock;
    char    server[80], srvfile[256];
    int     i;

    SCR_ISC_ERRNO = 0;
    // Connecte au serveur
    if(ISC_switch_server(filename)) return(fd);

    // Determine le serveur et la porte
    ISC_split_filename(filename, server, &port, srvfile);
    if(server[0] == 0) {
	fd = fopen(srvfile, mode);
	return(fd);
	}

    rc = ISC_printf("%d %s %s", ISS_FOPEN, srvfile, mode);
    if(rc < 0) {
	SCR_ISC_ERRNO = rc;
	return(fd);
	}
    rc = ISC_read_rc();
    if(rc < 0) {
	SCR_ISC_ERRNO = rc;
	return(fd);
	}
    i = 1;
    while(ISC_BUFS_SOCKET[i]) i++;

    SCR_free(ISC_BUFS[i]);
    ISC_BUFS[i] = SCR_malloc(ISC_FGETBUFSIZE);
    ISC_BUFS_POS[i] = 0;
    ISC_BUFS_MAXLG[i] = ISC_FGETBUFSIZE;
    ISC_BUFS_LG[i] = 0;
    ISC_BUFS_UNGETC[i] = -1;
    ISC_BUFS_MODE[i] = U_is_in('w', mode) || U_is_in('a', mode);
    ISC_BUFS_SOCKET[i] = ISC_SOCKET;
    ISC_BUFS_REEL[i] = rc;
    return((FILE *)i);
#endif
}

/* ========================================================================
ISC_fclose() ferme le flux fd ouvert par ISC_fopen().

&RT 0 en cas de succès, -1 en cas d'erreur.
&SA ISC_fopen(), ISC_fflush(), ISC_fclose(), ISC_fread(),
	ISC_getc(), ISC_putc(), ISC_ftell(), ISC_fseek()
================================================================== */

int ISC_fclose(FILE *fd)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(fclose(fd));
#else
    int     rc, fdr;

    if(fd == 0) return(0);
    if(ISC_ISLOCAL(fd)) {
	fclose(fd);
	return(0);
	}

    ISC_SOCKET = ISC_BUFS_SOCKET[(int)fd];
    if(ISC_BUFS_MODE[(int)fd]) ISC_fflush(fd);
    fdr = ISC_BUFS_REEL[(int)fd];

    SCR_free(ISC_BUFS[(int)fd]);
    ISC_BUFS[(int)fd] = 0;
    ISC_BUFS_SOCKET[(int)fd] = 0;

    rc = ISC_printf("%d %d", ISS_FCLOSE, (int) fdr);
    if(rc < 0) return(rc);
    rc = ISC_read_rc();
    return(rc);
#endif
}

/*========================================================================
ISC_fread() lit dans buf les nb blocs de lg bytes sur le flux fd ouvert par ISC_fopen().

&RT Nombre de blocs de lg bytes lus en cas de succès, -1 en cas d'erreur.
&SA ISC_fopen(), ISC_fflush(), ISC_fclose(), ISC_fread(),
	ISC_getc(), ISC_putc(), ISC_ftell(), ISC_fseek()
================================================================== */

int ISC_fread(char *buf, int lg, int nb, FILE *fd)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(fread(buf, lg, nb, fd));
#else
    int     rc;
    static int nbr = 0;

    if(fd == 0) return(-1);
    if(ISC_ISLOCAL(fd)) {
	return(fread(buf, lg, nb, fd));
	}
//  nbr++;
//  if(nbr > 2135) WSOCK_DEBUG = 2;

    ISC_SOCKET = ISC_BUFS_SOCKET[(int)fd];
    fd = (FILE *)ISC_BUFS_REEL[(int)fd];
    rc = ISC_printf("%d %d %d %d", ISS_FREAD, (int) fd, lg, nb);
    if(rc < 0) {
//      Debug("ISC_fread : ISC_printf = %d\n", rc);
	return(rc);
    }
    lg = ISC_read_rc();         // Nombre de bytes lus
    if(lg <= 0) {
	rc = ISC_feof(fd);
	if(rc == ISC_EOF) return(0);
//      Debug("ISC_fread : ISC_read_rc = %d\n", lg);
	return(0);
    }
    rc = ISC_read_rec(buf);
    if(rc < 0) {
//      Debug("ISC_fread : ISC_read_rec = %d\n", rc);
	return(rc);
    }
    return(lg);
#endif
}
/*========================================================================
ISC_fwrite() écrit nb blocs de lg bytes contenus dans buf sur le flux fd ouvert par ISC_fopen().

&RT Nombre de blocs de lg bytes écrits en cas de succès, -1 en cas d'erreur.
&SA ISC_fopen(), ISC_fflush(), ISC_fclose(), ISC_fread(),
	ISC_getc(), ISC_putc(), ISC_ftell(), ISC_fseek()
================================================================== */

int ISC_fwrite(char *buf, int lg, int nb, FILE *fd)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(fwrite(buf, lg, nb, fd));
#else
    int     rc, i;

    if(fd == 0) return(-1);
    if(ISC_ISLOCAL(fd)) {
	return(fwrite(buf, lg, nb, fd));
	}

    ISC_SOCKET = ISC_BUFS_SOCKET[(int)fd];
    fd = (FILE *)ISC_BUFS_REEL[(int)fd];
    rc = ISC_printf("%d %d %d %d", ISS_FWRITE, (int) fd, lg, nb);
    if(rc < 0) return(rc);
    /* BP_M 28-02-2011 14:37 */
    for(i = 0 ; i < nb ; i++) {
	rc = ISC_write_rec(buf + lg * i, lg);
	if(rc != 0) break;
    }
    if(rc < 0) return(rc);
    rc = ISC_read_rc();         // Nombre de blocs de lg bytes ecrits
    return(rc);
#endif
}
/*========================================================================
ISC_ungetc() place dans un buffer temporaire le caractère ch qui sera relus lors du prochain appel à ISC_getc().
fd est un flux ouvert par ISC_fopen().

&RT 0.
&SA ISC_fopen(), ISC_fflush(), ISC_fclose(), ISC_fread(),
	ISC_getc(), ISC_putc(), ISC_ftell(), ISC_fseek()
================================================================== */

int ISC_ungetc(int ch, FILE *fd)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(ungetc(ch, fd));
#else
    int     n = (int)fd;

    if(fd == 0) return(-1);
    if(ISC_ISLOCAL(fd)) {
	ungetc(ch, fd);
	return(0);
	}

    ISC_SOCKET = ISC_BUFS_SOCKET[n];
    if(n == 0) return(-1);
    ISC_BUFS_UNGETC[n] = ch;
    return(0);
#endif
}

/*========================================================================
ISC_getc() lit le prochain caractère sur le flux fd ouvert par ISC_fopen().

&RT le caractère lu ou -1 si fin de fichier ou problème.
&SA ISC_fopen(), ISC_fflush(), ISC_fclose(), ISC_fread(),
	ISC_getc(), ISC_putc(), ISC_ftell(), ISC_fseek()
================================================================== */

int ISC_getc(FILE *fd)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(getc(fd));
#else
    int     n = (int)fd, ch;

    if(n == 0) {
//      Debug("getc error 1\n");
	return(-1);
    }
    if(ISC_ISLOCAL(fd)) {
	return(getc(fd));
	}

    ISC_SOCKET = ISC_BUFS_SOCKET[n];
    if(ISC_BUFS_UNGETC[n] != -1) {
	ch = ISC_BUFS_UNGETC[n];
	ISC_BUFS_UNGETC[n] = -1;
//      Debug("getc return ungetc %d\n", ch);
	return(ch);
	}

    if(ISC_BUFS[n] == 0) {
//      Debug("getc error 2\n");
	return(-1);
    }
    fd = (FILE *)ISC_BUFS_REEL[(int)fd];
    if(ISC_BUFS_POS[n] >= ISC_BUFS_LG[n]) {
	ISC_BUFS_LG[n] = ISC_fread(ISC_BUFS[n], 1, ISC_BUFS_MAXLG[n], fd);
	ISC_BUFS_POS[n] = 0;
	if(ISC_BUFS_LG[n] <= 0) {
//          Debug("getc error 3\n");
	    return(-1); // Error ou End of File
	    }
	}
    ISC_BUFS_POS[n] += 1;
    return(ISC_BUFS[n][ISC_BUFS_POS[n] - 1]);
#endif
}
/*========================================================================
ISC_fflush() force l'écriture des bytes bufferisés sur le flux fd ouvert par ISC_fopen().

&RT 0 en cas de succès, -1 en cas d'échec.
&SA ISC_fopen(), ISC_fflush(), ISC_fclose(), ISC_fread(),
	ISC_getc(), ISC_putc(), ISC_ftell(), ISC_fseek()
================================================================== */

ISC_fflush(FILE *fd)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(fflush(fd));
#else
    int     n =(int)fd, rc;

    if(ISC_ISLOCAL(fd)) {
	return(fflush(fd));
	}
    ISC_SOCKET = ISC_BUFS_SOCKET[n];
    if(ISC_BUFS[n] == 0 || ISC_BUFS_POS[n] == 0) return(0);
    rc = ISC_fwrite(ISC_BUFS[n], ISC_BUFS_POS[n], 1, fd);   /* BP_M 17-03-2011 14:03 */
    ISC_BUFS_POS[n] = 0;
    if(rc) return(rc);
    n = ISC_BUFS_REEL[(int)fd];
    rc = ISC_printf("%d %d", ISS_FFLUSH, n);
    if(rc < 0) return(rc);
    return(ISC_read_rc());
#endif
}

/*========================================================================
La fonction ISC_fseek fixe l'indicateur de position du flux pointé par stream.
La nouvelle position, mesurée en octets, est obtenue en additionnant offset octets
au point de départ indique par from.

Si from vaut SEEK_SET, SEEK_CUR, ou SEEK_END,
le point de départ correspond respectivement au début du fichier,
à la position actuelle, ou à la fin du fichier.

&RT 0 en cas de succès, -1 en cas d'échec (voir errno).
&SA ISC_fopen(), ISC_fflush(), ISC_fclose(), ISC_fread(),
	ISC_getc(), ISC_putc(), ISC_ftell(), ISC_fseek()
================================================================== */

ISC_fseek(FILE *fd, long offset, int from)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(fseek(fd, offset, from));
#else
    int     n =(int)fd, rc;

    if(ISC_ISLOCAL(fd)) {
	return(fseek(fd, offset, from));
	}
    ISC_SOCKET = ISC_BUFS_SOCKET[n];
    if(ISC_BUFS[n] == 0) return(0);
    fd = (FILE *)ISC_BUFS_REEL[(int)fd];
    rc = ISC_printf("%d %d %ld %d", ISS_FSEEK, (int)fd, offset, from);
    if(rc < 0) return(rc);
    return(ISC_read_rc());
#endif
}
/*========================================================================
La fonction ISC_tell retourne la position courante dans le flux fd ouvert par ISC_fopen().

&RT position en cas de succès, -1 en cas d'échec (voir errno).
&SA ISC_fopen(), ISC_fflush(), ISC_fclose(), ISC_fread(),
	ISC_getc(), ISC_putc(), ISC_ftell(), ISC_fseek()
================================================================== */

long ISC_ftell(FILE *fd)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(ftell(fd));
#else
    int     n =(int)fd, rc;

    if(ISC_ISLOCAL(fd)) {
	return(ftell(fd));
	}
    ISC_SOCKET = ISC_BUFS_SOCKET[n];
    if(ISC_BUFS[n] == 0) return(0);
    fd = (FILE *)ISC_BUFS_REEL[(int)fd];
    rc = ISC_printf("%d %d", ISS_FTELL, (int)fd);
    if(rc < 0) return(rc);
    return(ISC_read_long());
#endif
}
/*========================================================================
La fonction ISC_feof indique si la fin du fichier est atteinte.

&RT 1 si fin de fichier, 0 sinon, -1 en cas d'erreur (fichier non ouvert)
&SA ISC_fopen(), ISC_fflush(), ISC_fclose(), ISC_fread(),
	ISC_getc(), ISC_putc(), ISC_ftell(), ISC_fseek()
================================================================== */

ISC_feof(FILE *fd)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(feof(fd));
#else
    int     n =(int)fd, rc = 0;

    if(ISC_ISLOCAL(fd)) {
	return(feof(fd));
	}
    if(ISC_BUFS[n] == 0) return(0);
    rc = ISC_printf("%d %d", ISS_FEOF, n);
    if(rc < 0) return(rc);
    rc = ISC_read_rc();
//  if(rc == 32) return(ISC_EOF);   /* BP_M 12-12-2009 15:57 */
    return(rc);
#endif
}
/*========================================================================
La fonction ISC_flock bloque les accès au fichier pointé par le flux
fd ouvert par ISC_fopen() à partir de la position courante pour lg bytes.

ISC_unlock() débloque l'accès.

&RT 0 en cas de succès, -1 en cas d'échec (voir errno).
&SA ISC_funlock(), ISC_fopen(), ISC_fflush(), ISC_fclose(), ISC_fread(),
	ISC_getc(), ISC_putc(), ISC_ftell(), ISC_fseek()
================================================================== */

ISC_flock(FILE *fd, long size)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(SCR_flock(fd, size));
#else
    int     n =(int)fd, rc;

    if(ISC_ISLOCAL(fd)) {
	return(SCR_flock(fd, size));
	}
    ISC_SOCKET = ISC_BUFS_SOCKET[n];
    if(ISC_BUFS[n] == 0) return(0);
    fd = (FILE *)ISC_BUFS_REEL[(int)fd];
    rc = ISC_printf("%d %d %ld", ISS_FLOCK, (int)fd, size);
    if(rc < 0) return(rc);
    return(ISC_read_rc());

#endif
}

/*========================================================================
La fonction ISC_funlock supprime le lock effectué par ISC_flock() sur le
fichier pointé par le flux
fd ouvert par ISC_fopen(). Il y a lieu de se repositionner au point
où ISC_flock() a été lancé avant d'appeler ISC_funlock().

&RT 0 en cas de succès, -1 en cas d'échec (voir errno).
&SA ISC_flock(), ISC_fopen(), ISC_fflush(), ISC_fclose(), ISC_fread(),
	ISC_getc(), ISC_putc(), ISC_ftell(), ISC_fseek()
================================================================== */

ISC_funlock(FILE *fd, long size)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(SCR_funlock(fd, size));
#else
    int     n =(int)fd, rc;

    if(ISC_ISLOCAL(fd)) {
	return(SCR_funlock(fd, size));
	}
    ISC_SOCKET = ISC_BUFS_SOCKET[n];
    if(ISC_BUFS[n] == 0) return(0);
    fd = (FILE *)ISC_BUFS_REEL[(int)fd];
    rc = ISC_printf("%d %d %ld", ISS_FUNLOCK, (int)fd, size);
    if(rc < 0) return(rc);
    return(ISC_read_rc());

#endif
}

/*========================================================================
ISC_putc() écrit le caractère ch sur le flux fd ouvert par ISC_fopen().

&RT 0.
&SA ISC_fopen(), ISC_fflush(), ISC_fclose(), ISC_fread(),
	ISC_getc(), ISC_putc(), ISC_ftell(), ISC_fseek()
================================================================== */

int ISC_putc(int ch, FILE *fd)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(putc(ch, fd));
#else
    int     n = (int)fd, rc = 0, fdr;

    if(n == 0) return(-1); // Mauvais fichier
    if(ISC_ISLOCAL(fd)) {
	if(putc(ch, fd) < 0) return(-1);
	return(0);
	}

    ISC_SOCKET = ISC_BUFS_SOCKET[n];
    if(ISC_BUFS[n] == 0) return(-1);
/*    if(ISC_BUFS[n] == 0) {
	ISC_BUFS[n] = SCR_malloc(ISC_FGETBUFSIZE);
	ISC_BUFS_POS[n] = 0;
	ISC_BUFS_LG[n] = ISC_FGETBUFSIZE;
	}
*/
    if(ISC_BUFS_POS[n] >= ISC_BUFS_MAXLG[n]) {
	rc = ISC_fwrite(ISC_BUFS[n], ISC_BUFS_POS[n], 1, fd);   /* BP_M 28-02-2011 13:44 */
	ISC_BUFS_POS[n] = 0;
	}

    ISC_BUFS[n][ISC_BUFS_POS[n]] = ch;
    ISC_BUFS_POS[n]++;
    if(rc < 0)
	return(rc);
    return(ch);
#endif
}

/* ======================================================================
Exécute une commande sur la machine locale ou sur le serveur distant.

La commande est lancée à partir de la fonction system(). Cette fonction
démarre un nouveau shell pour exécuter la commande.

Si cmd est préfixé par un nom d'hôte suivi d'une porte, suivi du
séparateur ! en DOS, @@ en Unix, l'opération est effectuée via le serveur
s4issrv (scr4_iss en Linux) tournant sur l'hôte.

Par exemple :
&CO
	192.168.2.1:6000!myprog.exe
&TX
pour exécuter le programme myprog sur la machine 192.168.2.1 sur laquelle s4issrv
est lancé sur la porte 6000.

S'il n'y a pas de nom de machine, l'opération a lieu sur la machine locale.

&SA ISC_wait(), ISC_winexec(), ISC_system()
------------------------------------------------------------------------- */

ISC_system(char *cmd)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(system(cmd));
#else
    int     rc, port;
    char    server[80], srvfile[256];

    // Connecte au serveur
    if(ISC_switch_server(cmd)) return(-1);

    // Determine le serveur et la porte
    ISC_split_filename(cmd, server, &port, srvfile);
    if(server[0] == 0) {
	return(system(srvfile));
	}

    rc = ISC_printf("%d %s", ISS_SYSTEM, srvfile);
    if(rc < 0) return(rc);
    return(ISC_read_rc());
#endif
}

/* ======================================================================
Exécute une commande sur la machine locale ou sur le serveur distant.

La commande est lancée

&EN sous Linux, à partir de la fonction system(), comme ISC_system()
&EN sous Windows, à partir de WinExec().

Si cmd est préfixé par un nom d'hôte suivi d'une porte, suivi du
séparateur ! en DOS, @@ en Unix, l'opération est effectuée via le serveur
s4issrv (scr4_iss en Linux) tournant sur l'hôte.

Par exemple :
&CO
	192.168.2.1:6000!myprog.exe
&TX
pour exécuter le programme myprog sur la machine 192.168.2.1 sur laquelle s4issrv
est lancé sur la porte 6000.

S'il n'y a pas de nom de machine, l'opération a lieu sur la machine locale.

&SA ISC_wait(), ISC_winexec(), ISC_system()
------------------------------------------------------------------------- */

ISC_winexec(char *cmd)
{
    int     rc, port;
    char    server[80], srvfile[256];

    // Connecte au serveur
    if(ISC_switch_server(cmd)) return(-1);

    // Determine le serveur et la porte
    ISC_split_filename(cmd, server, &port, srvfile);
    if(server[0] == 0) {
#if defined(DOS16) || defined(UNIX)
	return(system(srvfile));
#else
	return(WscrWinExecMin(srvfile));
#endif
	}

    rc = ISC_printf("%d %s", ISS_SYSTEM, srvfile);
    if(rc < 0) return(rc);
    return(ISC_read_rc());
}

/* ======================================================================
Exécute une commande sur la machine locale ou sur le serveur distant.

La commande est lancée

&EN sous Linux, à partir de la fonction system(), comme ISC_system()
&EN sous Windows, sous forme d'un process console

Si cmd est préfixé par un nom d'hôte suivi d'une porte, suivi du
séparateur ! en DOS, @@ en Unix, l'opération est effectuée via le serveur
s4issrv (scr4_iss en Linux) tournant sur l'hôte.

Par exemple :
&CO
	192.168.2.1:6000!myprog.exe
&TX
pour exécuter le programme myprog sur la machine 192.168.2.1 sur laquelle s4issrv
est lancé sur la porte 6000.

S'il n'y a pas de nom de machine, l'opération a lieu sur la machine locale.

&SA ISC_wait(), ISC_winexec(), ISC_system()
------------------------------------------------------------------------- */

ISC_wait(char *cmd)
{
    int     rc, port;
    char    server[80], srvfile[256];

    // Connecte au serveur
    if(ISC_switch_server(cmd)) return(-1);

    // Determine le serveur et la porte
    ISC_split_filename(cmd, server, &port, srvfile);
    if(server[0] == 0) {
#if defined(DOS16) || defined(UNIX)
	return(system(srvfile));
#else
	return(WscrCreateConsoleProcess(srvfile, 0));
#endif
	}

    rc = ISC_printf("%d %s", ISS_WAIT, srvfile);
    if(rc < 0) return(rc);
    return(ISC_read_rc());
}

/* ======================================================================
Retourne une table de pointeurs vers des structures SCRSTAT correspondant
aux spécifications données dans path (par ex. *c*). Les noms sont classés
par ordre alphabétique, mais les directories sont tous placés au début. La
table est terminée par un pointeur nul.

Si path est préfixé par un nom d'hôte suivi d'une porte, suivi du
séparateur ! en DOS, @@ en Unix, l'opération est effectuée via le serveur
s4issrv (scr4_iss en Linux) tournant sur l'hôte.

Par exemple :
&CO
	192.168.2.1:6000!c:\usr\*.c
&TX
pour opérer sur le fichier distant sur la machine 192.168.2.1 sur laquelle s4issrv
est lancé sur la porte 6000.

S'il n'y a pas de nom de machine, l'opération a lieu sur la machine locale.

Le paramètre add_dir indique la sélection à effectuer sur les directory :

&EN 0 pour une sélection normale (directory et fichiers)
&EN 1 pour une sélection de tous les directory et des seuls fichiers
    correspondant à path
&EN 2 pour une sélection des seuls fichiers, sans les directory
&EN 3 pour une sélection des seuls directory

Les variables dir et filename contiennent en output le nom absolu du
directory scanné et le masque de recherche. Elles doivent avoir une taille
suffisante.

La table est alloué à l'aide de SCR_add_ptr(). L'espace occupé est
libéré par SCR_free_tbl().

&EX
    SCRSTAT     **ss;
    int         i;
    char        dir[SCR_FILE_LG + 1];
    char        file[SCR_FILE_LG + 1];

    ss = ISC_dir("c*c", 0, dir, file);
    if(ss == 0) return;
    printf("Directory of %s\n", dir);
    for(i = 0 ; ss[i] ; i++)
	printf("Name : %s - size : %ld\n", ss[i]->ss_name, ss[i]->ss_size);
    SCR_free_tbl(ss);
&TX
&RT une table de SCRSTAT * contenant les fichiers sélectionnés et terminé
    par un pointeur nul. Si aucun fichier n'est trouvé, retourne NULL

&SA SCR_dir_ex(), ISC_dir_ex(), SCR_dir()
------------------------------------------------------------------------- */

SCRSTAT **ISC_dir(char *path, int add_dir, char *dir, char *filename)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(SCR_dir(path, add_dir, dir, filename));
#else
    int     rc, port, n, i;
    char    server[80], srvfile[256], buf[256];
    SCRSTAT **ss = 0;
    SCRSTATPRE458 *ssp = 0;

    // Connecte au serveur
    if(ISC_switch_server(path)) return(ss);

    // Determine le serveur et la porte
    ISC_split_filename(path, server, &port, srvfile);
    if(server[0] == 0) return(SCR_dir(srvfile, add_dir, dir, filename));

    rc = ISC_printf("%d %s %d", ISS_DIR, srvfile, add_dir);
    if(rc < 0) return(ss);
    n = ISC_read_rc();
    if(n <= 0) return(ss);
    ssp = (SCRSTATPRE458 *) SCR_malloc(n * sizeof(SCRSTATPRE458));
    rc = ISC_read_rec((char *)ssp);
    if(rc < 0) {
	SCR_free(ssp);
	return(ss);
	}

    ss = (SCRSTAT **) SCR_malloc((n + 1) * sizeof(SCRSTAT *));
    for(i = 0 ; i < n ; i++) {
	ss[i] = (SCRSTAT *) SCR_malloc(sizeof(SCRSTAT));
	strcpy(ss[i]->ss_name, ssp[i].ss_name);
	memcpy(&(ss[i]->ss_size), &(ssp[i].ss_size), sizeof(SCRSTATPRE458) - sizeof(ssp[i].ss_name));
	SCR_std_filename(ss[i]->ss_name);
	}
    SCR_free(ssp);
    rc = ISC_read_rec(buf); // dir
    if(port) sprintf(dir, "%s%c%d%c%s", server, SCR_ISC_SEPS[0], port, SCR_ISC_SEPS[1], buf);
    else     sprintf(dir, "%s%c%s", server, SCR_ISC_SEPS[1], buf);
    SCR_std_filename(dir);
    rc = ISC_read_rec(filename);
    return(ss);
#endif
}

/* ======================================================================
Retourne une table de pointeurs vers des structures SCRSTAT correspondant
aux spécifications données dans path (par ex. *c*). Les noms sont classés
par ordre alphabétique, mais les directories sont tous placés au début. La
table est terminée par un pointeur nul.

Si path est préfixé par un nom d'hôte suivi d'une porte, suivi du
séparateur ! en DOS, @@ en Unix, l'opération est effectuée via le serveur
s4issrv (scr4_iss en Linux) tournant sur l'hôte.

Par exemple :
&CO
	192.168.2.1:6000!c:\usr\*.c
&TX
pour opérer sur le fichier distant sur la machine 192.168.2.1 sur laquelle s4issrv
est lancé sur la porte 6000.

S'il n'y a pas de nom de machine, l'opération a lieu sur la machine locale.

Le paramètre add_dir indique la sélection à effectuer sur les directory :

&EN 0 pour une sélection normale (directory et fichiers)
&EN 1 pour une sélection de tous les directory et des seuls fichiers
    correspondant à path
&EN 2 pour une sélection des seuls fichiers, sans les directory
&EN 3 pour une sélection des seuls directory

Les variables dir et filename contiennent en output le nom absolu du
directory scanné et le masque de recherche. Elles doivent avoir une taille
suffisante.

La valeur de crc détermine le fait de calculer ou non le crc32 de chaque
fichier (voir SCR_stat_ex()).

La table est alloué à l'aide de SCR_add_ptr(). L'espace occupé est
libéré par SCR_free_tbl().

&EX
    SCRSTAT     **ss;
    int         i;
    char        dir[SCR_FILE_LG + 1];
    char        file[SCR_FILE_LG + 1];

    ss = ISC_dir("c*c", 0, dir, file);
    if(ss == 0) return;
    printf("Directory of %s\n", dir);
    for(i = 0 ; ss[i] ; i++)
	printf("Name : %s - crc32 : %u\n", ss[i]->ss_name, ss[i]->ssx_crc32);
    SCR_free_tbl(ss);
&TX
&RT une table de SCRSTAT * contenant les fichiers sélectionnés et terminé
    par un pointeur nul. Si aucun fichier n'est trouvé, retourne NULL

&SA SCR_dir_ex(), ISC_dir(), SCR_dir()
------------------------------------------------------------------------- */

SCRSTAT **ISC_dir_ex_fn(char *path, int add_dir, char *dir, char *filename, int crc, int *fn())
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(SCR_dir(path, add_dir, dir, filename));
#else
    int     rc, port, n, i;
    char    server[80], srvfile[256], *tmp, buf[256];
    SCRSTAT **ss = 0;
    SCRSTAT401 **ss401 = 0;
    int     sizestat;

    // Connecte au serveur
    if(ISC_switch_server(path)) return(ss);

    // Determine le serveur et la porte
    ISC_split_filename(path, server, &port, srvfile);
    if(server[0] == 0) return(SCR_dir_ex_fn(srvfile, add_dir, dir, filename, crc, fn));
/*
    if(ISC_SERVER_VERS != ISC_CLIENT_VERS) {
	sizestat = sizeof(SCRSTAT
*/
    rc = ISC_printf("%d %s %d %d", ISS_DIREX, srvfile, add_dir, crc);
    if(rc < 0) return(ss);
    n = ISC_read_rc();
    if(n <= 0) return(ss);
    if(ISC_SERVER_VERS <= 401)
	tmp = SCR_malloc(n * sizeof(SCRSTAT401));
    else
	tmp = SCR_malloc(n * sizeof(SCRSTAT));

    rc = ISC_read_rec(tmp);
    if(rc < 0) {
	SCR_free(tmp);
	return(ss);
	}

    if(ISC_SERVER_VERS <= 401)
	ss401 = (SCRSTAT401 **) SCR_malloc((n + 1) * sizeof(SCRSTAT401 *));
    else
	ss = (SCRSTAT **) SCR_malloc((n + 1) * sizeof(SCRSTAT *));

    for(i = 0 ; i < n ; i++) {
	if(ISC_SERVER_VERS <= 401) {
	    ss401[i] = (SCRSTAT401 *) SCR_malloc(sizeof(SCRSTAT401));
	    memcpy((char *)ss401[i], tmp + i * sizeof(SCRSTAT401), sizeof(SCRSTAT401));
	    SCR_std_filename(ss401[i]->ss_name);
	}
	else {
	    ss[i] = (SCRSTAT *) SCR_malloc(sizeof(SCRSTAT));
	    memcpy((char *)ss[i], tmp + i * sizeof(SCRSTAT), sizeof(SCRSTAT));
	    SCR_std_filename(ss[i]->ss_name);
	}
    }
    SCR_free(tmp);
    rc = ISC_read_rec(buf); // dir
    if(port) sprintf(dir, "%s%c%d%c%s", server, SCR_ISC_SEPS[0], port, SCR_ISC_SEPS[1], buf);
    else     sprintf(dir, "%s%c%s", server, SCR_ISC_SEPS[1], buf);
    SCR_std_filename(dir);
    rc = ISC_read_rec(filename);
    if(ISC_SERVER_VERS <= 401) {
	ss = (SCRSTAT **) SCR_malloc((n + 1) * sizeof(SCRSTAT *));
	for(i = 0 ; i < n ; i++) {
	    ss[i] = (SCRSTAT *) SCR_malloc(sizeof(SCRSTAT));
	    strncpy(ss[i]->ss_name, ss401[i]->ss_name, 128);
	    memcpy(&(ss[i]->ss_size), &(ss401[i]->ss_size), sizeof(SCRSTAT401) - 128);
	    SCR_free(ss401[i]);
	}
	SCR_free(ss401);
    }
    return(ss);
#endif
}

SCRSTAT **ISC_dir_ex(char *path, int add_dir, char *dir, char *filename, int crc)
{
    return(ISC_dir_ex_fn(path, add_dir, dir, filename, crc, 0));
}

/* ========================================================================
Crée le répertoire dir.

Si dir est préfixé par un nom d'hôte suivi d'une porte, suivi du
séparateur ! en DOS, @@ en Unix, l'opération est effectuée via le serveur
s4issrv (scr4_iss en Linux) tournant sur l'hôte.

Par exemple : ~c192.168.2.1:6000!c:\usr\mydir~C pour opérer sur le répertoire
distant sur la machine 192.168.2.1 sur laquelle s4issrv est lancé sur la
porte 6000.

S'il n'y a pas de nom de machine, l'opération a lieu sur la machine locale.

&EN dir : nom du répertoire à créer

&NO Sous Linux ou Unix, le répertoire est créé avec les attributs 777
    modifié par le umask courant.

&RT 0 en cas de succès, -1 en cas de problème
&SA ISC_rmdir()
================================================================== */

ISC_mkdir(char *dir)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(mkdir(dir, 0777));
#else
    int     rc, port;
    char    server[80], srvfile[256];

    // Connecte au serveur
    if(ISC_switch_server(dir)) return(-1);

    // Determine le serveur et la porte
    ISC_split_filename(dir, server, &port, srvfile);
    if(server[0] == 0) {
	return(mkdir(srvfile, 0777));
	}

    if(srvfile[0] == 0 || strcmp(srvfile, "/") == 0 ||
			  strcmp(srvfile, "\\") == 0) return(0);
    rc = ISC_printf("%d %s", ISS_MKDIR, srvfile);
    if(rc < 0) return(rc);
    return(ISC_read_rc());
#endif
}

/* ========================================================================
Change le directory courant.

Si dir est préfixé par un nom d'hôte suivi d'une porte, suivi du
séparateur ! en DOS, @@ en Unix, l'opération est effectuée via le serveur
s4issrv (scr4_iss en Linux) tournant sur l'hôte.

Par exemple : ~c192.168.2.1:6000!c:\usr\mydir~C pour opérer sur le répertoire
distant sur la machine 192.168.2.1 sur laquelle s4issrv est lancé sur la
porte 6000.

S'il n'y a pas de nom de machine, l'opération a lieu sur la machine locale.

&EN dir : nom du nouveau répertoire courant

&RT 0 en cas de succès, -1 en cas de problème
&SA ISC_rmdir(), ISC_mkdir()
================================================================== */

ISC_chdir(char *dir)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(chdir(dir));
#else
    int     rc, port;
    char    server[80], srvfile[256];

    // Connecte au serveur
    if(ISC_switch_server(dir)) return(-1);

    // Determine le serveur et la porte
    ISC_split_filename(dir, server, &port, srvfile);
    if(server[0] == 0) {
	return(chdir(srvfile));
	}

    rc = ISC_printf("%d %s", ISS_CHDIR, srvfile);
    if(rc < 0) return(rc);
    strcpy(ISC_CWD, srvfile);   /* BP_M 09-10-2012 11:19 */
    return(ISC_read_rc());
#endif
}

/* ========================================================================
Détruit le répertoire dir.

Si dir est préfixé par un nom d'hôte suivi d'une porte, suivi du
séparateur ! en DOS, @@ en Unix, l'opération est effectuée via le serveur
s4issrv (scr4_iss en Linux) tournant sur l'hôte.

Par exemple : ~c192.168.2.1:6000!c:\usr\mydir~C pour opérer sur le répertoire
distant sur la machine 192.168.2.1 sur laquelle s4issrv est lancé sur la
porte 6000.

S'il n'y a pas de nom de machine, l'opération a lieu sur la machine locale.

&EN dir : nom du répertoire à détruire

&RT 0 en cas de succès, -1 en cas de problème
&SA ISC_mkdir()
================================================================== */

ISC_rmdir(char *dir)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(rmdir(dir));
#else
    int     rc, port;
    char    server[80], srvfile[256];

    // Connecte au serveur
    if(ISC_switch_server(dir)) return(-1);

    // Determine le serveur et la porte
    ISC_split_filename(dir, server, &port, srvfile);
    if(server[0] == 0) {
	return(rmdir(srvfile));
	}

    rc = ISC_printf("%d %s", ISS_RMDIR, srvfile);
    if(rc < 0) return(rc);
    return(ISC_read_rc());
#endif
}

/*NH*/
ISC_switch_server(char *filename)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(0);
#else
    int     port, rc;
    char    server[80], srvfile[256];
    extern int ISC_SOCKET;

    // Determine le serveur et la porte
    ISC_split_filename(filename, server, &port, srvfile);
    if(server[0] == 0) return(0);

    if(port == 0) {
	port = 5000;
	if(SCR_ISC_VERSION >= 403) port = 6000 + SCR_ISC_VERSION; /* JMP 18-02-07 */
	}
    rc = ISC_check_connect(server, port);
    if(rc < 0) return(-1);
    ISC_SOCKET = rc;
    return(0);
#endif
}

/*NH*/
/* Découpe un nom de fichier avec nom de serveur
//   Ex :   fullname                  server         port      srvfile
//        www.plan.be:5000!c:\toto   www.plan.be     5000       c:\toto
//        c:\toto                       ''            0         c:\toto
//        193.190.190.12!c:\toto     193.190.190.12   0         c:\toto
//
*/

/*NH*/
ISC_split_filename(char *fullname, char *server, int *port, char *srvfile)
{
    int     pos;
    char    buf[255], buf2[255], *ptr;
    U_ch    **tbl, **ptr2;
    long    atol();

    server[0]  = 0;
    *port      = 0;
    srvfile[0] = 0;
    if(fullname == 0) return(0);

    pos = U_pos(SCR_ISC_SEPS[1], fullname);
    if(pos < 0) {
	strcpy(srvfile, fullname);
	return(0);
    }
    strncpy(buf2, fullname, pos);
    buf2[pos] = 0;
    strcpy(srvfile, fullname + pos + 1);

    if(SCR_find_alias(buf2, server, port) == 0) return(0);

    memcpy(server, fullname, pos);
    server[pos] = 0;
    pos = U_pos(SCR_ISC_SEPS[0], server);
    if(pos < 0) return(0);
    *port = atol(server + pos + 1);
    server[pos] = 0;
    return(0);
}

/* ====================================================================
Lit une ligne dans le fichier ouvert (éventuellement sur le serveur distant
via ISC_fopen()) pointé par fd. La ligne lue est stockée dans le buffer buf
de longueur maximum lg. Si la longueur de la ligne excède lg caractères, les
derniers caractères sont sautés.

Le caractère \n est lu mais n'est pas stocké dans buf. buf se termine
par le caractère \0.

&RT la longueur de la ligne lue ou -1 si la fin du fichier est atteinte
    avant qu'un caractère ne soit lu.
&EX
    FILE *fd;

    fd = ISC_fopen("190.190.190.1:5001!myfile.txt", "r");
    while(1) {
	if(ISC_read_line(fd, buf, 130) < 0) break;
	SCR_add_ptr(&tbl, &n, buf);
	}
    SCR_add_ptr(&tbl, &n, NULL);
    ISC_fclose(fd);
&TX
&SA SCR_read_line(), SCR_add_ptr()
=======================================================================*/

int ISC_read_line(FILE* fd, unsigned char* buf, int lg)
{
    int     i = 0, ch;

    while(1) {
	ch = ISC_getc(fd);
	if(ch == '\n' || ch == EOF) {
	    buf[i] = 0;
	    if(i == 0 && ch == EOF) return(-1);
	    return(i);
	    }
	if(i < lg - 1) {  /* JMP 28-05-99 */
	    buf[i] = ch;
	    i++;
	    }
	}
}


/* ====================================================================
Copie le fichier from dans le fichier to.

Un seul des deux fichiers peut se trouver sur le serveur distant. Si le
répertoire de to n'existe pas, il est créé.

&RT
&EN -1 si from ne peut être ouvert
&EN -2 si to ne peut être créé
&EN -3 si erreur d'envoi TCP/IP
&EN nbre de caractères copiés si Ok
&EX
    ISC_copy_1file("10.0.0.1:5001!myfile.txt", "myfile.txt");
&TX
&SA ISC_copy_1file_resume()
=======================================================================*/

ISC_copy_1file(char *from, char *to)
{
    FILE    *fdfrom, *fdto;
    int     rc, ch, nbytes = 0;

    return(ISC_copy_1file_resume(from, to, 0));
/*
    fdfrom = (FILE *)ISC_fopen(from, "rb");
    if(fdfrom == 0) return(-1);
    ISC_create_dirs(to);
    fdto = (FILE *)ISC_fopen(to, "wb+");
    if(fdto == 0) {
	ISC_fclose(fdfrom);
	return(-2);
	}

    while(1) {
	ch = ISC_getc(fdfrom);
	if(ch < 0) {
	    rc = nbytes;
	    break;
	    }
	nbytes++;
	rc = ISC_putc(ch, fdto);
	if(rc < 0) {
	    rc = -3;
	    break;
	    }
	}

    ISC_fclose(fdfrom);
    ISC_fclose(fdto);
    return(rc);
*/
}

extern int SCR_COPY_FILE_NB_TRY;

/* ====================================================================
Copie le fichier ~bfrom~B dans le fichier ~bto~B en minimisant les transferts : si
le transfert précédent s'est planté en cours de route, il est repris à son
point d'arrêt.

Si ~bfrom~B est un fichier qui s'accroît au fil du temps (comme un fichier de
log par exemple), seules les nouvelles données sont copiées.

&IT Méthodes

Si les 2 fichiers existent et que le crc des deux fichiers est identique,
seules les dates d'accès et de modification sont modifiées. Le mode du
fichier est également adapté.

Sinon, si le fichier to.s4~~ existe, il est comparé à from. En cas d'égalité, il
est mové dans le fichier to. S'il est plus court, on compare les CRC32 des
fichiers partiels pour limiter la copie.

Si to.s4~~ n'existe pas ou ne correspond plus à from, on compare les CRC du
début de from avec celui de to. En cas d'égalité, to est copié dans to.s4~~
et ce dernier est complété. Sinon, tout le transfert de from est effectué.

Finalement, le fichier to.s4~~ est mové dans to et les dates et heures,
ainsi que le mode est fixé à celui de la source.

&NO Sous Linux ou Unix, il n'est pas possible de changer les attributs d'un
    fichier appartenant à une partition Windows. De même, on ne peut fixer
    les temps d'accès et de modification sur une partition Windows (FAT32 en
    tout cas). Il est donc possible qu'une opération ne soit pas complétée
    parfaitement (mode ou time incorrect) lors d'une copie en Linux vers une
    partition Windows. Cela n'empêche pas la copie proprement dite d'être
    effectuée.

&EX
    ISC_copy_1file_resume("localhost:6000!toto", "tata");
    ISC_copy_1file_resume("toto", "192.168.1.2:6000!tata");
    ISC_copy_1file_resume("www.xon.be:5555!toto", "192.168.1.2:6000!tata");
&TX

&NO
Les deux fichiers peut se trouver sur un serveur distant différent. Si le
répertoire de to n'existe pas, il est créé.

&RT
&EN -1 si from ne peut être ouvert
&EN -2 si to ne peut être créé
&EN -3 si erreur d'envoi TCP/IP
&EN -4 si le time de to ne peut être fixé
&EN -5 si rename to.s4~~ impossible
&EN -6 si chmod(to) impossible
&EN nbre de caractères copiés si Ok, en particulier 0 si
&EN2 fichier to existe et est égal à from
&EN2 fichier to existe et est égal à from et que seule la date est modifiée
&EN2 si to;s4~~ existe, est égal à from et est copié dans to avec la date de from
&EX
    ISC_copy_1file_resume("10.0.0.1:5001!myfile.txt", "myfile.txt", 1);
&TX
&SA ISC_*()
=======================================================================*/

ISC_copy_1file_resume(char *from, char *to, int chkcrc)
{
    SCRSTAT ssfrom, ssfrompart, ssto, sstotmp;
    FILE    *fdfrom, *fdto;
    int     usetmp = 0, rc = 0, ch, nbytes = 0;
    char    totmp[512], *omode;
    unsigned long fromseek = 0, crcfromlg;
    int     nb_try = 0, i;

beg:
    usetmp = nbytes = rc = fromseek = 0;

    if(ISC_stat_ex(from, &ssfrom, chkcrc)) return(-1);   // From not found
    if(ISC_stat_ex(to, &ssto, chkcrc) == 0) {            // To exists
	/* Vérifier que les fichiers sont déjà identiques */
	if((chkcrc && ssto.ssx_crc32 == ssfrom.ssx_crc32 &&
		      ssto.ss_size == ssfrom.ss_size)
		    ||
	    (chkcrc == 0 && ssto.ssx_mtime == ssfrom.ssx_mtime &&
			    ssto.ss_size == ssfrom.ss_size)) {
	    // To == from
	    if(//ssto.ssx_atime == ssfrom.ssx_atime && /* JMP 11-04-03 */
	       ssto.ssx_mtime == ssfrom.ssx_mtime && // nécessaire si CHKCRC
	       ssto.ss_mode == ssfrom.ss_mode)
		    return(ssto.ss_size);   /* BP_M 21-09-2011 11:31 */ // Dates et mode identiques
	    if(!(ssto.ss_mode & 0200) && ISC_chmod(to, 0666)) return(-6);
	    if(ISC_set_file_time(to, ssfrom.ssx_atime, ssfrom.ssx_mtime)) return(-4);
	    if(ISC_chmod(to, ssfrom.ss_mode)) return(-6);
	    return(ssto.ss_size);   /* BP_M 21-09-2011 11:31 */
	    }
	}

    /* vérifier que le fichier to.s4~ est égal dans son début si chkcrc */
    sprintf(totmp, "%s.s4~", to);
    if(chkcrc) {
	if(ISC_stat_ex(totmp, &sstotmp, chkcrc) == 0) {       // Totmp exists
	    if(sstotmp.ssx_crc32 == ssfrom.ssx_crc32) {  // Totmp == from
		if(!(ssto.ss_mode & 0200)) ISC_chmod(to, 0666); /* JMP 05-02-05 */
		ISC_unlink(to);
		if(ISC_rename(totmp, to)) return(-5);    // Rename impossible
		if(!(ssto.ss_mode & 0200) && ISC_chmod(to, 0666)) return(-6);
		if(ISC_set_file_time(to, ssfrom.ssx_atime, ssfrom.ssx_mtime)) return(-4);
		if(ISC_chmod(to, ssfrom.ss_mode)) return(-6);
		return(ssto.ss_size);   /* BP_M 21-09-2011 11:31 */
		}

	    if(sstotmp.ss_size < ssfrom.ss_size) {          // size(Totmp) < size(from)
		crcfromlg = ISC_getcrc32_lg(from, sstotmp.ss_size);
		if(crcfromlg == sstotmp.ssx_crc32) {       // CRC partiels identiques
		    fromseek = sstotmp.ss_size;
		    usetmp = 1;                            // on peut utiliser tmp
		    }
		else
		    ISC_unlink(totmp);                     // Fichiers différents
		}
	    }
	}

    /* Vérifier que to est utilisable (égal dans son début à from)si chkcrc  */
    if(chkcrc && usetmp && ssto.ss_size > 0 && ssto.ss_size < ssfrom.ss_size) {   // size(To) < size(from)
	crcfromlg = ISC_getcrc32_lg(from, ssto.ss_size);
	if(crcfromlg == ssto.ssx_crc32) {            // CRC partiels identiques
	    if(ISC_local_copy(to, totmp) == 0)          // copie locale de fichiers
		fromseek = ssto.ss_size;
	    }
	}

    // Ouvrir from et placer sur le byte correspondant a la taille de totmp
    fdfrom = (FILE *)ISC_fopen(from, "rb");
    if(fdfrom == 0) {
	ISC_close_all_connect();
	nb_try ++;
	ISC_copy_1file_pause(nb_try);
//      Debug("ISC_copy_1file_resume(%s, %s) - > connexion plantée: fopen fdfrom: essai %d\n", from, to, nb_try);
	if(nb_try < SCR_COPY_FILE_NB_TRY) goto beg;
	return(-1);
    }
    if(fromseek)
	ISC_fseek(fdfrom, fromseek, SEEK_SET);

    // Ouvrir totmp et placer à la fin
    ISC_create_dirs(totmp);

    if(!chkcrc || fromseek == 0)
	omode = "wb+";
    else
	omode = "a+b";

    fdto = (FILE *)ISC_fopen(totmp, omode);
    if(fdto == 0) {
	ISC_fclose(fdfrom);
	ISC_close_all_connect();
	nb_try ++;
	ISC_copy_1file_pause(nb_try);
//      Debug("ISC_copy_1file_resume(%s, %s) - > connexion plantée: fopen fdto: essai %d\n", from, to, nb_try);
	if(nb_try < SCR_COPY_FILE_NB_TRY) goto beg;
	return(-2);
    }
    if(fromseek)
	ISC_fseek(fdto, 0L, SEEK_END);

    // Copier tout le reste
    while(1) {
	ch = ISC_getc(fdfrom);
	if(ch < 0) {
	    if(ch == ISC_EOF || ISC_ISLOCAL(fdfrom)) {
		rc = nbytes;
		break;
	    }
	    else {
		if(ISC_feof(fdfrom) == ISC_EOF) {
		    rc = nbytes;
		    break;
		}
		else {
		    ISC_fclose(fdfrom);
		    ISC_fclose(fdto);
		    ISC_close_all_connect();
		    nb_try ++;
		    if(SCR_GZIP_LEVEL < 9) SCR_GZIP_LEVEL++;
		    else SCR_GZIP_LEVEL = 0;

//                  Debug("ISC_copy_1file_resume(%s, %s, %d) - > connexion plantée à la lecture: essai %d\n", from, to, chkcrc, nb_try);
		    ISC_copy_1file_pause(nb_try);
		    chkcrc = 1;
		    if(nb_try < SCR_COPY_FILE_NB_TRY)
			goto beg;
		    rc = -3;
		    break;
		}
	    }
	    break;
	}
	nbytes++;
	rc = ISC_putc(ch, fdto);
	if(rc < 0) {
	    ISC_fclose(fdfrom);
	    ISC_fclose(fdto);
	    ISC_unlink(totmp);
	    ISC_close_all_connect();
	    nb_try ++;
	    if(SCR_GZIP_LEVEL < 9) SCR_GZIP_LEVEL++;
	    else SCR_GZIP_LEVEL = 0;
	    ISC_copy_1file_pause(nb_try);
//          Debug("ISC_copy_1file_resume(%s, %s, %d) - > connexion plantée: ISC_putc: essai %d\n", from, to, chkcrc, nb_try);
	    if(nb_try < SCR_COPY_FILE_NB_TRY) goto beg;
	    rc = -3;
	    break;
	}
	if(nbytes % 102400 == 0) ISC_fflush(fdto); /* JMP 14-11-02 */
    }

    ISC_fclose(fdfrom);
    ISC_fflush(fdto);   /* BP_M 05-11-2009 15:50 */
    ISC_fclose(fdto);



    // Détruit to, puis Renomme totmp en to et fixe les heures et les attributs
    if(!(ssto.ss_mode & 0200)) ISC_chmod(to, 0666);
    ISC_unlink(to);

    while(nb_try < SCR_COPY_FILE_NB_TRY && ISC_rename(totmp, to)) {
	nb_try ++;
	ISC_close_all_connect();
	ISC_copy_1file_pause(nb_try);
    }
    if(nb_try == SCR_COPY_FILE_NB_TRY) return(-5);    // Rename impossible

    if(!(ssto.ss_mode & 0200)) {
	while(nb_try < SCR_COPY_FILE_NB_TRY && ISC_chmod(to, 0666)) {
	   nb_try ++;
	   ISC_copy_1file_pause(nb_try);
	   ISC_close_all_connect();
	}
    }
    if(nb_try == SCR_COPY_FILE_NB_TRY) return(-6);

    while(nb_try < SCR_COPY_FILE_NB_TRY && ISC_set_file_time(to, ssfrom.ssx_atime, ssfrom.ssx_mtime)) {
	nb_try ++;
	ISC_close_all_connect();
	ISC_copy_1file_pause(nb_try);
    }
    if(nb_try == SCR_COPY_FILE_NB_TRY) return(-4);

    while(nb_try < SCR_COPY_FILE_NB_TRY && ISC_chmod(to, ssfrom.ss_mode)) {
	nb_try ++;
	ISC_close_all_connect();
	ISC_copy_1file_pause(nb_try);
    }
    if(nb_try == SCR_COPY_FILE_NB_TRY) return(-6);


    // Retourne le nombre de bytes écrits
    return(rc);
}


ISC_copy_1file_resume_2(char *from, char *to, SCRSTAT *ssfrom, SCRSTAT *ssto)
{
    SCRSTAT sstotmp;
    FILE    *fdfrom, *fdto;
    int     rc, ch, nbytes, nb_try = 0;
    char    totmp[512], *omode;
    unsigned long fromseek = 0, crcfromlg;

beg:
    nbytes = rc = fromseek = 0;

    /* Si même taille et modtime, seulement chmod */
    if(ssfrom->ss_size   == ssto->ss_size &&
       ssfrom->ssx_mtime == ssto->ssx_mtime) {
	    if(ssto->ss_mode == ssfrom->ss_mode) return(0);
	    if(!(ssto->ss_mode & 0200) && ISC_chmod(to, 0666)) return(-6);
	    if(ISC_chmod(to, ssfrom->ss_mode)) return(-6);
	    return(ssfrom->ss_size);    /* BP_M 21-09-2011 14:54 */
	    }

    /* Si ~~s4 existe, vérifier le début. Si ident, continuer */
    sprintf(totmp, "%s.s4~", to);
    if(ISC_stat_ex(totmp, &sstotmp, 1) == 0) {       // Totmp exists
	crcfromlg = ISC_getcrc32_lg(from, sstotmp.ss_size);
	if(crcfromlg == sstotmp.ssx_crc32)        // CRC partiels identiques
	    fromseek = sstotmp.ss_size;
	else
	    ISC_unlink(totmp);                     // Fichiers différents
	}

    // Ouvrir from et placer sur le byte correspondant a la taille de totmp
    fdfrom = (FILE *)ISC_fopen(from, "rb");
    if(fdfrom == 0) {
	ISC_close_all_connect();
	nb_try ++;
	Debug("ISC_copy_1file_resume(%s, %s) - > connexion plantée ISC_fopen(from): essai %d\n", from, to, nb_try);
	ISC_copy_1file_pause(nb_try);
	if(nb_try < SCR_COPY_FILE_NB_TRY) goto beg;
	return(-1);
    }
    if(fromseek)
	ISC_fseek(fdfrom, fromseek, SEEK_SET);

    // Ouvrir totmp et placer à la fin
    ISC_create_dirs(totmp);

    if(fromseek == 0)
	omode = "wb+";
    else
	omode = "a+b";

    fdto = (FILE *)ISC_fopen(totmp, omode);
    if(fdto == 0) {
	ISC_fclose(fdfrom);
	ISC_close_all_connect();
	nb_try ++;
	Debug("ISC_copy_1file_resume_2(%s, %s) - > connexion plantée ISC_fopen(fdto): essai %d\n", from, to, nb_try);
	ISC_copy_1file_pause(nb_try);
	if(nb_try < SCR_COPY_FILE_NB_TRY) goto beg;
	return(-2);
    }
    if(fromseek)
	ISC_fseek(fdto, 0L, SEEK_END);

    // Copier tout le reste
    while(1) {
	ch = ISC_getc(fdfrom);
	if(ch < 0) {
	    if(ISC_ISLOCAL(fdfrom)) {
		rc = nbytes;
		break;
	    }
	    else {
		if(ISC_feof(fdfrom) == ISC_EOF) {
		    rc = nbytes;
		    break;
		}
		else {
		    ISC_fclose(fdfrom);
		    ISC_fflush(fdto);
		    ISC_fclose(fdto);
		    ISC_close_all_connect();
		    nb_try ++;
		    if(SCR_GZIP_LEVEL < 9) SCR_GZIP_LEVEL++;
		    else SCR_GZIP_LEVEL = 0;
		    Debug("ISC_copy_1file_resume_2(%s, %s) - > connexion plantée à la lecture: essai %d\n", from, to, nb_try);
		    ISC_copy_1file_pause(nb_try);
		    if(nb_try < SCR_COPY_FILE_NB_TRY)
			goto beg;
		    rc = -3;
		    break;
		}
	    }
	    break;
	}
	nbytes++;
	rc = ISC_putc(ch, fdto);
	if(rc < 0) {
	    ISC_fclose(fdfrom);
	    ISC_fclose(fdto);
	    ISC_unlink(totmp);
	    ISC_close_all_connect();
	    nb_try ++;
	    if(SCR_GZIP_LEVEL < 9) SCR_GZIP_LEVEL++;
	    else SCR_GZIP_LEVEL = 0;
	    Debug("ISC_copy_1file_resume_2(%s, %s) - > connexion plantée ISC_putc(): essai %d\n", from, to, nb_try);
	    ISC_copy_1file_pause(nb_try);
	    if(nb_try < SCR_COPY_FILE_NB_TRY) goto beg;
	    rc = -4;
	    break;
	}
	if(nbytes % 102400 == 0)
	    ISC_fflush(fdto); /* JMP 14-11-02 */
    }

    ISC_fclose(fdfrom);
    ISC_fflush(fdto);   /* BP_M 05-11-2009 15:50 */
    ISC_fclose(fdto);

    // Détruit to, puis Renomme totmp en to et fixe les heures et les attributs
    if(!(ssto->ss_mode & 0200)) ISC_chmod(to, 0666);
    ISC_unlink(to);

    while(nb_try < SCR_COPY_FILE_NB_TRY && ISC_rename(totmp, to)) {
	nb_try ++;
	ISC_close_all_connect();
	ISC_copy_1file_pause(nb_try);
    }
    if(nb_try == SCR_COPY_FILE_NB_TRY) return(-5);    // Rename impossible

    if(!(ssto->ss_mode & 0200)) {
	while(nb_try < SCR_COPY_FILE_NB_TRY && ISC_chmod(to, 0666)) {
	   nb_try ++;
	   ISC_close_all_connect();
	   ISC_copy_1file_pause(nb_try);
	}
    }
    if(nb_try == SCR_COPY_FILE_NB_TRY) return(-6);

    while(nb_try < SCR_COPY_FILE_NB_TRY && ISC_set_file_time(to, ssfrom->ssx_atime, ssfrom->ssx_mtime)) {
	nb_try ++;
	ISC_close_all_connect();
	ISC_copy_1file_pause(nb_try);
    }
    if(nb_try == SCR_COPY_FILE_NB_TRY) return(-4);

    while(nb_try < SCR_COPY_FILE_NB_TRY && ISC_chmod(to, ssfrom->ss_mode)) {
	nb_try ++;
	ISC_close_all_connect();
	ISC_copy_1file_pause(nb_try);
    }
    if(nb_try == SCR_COPY_FILE_NB_TRY) return(-6);

    // Retourne le nombre de bytes écrits
    return(rc);
}

ISC_copy_1file_pause(int nb_try)
{
    if(nb_try > SCR_COPY_FILE_NB_TRY / 2)
	SCR_sleep(nb_try * 1000);
}

/* ====================================================================
Crée les "sur-directory" du fichier filename sur la machine locale ou sur une
machine distante.

&EN filename : nom d'un fichier à créer
&RT
&EN -1 en cas d'erreur
&EN 0 si Ok

Si filename est préfixé par un nom d'hôte suivi d'une porte, suivi du
séparateur ! en DOS, @@ en Unix, l'opération est effectuée via le serveur
s4issrv (scr4_iss en Linux) tournant sur l'hôte.

Par exemple : ~c192.168.2.1:6000!c:\usr\myfile~C pour opérer sur le fichier
distant sur la machine 192.168.2.1 sur laquelle s4issrv est lancé sur la
porte 6000.

S'il n'y a pas de nom de machine, l'opération a lieu sur la machine locale.

&EX
    ISC_create_dirs("c:\\usr\\essai\\myfile.txt");
    ISC_copy_1file("10.0.0.1:5001!myfile.txt", "c:\\usr\\essai\\myfile.txt");
&TX
La première fonction s'assure de l'existence du répertoire c:\\usr\\essai et
le crée s'il n'existe pas.

&SA ISC_*()
=======================================================================*/

ISC_create_dirs(char *filename)
{
    int     i = 1, ch;

    // skip \\localhost\...     /* JMP 07-02-05 */
    if(strncmp(filename, "\\\\", 2) == 0 || strncmp(filename, "//", 2) == 0) {
	for(i = 2 ; filename[i] && !U_is_in(filename[i], "\\/") ; i++);
	i++;
	}

    for( ; filename[i] ; i++) { /* JMP 07-02-05 */
	ch = filename[i];
	if((ch == '\\' || ch == '/') && filename[i - 1] != SCR_ISC_SEPS[0]) {
	    filename[i] = 0;
	    if(ISC_access(filename, 0)) {
		ISC_mkdir(filename);
		if(ISC_access(filename, 0)) return(-1);
		}
	    filename[i] = ch;
	    }
	}
    return(0);
}

/* ========================================================================
Envoie un string après formattage sur fd.

&RT Code retour de ISC_fwrite()
&SA ISC_fwrite()
================================================================== */

ISC_fprintf(fd, s1, s2, s3, s4, s5, s6, s7, s8, s9)
FILE    *fd;
char    *s1, *s2, *s3, *s4, *s5, *s6, *s7, *s8, *s9;
{
    char     buf[10240];

    sprintf(buf, s1, s2, s3, s4, s5, s6, s7, s8, s9);
    return(ISC_fwrite(buf, strlen(buf), 1, fd));
}


/* ========================================================================
Retourne l'heure du systeme distant ou local.

&RT l'heure en format hhmmss en cas de succès, -1 en cas d'erreur.
&SA SCR_current_time()
================================================================== */
long ISC_gettime(char *serverport)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(_SCR_current_time());
#else
    int     rc;
    int     port;
    char    server[80], srvfile[256];

    // Connecte au serveur
    if(ISC_switch_server(serverport)) return(-1);

    // Détermine le serveur et la porte (
    ISC_split_filename(serverport, server, &port, srvfile);
    if(server[0] == 0) {
	return(_SCR_current_time());
	}

    rc = ISC_printf("%d", ISS_GETTIME);
    if(rc < 0) return(rc);
    return(ISC_read_long());
#endif
}

/* ========================================================================
Retourne la date du système distant ou local.

&RT La date en format yyyymmdd en cas de succès, -1 en cas d'erreur.
&SA SCR_current_date()
================================================================== */
long ISC_getdate(char *serverport)
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(_SCR_current_date());
#else
    int     rc, port, date;
    char    server[80], srvfile[256];

    // Connecte au serveur
    if(ISC_switch_server(serverport)) return(-1);

    // Détermine le serveur et la porte (
    ISC_split_filename(serverport, server, &port, srvfile);
    if(server[0] == 0) {
	return(_SCR_current_date());
	}

    rc = ISC_printf("%d", ISS_GETDATE);
    if(rc < 0) return(rc);
    date = ISC_read_long();
//    Debug("Date : %ld\n", date);
    return(date);
#endif
}

/* ========================================================================
Retourne le niveau d'accès actuel au serveur s4issrv courant.

&RT
&EN -1 : pas de connexion
&EN 0 : accès limité aux fonctions de base
&EN 1 : accès complet
&EN 2 : accès complet avec identification
================================================================== */
ISC_getaccess()
{
#if defined(DOS16) || (defined(UNIX) && !defined(LINUX))
    return(0);
#else
    int     rc;

    // Connecte au serveur
    if(ISC_SOCKET <= 0) return(-1);

    rc = ISC_printf("%d", ISS_GETACCESS);
    if(rc < 0) return(rc);
    return(ISC_read_long());
#endif
}

/* ========================================================================
Change l'heure d'accès et de modification d'un fichier.

Attention, il faut que fichier ne soit pas readonly pour que cela fonctionne.

Si filename est préfixé par un nom d'hôte suivi d'une porte, suivi du
séparateur ! en DOS, @@ en Unix, l'opération est effectuée via le serveur
s4issrv (scr4_iss en Linux) tournant sur l'hôte.

Par exemple : ~c192.168.2.1:6000!c:\usr\myfile~C pour opérer sur le fichier
distant sur la machine 192.168.2.1 sur laquelle s4issrv est lancé sur la
porte 6000.

S'il n'y a pas de nom de machine, l'opération a lieu sur la machine locale.


&EN filename : nom fichier
&EN actime : nombre de secondes depuis le 1/1/1970  (format de stat)
&EN modtime : nombre de secondes depuis le 1/1/1970 (format de stat)

RT 0 en cas de succès, -1 en cas de problème

&RT
&EN -1 : erreur
&EN 0 : Ok

&SA ISC_chmod()
================================================================== */

ISC_set_file_time(char *filename, long actime, long modtime)
{
    int     rc, port;
    char    server[80], srvfile[256];

    // Connecte au serveur
    if(ISC_switch_server(filename)) return(-1);

    // Determine le serveur et la porte
    ISC_split_filename(filename, server, &port, srvfile);
    if(server[0] == 0) {
	rc = WscrSetFileTime(filename, actime, modtime);
//      Debug("WscrSetFileTime(%s, %d, %d)=%d\n", filename, actime, modtime, rc);
	return(rc);
	}

    rc = ISC_printf("%d %s %ld %ld", ISS_SETFILETIME, srvfile, actime, modtime);
    if(rc < 0) return(rc);
    return(ISC_read_rc());
}

/* ========================================================================
Set les attributs (mode) du fichier filename selon la définition Unix (p.ex. 0644
pour -rw-r--r--).

Si filename est préfixé par un nom d'hôte suivi d'une porte, suivi du
séparateur ! en DOS, @@ en Unix, l'opération est effectuée via le serveur
s4issrv (scr4_iss en Linux) tournant sur l'hôte.

Par exemple : ~c192.168.2.1:6000!c:\usr\myfile~C pour opérer sur le fichier
distant sur la machine 192.168.2.1 sur laquelle s4issrv est lancé sur la
porte 6000.

S'il n'y a pas de nom de machine, l'opération a lieu sur la machine locale.

&EN filename : nom fichier
&EN attr : attributs du fichier en mode Unix 0644 par exemple pour -rw-r--r--.

L'équivalent le plus proche est utilisé en Windows pour les attributs correspondant en Unix.

Ainsi un fichier Hidden devient --w--w--w-, un fichier READONLY devient -r--r--r--.

&NO Sous Linux ou Unix, il n'est pas possible de changer les attributs d'un
    fichier appartenant à une partition Windows.

&RT 0 en cas de succès, -1 en cas de problème

&RT
&EN -1 : erreur
&EN 0 : accès limité aux fonctions de base
&EN 1 : accès complet
&EN 2 : accès complet avec identification
================================================================== */

ISC_chmod(char *filename, int attr)
{
    int     rc, port;
    char    server[80], srvfile[256];

    // Connecte au serveur
    if(ISC_switch_server(filename)) return(-1);

    // Determine le serveur et la porte
    ISC_split_filename(filename, server, &port, srvfile);
    if(server[0] == 0) {
	return(SCR_chmod(srvfile, attr));
	}

    rc = ISC_printf("%d %s %d", ISS_CHMOD, srvfile, attr);
    if(rc < 0) return(rc);
    return(ISC_read_rc());
}


/* ====================================================================
Full Path : retourne le nom complet d'un fichier à partir d'un nom
relatif. filename contient le nom du fichier, res le résultat.

Si filename est préfixé par un nom d'hôte suivi d'une porte, suivi du
séparateur ! en DOS, @@ en Unix, l'opération est effectuée via le serveur
s4issrv (scr4_iss en Linux) tournant sur l'hôte.

Par exemple : ~c192.168.2.1:6000!c:\usr\myfile~C pour opérer sur le fichier
distant sur la machine 192.168.2.1 sur laquelle s4issrv est lancé sur la
porte 6000.

S'il n'y a pas de nom de machine, l'opération a lieu sur la machine locale.


En cas d'erreur, res est vide et la variable DOS errno indique l'origine
du problème (Drive not ready, etc).

&RT pointeur vers le nom complet du fichier (res) ou NULL en cas d'erreur.
&EX
    Soit le directory courant sur c: \usr
			      sur a: \

    SCR_fullpath("toto", res)     c:\usr\toto
    SCR_fullpath(".\\toto", res)  c:\usr\toto
    SCR_fullpath("./toto", res)   c:\usr\toto
    SCR_fullpath("../toto", res)  c:\toto
    SCR_fullpath("\\toto", res)   c:\toto
    SCR_fullpath("a:toto", res)   a:\toto
       ...
&TX
&SA ISC_dir(), ISC_getcwd(), SCR_relpath()
======================================================================= */

char *ISC_fullpath(filename, res)
char    *filename, *res;
{
    int     rc, port;
    char    server[80], srvfile[256];

    // Connecte au serveur
    if(ISC_switch_server(filename)) return(NULL);

    // Determine le serveur et la porte
    ISC_split_filename(filename, server, &port, srvfile);
    if(server[0] == 0) {
	return(SCR_fullpath(srvfile, res));
	}

    if(ISC_printf("%d %s", ISS_FULLPATH, srvfile)) return(NULL);
    if(ISC_read_rc()) return(NULL);
    if(ISC_read_rec(res)) return(NULL);
    return(res);
}

/* ====================================================================
Retourne dans dirname le nom du directory courant sur la machine où tourne s4issrv.
Drive n'est utilisé qu'en Windows sur la machine locale.

Si dirname est préfixé par un nom d'hôte suivi d'une porte, suivi du
séparateur ! en DOS, @@ en Unix, l'opération est effectuée via le serveur
s4issrv (scr4_iss en Linux) tournant sur l'hôte.

Par exemple : ~c192.168.2.1:6000!c:\usr\myfile~C pour opérer sur le fichier
distant sur la machine 192.168.2.1 sur laquelle s4issrv est lancé sur la
porte 6000.

S'il n'y a pas de nom de machine, l'opération a lieu sur la machine locale.


En cas d'erreur, res est vide et la variable DOS errno indique l'origine
du problème (Drive not ready, etc).

&RT pointeur vers le nom du répertoire courant ou NULL en cas d'erreur.
&TX
&SA ISC_dir(), SCR_getcwd(), SCR_relpath()
======================================================================= */

char *ISC_getcwd(int drive, char *dirname)
{
    int     rc, port;
    char    server[512], srvfile[512];

    // Connecte au serveur
    if(ISC_switch_server(dirname)) return(NULL);

    // Determine le serveur et la porte
    ISC_split_filename(dirname, server, &port, srvfile);
    if(server[0] == 0) {
	return(SCR_getcwd(drive, dirname));
	}

    if(ISC_printf("%d", ISS_GETCWD)) return(NULL);
    if(ISC_read_rc()) return(NULL);
    if(ISC_read_rec(dirname)) return(NULL);
    return(dirname);
}

ISC_setcwd(char *cwd)
{
    S4C     *s4c;

    s4c = ISC_S4C[ISC_check_socket(ISC_SOCKET)];
    strcpy(s4c->cwd, cwd);
    return(ISC_cmd("%d %s", ISS_SETCWD, cwd));
}


/* ========================================================================
Change le nom du fichier Debug.win sur le serveur

&RT 0 en cas de succès, -1 en cas d'erreur.
================================================================== */

ISC_set_debug_name(char *name)
{
    int     rc;
    int     port;

    rc = ISC_printf("%d %s", ISS_SETDEBUGNAME, name);
    if(rc < 0) return(rc);
    return(ISC_read_rc());
}

#ifdef DOS
#define TMPDIRREP       "."
#else
#define TMPDIRREP       "/tmp"
#endif


/* =======================================================================
Remplace les ALIAS dans un fichier
   ex1:TOTO=fnac.evere.xon.be:5555
       SERVEUR=$TOTO
   --> après la fontion
	    TOTO=fnac.evere.xon.be:5555
   ex2:TOTO=$.COMAGA                // Indique qu'il faut aller chercher la variable d'environnement COMAGA
       SERVEUR=$TOTO.xon.be
   --> après la fontion si COMAGA=NIV
	    TOTO=NIV
	    SERVEUR=NIV.xon.be

&RT 0 si tout ok, -1 en cas d'erreur
======================================================================= */


SCR_replace_alias(char *file, U_ch **alias)
{
    FILE    *fd_in, *fd_out;
    int     c, i, j, flag_glob;
    char    buf[512], tmp_file[30], *ptr, *getenv();

//  if(alias == 0) return(0);

    fd_in = ISC_fopen(file, "r");
    if(fd_in == 0) {
	Debug("SCR_replica_alias: file '%s' not found\n", file);
	return(-1);
    }
    sprintf(tmp_file, "%s%c__tmp_replica.tmp", TMPDIRREP, DIR_CHAR);
    fd_out = ISC_fopen(tmp_file, "w+");
    if(fd_out == 0) {
	ISC_fclose(fd_in);
	Debug("SCR_replica_alias: cannot open tempory file '%s'\n", tmp_file);
	return(-1);
    }
    while(1) {
	c = ISC_getc(fd_in);
	if(c == EOF) break;
	if(c == '$') {
	    i = flag_glob = 0;
	    while(1) {
		c = ISC_getc(fd_in);
		if(i == 0 && c == '.') {
		    flag_glob = 1;
		    continue;
		}
		if(U_is_in(c, " \n\t.,;:/\\&\"'()!-?=+[]<>$*@#_") != 0 || c > 126) {
		    ISC_ungetc(c, fd_in);
		    break;
		}
		buf[i++] = c;
	    }
	    buf[i] = 0;
	    if(flag_glob) {
		ptr = getenv(buf);
		if(ptr)
		    fprintf(fd_out, ptr);
		else
		    fprintf(fd_out, "$.%s", buf);
	    }
	    else {
		if(alias) {
		    for(j = 0 ; alias[j] ; j++) {
			if(strncmp(buf, alias[j], strlen(buf)) == 0) {
			    fprintf(fd_out, "%s", alias[j] + strlen(buf) + 1);
			    break;
			}
		    }
		}
		if(alias == 0 || alias[j] == 0)
		    fprintf(fd_out, "$%s", buf);
	    }
	}
	else
	    fputc(c, fd_out);
    }
    ISC_fclose(fd_in);
    fclose(fd_out);
    ISC_unlink(file);
    SCR_rename(tmp_file, file);
    return(0);
}

/* =======================================================================
Remplace les GROUP de la section [REPLICA_GROUP]
   ex:
	[GROUP1]
	MAGA=LIP,WST

	[REPLICA_GROUP1]
	DSN=$MAGA
	DIRECTORY=$LDIR.$MAGA;$ODIR.$MAGA

   --> après la fonction
	[REPLICA_GROUP1]
	DSN=LIP
	DIRECTORY=$LDIR.LIP;$ODIR.LIP

	[REPLICA_GROUP2]
	DSN=WST
	DIRECTORY=$LDIR.WST;$ODIR.WST


&RT 0 si tout ok, -1 en cas d'erreur
======================================================================= */


SCR_replace_group(char *file)
{
    FILE    *fd_in, *fd_out;
    int     c, i, j, k, m, n, nu_repg = 1;
    char    buf[512], buf2[512], topic[255], tmp_file[30], section[255];
    U_ch    ***ptr, **ptr2, **grp, **repg;

    for(n = 0 ; n < 100 ; n++) {
	sprintf(buf, "GROUP%d", n + 1);
	grp = IniReadSection(file, buf);
	if(grp == 0) break;
	sprintf(section, "REPLICA_GROUP%d", n + 1);
	repg = IniReadSection(file, section);
	if(repg == 0) continue;

	fd_in = ISC_fopen(file, "r");
	if(fd_in == 0) {
	    Debug("SCR_replica_group: file '%s' not found\n", file);
	    return(-1);
	}
	sprintf(tmp_file, "%s%c__tmp_replicag.tmp", TMPDIRREP, DIR_CHAR);
	fd_out = fopen(tmp_file, "w+");
	if(fd_out == 0) {
	    ISC_fclose(fd_in);
	    Debug("SCR_replica_alias: cannot open tempory file '%s'\n", tmp_file);
	    return(-1);
	}
	while(1) {
	    i = SCR_read_line(fd_in, buf, 512);
	    if(i < 0) break;
	    if(U_index(buf, section) >= 0) {
		break;
	    }
	    fwrite(buf, i, 1, fd_out);
	    fputc('\n', fd_out);
	}
	if(i < 0) {
	    ISC_fclose(fd_in);
	    fclose(fd_out);
	    return(0);
	}

	ptr =  (U_ch ***)SCR_malloc((SCR_tbl_size(grp) + 1) * sizeof(char **));
	ptr2 = (U_ch **)SCR_malloc((SCR_tbl_size(grp) + 1) * sizeof(char *));
	for(i = 0 ; grp[i] ; i++) {
	    j = U_index(grp[i], "=");
	    if(j < 0) continue;
	    ptr2[i] = SCR_malloc(j + 1);
	    strncpy(ptr2[i], grp[i], j);
	    ptr[i] = SCR_vtoms(grp[i] + j + 1, "|");
	}

	for(i = 0 ; i < SCR_tbl_size(ptr[0]) ; i++) {
	    fprintf(fd_out, "[REPLICA_%d]\n", nu_repg++);
	    for(j = 0 ; repg[j] ; j++) {
		strcpy(buf2, repg[j]);
		for(k = 0 ; ptr[k] ; k++) {
		    sprintf(topic, "$%s", ptr2[k]);
		    if(U_index(buf2, topic) >= 0)
			SCR_replace(buf2, topic, ptr[k][i]);
		}
		fprintf(fd_out, "%s\n", buf2);
	    }
	    fputc('\n', fd_out);
	}
	while(1) {
	    i = SCR_read_line(fd_in, buf, 512);
	    if(i < 0) break;
	    if(buf[0] == '[') {
		fprintf(fd_out, "%s\n", buf);
		break;
	    }
	}

	for(i = 0 ; grp[i] ; i++)
	    SCR_free_tbl(ptr[i]);
	SCR_free(ptr);
	SCR_free(ptr2);
	while(1) {
	    c = ISC_getc(fd_in);
	    if(c == EOF) break;
	    fputc(c, fd_out);
	}
	ISC_fclose(fd_in);
	fclose(fd_out);
	ISC_unlink(file);
	SCR_rename(tmp_file, file);
    }
    return(0);
}

/* =======================================================================
Remplace un text pour un autre dans un fichier
   ex:
	SCR_replica_txt_file("toto.txt", "Hello", "Bonjour");

&RT 0 si tout ok, -1 en cas d'erreur
======================================================================= */


SCR_replace_txt_file(char *file, char *from, char *to)
{
    FILE    *fd_in, *fd_out;
    int     i;
    char    buf[512], tmp_file[30];

    fd_in = ISC_fopen(file, "r");
    if(fd_in == 0) {
	Debug("SCR_replica_txt_file: file '%s' not found\n", file);
	return(-1);
    }

    sprintf(tmp_file, "%s%c__tmp_reptxt.tmp", TMPDIRREP, DIR_CHAR);
    fd_out = fopen(tmp_file, "w+");
    if(fd_out == 0) {
	ISC_fclose(fd_in);
	Debug("SCR_replica_txt_file : cannot open tempory file '%s'\n", tmp_file);
	return(-1);
    }
    while(1) {
	i = SCR_read_line(fd_in, buf, 512);
	if(i < 0) break;
	SCR_replace(buf, from, to);
	fwrite(buf, strlen(buf), 1, fd_out);
	fputc('\n', fd_out);
    }
    ISC_fclose(fd_in);
    fclose(fd_out);
    ISC_unlink(file);
    SCR_rename(tmp_file, file);
    return(0);
}

// déplacement de fonctions à vérifier

IS_end_database_1(int i)
{
    int     rc;
    int     j;
    FILE_TYPE   *flt;
    ISAM    *is;


    ISC_SOCKET = ISC_S4C[i]->socket;
    rc = ISC_printf("%d", ISS_ENDDB);
    if(rc) return(rc);
    ISC_outbuf_flush();
    WSockClose(ISC_SOCKET);
    rc = ISC_SOCKET;
    ISC_SOCKET = 0; /* BP_M 23-10-2007 */

    ISC_S4C[i]->port = 0;
    SCR_free(ISC_S4C[i]->srv);
    ISC_S4C[i]->srv = 0;
    ISC_S4C[i]->socket = 0;

    for(j = 1 ; j < SCR_READ_FILES.tbl_nb_fl ; j++) {
	flt = SCR_READ_FILES.tbl_fl[j];
	if(TYPE16(flt->fl_type) == SCR_TYPE_ISAM) {
	    is = (ISAM *)flt->fl_ptr;
	    if(is->is_socket == rc) {
		is->is_socket = 0;
		is->is_s4srv = 0;       /* BP_M 25-01-2011 16:04 */
		is->is_s4srv_port = 0;  /* BP_M 25-01-2011 16:04 */
	    }
	}
    }
    return(0);
}

ISC_close_all_connect()
{
    int     i;

    for(i = 0 ; i < ISC_NB_CONN ; i++)
	IS_end_database_1(i);
    ISC_NB_CONN = 0;
}

ISC_StartAllTimer()
{
    int     i;

    for(i = 0 ; i < ISC_NB_CONN ; i++)  /* BP_M 10-12-2010 09:56 */
	ISC_starttimer(ISC_S4C[i]->socket);
}

ISC_StopAllTimer()
{
    int     i;

    for(i = 0 ; i < ISC_NB_CONN ; i++)  /* BP_M 10-12-2010 09:56 */
	ISC_stoptimer(ISC_S4C[i]->socket);
}


ISC_starttimer(int socket)
{
    int     rc, t_socket = ISC_SOCKET;

    ISC_SOCKET = socket;
    rc = ISC_printf("%d", ISS_SETTIMER);
    if(rc < 0) {
	ISC_SOCKET = t_socket;
	return(rc);
    }
    rc = ISC_read_rc();
    ISC_SOCKET = t_socket;
    return(rc);
}

ISC_stoptimer(int socket)
{
    int     rc, t_socket = ISC_SOCKET;

    ISC_SOCKET = socket;
    if(ISC_printf("%d", ISS_STOPTIMER) < 0) {
	ISC_SOCKET = t_socket;
	return(-1);
    }
    rc = ISC_read_rc();
    ISC_SOCKET = t_socket;
    return(rc);
}






