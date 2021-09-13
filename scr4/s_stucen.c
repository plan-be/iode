#include "s_strs.h"
/* =+=================================================================
Centre un string sur lui-mˆme en ‚quilibrant le nombre de blancs
… gauche et … droite. En cas de nombre impair de blancs … r‚partir, il
y aura un blanc de plus … droite qu'… gauche.

La longueur du string n'est pas modifi‚e.

&RT un pointeur vers le string modifi‚
&EX
    printf("'%s'\n", U_center_text("Jean-Marc   "));

	vaut

    ' Jean-Marc  '
&TX
&SA U_rjust_text(), U_ljust_text(), U_ljust_text()
====================================================================== */

unsigned char *U_center_text(text)
unsigned char *text;
{
    int     beg_bl = 0;
    int     lg;

    lg = strlen(text);
    U_rjust_text(text);

    while( (beg_bl < lg) && (text[beg_bl] == ' ')) beg_bl++;
    if(beg_bl == lg) return(text);

    beg_bl = (1 + beg_bl)/2;
    U_shift_text(text, beg_bl);
    return(text);
}

