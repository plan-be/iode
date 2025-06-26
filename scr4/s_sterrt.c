#include <scr4_str.h>
#include "s_strs.h"
//#include <stdio.h>

//char    SCR_NAME_ERR[65] = "scr_err.txt";
char    SCR_NAME_ERR[SCR_MAX_FILE_LG_MAX] = "scr_err.txt"; // JMP 10/4/2015
int     SCR_ERR_LANG = 0;
FILE    *fd_msg = 0;

/* ====================================================================
Cette fonction retourne un message sur base d'un nom de message et d'un
numéro de langue dans le fichier ouvert fd_msg.

Il s'agit d'une généralisation de la fonction SCR_get_err_txt_lang() au
cas des messages définis par un mot-clé au lieu d'un numéro.

&SA SCR_search_file(), SCR_err_txt(), SCR_get_err_txt(),
    SCR_err_txt_lang(), SCR_get_err_txt_lang(), SCR_err_txt_t(),
    SCR_get_err_txt_t(), SCR_err_txt_t_lang().
=======================================================================*/

char *SCR_get_err_txt_t_lang(fd_msg, ckeyw, curlang)
FILE    *fd_msg;
U_ch    *ckeyw;
int     curlang;
{
    int         i = 0, j, c, lang = 0;                /* JMP 16-04-95 */
    static char msgs[2][256];                           /* JMP 23-02-03 */
    static int  last_msg = 0;
    U_ch        *msg, buf[1024]; /* JMP 10-01-11 */
    U_ch        keyw[256];

    strcpy(keyw, ckeyw);
    SCR_strip(keyw);                    /* BP_M 19-12-2002 09:27 */
    if(strlen(keyw) == 0) return(0L);   /* BP_M 19-12-2002 09:27 */

    last_msg = (last_msg + 1) % 2;
    msg = msgs[last_msg];
    msg[0] = 0;
    if(fd_msg == 0 || SCR_ERR_FILE_POS < 0) goto fin;  /* JMP_M 4.19 08-05-95 */
    fseek(fd_msg, SCR_ERR_FILE_POS, 0);     /* JMP 16-04-95 */
    while(!feof(fd_msg)) {
	j = 0;
	while(!feof(fd_msg)) {
	    c = fgetc(fd_msg);
	    if(keyw[j] == 0 && (c == ' ' || c == '|')) goto found; /* JMP_M 4.19 13-05-95 */
	    if(c != keyw[j++]) break;                              /* JMP_M 4.19 13-05-95 */
	}                                                          /* JMP_M 4.19 13-05-95 */
	while(!feof(fd_msg) && fgetc(fd_msg) != '\n') ;
    }

found :
    if(c != '|') while(!feof(fd_msg) && fgetc(fd_msg) != '|') ; /* JMP_M 4.19 08-05-95 */
    lang = 0;
ag:
    i = 0;
    while(!feof(fd_msg)) {
	buf[i] = c = fgetc(fd_msg);
	if(i >= 80 || c == '\n' || c == '\r' || c == '|') break;
	i++;
    }
    buf[i] = 0;
    if(i > 0) strcpy(msg, buf);
    if(c == '|' && curlang > lang) {
	lang++;
	goto ag;
	}

fin:
    if(msg[0] == 0) sprintf(msg, "Msg#%s", keyw); /* JMP 16-04-97 */

    return(msg);
}

/* ====================================================================
Retourne un message sur base d'un nom de message, et du numéro de langue
courante définie par la variable SCR_ERR_LANG. Ce message est lu dans le
fichier ouvert fd_msg.

Il s'agit d'une généralisation de la fonction SCR_get_err_txt() au
cas des messages définis par un mot-clé au lieu d'un numéro.

&SA SCR_search_file(), SCR_err_txt(), SCR_get_err_txt(),
    SCR_err_txt_lang(), SCR_get_err_txt_lang(), SCR_err_txt_t(),
    SCR_err_txt_t_lang(), SCR_get_err_txt_t_lang().
=======================================================================*/

char *SCR_get_err_txt_t(fd_msg, keyw)
FILE    *fd_msg;
U_ch    *keyw;
{
    return(SCR_get_err_txt_t_lang(fd_msg, keyw, SCR_ERR_LANG));
}

/* ====================================================================
Retourne un message sur base d'un nom de message et d'un numéro de
langue.

Il s'agit d'une généralisation de la fonction SCR_err_txt_lang() au
cas des messages définis par un mot-clé au lieu d'un numéro.

&SA SCR_search_file(), SCR_err_txt(), SCR_get_err_txt(),
    SCR_err_txt_lang(), SCR_get_err_txt_lang(), SCR_err_txt_t(),
    SCR_get_err_txt_t(), SCR_get_err_txt_t_lang().
=======================================================================*/

char *SCR_err_txt_t_lang(keyw, lang)
U_ch    *keyw;
int     lang;
{
    static FILE *fd_msg = 0;
    char        *pt;
/*  char        *SCR_search_file(); /* JMP 04-01-98 */
    static long pos = 0;

    if(fd_msg == 0) {
        pt = SCR_search_file(SCR_NAME_ERR);
        if(pt) {
#ifdef UNIX
            fd_msg = fopen(pt, "r");
#else
            fd_msg = fopen(pt, "rb");  /* JMP 24-12-98 */
#endif
            if(fd_msg)
                pos = SCR_get_app_pos(fd_msg, SCR_ERR_FILE_NB);
	    }
	}
    
    SCR_ERR_FILE_POS = pos;
    if(pos < 0) return(NULL); // JMP 17/05/2022
    pt = SCR_get_err_txt_t_lang(fd_msg, keyw, lang);
    SCR_ERR_FILE_POS = 0L;
    return(pt);
}

/* ====================================================================
Retourne un message sur base d'un nom et de la langue courante
définie par la variable SCR_ERR_LANG. Le message est lu dans un fichier
ASCII dont le nom se trouve dans la variable SCR_ERR_NAME. Cette
variable vaut par défaut "scr_err.txt". Le nom peut cependant étre
changé par l'utilisateur.

Le fichier réel est recherché à l'aide de la fonction SCR_search_file().

&RT Un pointeur vers un buffer statique contenant le dernier message
    lu. 2 buffers sont successivement utilisés de faéon à permettre des
    expressions du type

Il s'agit d'une généralisation de la fonction SCR_err_txt() au
cas des messages définis par un mot-clé au lieu d'un numéro.

&SA SCR_search_file(), SCR_err_txt(), SCR_get_err_txt(),
    SCR_err_txt_lang(), SCR_get_err_txt_lang(), SCR_get_err_txt_t(),
    SCR_err_txt_t_lang(), SCR_get_err_txt_t_lang().
=======================================================================*/

char *SCR_err_txt_t(keyw)
U_ch    *keyw;
{
    return(SCR_err_txt_t_lang(keyw, SCR_ERR_LANG));
}

/* ====================================================================
Retourne un message sur base d'un numéro de message et d'un numéro de
langue dans le fichier ouvert fd_msg. Le message est lu dans un fichier
ASCII dont la structure est la suivante :

&CO
    n1 |message langue 0|langue 1|...
    n2 |message langue 0|langue 1|...
    n3 |message langue 0|langue 1|...
      ...
&TX

Chaque record commence par un nom (ou numéro) identifiant le message,
suivi d'une barre verticale qui sert de séparateur, suivi par le message
lui-méme dans la langue 0, suivi du séparateur, etc.

La fonction SCR_get_err_txt_lang() se positionne au début du fichier,
recherche le numéro indiqué, lit le message dans la langue courante
définie par la variable SCR_ERR_LANG et retourne ce message dans une
variable statique. La longueur de chaque message ne peut excéder 80
caractéres.

Pour placer des blancs en fin de ligne, on peut terminer celle-ci par le
caractére |. Ce caractére est supprimé du string s'il se trouve en fin
de ligne.

&RT Un pointeur vers un buffer statique contenant le dernier message
    lu. 2 buffers sont successivement utilisés de faéon à permettre des
    expressions du type

&CO
    printf("Error : %s %s\n", SCR_get_err_txt_lang(fd, 124, 0),
			      SCR_get_err_txt_lang(fd, 12, 1));
&TX
&SA SCR_search_file(), SCR_err_txt(), SCR_get_err_txt(),
    SCR_err_txt_lang(), SCR_get_err_txt_t(), SCR_err_txt_t_lang(),
    SCR_get_err_txt_t_lang(), SCR_err_txt_t().
=======================================================================*/

char *SCR_get_err_txt_lang(fd_msg, val, lang)
FILE    *fd_msg;
int     val, lang;
{
    char    keyw[20];

    sprintf(keyw, "%d", val);
    return(SCR_get_err_txt_t_lang(fd_msg, keyw, lang));
}

/* ====================================================================
Retourne un message sur base d'un numéro et de la langue courante
définie par la variable SCR_ERR_LANG. Le message est lu dans un fichier
ASCII dont la structure est la suivante :

&CO
    n1 | message langue 0 | langue 1 | ...
    n2 | message langue 0 | langue 1 | ...
    n3 | message langue 0 | langue 1 | ...
      ...
&TX

Chaque record commence par un nom (ou numéro) identifiant le message,
suivi d'une barre verticale qui sert de séparateur, suivi par le message
lui-méme dans la langue 0, suivi du séparateur, etc.

Le nom du fichier se trouve dans la variable SCR_ERR_NAME. Cette
variable vaut par défaut "scr_err.txt". Le nom peut cependant étre
changé par l'utilisateur.

Le fichier réel est recherché à l'aide de la fonction SCR_search_file().

&RT Un pointeur vers un buffer statique contenant le dernier message
    lu. 2 buffers sont successivement utilisés de faéon à permettre des
    expressions du type

&CO
    printf("Error : %s %s\n", SCR_err_txt_lang(124, 0),
			      SCR_err_txt_lang(12, 0));
&TX
&SA SCR_search_file(), SCR_err_txt(), SCR_get_err_txt(),
    SCR_get_err_txt_lang(), SCR_get_err_txt_t(), SCR_err_txt_t_lang(),
    SCR_get_err_txt_t_lang(), SCR_err_txt_t().
=======================================================================*/

char *SCR_err_txt_lang(val, lang)
int     val, lang;
{
    char    keyw[20];

    sprintf(keyw, "%d", val);
    return(SCR_err_txt_t_lang(keyw, lang));
}

/* ====================================================================
Retourne un message sur base d'un numéro de message, du numéro de langue
courante définie par la variable SCR_ERR_LANG. Ce message est lu dans le
fichier ouvert fd_msg. La fonction est équivalente é

&CO
    SCR_get_err_txt_lang(fd_msg, val, SCR_ERR_LANG);
&TX

&RT Un pointeur vers un buffer statique contenant le dernier message
    lu. 2 buffers sont utilisés en alternance de faéon à permettre des
    expressions du type :

&CO
    printf("Error : %s %s\n", SCR_get_err_txt(fd, 124),
			      SCR_get_err_txt(fd, 12));
&TX
&SA SCR_search_file(), SCR_err_txt(), SCR_err_txt_lang(),
    SCR_get_err_txt_lang(), SCR_get_err_txt_t(), SCR_err_txt_t_lang(),
    SCR_get_err_txt_t_lang(), SCR_err_txt_t().
=======================================================================*/

char *SCR_get_err_txt(fd_msg, val)
FILE    *fd_msg;
int     val;
{
    return(SCR_get_err_txt_lang(fd_msg, val, SCR_ERR_LANG));
}

/* ====================================================================
Retourne un message sur base d'un numéro et de la langue courante
définie par la variable SCR_ERR_LANG. Le message est lu dans un fichier
ASCII dont le nom se trouve dans la variable SCR_ERR_NAME. Cette
variable vaut par défaut "scr_err.txt". Le nom peut cependant étre
changé par l'utilisateur.

Cette fonction est équivalente à :

&CO
    SCR_err_txt_lang(val, SCR_ERR_LANG);
&TX

Le fichier réel est recherché à l'aide de la fonction SCR_search_file().

&RT Un pointeur vers un buffer statique contenant le dernier message
    lu. 2 buffers sont successivement utilisés de faéon à permettre des
    expressions du type

&CO
    printf("Error : %s %s\n", SCR_err_txt(124),
			      SCR_err_txt(12));
&TX
&SA SCR_search_file(), SCR_get_err_txt(), SCR_err_txt_lang(),
    SCR_get_err_txt_lang(), SCR_get_err_txt_t(), SCR_err_txt_t_lang(),
    SCR_get_err_txt_t_lang(), SCR_err_txt_t().
=======================================================================*/

char* (*SCR_err_txt_super)(int val);

char *SCR_err_txt(int val)
{
    if(SCR_err_txt_super != 0) 
        return (*SCR_err_txt_super)(val);
    else
        return(SCR_err_txt_lang(val, SCR_ERR_LANG));
}






