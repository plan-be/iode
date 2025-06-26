#include <s_swap.h>
#ifndef NOEMS
#include <dos.h>
#include <stdlib.h>
#include <string.h>

#ifndef MK_FP
#define MK_FP(seg, ofs)     ((void far *) (unsigned long) (seg)<<16 | (ofs)))
#endif

#define EMS_INT         0x67

EMSDEF  EMS_DEF;

/*
Initialise la structure globale EMS_DEF.

&RT nombre de pages EMS disponibles ou -1 si toutes les
pages sont déjà utilisées ou si le driver EMM n'est pas installé.

&SA EMS_end()
*/

EMS_init()
{
    union REGS      regs;

    /* Installed ? */
    if(!EMS_inst()) return(EMS_DEF.swerrno = -1);

    /* Number of pages (total and free) */
    regs.h.ah = 0x42;
    int86(EMS_INT, &regs, &regs);
    if(EMS_DEF.swerrno = regs.h.ah) return(-1);
    EMS_DEF.nb_pages = regs.x.dx;
    EMS_DEF.nb_free  = regs.x.bx;

    /* Physical pointer */
    regs.h.ah = 0x41;
    int86(EMS_INT, &regs, &regs);
    if(EMS_DEF.swerrno = regs.h.ah) return(-1);
    EMS_DEF.ptr = (unsigned long)(regs.x.bx) << 16;

    /* EMS Version */
    regs.h.ah = 0x46;
    int86(EMS_INT, &regs, &regs);
    if(EMS_DEF.swerrno = regs.h.ah) return(-1);
    EMS_DEF.ver = (regs.h.al & 15) + (regs.h.al >> 4) * 10;

    /* ALLOC ALL FREE PAGES */
    regs.h.ah = 0x43;
    regs.x.bx = EMS_DEF.nb_free;
    int86(EMS_INT, &regs, &regs);
    if(EMS_DEF.swerrno = regs.h.ah) return(-1);
    EMS_DEF.handle   = regs.x.dx;
    EMS_DEF.nb_alloc = EMS_DEF.nb_free;

    return(0);
}

/*
Libère toutes les pages EMS allouées.

&RT -1 en cas d'erreur, 0 sinon.
*/

EMS_end()
{
    union REGS      regs;

    if(EMS_DEF.handle == 0 || EMS_DEF.nb_alloc == 0) return(0);
    regs.h.ah = 0x45;
    regs.x.dx = EMS_DEF.handle;
    int86(EMS_INT, &regs, &regs);
    if(EMS_DEF.swerrno = regs.h.ah) return(-1);
    EMS_DEF.handle   = 0;
    EMS_DEF.nb_alloc = 0;
    return(0);
}

/*
Mappe la page logique logique (16K) sur la page physique phys (0-3)

&RT -1 en cas d'erreur, 0 sinon.
*/

EMS_map(logp, physp)
int     logp, physp;
{
    union REGS      regs;

    if(physp < 0 || physp > 3) return(-1);
    if(logp  < 0 || logp >= EMS_DEF.nb_alloc) return(-1);
    if(EMS_DEF.handle == 0 || EMS_DEF.nb_alloc == 0) return(-1);
    regs.h.ah = 0x44;
    regs.h.al = physp;
    regs.x.bx = logp;
    regs.x.dx = EMS_DEF.handle;
    int86(EMS_INT, &regs, &regs);
    if(EMS_DEF.swerrno = regs.h.ah) return(-1);
    EMS_DEF.log_pgs[physp] = logp;
    return(0);
}

/*
Retourne un pointeur statique un des messages d'erreur du système EMS.
Le numéro de la dernière erreur se trouve dans la structure globale
EMS_DEF et peut être obtenu par
&CO
    EMS_DEF.swerrno
&TX

Les messages possibles sont les suivants :

&EN EMM driver error
&EN EMS harware error
&EN Incorrect EMS handle
&EN Unknown EMS function
&EN No handle free
&EN Mapping error
&EN No enough EMS pages
&EN No enough free EMS pages
&EN No page requested
&EN Incorrect logical page
&EN Incorrect physical page
&EN Mapping space full
&EN Mapping already saved
&EN No mapping save

*/

char *EMS_error_text(n)
int     n;
{
    static char undef[] = "Unknown error";
    static char *errors[] = {
	"EMM driver error",
	"EMS harware error",
	undef,
	"Incorrect EMS handle",
	"Unknown EMS function",
	"No handle free",
	"Mapping error",
	"No enough EMS pages",
	"No enough free EMS pages",
	"No page requested",
	"Incorrect logical page",
	"Incorrect physical page",
	"Mapping space full",
	"Mapping already saved",
	"No mapping saved"
	};

    return((n < 0x80 || n > 0x8E) ? undef : errors[n - 0x80]);
}

/*
Vérifie qu'un driver EMM est installé (en général via le fichier
config.sys).

&RT 1 si EMS installé, 0 sinon.
*/

EMS_inst()
{
    union REGS      regs;
    struct SREGS    sregs;
    char            *ptr;

    regs.x.ax = 0x3567;
    intdosx(&regs, &regs, &sregs);
    ptr = (char *) MK_FP(sregs.es, 10);
    return(memcmp(ptr, "EMMXXXX0", 8) == 0);
}
/*NH
main()
{
    if(EMS_init()) {
	printf("%s\n", EMS_error_text());
	return(-1);
	}
    printf("Nombre de pages total  : %d\n", EMS_DEF.nb_pages);
    printf("Nombre de pages libres : %d\n", EMS_DEF.nb_free);
    printf("Pointeur première page : %p\n", EMS_DEF.ptr);
    printf("Version EMM driver     : %d\n", EMS_DEF.ver);
    EMS_end();
}
*/

#endif /*NOEMS*/

