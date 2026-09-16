#include "s_cpp.h"

/* ================================================================
Fonction appelée par les fonctions Cpp*() en cas d'erreur. Cette
fonction affiche par défaut dans le stdout (printf) le fichier, la ligne
et le caractère courants, suivi du message msg.

Elle peut (doit) être remplacée par le programmeur en fonction de
l'environnement de programmation (Windows, ...).

&EN cpp = pointeur retourné par CppOpen()
&EN msg = message d'erreur

&SA CppOpen(), CppClose()
================================================================ */

int CppPrintError(cpp, msg)
CPPFILE *cpp;
char    *msg;
{
    printf("%s: %s\n", CppError(cpp), msg);
    return(0);
}



