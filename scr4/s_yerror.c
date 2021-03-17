#include "s_ydvlp.h"

/* ====================================================================
Retourne un string d‚crivant la position actuelle de la lecture dans le
fichier ou dans le string interpr‚t‚. Le message retourn‚ se pr‚sente
diff‚remment suivant le type de texte lu :

&EN fichier : filename[line; col]
&EN string  : (string)[pos]
&EN stdin   : (stdin)[line;col]

&RT le pointeur vers un buffer statique contenant le message
&EX
    printf("%s : syntax error\n", YY_error(yy));
&TX
=======================================================================*/

char *YY_error(yy)
YYFILE  *yy;
{
    static char ptr[81];

    switch(yy->yy_input_type) {
	case YY_FILE :
	    sprintf(ptr, "%s[%d;%d]", yy->yy_name, yy->yy_line, yy->yy_col);
	    break;
	case YY_MEM :
	    sprintf(ptr, "(string)[%d]", yy->yy_cpos);
	    break;
	case YY_STDIN :
	    sprintf(ptr, "(stdin)[%d;%d]", yy->yy_line, yy->yy_col);
	    break;
	}
    return(ptr);
}

