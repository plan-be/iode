#include "scr4.h"

/*NH*/
InfCmp(a, b)
char    *a, *b;
{
    InfStrip0(a);
    InfStrip0(b);
    if(strlen(a) < strlen(b)) return(-1);
    if(strlen(a) > strlen(b)) return(1);
    return(strcmp(a, b));
}

/* ===================================================================
Effectue la division d'un entier par un autre en arithmétique de
précision infinie.

Chaque entier est représenté par une chaîne de caractères numériques de
longueur arbitraire.

&RT res contient le dividende et reste le reste.

&EX
    InfDiv("333333333333333333333333334", "3", res, reste);
    printf("resultat=%s, reste=%s\n", res, reste);

    vaut :

    resultat=111111111111111111111111111, reste=1
&TX

&SA InfMult(), InfAdd(), InfDiff()
====================================================================== */

InfDiv(a, b, res, reste)
char    *a, *b, *res, *reste;
{
    int     i, k = 0, lgb = strlen(b), lga = strlen(a), j = lgb, lgt;
    char    res1[100], res2[100];

    if(InfCmp(a, b) < 0) {
	strcpy(res, "0");
	strcpy(reste, a);
	InfStrip0(reste);
	return(0);
	}

    memcpy(reste, a, j);
    if(InfCmp(reste, b) < 0) {reste[j] = a[j]; j++;}
    reste[j] = 0;

    while(1) {
	strcpy(res1, "0");
	for(i = 1 ; i <= 9 ; i++) {
	    InfAdd(res1, b, res2);
	    strcpy(res1, res2);
	    if(InfCmp(res1, reste) > 0) break;
	    }
	res[k++] = '0' + i - 1;
	InfMult1(b, i - 1, res1);
	InfDiff(reste, res1, res2);
	strcpy(reste, res2);
	while(1) {
	    if(j >= lga) {
		res[k] = 0;
		InfStrip0(res);
		return(0);
		}
	    lgt = strlen(reste);
	    reste[lgt++] = a[j++];
	    reste[lgt] = 0;
	    if(InfCmp(reste, b) >= 0) break;
	    res[k++] = '0';
	    }
	}
}

