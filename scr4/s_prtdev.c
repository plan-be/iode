#include "scr.h"

/* ======================================================================
Cette fonction modifie le nom (name) et le type (FILE, PRINTER ou
DIRECT) du device ou du fichier associé à un PRINTER. Le nom est strippé
par la fonction et remplace l'ancien nom. Elle se comporte différemment
en DOS et en UNIX : en UNIX, s'il s'agit d'un printer, il est replacé
par "lp -s -dname", ce qui évite à l'utilisateur de connaître la syntaxe
de lp.

&EN type définit la destination
&EN2 0 = FILE
&EN2 1 = PRINTER
&EN2 2 = DIRECT (uniquement sous Windows)

&NO Le nom (éventuellement précédé de "lp -s -d") ne peut dépasser la
    zone associée au nom dans la définition initiale du PRINTER (FILE
    "tmp" ne suffit pas en général car il n'y a que trois caractères
    disponibles).

&EX
     PR_set_device(my_pr, 1, "lw1");

     PR_select(my_pr);
     ...
     PR_send();
&TX
&SA PR_select(), PR_set_size(), PR_set_name()
------------------------------------------------------------------------ */
PR_set_device(pr, type, name)
PR_DEF  *pr;
int     type;
char    *name;
{
    pr->pr_type = "FPD"[type];
    strcpy(pr->pr_name, name);
#ifdef UNIX
    if(type == 1) sprintf(pr->pr_name, "lp -s -d%s", name);
#endif
    SCR_strip(pr->pr_name);

    return(0);
}
