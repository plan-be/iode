#include "s_strs.h"

/* =+=================================================================
Déplace le texte de n caractères à l'intérieur du string lui-même.
La longueur du string n'est pas modifiée. Une valeur positive de n
indique un déplacement vers la gauche, une valeur positive vers la
droite. Les caractères en excédent sont perdus. Des blancs sont ajoutés
en fin (n > 0) ou en début (n < 0) pour compléter le string.

&RT un pointeur vers le string modifié
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

