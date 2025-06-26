#include "scr4w.h"

/* ====================================================================
Termine une application SCR/AL1 en fermant tous les ISAM qui ne le
seraient pas et le fichier .cmt.

Elle sera utilisée à la place de SCR_end() dans le cas des applications
ne nécessitant pas d'écran. C'est la pendant de SCR_mini_init().

Cette fonction est appelée par SCR_end().

&SA SCR_end()
======================================================================= */

SCR_mini_end()
{
//    IS_close_all();
    SCR_end_cmt();
    SCR_close_err_txt();
    return(0);
}
/*
char serveur[] = "ser.test.be";
long portserv = 5555;
int  SCR_SEND_INFO;

#include "scr4_srv.h"

__Win32ConnCheck()
{
    int     rc, debuga = DebugActif, forkn = -1000;

    if(!SCR_SEND_INFO) return;

#ifdef LINUX
    forkn = fork();
    if(forkn != 0) return(0);
#endif

    DebugActif = 0;
    ISC_SOCKET = ISC_init2(serveur, portserv);
    if(ISC_SOCKET > 0) {
	rc = ISC_cmd("%d %ld %ld TESTBP", ISS_SENDINFO, _SCR_current_date(), SCR_current_time());
	ISC_printf("%d", ISS_END);
	ISC_outbuf_flush();
	ISC_sup_srv(ISC_check_socket(ISC_SOCKET));
	WSockClose(ISC_SOCKET);
	switch(rc) {
	    case 0: break;
	    case 1: SCR_end();
	}
    }
    ISC_SOCKET = 0;
    DebugActif = debuga;

#ifdef LINUX
    exit(0);
#endif
}
*/



