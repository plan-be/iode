#include <stdio.h>

int SW_abort(code, text)
int     code;
char    *text;
{
    kmsg("Swap Panic. Attempting to save ws...");
#if defined(DOSWIN) || defined(SCRW32) /* JMP 16-02-97 */
    PG_display_error("Swap Panic (%s).\nData saved in ws.* files\n", text); /* JMP 16-02-97 */
#else
    Wprintf("\nSwap Panic (%s).\nData saved in ws.* files\n", text); /* JMP 16-02-97 */
    Wprintf("To restart iode with the last data, type :\n");         /* JMP 16-02-97 */
    Wprintf("        iode -ws\n\n");                                 /* JMP 16-02-97 */
#endif
    K_end_ws(1);
    ODE_end(1);
    exit(code);
    return(0);
}

int SCR_panic()
{
    SW_abort(-1, "memory full"); /* JMP 16-02-97 */
    return(0);
}


