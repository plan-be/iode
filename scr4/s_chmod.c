#if defined(DOSW32) || defined(SCRW32) || defined(LINUX)

#include "scr4w.h"
#include <sys/types.h>
#include <sys/stat.h>

/* ========================================================================
Modifie les attributs d'accès d'un fichier.

&EN filename : Name of existing file.
&EN pmode : Permission setting for file.

&RT
These functions return 0 if the permission setting is successfully changed.
A return value of -1 indicates that the specified file could not be found.

&NO


&IT DOS-Windows
───────────────
The SCR_chmod() function changes the permission setting of the file
specified by filename. The permission setting controls read and write access
to the file. The integer expression pmode contains one or both of the
following manifest constants, defined in SYS\STAT.H:


&EN _S_IWRITE : Writing permitted.
&EN _S_IREAD : Reading permitted.
&EN _S_IREAD | _S_IWRITE : Reading and writing permitted.

Any other values for pmode are ignored. When both constants are given, they
are joined with the bitwise OR operator ( | ). If write permission is not
given, the file is read-only. Note that all files are always readable; it is
not possible to give write-only permission. Thus the modes _S_IWRITE and
_S_IREAD | _S_IWRITE are equivalent.

================================================================== */


SCR_chmod(char *filename, int attr)
{
#if defined(SCRW32) || defined(DOSW32)

    DWORD   res = 0;

    if(!(attr & 0200))          res += FILE_ATTRIBUTE_READONLY;
    if(!(attr & 0400))          res += FILE_ATTRIBUTE_HIDDEN;
    if(attr & SCR_FTYPE_SYS)    res += FILE_ATTRIBUTE_SYSTEM;
    if(attr & SCR_FTYPE_ARCH)   res += FILE_ATTRIBUTE_ARCHIVE;

    return(!SetFileAttributes(filename, res));
#else
#ifdef UNIX
    return(chmod(filename, attr));
#else
    int     res = 0;

    if(!(attr & 2))             res += _S_IWRITE;
    if(!(attr & 4))             res += _S_IREAD;
    return(_chmod(filename, res);
#endif
#endif
}

#endif


