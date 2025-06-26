#include "s_args.h"

/* ================================================================
    Affiche tableau sur base de la structure ADEF. Le titre du
    tableau est passé comme premier argument à la fonction.

    Si le tableau dépasse 20 lignes, le programme affiche le
    message
&CO
	printf("Press ENTER to continue");
&TX
    après 20 lignes et reprend après réception de la touche ENTER.
=================================================================== */

int A_usage(title, adef)
char    *title;
ADEF    *adef;
{
    int i;

    printf(title);
    printf("\nOPTION   ARGUMENTS      REQ DEFAULT   DESCRIPTION\n");
    printf(  "------   ---------      --- -------   -----------\n");
    for(i = 0; adef[i].a_arg != NULL; i++) {
	if(adef[i].a_arg[0] == '\0') continue;
	if(i % 20 == 0 && i != 0) {
	    printf("Press ENTER to continue");
	    getchar();
	    printf("\r");
	    }
	printf("%-9s%-15s %s  %-10s%s\n",
		adef[i].a_arg,
		(adef[i].a_args == 0) ? ""  : adef[i].a_args,
		(adef[i].a_req  == 0) ? "N" : "Y",
		(adef[i].a_dft  == 0) ? ""  : adef[i].a_dft,
		(adef[i].a_desc == 0) ? ""  : adef[i].a_desc);
	}
    return(0);
}

