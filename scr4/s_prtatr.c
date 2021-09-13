#include "scr.h"

/* ======================================================================
Cette fonction indique que la suite de l'impression doit ˆtre faite en
BOLD. Les ESCAPE SEQUENCES correspondant aux diff‚rents attributs sont
d‚finis dans le PRINTER.

Le BOLD reste actif jusqu'au changement d'attribut par une des commandes
UNDERLINE, ENLARGED, NORMAL ou CONDENSED. L'attribut est
automatiquement chang‚ lors de l'impression d'une PAGE contenant des
champs aux attributs particuliers (BOLD, UNDERLINE, ...).

&SA PR_enlarged(), PR_condensed(), PR_underline(), PR_normal()
------------------------------------------------------------------------ */

PR_bold()
{
    PR_CURRENT->pr_attr = SCR_BOLD;
    return(0);
}

/* ======================================================================
Cette fonction indique que la suite de l'impression doit ˆtre faite en
caractŠres ‚largis. Les ESCAPE SEQUENCES correspondant aux diff‚rents
attributs sont d‚finis dans le PRINTER.

Le ENLARGED reste actif jusqu'au changement d'attribut par une des
commandes UNDERLINE, BOLD, ENLARGED, NORMAL ou CONDENSED.
L'attribut est automatiquement chang‚ lors de l'impression d'une PAGE
contenant des champs aux attributs particuliers (BOLD, UNDERLINE, ...).

&SA PR_bold(), PR_condensed(), PR_underline(), PR_normal()
------------------------------------------------------------------------ */
PR_enlarged()
{
    PR_CURRENT->pr_attr = SCR_ENLARGED;
    return(0);
}

/* ======================================================================
Cette fonction indique que la suite de l'impression doit ˆtre faite en
caractŠres condens‚s. Les ESCAPE SEQUENCES correspondant aux diff‚rents
attributs sont d‚finis dans le PRINTER.

Le CONDENSED reste actif jusqu'au changement d'attribut par une des
commandes UNDERLINE, BOLD, ENLARGED, NORMAL ou CONDENSED.
L'attribut est automatiquement chang‚ lors de l'impression d'une PAGE
contenant des champs aux attributs particuliers (BOLD, UNDERLINE, ...).

&SA PR_bold(), PR_enlarged(), PR_underline(), PR_normal()
------------------------------------------------------------------------ */
PR_condensed()
{
    PR_CURRENT->pr_attr = SCR_CONDENSED;
    return(0);
}
/* ======================================================================
Cette fonction indique que la suite de l'impression doit ˆtre faite en
caractŠres soulign‚s. Les ESCAPE SEQUENCES correspondant aux diff‚rents
attributs sont d‚finis dans le PRINTER.

Le soulignement reste actif jusqu'au changement d'attribut par une des
commandes BOLD, ENLARGED, NORMAL ou CONDENSED. L'attribut est
automatiquement chang‚ lors de l'impression d'une PAGE contenant des
champs aux attributs particuliers (BOLD, UNDERLINE, ...).

&SA PR_bold(), PR_enlarged(), PR_condensed(), PR_normal()
------------------------------------------------------------------------ */
PR_underline()
{
    PR_CURRENT->pr_attr = SCR_UNDERLINE;
    return(0);
}

/* ======================================================================
Cette fonction indique que la suite de l'impression doit ˆtre faite en
caractŠres normaux. Les ESCAPE SEQUENCES correspondant aux diff‚rents
attributs sont d‚finis dans le PRINTER.

Les caractŠres reste normaux jusqu'au changement d'attribut par une des
commandes UNDERLINE, BOLD, ENLARGED, NORMAL ou CONDENSED.
L'attribut est automatiquement chang‚ lors de l'impression d'une PAGE
contenant des champs aux attributs particuliers (BOLD, UNDERLINE, ...).

&SA PR_bold(), PR_enlarged(), PR_condensed(), PR_underline()
------------------------------------------------------------------------ */
PR_normal()
{
    PR_CURRENT->pr_attr = SCR_DEFAULT;
    return(0);
}

