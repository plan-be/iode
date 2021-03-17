#include "s_yy.h"

/* ====================================================================
Fonction de debugging d‚clench‚e par la directive ##debug dans un
programme analys‚ par YY_read(), YY_lex() ou YY_lex2().

Cette fonction peut ˆtre remplac‚e par une fonction utilisateur. Elle
est appel‚e avec pour argument le pointeur vers le fichier ouvert
(YYFILE *).

Par d‚faut, elle affiche le nom du fichier, la ligne courante suivis du
message debug et de la m‚moire disponible.
======================================================================= */


int YY_debug(yy)
YYFILE  *yy;
{
    fprintf(stderr, "%s : debug", YY_error(yy));
#ifdef DOS
    {
	extern long SCR_free_mem();
	fprintf(stderr, " - core=%ld", SCR_free_mem());
    }
#endif
    fprintf(stderr, "\n");
    return(0);
}



