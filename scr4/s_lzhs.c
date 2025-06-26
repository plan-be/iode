#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "s_lzh.h"

/* ====== Interface String ======== */

unsigned        LZH_inlen, LZH_inpos, LZH_outpos;
unsigned char   *LZH_instr, *LZH_outstr;

/*NH*/
LzhGetStr()
{
    if(LZH_inlen <= LZH_inpos) return(-1);
    LZH_inpos++;
    return(LZH_instr[LZH_inpos - 1]);
}

/*NH*/
LzhPutStr(c)
int     c;
{
    if(LZH_outpos && LZH_outpos % 100 == 0)
	LZH_outstr = realloc(LZH_outstr, LZH_outpos + 100);
    if(LZH_outstr == 0) return(-1);
    LZH_outstr[LZH_outpos] = c;
    LZH_outpos++;
    return(0);
}

/*NH*/
long LzhSizStr()
{
    return((long)LZH_inlen);
}

/* ══════════════════════════════════════════════════════════════════
Fonction de compactage de string basée sur l'algorithme de Lempel-Zev et
sur la méthode de Huffmann.

instr est le string à compacter et inlen sa longueur. instr peut
contenir des zéros. outstr est un pointeur vers le string résultat
alloué par la fonction. outlen est un pointeur vers sa longueur.

&RT 0 en cas de succès, -1 en cas d'erreur
&SA LzhEncodeFile(), LzhDecodeStr(), LzhEncodeGnl()
────────────────────────────────────────────────────────────────── */
LzhEncodeStr(instr, inlen, outstr, outlen)
unsigned char   *instr, **outstr;
unsigned        inlen, *outlen;
{
    int     rc;

    LZH_inlen = inlen;
    LZH_instr = instr;
    LZH_inpos = LZH_outpos = 0;
    LZH_outstr = malloc(100);
    rc = LzhEncodeGnl(LzhGetStr, LzhPutStr, LzhSizStr);
    if(rc) {
	free(LZH_outstr);
	*outstr = 0;
	*outlen = 0;
	}
    else {
	*outstr = realloc(LZH_outstr, LZH_outpos);
	*outlen = LZH_outpos;
	}
    return(rc);
}

/* ══════════════════════════════════════════════════════════════════
Fonction de décompactage de string basée sur l'algorithme de Lempel-Zev
et sur la méthode de Huffmann. Le fichier traité doit être le résultat
de la fonction LzhEncodeStr().

instr est le string input et inlen sa longueur. Ce string est le
résultat de l'appel à la fonction LzhEncodeStr(). outstr est un pointeur
vers le string résultat qui est alloué via malloc par la fonction. La
longueur du résultat est stockée dans outlen.

&RT 0 en cas de succès, -1 en cas d'erreur
&SA LzhEncodeFile(), LzhDecodeStr(), LzhDecodeGnl()
────────────────────────────────────────────────────────────────── */
LzhDecodeStr(instr, inlen, outstr, outlen)
unsigned char   *instr, **outstr;
unsigned        inlen, *outlen;
{
    int     rc;

    LZH_inlen = inlen;
    LZH_instr = instr;
    LZH_inpos = LZH_outpos = 0;
    LZH_outstr = malloc(100);
    rc = LzhDecodeGnl(LzhGetStr, LzhPutStr);
    if(rc) {
	free(LZH_outstr);
	*outstr = 0;
	*outlen = 0;
	}
    else {
	*outstr = realloc(LZH_outstr, LZH_outpos);
	*outlen = LZH_outpos;
	}
    return(rc);
}



