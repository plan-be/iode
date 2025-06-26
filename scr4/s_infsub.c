#include "scr4.h"

/* ===================================================================
Effectue la soustraction d'un entier a par un entier b en arithmétique
de précision infinie.

Chaque entier est représenté par une chaîne de caractères numériques de
longueur arbitraire.

&RT res contient le résultat.

&EX
    InfDiff("399999999999999999999999999", "1", res);
    printf("resultat=%s\n", res);

    vaut :

    resultat=399999999999999999999999998
&TX

&SA InfMult(), InfAdd(), InfDiv()
====================================================================== */

InfDiff(a, b, res)
char    *a, *b, *res;
{
    InfRev(a);
    InfRev(b);
    InfRDiff(a, b, res);
    InfRev(res);
    InfRev(a);
    InfRev(b);
    return(0);
}

/*NH*/
InfRDiff(a, b, res)
char    *a, *b, *res;
{
    int     i, lga = strlen(a), lgb = strlen(b), carry = 0, va, vb, v;

    for(i = 0 ; i < lga ; i++) {
	va = vb = 0;
	if(i < lga) va = a[i] - '0';
	if(i < lgb) vb = b[i] - '0';
	v = vb + carry;
	if(va < v) {carry = 1; va += 10;}
	else        carry = 0;
	res[i] = '0' + va - v;
	}
    if(carry) res[i++] = '1';
    res[i] = 0;
    InfRStrip0(res);
    return(0);
}

