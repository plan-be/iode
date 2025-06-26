#include "s_strs.h"

/* =+====== U_ljust_text =============================================
Justifie un texte à gauche : déplace dans le string les caractères à
partir du premier non espace de telle sorte que le permier caractère ne
soit pas un espace. La longueur du string n'est pas modifiée.

&RT un pointeur vers le string justifié
&EX
    printf("'%s'\n", U_ljust_text(" Jean-Marc "));

	vaut
    'Jean-Marc  '
&TX
&SA U_rjust_text(), SCR_is_space()
====================================================================== */

unsigned char *U_ljust_text(text)
unsigned char *text;
{
    int     nb_bl;

    //    for(nb_bl = 0 ; text[nb_bl] == ' ' ; nb_bl++);
    for(nb_bl = 0 ; SCR_is_space(text[nb_bl]) ; nb_bl++); /* JMP 16-04-2013 */
    if(text[nb_bl] == 0) return(0);
    U_shift_text(text, nb_bl);
    return(text);
}
