#include "s_cpp.h"

/* ================================================================
Fonction appel‚e par les fonctions Cpp*() en cas d'erreur. Cette
fonction affiche par d‚faut dans le stdout (printf) le fichier, la ligne
et le caractŠre courants, suivi du message msg.

Elle peut (doit) ˆtre remplac‚e par le programmeur en fonction de
l'environnement de programmation (Windows, ...).

&EN cpp = pointeur retourn‚ par CppOpen()
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



