#include "scr4.h"
#ifndef UNIX
#include <dos.h>

#include "s_tcwc.h"

#if defined(DOS) && !defined(DOSW32) && !defined(WIN32)

#define PARAL_TIMEOUT 1
#define PARAL_ERROR   8
#define PARAL_ONLINE  16
#define PARAL_PAPER   32
#define PARAL_AK      64
#define PARAL_NBUSY   128

/*NH*/
PciParalInt86(fn, nb)
int     fn, nb;
{
    union REGS  inregs,outregs;

    inregs.h.ah = fn;
#ifdef WATCOM
    inregs.x.edx = nb - 1;
#else
    inregs.x.dx = nb - 1;
#endif
    int86(0x17, &inregs, &outregs);
    return(outregs.h.ah);
}
#endif

/* =====================================================================
Réinitialise l'imprimante parallèle nb (1 pour lpt1, 2 pour lpt2, 3 pour
lpt3). Cette fonction peut être utilisée avant de lancer une impression
sur une imprimante parallèle. En effet, en cas d'absence d'imprimante,
le programme en cours se bloque et ne peut bien souvent être relancé que
par un reboot du système.

Si l'imprimante est connectée et online, la fonction retourne 0. Si elle
n'a plus de papier, le code retour est 1. Si aucune imprimante n'est
présente ou si une autre erreur est détectée, le code retour est -1.

&RT La fonction retourne 0 si l'imprimante est disponible, 1 s'il
    manque de papier, -1 en cas d'erreur (pas de connexion, time out,
    imprimante occupée)

&EX
    if(PciParalInit(1)) {
	Message("Imprimante indisponible");
	return(-1);
	}
    fd = fopen("lpt1:", "w+");
    fprintf(fd, "...");
&TX

&SA PciParalInit()
====================================================================== */

PciParalInit(nb)
int     nb;
{
#if defined(DOS) && !defined(DOSW32) && !defined(WIN32)
    switch(PciParalInt86(0x01, nb)) {
	case 0 :
	case 144 : return(0);
	case 176 : return(1);
	default  : return(-1);
	}
#else
    return(-1);
#endif
}


/* =====================================================================
Teste l'imprimante parallèle nb (1 pour lpt1, 2 pour lpt2, 3 pour lpt3).
Cette fonction peut être utilisée avant de lancer une impression
sur une imprimante parallèle. En effet, en cas d'absence d'imprimante,
le programme en cours se bloque et ne peut bien souvent être relancé que
par un reboot du système.

Si l'imprimante est connectée et online, la fonction retourne 0. Si elle
n'a plus de papier, le code retour est 1. Si aucune imprimante n'est
présente ou si une autre erreur est détectée, le code retour est -1.

&RT La fonction retourne 0 si l'imprimante est disponible, 1 s'il
    manque de papier, -1 en cas d'erreur (pas de connexion, time out,
    imprimante occupée)

&EX
    if(PciParalTest(1)) {
	Message("Imprimante indisponible");
	return(-1);
	}
    fd = fopen("lpt1:", "w+");
    fprintf(fd, "...");
&TX

&SA PciParalInit()
====================================================================== */

PciParalTest(nb)
int     nb;
{
#if defined(DOS) && !defined(DOSW32) && !defined(WIN32)
    switch(PciParalInt86(0x02, nb)) {
	case 144 : return(0);
	case 176 : return(1);
	default  : return(-1);
	}
#else
    return(-1);
#endif
}

/* =====================================================================
Teste la disponibilité de l'imprimante parallèle dont le nom est
fourni dans name. Si ce nom commence par

&EN LPT1
&EN LPT2
&EN LPT3
&EN PRN

(en majuscule ou minuscule), la fonction teste l'imprimante
correspondante à l'aide de PciParalTest().

Si l'imprimante est connectée et online, la fonction retourne 0. Si elle
n'a plus de papier, le code retour est 1. Si aucune imprimante n'est
présente ou si une autre erreur est détectée, le code retour est -1.

Si le nom n'est pas celui d'une imprimante parallèle, elle retourne
0 également.


&RT La fonction retourne 0 si l'imprimante est disponible, 1 s'il
    manque de papier, -1 en cas d'erreur (pas de connexion, time out,
    imprimante occupée)

&EX
    RC_set_prname(pr, name)
    PR_DEF  *pr;
    char    *name;
    {
	if(PrTestLPT(name)) {
	    PG_display_error("Imprimante %s non disponible", name);
	    return(-1);
	    }
	return(0);
    }
&TX

&SA PciParalInit()
====================================================================== */

PciTestLPT(name)
char    *name;
{
#if defined(DOS) && !defined(DOSW32)  && !defined(WIN32)
    int     pr = 0;
    char    buf[80];

    SCR_strlcpy(buf, name, 4);
    SCR_upper(buf);
    if(memcmp(buf, "PRN", 3) == 0) pr = 1;
    else if(memcmp(buf, "LPT1", 4) == 0) pr = 1;
    else if(memcmp(buf, "LPT2", 4) == 0) pr = 2;
    else if(memcmp(buf, "LPT3", 4) == 0) pr = 3;
    if(pr) return(PciParalTest(pr));
#endif /* DOS && ! DOSW32 */
    return(0);
}

#endif
