#include "scr4.h"
#ifndef UNIX
#include <dos.h>

#if defined(SCRW32) || defined(DOSW32)
#include "scr4w.h"
#endif

/* =====================================================================
Recherche la taille d'un disque et l'espace disponible sur ce disque (ou
disquette). Le numéro du disque est 0 pour le disque courant, 1 pour A:,
2 pour B:, etc.

Les valeurs sont retournées dans les arguments total et free.

&RT La fonction retourne -1 si le disque n'est pas accessible, 0 sinon.

&EX

    long    total, free;

    if(PciGetDriveInfo(disk, &total, &free))
	printf("Drive nb %c not ready\n", disk);
    else
	printf("Drive nb %c : %ld/%ld bytes free\n", disk, free, total);
&TX

&SA PciGetInfos(), PciGetDriveInfoKB()
====================================================================== */

PciGetDriveInfo(disk, total, free)
int             disk;
unsigned long   *total, *free;
{
#if defined(DOS) && !defined(SCRWIN) && !defined(DOSW32)
    struct diskfree_t   sleft;

    *total = *free = 0;
    if(_dos_getdiskfree(disk, &sleft)) return(-1);
    *free = (long) sleft.avail_clusters *
	    (long) sleft.bytes_per_sector *
	    (long) sleft.sectors_per_cluster;
    *total= (long) sleft.total_clusters *
	    (long) sleft.bytes_per_sector *
	    (long) sleft.sectors_per_cluster;
#endif
#if defined(SCRW32) || defined(DOSW32) /* JMP 17-08-07 */
    unsigned long    spcl, bps, fcl, acl; /* JMP 12-01-98 */
    char    buf[10];

    strcpy(buf, "C:\\");
    buf[0] = 'A' + disk - 1;
    *total = *free = 0;
    if(GetDiskFreeSpace(buf, &spcl, &bps, &fcl, &acl) == 0) return(-1); /* JMP 17-08-07 */
//    Debug("%s : %ld - %ld - %ld - %ld\n", buf, spcl, bps, fcl, acl);
    *free  = fcl * bps * spcl;
    *total = acl * bps * spcl;
//    Debug("  Free : %uld - Total : %uld\n", *free, *total);
#endif
    return(0);
}

/* =====================================================================
Recherche la taille d'un disque et l'espace disponible sur ce disque (ou
disquette). Le numéro du disque est 0 pour le disque courant, 1 pour A:,
2 pour B:, etc.

Les valeurs sont retournées dans les arguments total, free, KBtotal et
KBfree.

Cette fonction est une extension de la fonction PciGetDriveInfo()
pour le cas de disques supérieurs à 2 GB.

&RT La fonction retourne -1 si le disque n'est pas accessible, 0 sinon.

&SA PciGetInfos(), PciGetDriveInfo()
====================================================================== */

PciGetDriveInfoKB(disk, total, free, KBtotal, KBfree)
int             disk;
unsigned long   *total, *free, *KBtotal, *KBfree;
{
#if defined(DOS) && !defined(SCRWIN) && !defined(DOSW32)
    struct diskfree_t   sleft;

    *total = *free = 0;
    if(_dos_getdiskfree(disk, &sleft)) return(-1);
    *free = (long) sleft.avail_clusters *
	    (long) sleft.bytes_per_sector *
	    (long) sleft.sectors_per_cluster;
    *total= (long) sleft.total_clusters *
	    (long) sleft.bytes_per_sector *
	    (long) sleft.sectors_per_cluster;
    *KBfree = (long) sleft.avail_clusters *
	      (long) sleft.sectors_per_cluster;
    *KBfree = (*KBfree / 8L) * ((long) sleft.bytes_per_sector / 128L);
    *KBtotal= (long) sleft.total_clusters *
	    (long) sleft.sectors_per_cluster;
    *KBtotal= (*KBtotal / 8L) * ((long) sleft.bytes_per_sector / 128L);
#endif
#if defined(SCRW32) || defined(DOSW32) /* JMP 17-08-07 */
    unsigned long    spcl, bps, fcl, acl; /* JMP 12-01-98 */
    char    buf[10];

    strcpy(buf, "C:\\");
    buf[0] = 'A' + disk - 1;
    *total = *free = 0;
    if(GetDiskFreeSpace(buf, &spcl, &bps, &fcl, &acl) == 0) return(-1); /* JMP 17-08-07 */
    *free  = fcl * bps * spcl;
    *total = acl * bps * spcl;
    *KBfree  = fcl * spcl;
    *KBfree  = (*KBfree / 8L) * (bps / 128L);
    *KBtotal = acl * spcl;
    *KBtotal = (*KBtotal / 8L) * (bps / 128L);
#endif
    return(0);
}

typedef struct _pcd_ {
    unsigned int    nprts  : 2;
    unsigned int    serprt : 1;
    unsigned int    gameio : 1;
    unsigned int    ncoms  : 3;
    unsigned int    dma    : 1;
    unsigned int    ndisks : 2;
    unsigned int    video  : 2;
    unsigned int    ramsize: 2;
    unsigned int    fpu    : 1;
    unsigned int    boot   : 1;
} PCD;

/* =====================================================================
Analyse le PC et retourne dans une structure PCINFOS différentes
informations utiles notamment pour une installation de programme.

La structure PCINFOS doit être passée comme paramètre et est modifiée
par la fonction. Ses éléments sont définis comme suit :

&CO
##define MAXFDISKS  26

typedef struct _pcfdisk_ {
    char          drive;               Lettre identifiant le drive
    unsigned long total;               Espace total du disque (bytes)
    unsigned long free;                Espace restant sur le disque (bytes)
    unsigned long KBtotal;             Espace total du disque (Kbytes)
    unsigned long KBfree;              Espace restant sur le disque (Kbytes)
} PCFDISK;

typedef struct _pcinfos_ {
    short   fpu;                 1 si un processeur math. est présent
    short   nbfdisks;            Nbre de disques logiques (fixes)
    PCFDISK fdisks[MAXFDISKS];   Infos sur chaque disque
    short   lpts[3];             Etat de chaque imprimante parallèle
    short   nbcoms;              Nombre de portes séries COMx
} PCINFOS;
&TX


&EX
    PCINFOS         pci;
    int             i;

    PciGetInfos(&pci);
    printf("Nb coms  : %d\n", pci.nbcoms);
    printf("FPU      : %d\n", pci.fpu);
    printf("Nb disks : %d\n", pci.nbfdisks);
    for(i = 0 ; i < pci.nbfdisks ; i++)
	printf("Disk %c : %ld/%ld bytes free\n", pci.fdisks[i].drive,
						 pci.fdisks[i].free,
						 pci.fdisks[i].total);
    for(i = 0 ; i < 3 ; i++) {
	printf("LPT%d    : ", i + 1);
	switch(pci.lpts[i]) {
	    case 1  : printf("Out of paper\n"); break;
	    case -1 : printf("Not present\n"); break;
	    case 0  : printf("Ready\n"); break;
	    }
	}
&TX

&SA PciGetDriveInfo(), PciParalTest()
====================================================================== */

int PciGetInfos(pci)
PCINFOS *pci;
{
    unsigned int    upcd;
    PCD             *pcd = (PCD *)&upcd;
    int             i, nb = 0;
    unsigned long   free, total, KBtotal, KBfree; /* JMP 12-01-98 */

#if defined(DOS) && !defined(SCRWIN) && !defined(DOSW32)
    upcd = _bios_equiplist();
    pci->fpu = pcd->fpu;
    pci->nbcoms = pcd->ncoms;
#endif

    for(i = 3 ; i <= MAXFDISKS ; i++) {
	if(PciGetDriveInfoKB(i, &total, &free, &KBtotal, &KBfree)) continue;
	pci->fdisks[nb].drive = 'A' + i - 1;
	pci->fdisks[nb].total = total;
	pci->fdisks[nb].free  = free;
	pci->fdisks[nb].KBtotal = KBtotal;
	pci->fdisks[nb].KBfree  = KBfree;
	nb++;
	}
    pci->nbfdisks = nb;
    for(i = 0 ; i < 3 ; i++)
	pci->lpts[i] = PciParalTest(i + 1);

    return(0);
}

#endif

