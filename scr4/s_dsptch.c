/******************* SCR_wdispatch *************************/

/*********************** WINDOWS (SCRW32 & DOSW32) ****************************/

#if defined(SCRW32) || defined(DOSW32)
#include <windows.h>
#define SCRSUPPROTO
#include "scr4.h"

SCR_wdispatch()
{
    MSG     msg;

    while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
	TranslateMessage(&msg);
	DispatchMessage(&msg);
	}
}


#endif

/*************** GNOME *******************/

#if defined(SCRGNOME) && !defined(X11)

SCR_wdispatch()
{
    while(gtk_events_pending())
	gtk_main_iteration();

}

#endif


/*************** X11 *******************/

#if defined(X11)

SCR_wdispatch()
{
}

#endif

/*************** UNIX TERM *******************/

#if defined(UNIX) && !defined(SCRGNOME) && !defined(X11)

SCR_wdispatch()
{
}

#endif









