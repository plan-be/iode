#include "scr4w.h"
#include "scr4_srv.h"

char serveur[] = "ser.test.be";
long portserv = 5555;
int  SCR_SEND_INFO;


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



