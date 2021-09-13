#include <stdio.h>

#ifdef SCRWIN
#include "scr4w.h"
#endif

#ifdef SCRCPP  /* JMP 07-01-98 */
    extern "C" {
    void exit(int);
}
#endif

/*==================================================================
Fonction appel‚e en cas d'erreur de SCR_malloc() ou SCR_realloc().

Cette fonction peut ˆtre replac‚e par une fonction utilisateur,
permettant par exemple de sauver le travail en cours avant de
quitter le systŠme.

La fonction de librairie effectue les op‚rations suivantes :

&CO
    fprintf(stderr, "\n\nMemory full. Exit(2)\n");
    exit(2);
&TX
&SA SCR_malloc(), SCR_realloc()
=============================================================== */

int SCR_panic()
{
#ifdef SCRWIN
    MessageBox(0, "Memory full.", "Fatal error", MB_OK | MB_ICONSTOP | MB_APPLMODAL);
#else
    fprintf(stderr, "\n\nMemory full. Exit(2)\n");
#endif
    exit(2);
    return(0);
}
