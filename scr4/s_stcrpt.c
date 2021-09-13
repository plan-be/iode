
/* ====================================================================
Encrypte le vecteur de caractŠres buf de longueur len. La base de
l'encodage est le long key qui sera fourni … la fonction inverse pour un
d‚cryptage correct.

La fonction inverse est SCR_decrypt().

&SA SCR_decrypt()
=======================================================================*/

int SCR_crypt(buf, len, key)
unsigned char   *buf;
int             len;
long            key;
{
    int             i;
    unsigned char   a, b, c, j;

    for(i = 0 ; i < len ; i++) {
	j = (key + i) % 8L;
	if(j % 5 == 0) j = 3;
	c = buf[i];
	a = c << j;
	b = c >> (8 - j);
	buf[i] = a | b;
	}
    return(0);
}

/* ====================================================================
Decrypte le vecteur de caractŠres buf de longueur len. La base du
d‚codage est le long key qui a ‚t‚ fourni … la fonction inverse
SCR_crypt().

&SA SCR_crypt()
=======================================================================*/

int SCR_decrypt(buf, len, key)
unsigned char   *buf;
int             len;
long            key;
{
    int             i;
    unsigned char   a, b, c, j;

    for(i = 0 ; i < len ; i++) {
	j = (key + i) % 8L;
	if(j % 5 == 0) j = 3;
	c = buf[i];
	a = c >> j;
	b = c << (8 - j);
	buf[i] = a | b;
	}

    return(0);
}





