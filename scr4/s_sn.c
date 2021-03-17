#include "scr.h"

int  SCR_sn_done = 0;

/* ==================================================================
     SN : Affichage du nom du programme, du client et du serial nb
   ================================================================== */

SCR_sn()
{
    char        buf[15];

    if(SCR_sn_done) return(0);
    SCR_sn_done = 1;
    if(SCR_applname == 0 && SCR_username == 0) return(0);
#if !defined(WINDOWS) && !defined(DOSWIN)
    Wprintf("\n");
    if(SCR_applname) {
	SCR_long_to_date(SCR_crdate, buf);
	SCR_pad_center_print("%s [%s]", SCR_applname, buf);
	}

    if(SCR_username)
	SCR_pad_center_print("Licence delivered to %s", SCR_username, 0L);

    if(SCR_serialnb)
	SCR_pad_center_print("Serial number : %s", SCR_serialnb, 0L);

    Wprintf("\n");
#endif
    return(0);
}

#if !defined(WINDOWS) && !defined(DOSWIN)
SCR_pad_center_print(fmt, a ,b)      /* JMP 29-12-96 */
char    *fmt, *a, *b;              /* JMP 29-12-96 */
{
    char    buf2[256];

    sprintf(buf2, fmt, a, b);        /* JMP 29-12-96 */
    if(!U_is_in('\n', buf2)) {
	SCR_pad(buf2, 79);
	U_center_text(buf2);
	Wprintf("%s\n", buf2);
	}
    else
	Wprintf("%s", buf2);

    return(0);
}
#endif




