#include "s_strs.h"
#include "s_dir.h"

/*NH*/
int SCR_mode_fmt_1(mode, txt)
unsigned int    mode;
char            *txt;
{
    memset(txt, '-', 3);
    if(mode & 4) txt[0] = 'r';
    if(mode & 2) txt[1] = 'w';
    if(mode & 1) txt[2] = 'x';
    txt[3] = 0;
    return(0);
}

/* ====================================================================
Formatte le mode d'un fichier (retourné par SCR_stat() et autres fonctions)
dans le format Unix (drwxrwxrwx par exemple). txt contient en output
un string de 10 caractères terminé par un 0.

Cette fonction est appropriée pour tous les types de fichiers, DOS
ou UNIX. Le premier caractère peut valoir :

&EN - = fichier normal
&EN d = directory
&EN s = special file (FIFO en Unix, System file en DOS)
&EN b = block special file (Unix)
&EN c = character special file (en Unix)
&EN v = Volume label (en DOS)

Des define standards pour tous les attributs sont définis dans le fichier
s_dir.h.

&RT void
&EX
    SCR_mode_fmt(0755, res) place dans res "-rwxr-xr-x"
&TX

&SA structure SCRSTAT, SCR_dir(), SCR_stat(), ...
======================================================================= */

int SCR_mode_fmt(mode, txt)
unsigned int    mode;
char            *txt;
{
    SCR_mode_fmt_1((mode >> 6), txt + 1);
    SCR_mode_fmt_1((mode >> 3), txt + 4);
    SCR_mode_fmt_1(mode       , txt + 7);

    switch(mode & SCR_FTYPE) {
#ifdef DOS
	case SCR_FTYPE_CHAR : txt[0] = 'v'; break;
#else
	case SCR_FTYPE_CHAR : txt[0] = 'c'; break;
#endif
	case SCR_FTYPE_DIR  : txt[0] = 'd'; break;
	case SCR_FTYPE_SPEC : txt[0] = 's'; break;
	case SCR_FTYPE_BLK  : txt[0] = 'b'; break;
	default             : txt[0] = '-'; break;
	}
    return(0);
}

