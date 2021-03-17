#include "s_strs.h"

/* ====================================================================
Change le nom d'un fichier. Cette fonction assure la compatibiliÇ entre
le DOS et certaines versions anciennes de UNIX (XENIX) qui ne possädent
pas le system call rename.

&RT 0 en cas de succäs, -1 sinon.
=======================================================================*/

SCR_rename(fold, fnew)
char    *fold, *fnew;
{
#if defined(XENIX) || defined(LINUX)
    if(link(fold, fnew)) return(-1);
    return(unlink(fold));
#else
#ifdef INTAU
    if(link(fold, fnew)) return(-1);
    return(unlink(fold));
#else
    return(rename(fold, fnew));
#endif
#endif
}


