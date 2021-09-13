#include "scr.h"

/* ======================================================================
Cette fonction modifie le nom (name) et le type (FILE, PRINTER ou
DIRECT) du device ou du fichier associ‚ … un PRINTER. Le nom est stripp‚
par la fonction et remplace l'ancien nom. Elle se comporte diff‚remment
en DOS et en UNIX : en UNIX, s'il s'agit d'un printer, il est replac‚
par "lp -s -dname", ce qui ‚vite … l'utilisateur de connaŒtre la syntaxe
de lp.

&EN type d‚finit la destination
&EN2 0 = FILE
&EN2 1 = PRINTER
&EN2 2 = DIRECT (uniquement sous Windows)

&NO Le nom (‚ventuellement pr‚c‚d‚ de "lp -s -d") ne peut d‚passer la
    zone associ‚e au nom dans la d‚finition initiale du PRINTER (FILE
    "tmp" ne suffit pas en g‚n‚ral car il n'y a que trois caractŠres
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
