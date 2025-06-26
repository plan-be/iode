#include "scr.h"

#define LINE_BLOCK 20


/* ====================================================================
Retourne un attribut (en principe) inverse de celui passé comme
argument. Ainsi, REVERSE devient BOLD, et tout autre attribut devient
REVERSE.

&RT L'attribut inversé
&SA SCR_dim_attr()
=======================================================================*/
SCR_invert_attr(attr)
int     attr;
{
    switch(attr) {
	case SCR_CYAN        :          /*JMP 5/10/93 */
	case SCR_REVERSE     : return(SCR_BOLD);
	default              : return(SCR_REVERSE);
	}
}

/* ====================================================================
Fonction de modification d'attribut (diminution d'intensité) :

&EN SCR_REVERSE devient SCR_YELLOW
&EN SCR_YELLOW  devient SCR_REVERSE
&EN SCR_BOLD    devient SCR_DEFAULT
&EN les autres deviennent SCR_BOLD

&RT L'attribut diminué
&SA SCR_invert_attr()
=======================================================================*/

SCR_dim_attr(attr)
int     attr;
{
    switch(attr) {
	case SCR_REVERSE     : return(SCR_YELLOW);
	case SCR_YELLOW      : return(SCR_REVERSE);
	case SCR_BOLD        : return(SCR_DEFAULT);
	default              : return(SCR_BOLD);
	}
}

