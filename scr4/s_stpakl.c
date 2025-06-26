#include "scr.h"
#include <stdlib.h>

extern unsigned char  AR_CH;

/* ====================================================================
Retourne la liste des fichiers contenus dans un fichier d'archives
construit à l'aide de la fonction AR_add().

Le résultat est un tableau de structures ARLIST contenant pour chaque
entrée la définition d'un fichier archivé :
&CO
    typedef struct _arlist_ {
	long    ar_rsize,       Taille réelle
		ar_psize,       Taille packée
		ar_date;        Date de sauvetage YYYYMMDD
	char    ar_name[51];    Nom du fichier
    } ARLIST;
&TX
Le tableau se termine par un pointeur NULL pour undiquer qu'il
est terminé. La fonction SCR_free_tbl() permet de libérer l'espace.

&RT NULL en cas d'erreur. La variable AR_ERRNO indique le code d'erreur
    et la fonction AR_error() fournit un texte. En cas de succès,
    un tableau de structures ARLIST.

&EX
    ARLIST **arl;

    arl = AR_list("myarch.ar");
    for(i = 0 ; arl[i] ; i++)
	printf("%s\n", arl[i]->ar_name);

    SCR_free_tbl(arl);
&TX
&SA AR_error(), AR_extract(), AR_add(), SCR_free_tbl()
======================================================================= */

ARLIST **AR_list(arname)
char    *arname;
{
    FILE    *fd, *AR_open();
    ARLIST  **arl = 0, carl;
    int     arnb = 0;

    AR_ERRNO = 0;
    fd = AR_open(arname, 0);
    if(fd == 0) return(arl);

    while(1) {
	if(AR_readnext(fd, &carl)) break;
	SCR_add_ptr((U_ch ***)&arl, &arnb, 0L);
	arl[arnb - 1] = (ARLIST *)SCR_malloc(sizeof(ARLIST));
	memcpy(arl[arnb - 1], &carl, sizeof(ARLIST));
	fseek(fd, carl.ar_psize, 1);
	}
    SCR_add_ptr((U_ch ***)&arl, &arnb, 0L);
    fclose(fd);
    return(arl);
}
