#if defined(SCRW32) || defined(DOSW32)
#include "scr4w.h"
#include <tlhelp32.h> // needed for tool help declarations

// Type definitions for pointers to call tool help functions.
typedef BOOL (WINAPI *MODULEWALK)(HANDLE hSnapshot,
    LPMODULEENTRY32 lpme);
typedef BOOL (WINAPI *THREADWALK)(HANDLE hSnapshot,
    LPTHREADENTRY32 lpte);
typedef BOOL (WINAPI *PROCESSWALK)(HANDLE hSnapshot,
    LPPROCESSENTRY32 lppe);
typedef HANDLE (WINAPI *CREATESNAPSHOT)(DWORD dwFlags,
    DWORD th32ProcessID);

/*NH*/
// File scope globals. These pointers are declared because of the need
// to dynamically link to the functions.  They are exported only by
// the Windows 95 kernel. Explicitly linking to them will make this
// application unloadable in Microsoft(R) Windows NT(TM) and will
// produce an ugly system dialog box.

static CREATESNAPSHOT pCreateToolhelp32Snapshot = NULL;
static MODULEWALK  pModule32First  = NULL;
static MODULEWALK  pModule32Next   = NULL;
static PROCESSWALK pProcess32First = NULL;
static PROCESSWALK pProcess32Next  = NULL;

static THREADWALK  pThread32First  = NULL;
static THREADWALK  pThread32Next   = NULL;

/*NH Function that initializes tool help functions. */
WscrInitToolhelp32()
{
    HANDLE hKernel;

    // Obtain the module handle of the kernel to retrieve addresses of
    // the tool helper functions.
    hKernel = GetModuleHandle("KERNEL32.DLL");

    if(hKernel == 0) return(-1);
    pCreateToolhelp32Snapshot =
	(CREATESNAPSHOT)GetProcAddress(hKernel,
	"CreateToolhelp32Snapshot");

    pModule32First  = (MODULEWALK)GetProcAddress(hKernel,
	"Module32First");
    pModule32Next   = (MODULEWALK)GetProcAddress(hKernel,
	"Module32Next");

    pProcess32First = (PROCESSWALK)GetProcAddress(hKernel,
	"Process32First");
    pProcess32Next  = (PROCESSWALK)GetProcAddress(hKernel,
	"Process32Next");

    pThread32First  = (THREADWALK)GetProcAddress(hKernel,
	"Thread32First");

    pThread32Next   = (THREADWALK)GetProcAddress(hKernel,
	"Thread32Next");

    // All addresses must be non-NULL to be successful.
    // If one of these addresses is NULL, one of
    // the needed lists cannot be walked.
    if(pModule32First && pModule32Next  && pProcess32First &&
	    pProcess32Next && pThread32First && pThread32Next &&
	    pCreateToolhelp32Snapshot) return(0);

    return(-1);
}

/* ==================================================================
Cette fonction retourne le nombre de fois que le programme ExeFile
tourne sur la machine.

&SA WscrGetCurrentProcessCount();
===================================================================== */

WscrGetProcessCount(char *ExeFile)
{
    HANDLE         hSnapshot;
    PROCESSENTRY32 pe32      = {0};
    int            rc        = 0;

    if(WscrInitToolhelp32()) return(-1);

    //  Take a snapshot of all processes currently in the system.
    hSnapshot = pCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hSnapshot == (HANDLE)-1) return(-1);

    //  Fill in the size of the structure before using it.
    pe32.dwSize = sizeof(PROCESSENTRY32);

    //  Walk the snapshot of the processes, and for each process, get
    //  information to display.
    if(pProcess32First(hSnapshot, &pe32) == 0) rc = -1;
    while(1) {
	if(SCR_cstrcmp(pe32.szExeFile, ExeFile) == 0) rc++;
	if(pProcess32Next(hSnapshot, &pe32) == 0) break;
    }

    // Do not forget to clean up the snapshot object.
    CloseHandle (hSnapshot);
    return(rc);
}

/* ==================================================================
Cette fonction retourne le nombre de fois que le programme courant
tourne sur la machine.

&SA WscrGetCurrentProcessCount();
===================================================================== */

WscrGetCurrentProcessCount()
{
    char           ExeFile[256];

    GetModuleFileName(hInst, ExeFile, 255);
    return(WscrGetProcessCount(ExeFile));
}

/*
WscrGetProcessList()
{
    HANDLE         hSnapshot = NULL;
    PROCESSENTRY32 pe32      = {0};
    int            rc        = 0;

    if(WscrInitToolhelp32()) return(-1);

    //  Take a snapshot of all processes currently in the system.
    hSnapshot = pCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hSnapshot == (HANDLE)-1) return(-1);

    //  Fill in the size of the structure before using it.
    pe32.dwSize = sizeof(PROCESSENTRY32);

    //  Walk the snapshot of the processes, and for each process, get
    //  information to display.
    if(pProcess32First(hSnapshot, &pe32) == 0) rc = -1;
    while(1) {
//        Debug("Count=%uld\tPID=%uld\tPPID=%uld\t%s\n",
//            pe32.cntUsage,
//            pe32.th32ProcessID,
//            pe32.th32ParentProcessID,
//            pe32.szExeFile);
	if(pProcess32Next(hSnapshot, &pe32) == 0) break;
    }

    // Do not forget to clean up the snapshot object.
    CloseHandle(hSnapshot);
    return(rc);
}
*/



#endif





