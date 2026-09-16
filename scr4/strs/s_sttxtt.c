#include "s_strs.h"

/* ====================================================================
Imprime le texte str (string terminé par 0) en coupant les lignes à une
longueur lg. La première ligne commence par le titre défini dans title.
Les lignes suivantes commencent par des blancs en nombre égal à la
longueur de title.

La fonction d'impression est passée comme paramètre : elle est appelée
avec un string comme argument.

Les coupures de lignes se font sur les caractères trouvés dans le string
seps.

Les blancs de début de ligne sont supprimés. Les mots plus longs
que (lg - strlen(title)) sont coupés sur plusieurs lignes.

&EX
    PrintPays("Pays : ",
	      "Belgique, France, Pays-Bas, Italie, Suisse, Danemark",
	      " ,",
	      25,
	      printf);
    donne :

	"Pays : Belgique, France,"
	"       Pays-Bas, Italie,"
	"       Suisse, Danemark"
&TX
&SA SCR_text()
=======================================================================*/

#ifdef SCRPROTO
int SCR_text_title(
unsigned char   *title,
unsigned char   *str,
unsigned char   *seps,
int             lg,
int             (*fn)(unsigned char *)
)
#else
int SCR_text_title(title, str, seps, lg, fn)
unsigned char   *title,
		*str,
		*seps;
int             lg;
int             (*fn)();
#endif
{
    char    buf[133];
    U_ch    **tbl;
    int     i;

    strcpy(buf, title);
    memset(buf, ' ', (int)strlen(title));

    lg -= (int)strlen(title);
    if(lg < 1) {
	(*fn)(title);
	return(0);
	}
    tbl = SCR_text(str, seps, lg);
    for(i = 0 ; tbl[i]; i++) {
	if(i == 0) (*fn)(title);
	else       (*fn)(buf);
	(*fn)(tbl[i]);
	(*fn)("\n");
	}

    SCR_free_tbl(tbl);
    return(0);
}
