#include "s_strs.h"

/* =+=================================================================
D‚place le texte de n caractŠres … l'int‚rieur du string lui-mˆme.
La longueur du string n'est pas modifi‚e. Une valeur positive de n
indique un d‚placement vers la gauche, une valeur positive vers la
droite. Les caractŠres en exc‚dent sont perdus. Des blancs sont ajout‚s
en fin (n > 0) ou en d‚but (n < 0) pour compl‚ter le string.

&RT un pointeur vers le string modifi‚
&EX
    printf("'%s'\n", U_shift_text("Jean-Marc  ", 2));
    printf("'%s'\n", U_shift_text("Jean-Marc  ", -2));

	vaut

    'an-Marc    '
    '  Jean-Marc'
&TX
&SA U_rjust_text(), U_ljust_text(), U_center_text()
====================================================================== */

unsigned char *U_shift_text(text, n)
unsigned char *text;
int     n;
{
    int     i;
    int     lg;

    if(n == 0) return(text);
    lg = (int)strlen(text);
    if(n > 0)
    {
        for(i = 0; i < lg-n; i++) text[i] = text[i+n];
        for(i = lg - n ; i < lg ; i++) text[i] = ' ';
    }
    else
    {
        for(i = lg + n - 1 ; i >= 0; i--) text[i-n] = text[i];
        for(i = 0 ; i < (-n) ; i++) text[i] = ' ';
    }
    return(text);
}

