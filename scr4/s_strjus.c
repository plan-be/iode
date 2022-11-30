#include "s_strs.h"

/* =+====== U_rjust_text =============================================
Justifie un texte � droite : d�place dans le string les caract�res �
partir du premier non espace de telle sorte que le dernier caract�re ne
soit pas un espace. La longueur du string n'est pas modifi�e.

&RT un pointeur vers le string justifi�
&EX
    printf("'%s'\n", U_rjust_text(" Jean-Marc "));

	vaut
    '  Jean-Marc'
&TX
&SA U_ljust_text()
====================================================================== */

unsigned char *U_rjust_text(text)
unsigned char *text;
{
    int     nb_bl;
    int     lg;

    lg = (int)strlen(text);
    nb_bl = lg - 1;
    //while( (nb_bl >= 0) && (text[nb_bl] == ' ')) nb_bl--;
    while(nb_bl >= 0 && SCR_is_space(text[nb_bl])) nb_bl--; /* JMP 16-04-2013 */
    nb_bl = nb_bl + 1 - lg;
    U_shift_text(text, nb_bl);
    return(text);
}

