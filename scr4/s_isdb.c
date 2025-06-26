#include "scr.h"

/* ====================================================================
Crée une database. Dans le cas d'un ISAM, il s'agit de 2 ou trois
fichiers :

&EN fichier .dat contenant les données
&EN fichier .idx contenant les index
&EN fichier .num contenant le numéro automatique si un champ AUTO est
    présent

Le fichier créé n'est pas ouvert.

&RT 0 en cas de succès, -1 sinon.
&EX
    IS_set_name(is_tmp, "tmp");
    if(IS_create_db(is_tmp)) return(-1);
    if(IS_open_files(is_tmp, 0L)) return(-1);
    ...
    IS_close(is_tmp);
&TX
&REM En DOS, et avec CTREE comme système de database ISAM, il est
impossible de créer un fichier et de l'utiliser au cours du même
programme. Le fichier est cependant créé correctement et peut être
utilisé après avoir relancer le programme.

&SA IS_open(), IS_close(), IS_set_name()
=======================================================================*/
IS_create_dbase(is)
ISAM    *is;
{
    return(IS_create_db(is));
}

/* ====================================================================
Détruit un ISAM. Les 3 fichiers (dat, idx et num) associés à cet ISAM
sont détruits à l'aide de la fonction unlink().

&RT 0.
&SA IS_create_dbase()
=======================================================================*/
IS_delete_dbase(is)
ISAM    *is;
{
    return(IS_delete_db(is));
}

/* ====================================================================
Reconstruit les données et les index d'un ISAM. Le fichier .idx est
détruit et reconstruit.

&REM En DOS, et avec CTREE comme système de database ISAM, il est
impossible de reconstruire un fichier et de l'utiliser au cours du même
programme. Il faut quitter le programme après un rebuild et
le relancer ensuite pour pouvoir accéder au fichier.

&RT 0 en cas de succès, code d'erreur du système de base de données
sinon.
&SA IS_create_dbase()
=======================================================================*/
IS_rebuild_dbase(is)
ISAM    *is;
{
    return(IS_rebuild(is));
}

