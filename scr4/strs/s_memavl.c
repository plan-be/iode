
/* =============================================================
Cette fonction retourne l'espace mémoire disponible en bytes.

&NO En DOS, la valeur ne correspond pas à l'espace total libre : il
s'agit d'un minimum disponible qui correspond au pointeur le plus éloigné
alloué et pas encore libéré.

&EX
    printf("Mémoire libre : %ld\n", SCR_free_mem());
&TX
&PO Cette fonction ne fournit un résultat qu'en DOS avec les
    compilateur Turbo C ou Watcom C. Dans tous les autres cas, elle
    retourne une valeur nulle.

============================================================== */

#ifdef UNIX
long SCR_free_mem()
{
    return(0L);
}
#endif

#ifdef DOS
#ifdef MSC
long SCR_free_mem()
{
    return(0L);
}
#else
#ifdef WATCOM
#include <i86.h>
#include <dos.h>
#define DPMI_INT    0x31

struct _meminfo_ {
	unsigned LargestBlockAvail;
	unsigned MaxUnlockedPage;
	unsigned LargestLockablePage;
	unsigned LinAddrSpace;
	unsigned NumFreePagesAvail;
	unsigned NumPhysicalPagesFree;
	unsigned TotalPhysicalPages;
	unsigned FreeLinAddrSpace;
	unsigned SizeOfPageFile;
	unsigned Reserved[3];
} SCR_MemInfo;
union REGS  SCR_Memregs;
struct SREGS SCR_Memsregs;

long SCR_free_mem()
{
    SCR_Memregs.x.eax = 0x00000500;
    SCR_Memsregs.es = FP_SEG(&SCR_MemInfo);
    SCR_Memregs.x.edi = FP_OFF(&SCR_MemInfo);
    int386x(DPMI_INT, &SCR_Memregs, &SCR_Memregs, &SCR_Memsregs);

    return((long)(_memavl() + SCR_MemInfo.LargestBlockAvail));

/*  return(_memavl()); */
}
#else

#if defined(WINDOWS) || defined(DOSWIN) /* JMP 4.23 13-12-95 */
    #if !defined(DOSW32) && !defined(SCRW32)
    long SCR_free_mem()                     /* JMP_M 4.21 05-11-95 */
    {
	long _far _pascal GetFreeSpace(); /* JMP_M 4.21 05-11-95 */

	return(GetFreeSpace(0));          /* JMP_M 4.21 05-11-95 */
    }
    #else
    #include <windows.h>
    long SCR_free_mem()                     /* JMP_M 4.21 05-11-95 */
    {
	MEMORYSTATUS    mst;

	mst.dwLength = sizeof(DWORD);
	GlobalMemoryStatus(&mst);
       // return(mst.dwAvailPhys + mst.dwAvailPageFile); /* JMP 23-01-07 */
	return((long)mst.dwAvailPhys); /* JMP 23-01-07 */
/*        return(mst.dwAvailPhys + mst.dwAvailVirtual); */
	/*return(mst.dwAvailPhys); */
    }
    #endif
#else

/* =============================================================
Cette fonction retourne l'espace mémoire disponible en bytes.

&NO En DOS, la valeur ne correspond pas à l'espace total libre : il
s'agit d'un minimum disponible qui correspond au pointeur le plus éloigné
alloué et pas encore libéré.

&EX
    printf("Mémoire libre : %ld\n", SCR_free_mem());
&TX
&PO Cette fonction ne fournit un résultat qu'en DOS avec les
    compilateur Turbo C ou Watcom C. Dans tous les autres cas, elle
    retourne une valeur nulle.

============================================================== */

long SCR_free_mem()
{
    extern long coreleft();

    return(coreleft());
}
#endif
#endif
#endif
#endif


