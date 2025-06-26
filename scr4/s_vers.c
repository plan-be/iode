#include "scr.h"

#ifdef PDEV
#include "s_prot.h"         /* BP_M 23-10-95 */
#endif

U_ch    SCR_magic[] = SCR_VERSION;
long    SCR_libsn = 942L;
int     SCR_vers_done = 0;

/* ==================================================================
	SN:001 = Librairie générale SCR4C
   ================================================================== */

SCR_version()
{
/*    char    *getenv(); /* JMP 06-01-98 */

#ifdef PDEV
    if(SCR_CURRENT_NMS == 0L)
	SCR_CURRENT_NMS = PG_ip(JMPROT);
    if(PG_pgvr()) {
	exit(0);
	}
    if(SCR_vers_done) return(0);
    if(getenv("SCR_BANNER")) {
	SCR_vers_done = 1;
	return(0);
	}

    Wprintf("\nAtelier De Developpement %s\n", SCR_VERSION);
    Wprintf("%s", SCR_COPYRIGHT);

#ifdef X11
    Wprintf(" - X11\n");
#else
 #ifdef SCRGNOME
    Wprintf(" - Gnome-Gtk+\n");
 #else
    Wprintf("\n");
 #endif
#endif

    Wprintf("Application realisee par %s\n",   SCR_CURRENT_SCT);
    Wprintf("Numero de serie : %s\n\n",      SCR_CURRENT_NMS);
#else
    if(SCR_vers_done) return(0);
    if(getenv("SCR_BANNER")) {
	SCR_vers_done = 1;
	return(0);
	}

    Wprintf("%s - %s (SN:%03ld)\n", SCR_magic, SCR_COPYRIGHT, SCR_libsn);
#endif
    SCR_vers_done = 1;
    W_PRINTF_USED = 0;
    return(0);
}



