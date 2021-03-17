#if defined(SCRW32) || defined(DOSW32)

#include "scr4w.h"
#include <psapi.h>


// GetProcessTimes()

unsigned long WscrGetCurrentPID()
{
    return(GetCurrentProcessId());
}

unsigned long WscrGetMemorySize(DWORD pid)
{
    HANDLE hProcess;
    PROCESS_MEMORY_COUNTERS pmc;
    unsigned long memsize;

    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
				    PROCESS_VM_READ,
				    FALSE, pid);
    if(NULL == hProcess) return(0);

    if(GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
	memsize = pmc.WorkingSetSize;
     // memsize = pmc.PeakWorkingSetSize;
    }
    CloseHandle(hProcess);
    return(memsize);
}

#endif
