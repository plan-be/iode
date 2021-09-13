#if defined(SCRW32) || defined(DOSW32)

#include "scr4w.h"
#include <tlhelp32.h>

/***********************************************************************
Retourne la liste des process tournant sur le PC sous forme d'une liste
termin‚e par un NULL.

La fonction PSFreeList(PSDESCR **) libŠre l'espace allou‚.

&RT Un tableau allou‚ avec la liste des process ou NULL en cas d'erreur

&EX
    #include <scr4w.h>
    ...
    PrintProcessList()
    {
	PSDESCR **psd;
	int     i;

	psd = WscrGetPSList();
	if(psd == 0) {
	    printf("Erreur WscrGetPSList()\n");
	    return(-1);
	    }
	for(i = 0 ; psd[i] ; i++) {
	    printf( "PID=%-7d", psd[i]->pid);
	    printf( "PPID=%-7d", psd[i]->ppid);
	    printf( "Priority=%-3d", psd[i]->prior);
	    printf( "Threads=%-3d", psd[i]->nbthreads);
	    printf( "Module=%s\n", psd[i]->exefile);
	    }
	WscrFreePSList(psd);
	return(0);
    }
&TX

&SA PSFreeList()
************************************************************************/

PSDESCR **WscrGetPSList()
{
    HANDLE          hProcessSnap = NULL, hProcess;
    PROCESSENTRY32  pe32      = {0};
    PSDESCR         **psd = 0;
    int             nb = 0;

     //  Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
	return (FALSE);

    //  Fill in the size of the structure before using it.
    pe32.dwSize = sizeof(PROCESSENTRY32);

    //  Walk the snapshot of the processes, and for each process,
    //  display information.
    if(Process32First(hProcessSnap, &pe32) == 0) return(psd);

    while(1) {
	if(nb % 400 == 0)
	    psd = (PSDESCR **)SCR_realloc(psd, sizeof(PSDESCR*), nb, nb + 500);
	psd[nb] = (PSDESCR *) SCR_malloc(sizeof(PSDESCR));
	psd[nb]->pid = pe32.th32ProcessID;
	psd[nb]->ppid = pe32.th32ParentProcessID;
	psd[nb]->prior = pe32.pcPriClassBase;
	psd[nb]->nbthreads = pe32.cntThreads;
	psd[nb]->exefile = SCR_stracpy(pe32.szExeFile);
	nb++;
	if(Process32Next(hProcessSnap, &pe32) == 0) break;
    }
    psd[nb] = 0;
    CloseHandle (hProcessSnap);
    return(psd);
}

/***********************************************************************
Free une liste de process retourn‚e par WscrGetPSList().

&SA WscrGetPSList()
************************************************************************/

WscrFreePSList(PSDESCR **psd)
{
    int     i;

    if(psd == 0) return;
    for(i = 0 ; psd[i] ; i++)
	SCR_free(psd[i]->exefile);

    SCR_free_tbl(psd);
}

/***********************************************************************
Recherche le nombre d'occurence d'un process en m‚moire. Le nom du process est
celui de l'ex‚cutable (‚ventuellement sans le directory, mais avec .exe).

&RT -1 en cas d'erreur, 0 si le process ne tourne pas, le nombre d'occurence sinon

&EX
    WscrFindPS("winword.exe");
&TX

&SA WscrGetPSList()
************************************************************************/

WscrFindPS(char *exename)
{
    PSDESCR **psd;
    int     i, lg = strlen(exename), lg2, count = 0;

    psd = WscrGetPSList();
    if(psd == 0) return(-1);
    for(i = 0 ; psd[i] ; i++) {
	lg2 = strlen(psd[i]->exefile);
	if(lg2 < lg) continue;
	if(SCR_cstrcmp(exename, psd[i]->exefile + lg2 - lg) == 0) count++;
    }
    WscrFreePSList(psd);
    return(count);
}

/*
main()
{
	PSDESCR **psd;
	int     i;

	psd = WscrGetPSList();
	if(psd == 0) {
	    printf("Erreur WscrGetPSList()\n");
	    return(-1);
	    }
	for(i = 0 ; psd[i] ; i++) {
	    printf( "PID=%-7d", psd[i]->pid);
	    printf( "PPID=%-7d", psd[i]->ppid);
	    printf( "Priority=%-3d", psd[i]->prior);
	    printf( "Threads=%-3d", psd[i]->nbthreads);
	    printf( "Module=%s\n", psd[i]->exefile);
	    }
	WscrFreePSList(psd);
	return(0);


}
*/

#endif




