#include "scr4.h"

/*NH*/
InfRMult1(a, ib, res)
char    *a, *res;
int     ib;
{
    int     i, lga = strlen(a), carry = 0, v;

    res[0] = 0;

    for(i = 0 ; i < lga ; i++) {
	v = carry + ib * (a[i] - '0');
	if(v > 9) {carry = v / 10; v = v % 10;}
	else       carry = 0;
	res[i] = '0' + v;
	}
    while(carry) {
	res[i++] = '0' + carry % 10;
	carry = carry / 10;
	}

    res[i] = 0;
    InfRStrip0(res);
    return(0);
}

/*NH*/
InfRMult(a, b, res)
char    *a, *b, *res;
{
    int     i, lgb = strlen(b), v;
    char    res1[256];

    strcpy(res, "0");
    for(i = 0 ; i < lgb ; i++) {
	v = b[i] - '0';
	InfRMult1(a, v, res1);
	InfRAdd(res + i, res1, res + i);
	}
    return(0);
}

/*NH*/
InfMult1(a, b, res)
char    *a, *res;
int     b;
{
    InfRev(a);
    InfRMult1(a, b, res);
    InfRev(res);
    InfRev(a);
    return(0);
}

/* ===================================================================
Effectue la multiplication d'un entier a par un entier b en arithmétique
de précision infinie.

Chaque entier est représenté par une chaîne de caractères numériques de
longueur arbitraire.

&RT res contient le résultat.

&EX
    InfMult("300000000000", "3000000000", res);
    printf("resultat=%s\n", res);

    vaut :

    resultat=9000000000000000000000
&TX

&SA InfDiv(), InfAdd(), InfDiff()
====================================================================== */

InfMult(a, b, res)
char    *a, *b, *res;
{
    InfRev(a);
    InfRev(b);
    InfRMult(a, b, res);
    InfRev(res);
    InfRev(a);
    InfRev(b);
    return(0);
}


