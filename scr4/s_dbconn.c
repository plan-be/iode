#include "scr4.h"

S4C     **ISC_S4C;  /* BP_M 04-02-2008 */
int     ISC_NB_CONN; /* Nombre de connxion S4C */
char    ISC_CWD[SCR_MAX_FILE_LG_MAX + 1];   /* BP_M 09-10-2012 11:20 */

extern int ISC_SOCKET;

/* Initialise pour une session ISAM */
S4C     *ISC_CURRENT_CONN;

char *ISC_get_ip(char *srv)
{
    int     ip[4];
    static char buf[50];


    if(WSockGetIp(srv, ip) == 0) {
	sprintf(buf, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
	return(buf);
    }
    return((char *)0);
}

/* BP_M 17-12-2007 */

ISC_check_connect(srv, port)
char    *srv;
int     port;
{
    int     i;
    char    *tmp;

    tmp = ISC_get_ip(srv);
    if(tmp) srv = tmp;

    i = ISC_check_exist(srv, port);
    if(i >= 0) {
	ISC_SOCKET = ISC_S4C[i]->socket;
	ISC_CURRENT_CONN = ISC_S4C[i];
	return(ISC_SOCKET);
    }

    i = ISC_init2(srv, port);

    if(i >= 0) {
	ISC_SOCKET = i;
	ISC_add_srv(ISC_SOCKET, srv, port);
	return(ISC_SOCKET);
    }
    else
	return(-1);
}


ISC_check_exist(srv, port)
char    *srv;
int     port;
{
    int     i;
    char    *tmp;

    tmp = ISC_get_ip(srv);
    if(tmp) srv = tmp;

    for(i = 0 ; i < ISC_NB_CONN ; i++)
	if(ISC_S4C[i]->srv && strcmp(srv, ISC_S4C[i]->srv) == 0 && port == ISC_S4C[i]->port)
	    return(i);
    return(-1);
}

ISC_check_socket(int socket)
{
    int     i;

    for(i = 0 ; i < ISC_NB_CONN ; i++)
	if(ISC_S4C[i]->srv && ISC_S4C[i]->socket == socket)
	    break;
    if(i == ISC_NB_CONN) i = -1;

    return(i);
}

ISC_add_srv(int conn, char *srv, int port)
{
    char    *tmp;
    int     i = ISC_NB_CONN;

    if(ISC_check_socket(conn) >= 0) {
	ISC_SOCKET = conn;
	return(0);
    }
    tmp = ISC_get_ip(srv);
    if(tmp) srv = tmp;

    ISC_S4C = (S4C **)SCR_realloc(ISC_S4C, sizeof(S4C *), ISC_NB_CONN, ISC_NB_CONN + 1);
    ISC_S4C[i] = (S4C *)SCR_malloc(sizeof(S4C));
    ISC_S4C[i]->srv  = SCR_stracpy(srv);
    ISC_S4C[i]->port = port;
    ISC_S4C[i]->socket = conn;

    ISC_CURRENT_CONN = ISC_S4C[i];
    ISC_NB_CONN++;
    ISC_SOCKET = conn;
    return(0);
}

ISC_sup_srv(int nb)
{
    if(nb >= ISC_NB_CONN || nb < 0) return(0);

    SCR_free(ISC_S4C[nb]->srv);
    SCR_free(ISC_S4C[nb]->dsn);
    SCR_free(ISC_S4C[nb]);

    memcpy(ISC_S4C + nb, ISC_S4C + nb + 1, sizeof(S4C *) * (ISC_NB_CONN - nb - 1));

    ISC_NB_CONN--;

    return(0);
}


/* BP_M 16-10-2008 19:20 */ /* BP_M 16-10-2008 19:20 */ /* BP_M 16-10-2008 19:20 */
SCR_find_alias(char *srv, char *res, long *port)
{
    char    buf[1024], *ptrh, *ptr;
    U_ch    **tbl, **ptr2, **ip;
    long    atol();

    if(SCR_ISC_ALIAS == 0) {
	ptr = getenv("SCR_ISC_ALIAS");
	if(!ptr) {
#ifdef LINUX
	    ptrh = getenv("HOME");
	    if(!ptrh) ptrh = "/home/paul";
	    sprintf(buf, "%s/aliasips4.ini", ptrh);
	    SCR_ISC_ALIAS = SCR_stracpy(buf);
#else
	    SCR_ISC_ALIAS = SCR_stracpy("c:\\scr\\aliasips4.ini");
#endif
	}
	else
	    SCR_ISC_ALIAS = SCR_stracpy(ptr);
//      Debug("SCR_ISC_ALIAS set to '%s'\n", SCR_ISC_ALIAS);
    }
//  Debug("-- Recherche de '%s' dans '%s' ", srv, SCR_ISC_ALIAS);
/*

    ip = SCR_vtoms(srv, ".");
    if(ip && SCR_tbl_size(ip) == 4) {
	strcpy(res, srv);
	SCR_free_tbl(ip);
	return(-1);
    }
    if(ip) SCR_free_tbl(ip);
*/
    if(IniReadTxtParm(SCR_ISC_ALIAS, "IP", srv, buf, 254) == 0) {
//      Debug("= %s\n", buf);
	if(buf[0] == '!') {
	    ptr2 = SCR_vtoms(buf + 1, ",");
	    if(ptr2) {
		if(SCR_tbl_size(ptr2) >= 3) {
		    SCR_upper(ptr2[1]);
		    SCR_upper(ptr2[2]);
		    if(IniReadTxtParm(ptr2[0], ptr2[1], ptr2[2], buf, 254) == 0) {
			if(ptr2[3])
			    *port = atol(ptr2[3]);
			else
			    *port = SCR_ISC_PORT;
			strcpy(res, buf);
			SCR_free_tbl(ptr2);
			return(0);
		    }
		}
	    }
	    return(-1);
	}
	tbl = SCR_vtom(buf, ':');
	if(tbl) {
	    strcpy(res, tbl[0]);
	    if(tbl[1])
		*port = atol(tbl[1]);
	    else
		*port = SCR_ISC_PORT;
	    SCR_free_tbl(tbl);
	    return(0);
	}
    }
//  Debug("non trouve\n");
    return(-1);
}

/*NH BP_M 27-05-2010 12:23 */
SCR_find_alias_db(char *adb, char *res)
{
    char    buf[255], *ptrh, *ptr;

    if(SCR_ISC_ALIAS == 0) {
	ptr = getenv("SCR_ISC_ALIAS");
	if(!ptr) {
#ifdef LINUX
	    ptrh = getenv("HOME");
	    if(!ptrh) ptrh = "/home/paul";
	    sprintf(buf, "%s/aliasips4.ini", ptrh);
	    SCR_ISC_ALIAS = SCR_stracpy(buf);
#else
	    SCR_ISC_ALIAS = SCR_stracpy("c:\\scr\\aliasips4.ini");
#endif
	}
	else
	    SCR_ISC_ALIAS = SCR_stracpy(ptr);
//      Debug("SCR_ISC_ALIAS set to '%s'\n", SCR_ISC_ALIAS);
    }
//  Debug("-- Recherche de '%s' dans '%s' ", srv, SCR_ISC_ALIAS);
    if(IniReadTxtParm(SCR_ISC_ALIAS, "DB", adb, buf, 254) == 0) {
//      Debug("= %s\n", buf);
	strcpy(res, buf);
	return(0);
    }
    else
	strcpy(res, adb);
//  Debug("non trouve\n");

    return(-1);
}
/* BP_M 27-05-2010 12:23 */ /* BP_M 27-05-2010 12:23 */ /* BP_M 27-05-2010 12:23 */


