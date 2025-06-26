#include "scr4.h"

/* ===================================================================
Effectue l'addition d'un entier a et d'un entier b en arithmétique
de précision infinie.

Chaque entier est représenté par une chaîne de caractères numériques de
longueur arbitraire.

&RT res contient le résultat.

&EX
    InfAdd("399999999999999999999999999", "1", res);
    printf("resultat=%s\n", res);

    vaut :

    resultat=400000000000000000000000000
&TX

&SA InfMult(), InfDiff(), InfDiv()
====================================================================== */

InfAdd(a, b, res)
char    *a, *b, *res;
{
    InfRev(a);
    InfRev(b);
    InfRAdd(a, b, res);
    InfRev(res);
    InfRev(a);
    InfRev(b);
    return(0);
}


/*NH*/
InfRAdd(a, b, res)
char    *a, *b, *res;
{
    int     i, lga = strlen(a), lgb = strlen(b), carry = 0, va, vb, v;

    for(i = 0 ; i < lga || i < lgb ; i++) {
	va = vb = 0;
	if(i < lga) va = a[i] - '0';
	if(i < lgb) vb = b[i] - '0';
	v = va + vb + carry;
	if(v > 9) {carry = 1; v -= 10;}
	else       carry = 0;
	res[i] = '0' + v;
	}
    if(carry) res[i++] = '1';
    res[i] = 0;
    InfRStrip0(res);
    return(0);
}

