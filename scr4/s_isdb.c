#include "scr.h"

/* ====================================================================
Cr�e une database. Dans le cas d'un ISAM, il s'agit de 2 ou trois
fichiers :

&EN fichier .dat contenant les donn�es
&EN fichier .idx contenant les index
&EN fichier .num contenant le num�ro automatique si un champ AUTO est
    pr�sent

Le fichier cr�� n'est pas ouvert.

&RT 0 en cas de succ�s, -1 sinon.
&EX
    IS_set_name(is_tmp, "tmp");
    if(IS_create_db(is_tmp)) return(-1);
    if(IS_open_files(is_tmp, 0L)) return(-1);
    ...
    IS_close(is_tmp);
&TX
&REM En DOS, et avec CTREE comme syst�me de database ISAM, il est
impossible de cr�er un fichier et de l'utiliser au cours du m�me
programme. Le fichier est cependant cr�� correctement et peut �tre
utilis� apr�s avoir relancer le programme.

&SA IS_open(), IS_close(), IS_set_name()
=======================================================================*/
IS_create_dbase(is)
ISAM    *is;
{
    return(IS_create_db(is));
}

/* ====================================================================
D�truit un ISAM. Les 3 fichiers (dat, idx et num) associ�s � cet ISAM
sont d�truits � l'aide de la fonction unlink().

&RT 0.
&SA IS_create_dbase()
=======================================================================*/
IS_delete_dbase(is)
ISAM    *is;
{
    return(IS_delete_db(is));
}

/* ====================================================================
Reconstruit les donn�es et les index d'un ISAM. Le fichier .idx est
d�truit et reconstruit.

&REM En DOS, et avec CTREE comme syst�me de database ISAM, il est
impossible de reconstruire un fichier et de l'utiliser au cours du m�me
programme. Il faut quitter le programme apr�s un rebuild et
le relancer ensuite pour pouvoir acc�der au fichier.

&RT 0 en cas de succ�s, code d'erreur du syst�me de base de donn�es
sinon.
&SA IS_create_dbase()
=======================================================================*/
IS_rebuild_dbase(is)
ISAM    *is;
{
    return(IS_rebuild(is));
}

