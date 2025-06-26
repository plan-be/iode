#include "scr.h"


/* ════════════════════════════════════════════════════════════════════
Retourne un pointeur vers un buffer statique contenant le nom
du device défini par disk et type.

disk vaut 0 pour le lecteur A: et 1 pour B:

type vaut 0 pour une disquette 3.5" 1,44 MB, 1 pour une disquette
3,5" 720K et 2 pour une disquette 5.25" 1.2 MB.

&EX
    STD_unit(1, 1) vaut en DOS     "B:"
			en UNIX5R4 "B:"
			en UNIX    "/dev/fd1135ds9"
&TX

&RT buffer statique contenant le nom du device
&SA STD_doscpin(), STD_doscpout(), STD_dosformat()
─────────────────────────────────────────────────────────────────────── */
char *STD_unit(disk, type)
int     disk,  type;
{
    static char buf[25];
#ifdef DOS
    strcpy(buf, ((disk == 0) ? "a:" : "b:"));
#else
#ifdef UNIX5R4
    strcpy(buf, ((disk == 0) ? "a:" : "b:"));
#else
    sprintf(buf, "/dev/dsk/fd%d", disk);
    switch(type) {
	case 0 : strcat(buf, "135ds8"); break;
	case 1 : strcat(buf, "135ds9"); break;
	case 2 : strcat(buf, "96ds15"); break;
	}
#endif
#endif
    return(buf);
}

/* ════════════════════════════════════════════════════════════════════
Copie le fichier from vers une disquette DOS dans l'unité unit et type type
sous le nom to.

unit vaut 0 pour le lecteur A: et 1 pour B:

type vaut 0 pour une disquette 3.5" 1,44 MB, 1 pour une disquette
3,5" 720K et 2 pour une disquette 5.25" 1.2 MB.

&EX
    STD_doscpout("../usr/test", "test", 0, 0)
&TX

&RT void
&SA STD_doscpin(), STD_unit(), STD_dosformat()
─────────────────────────────────────────────────────────────────────── */
STD_doscpout(from, to, unit, type)
char    *from, *to;
int     unit, type;
{
    char    buf[200];
#ifdef DOS
    #if !defined(DOSWIN) && !defined(DOSW32) && !defined(WINDOWS)
	sprintf(buf, "copy %s %s%s", from, STD_unit(unit, type), to);
	system(buf);
	SCR_refresh();
    #else
	sprintf(buf, "%s%s", STD_unit(unit, type), to);   /* JMP 02-01-97 */
	SCR_copy_file(from, buf);                         /* JMP 02-01-97 */
    #endif
#else
    sprintf(buf, "clear;echo Writing to DOS floppy...;doscp -r %s %s:%s", from, STD_unit(unit, type), to);
    ACT_exec_system(buf);
#endif
    return(0);
}

/* ════════════════════════════════════════════════════════════════════
Copie le fichier from à partir d'une disquette DOS dans l'unité unit et
type type sous le nom to.

unit vaut 0 pour le lecteur A: et 1 pour B:

type vaut 0 pour une disquette 3.5" 1,44 MB, 1 pour une disquette
3,5" 720K et 2 pour une disquette 5.25" 1.2 MB.

&EX
    STD_doscpin("\usr\test", "/usr/home/test", 0, 0)
&TX

&RT void
&SA STD_doscpout(), STD_unit(), STD_dosformat()
─────────────────────────────────────────────────────────────────────── */
STD_doscpin(from, to, unit, type)
char    *from, *to;
int     unit, type;
{
    char    buf[200];
#ifdef DOS
    #if !defined(DOSWIN) && !defined(DOSW32) && !defined(WINDOWS)
    sprintf(buf, "copy %s%s %s", STD_unit(unit, type), from, to);
    system(buf);
    SCR_refresh();
    #else
	sprintf(buf, "%s%s", STD_unit(unit, type), from);   /* JMP 02-01-97 */
	SCR_copy_file(buf, to);                             /* JMP 02-01-97 */
    #endif
#else
    sprintf(buf, "clear;echo Reading from DOS floppy...;doscp -r %s:%s %s", STD_unit(unit, type), from, to);
    ACT_exec_system(buf);
#endif
    return(0);
}

/* ════════════════════════════════════════════════════════════════════
Formatte une disquette au format DOS (si la commande dosformat existe
sur le système UNIX).

unit vaut 0 pour le lecteur A: et 1 pour B:

type vaut 0 pour une disquette 3.5" 1,44 MB, 1 pour une disquette
3,5" 720K et 2 pour une disquette 5.25" 1.2 MB.

&EX
    STD_dosformat(0, 2)
&TX

&NO Cette fonction n'effectue aucune opération sous DOSWIN

&RT void
&SA STD_doscpin(), STD_unit(), STD_doscpout()
─────────────────────────────────────────────────────────────────────── */
STD_dosformat(unit, type)
int     unit, type;
{
    char    buf[80];

#ifdef DOS
#if !defined(DOSWIN) && !defined(DOSW32) && !defined(WINDOWS)
    sprintf(buf, "cls;format %s", STD_unit(unit, type));
#endif
#else
    sprintf(buf, "clear;dosformat %s", STD_unit(unit, type));
#endif
    ACT_exec_system(buf);
    return(0);
}

/* ════════════════════════════════════════════════════════════════════
Lance un sous-shell (command.com en DOS et /bin/sh en unix). Ce shell
se termine par exit. L'écran est vidé et un message indique à
l'utilisateur comment revenir au programme.

Après exit, l'application reprend à l'endroit où elle avait été quittée.

&EX
    STD_subshell();
&TX

&NO Cette fonction n'effectue aucune opération sous DOSWIN

&RT void
&SA STD_doscpin(), STD_unit(), STD_dosformat()
─────────────────────────────────────────────────────────────────────── */
STD_subshell()
{
#ifdef DOS
#if !defined(DOSWIN) && !defined(DOSW32) && !defined(WINDOWS)
    ACT_exec_system("cls;echo EXIT quits the shell;command");
#endif
#else
    ACT_exec_system("clear;echo exit quits the shell;/bin/sh");
#endif
    return(0);
}




