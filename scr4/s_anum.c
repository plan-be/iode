#include "scr.h"
#if defined(XENIX) || defined(UNIX)  /* BP_M 26-06-1998 */
#   include <sys/types.h>
#   include <sys/file.h>
#endif

long ScrIncrAutoNum();

/*NH*/
long SCR_read_long(fd)  /* BP_M 29-01-2003 14:41 */
FILE    *fd;
{
    int     i = 0;
    long    c_num = 0L;
    int     ch;
    char    buf[20];

    ch = ISC_getc(fd);
    while(ch >= '0' && ch <= '9' && i < 19) {
	buf[i++] = ch;
	ch = ISC_getc(fd);
	}
    buf[i] = 0;
    if(i > 0) sscanf(buf, "%ld", &c_num);
    return(c_num);
}

/* ====================================================================
Lit dans le fichier filename.num la valeur qui s'y trouve et retourne
cette valeur. Utilisé dans les ISAM dont un champ est AUTO.

&RT valeur du fichier (long) ou -1 si le fichier n'est pas trouvé

&SA SCR_auto_num()
=======================================================================*/

long ScrGetAutoNum(filename)      /* BP_M 29-01-2003 14:41 */
char    *filename;
{
    FILE    *fd;
    char    buf[SCR_MAX_FILE_LG +1];
    long    c_num;
    char    ch;

    sprintf(buf, "%s.num", filename);
    fd = ISC_fopen(buf, "r");
    if(fd == 0) return((long)(-1));
    c_num = SCR_read_long(fd);
    ISC_fclose(fd);
    return(c_num);
}

/* ====================================================================
Lit dans le fichier filename.num la valeur qui s'y trouve et retourne
cette valeur. Utilisé dans les ISAM dont un champ est AUTO.

&RT valeur du fichier (long) ou -1 si le fichier n'est pas trouvé

&SA SCR_auto_num()
=======================================================================*/
long SCR_current_auto_num_isam(filename)
char    *filename;
{
    return(ScrGetAutoNum(filename));
}

/* ====================================================================
Lit dans le fichier filename.num la valeur qui s'y trouve et retourne
cette valeur. Utilisé dans les ISAM dont un champ est AUTO.

&RT valeur du fichier (long) ou -1 si le fichier n'est pas trouvé

&SA SCR_auto_num()
=======================================================================*/
long SCR_auto_num(filename)
char    *filename;
{
    return(ScrIncrAutoNum(filename));
}

/* ====================================================================
Ecrit dans le fichier filename.num la valeur num.

En Unix, cette fonction fait appel au system call locking() ou lockf()
pour s'assurer de l'accès exclusif au numéro à lire, et donc de
l'unicité du numéro dans un système multi-utilisateurs. Après 10 essais
de locking, la fonction retourne -1L.

&RT 0 en cas de succès, -1 en cas d'erreur
&EX
    SCR_set_auto_num(is_client->is_filename, 234L);
&TX
&SA SCR_current_auto_num_isam()
=======================================================================*/

SCR_set_auto_num(filename, auton)       /* BP_M 04-05-2001 12:00 */
char    *filename;
long    auton;
{
    return(ScrSetAutoNum(filename, auton)); /* BP_M 04-05-2001 12:00 */
}

/* ====================================================================
Lit dans le fichier filename.num la valeur qui s'y trouve et retourne
cette valeur plus un. Utilisé dans les ISAM dont un champ est AUTO.

La valeur du fichier est incrémentée.

En Unix, cette fonction fait appel au system call locking() ou lockf()
pour s'assurer de l'accès exclusif au numéro à lire, et donc de
l'unicité du numéro dans un système multi-utilisateurs. Après 10 essais
de locking, la fonction retourne -1L.

&RT valeur du fichier (long) ou -1 si le fichier n'est pas trouvé
&EX
    SCR_set_long(pclient_num, SCR_auto_num("is_client"));
&TX
&SA SCR_current_auto_num()
=======================================================================*/

long ScrIncrAutoNum(filename)  /* BP_M 29-01-2003 14:39 */
char    *filename;
{
    FILE    *fd;
    char    buf[SCR_MAX_FILE_LG +1];
    long    c_num = -1L;        /*JMP 25-05-95*/
    int     nbtry;              /*JMP 25-05-95*/

    sprintf(buf, "%s.num", filename);
    fd = ISC_fopen(buf, "r+");
    if(fd == 0) return((long)(-1));
    nbtry = 0;                                  /*JMP 25-05-95*/
ag: if(ISC_flock(fd, 10L)) {           /* JMP */
	nbtry++;                                /*JMP 25-05-95*/
	if(nbtry > 10) goto fin;                /*JMP 25-05-95*/
	SCR_sleep(1000);                        /* JMP 07-03-17 */ /* JMP 07-03-17 */
	//sleep(1);                               /* JMP 03-08-02 */
	goto ag;                                /*JMP 25-05-95*/
	}                                       /*JMP 25-05-95*/
    c_num = SCR_read_long(fd);
    c_num++;
    ISC_fseek(fd, 0L, 0);
    ISC_fprintf(fd, "%ld ", c_num);
    ISC_funlock(fd, 10L);
fin:
    ISC_fclose(fd);
    return(c_num);
}

/* ====================================================================
Ecrit dans le fichier filename.num la valeur num.

En Unix, cette fonction fait appel au system call locking() ou lockf()
pour s'assurer de l'accès exclusif au numéro à lire, et donc de
l'unicité du numéro dans un système multi-utilisateurs. Après 10 essais
de locking, la fonction retourne -1L.

&RT 0 en cas de succès, -1 en cas d'erreur
&EX
    SCR_set_auto_num(is_client->is_filename, 234L);
&TX
&SA SCR_current_auto_num_isam()
=======================================================================*/

ScrSetAutoNum(filename, auton) /* BP_M 29-01-2003 14:39 */
char    *filename;
long    auton;
{
    FILE    *fd;
    char    buf[SCR_MAX_FILE_LG +1];
    int     nbtry, rc = 0;              /*JMP 25-05-95*/

    sprintf(buf, "%s.num", filename);
    fd = ISC_fopen(buf, "w+");
    if(fd == 0) return((long)(-1));
    nbtry = 0;
ag: if(ISC_flock(fileno(fd), 10L)) {           /* JMP */
	nbtry++;
	if(nbtry > 10) {
	    rc = -1;                            /* JMP 20-01-99 */
	    goto fin;                           /* JMP 20-01-99 */
	    }
	SCR_sleep(1000);                        /* JMP 07-03-17 */ /* JMP 07-03-17 */
	//sleep(1); /* JMP 07-03-17 */
	goto ag;
	}
    ISC_fprintf(fd, "%ld ", auton);
    ISC_funlock(fileno(fd), 10L);
fin:
    ISC_fclose(fd);
    return(rc);
}







