#if defined(DOSW32) || defined(SCRW32)

#include <scr4w.h>


long    WSCR_TIMER_DELAY = 1000L;
int     (*WSCR_TIMER_FN)() = 0;
DWORD   WSCR_LASTKEY_TIME = 0;
extern  VOID CALLBACK WscrCheckTimer(HWND, UINT, UINT, DWORD);

/* BP_M 10-12-2010 09:56 */
extern S4C **ISC_S4C;
extern int ISC_NB_CONN;

WscrSetTimer(ms)
long     ms;
{
    static int WSCR_SET_TIMER = 0;
    int     i;

    if(ms == 0) {
	if(WSCR_SET_TIMER)
	    KillTimer(0, WSCR_SET_TIMER);
	WSCR_SET_TIMER = 0;
	ISC_StopAllTimer(); /* BP_M 23-12-2010 16:15 */
    }
    else {
	if(WSCR_SET_TIMER) WscrSetTimer(0);
	WSCR_SET_TIMER = SetTimer(0, 2, ms, (TIMERPROC)WscrCheckTimer);
	WSCR_TIMER_DELAY = ms;
	ISC_StartAllTimer();
    }

    return(0);
}

int     SCR_WTIMER_REENTR;  /* BP_M 07-11-2011 16:10 */

/* BP_M 12-12-2011 11:51 */
VOID CALLBACK WscrCheckTimer(HWND hwnd, UINT umsg, UINT idevent, DWORD lkt)
{
    static  int WSCR_CHECK_LKT = 1;
    extern  int ACT_lscreen_fn;
    long    l;
    int     i;

    ISC_check_read();   /* BP_M 08-02-2008 */
    if(WSCR_CHECK_LKT) {
	if(SCR_LOCK_TIME) {
	    if(WSCR_LASTKEY_TIME == 0) WSCR_LASTKEY_TIME = lkt;
	    else {
		if(WSCR_LASTKEY_TIME + (long)SCR_LOCK_TIME * 1000L <= lkt) {    /* BP_M 12-12-2011 11:51 */
		    if(!SCR_WTIMER_REENTR)  /* BP_M 07-11-2011 16:10 */
			WSCR_CHECK_LKT = 0;
		    ACT_exec_nu(ACT_lscreen_fn);
		    WSCR_CHECK_LKT = 1;
		    WSCR_LASTKEY_TIME = GetTickCount();
		}
	    }
	}
	if(WSCR_TIMER_FN) {
	    if((*WSCR_TIMER_FN)() != 0) {
		KillTimer(0, 1);
		SCR_end();
		exit(0);
	    }
	}
    }
    if(SCR_END_TIME_1) {
	l = _SCR_current_time();
	if(l >= SCR_END_TIME_1 && l <= SCR_END_TIME_2) {
	    KillTimer(0, 1);
	    SCR_end();
	    IS_end_database_1();  /* BP_M 18-03-2008 */
	    exit(0);
	}
    }
}

/********* KEYBOARD TIMER **************/
extern int *SCR_REC_KEYS;
extern int SCR_NB_REC_KEYS;
int WSCR_KEYBOARD_TIMER;
int SCR_ENTER2TAB = 1;      /* BP_M 01-04-2003 09:44 */

VOID CALLBACK WscrKeyboardTimerFn(HWND hwnd, UINT umsg, UINT idevent, DWORD lkt)
{
    int     key, wait;

ag:
    if(SCR_NB_REC_KEYS == 0) return;
    SCR_NB_REC_KEYS--;
    key = SCR_REC_KEYS[SCR_NB_REC_KEYS];

    /* BP_M 01-02-2012 13:12 */
    if(key >= SCR_WAIT_KEY && key <= SCR_WAIT_KEY + 1000) {
	wait = key - SCR_WAIT_KEY;
	if(wait == 0) wait = 1;
	SCR_sleep(wait);
	goto ag;
    }

//    Debug("Key : %d\n", key);         /* BP_M 27-02-2003 12:51 */
    if(key == SCR_ENTER && SCR_ENTER2TAB)       /* BP_M 01-04-2003 09:44 */
	WscrRecordKey2(SCR_TAB);
//  else if(key == SCR_F3)
//      WscrRecordKey2(SCR_ESCAPE);
    else  {
	WscrRecordKey2(key);
	SCR_wdispatch();
	SCR_sleep(1);
    }

    if(key > ' ' && key < 255) goto ag;     /* BP_M 13-09-2000 */
}

WscrSetKeyboardTimer()
{
    WSCR_KEYBOARD_TIMER = SetTimer(0, 3, 100, (TIMERPROC)WscrKeyboardTimerFn);
}

WscrKillKeyboardTimer()
{
    KillTimer(0, WSCR_KEYBOARD_TIMER);
}

#endif /* DOSW32 || SCRW32 */


