/* DEFINES */
/* ======= */

#define VERSION "I.O.D.E. 6.63"
#ifdef DOSW32
#define HELPFILE "iode.chm"
#else
#define HELPFILE "iode.chm"
#endif
#include "../api/vers.h"

GLOBAL WIN_HLINE No
GLOBAL WIN_VLINE No
GLOBAL WIN_SKIP 2

/* LICENCE DEFINITIONS */
/* =================== */

GLOBAL {
    USER_NAME   "Federal Planning Bureau - Brussels"
#ifdef DOS
    APPL_NAME   "I.O.D.E. 6.63 - Econometric development tools (c) FPB 90-2018"
#endif
#ifdef WATCOM
    APPL_NAME   "I.O.D.E. 6.63 for Dos32 - Econometric development tools (c) FPB 90-2018"
#endif
#ifdef DOSWIN
    APPL_NAME   "I.O.D.E. 6.63 for Windows - Econometric development tools (c) FPB 90-2018"
#endif
#ifdef DOSW32
    APPL_NAME   "I.O.D.E. 6.63 for Win32 - Econometric development tools (c) FPB 90-2018"
#endif
#ifdef DOSW32D
    APPL_NAME   "I.O.D.E. 6.63 for Win32/Double - Econometric development tools (c) FPB 90-2018"
#endif
#ifdef SCRW32
    APPL_NAME   "I.O.D.E. 6.63 for Windows 95 - Econometric development tools (c) FPB 90-2018"
#endif
#ifdef UNIX
    APPL_NAME   "I.O.D.E. 6.63 - Econometric development tools (c) FPB 90-2018"
#endif

/*  SERIAL_NB   "BUP01" */
/*  LIM_DATE    19931231 */

    KEY_POS     1  84
    CLOCK_POS   29 75
/*    DATE_POS    24 62 */
    // MEM_POS     29 70 /* JMP 04-02-11 */
    DEL_EOF     YES
    CLOSEBOX    YES
    PAGE_SHADOW 2
}

/* BUTTONS */
/* ======= */

FNKEY _fk_esc {ENTER {Record {ESCAPE}}}
FNKEY _fk_F1  {ENTER {Record {HELP}}}
FNKEY _fk_F2  {ENTER {Record {F2}}}
FNKEY _fk_F3  {ENTER {Record {F3}}}
FNKEY _fk_F4  {ENTER {Record {F4}}}
FNKEY _fk_F5  {ENTER {Record {F5}}}
FNKEY _fk_F6  {ENTER {Record {F6}}}
FNKEY _fk_F7  {ENTER {Record {F7}}}
FNKEY _fk_F8  {ENTER {Record {F8}}}
FNKEY _fk_F9  {ENTER {Record {F9}}}
FNKEY _fk_F10 {ENTER {Record {F10}}}
FNKEY _fk_all {F1 "Help" ESCAPE {ABORT} "Cancel" F10 "All" F2 "Cmts" F3 "Eqs" F4 "Idts" F5 "Lsts" F6 "Scls" F7 "Tbls" F8 "Vars"}

#define BUTTONESC FIELD string yellow accessible center FNKEY  _fk_esc   NAME ESC BOX
#define BUTTONF1  FIELD string yellow accessible center FNKEY  _fk_F1    NAME F1  BOX
#define BUTTONF2  FIELD string yellow accessible center FNKEY  _fk_F2    NAME F2  BOX
#define BUTTONF3  FIELD string yellow accessible center FNKEY  _fk_F3    NAME F3  BOX
#define BUTTONF4  FIELD string yellow accessible center FNKEY  _fk_F4    NAME F4  BOX
#define BUTTONF5  FIELD string yellow accessible center FNKEY  _fk_F5    NAME F5  BOX
#define BUTTONF6  FIELD string yellow accessible center FNKEY  _fk_F6    NAME F6  BOX
#define BUTTONF7  FIELD string yellow accessible center FNKEY  _fk_F7    NAME F7  BOX
#define BUTTONF8  FIELD string yellow accessible center FNKEY  _fk_F8    NAME F8  BOX
#define BUTTONF9  FIELD string yellow accessible center FNKEY  _fk_F9    NAME F9  BOX
#define BUTTONF10 FIELD string yellow accessible center FNKEY  _fk_F10   NAME F10 BOX

#define TITBOX FIELD string name titbox output center blue
#define PGHEAD Center back cyan PROFILE
#define PGHEADNPR Center back cyan
#define FLDBOX BOX blue
#define FLDNBOX blue


/* INFO PAGE */
/* ========= */
GLOBAL CLOSEBOX  NO

PAGE odep_info {
    Center back cyan
/*    Display_fn {
	C_FN {{
	    extern int ODE_BANNER;
	    if(ODE_BANNER == 0) return;
	}}
    }
*/
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ       P L A N N I N G    O F F I C E    B E L G I U M          บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
บ ฺฤฤฤฤÜ    ฺฤฤฤฤÜ    ฺฤฤฤÜ     ฺฤฤฤฤÜ                           บ
บ ภฟ  ÜÛ    ณ ฺฟ Û    ณ ฺฟÛ     ณ ฤÜÜÛ      M O D E L L I N G    บ
บ  ณ  Û     ณ ณณ Û    ณ ณณ Û    ณ Ü฿         S O F T W A R E     บ
บ ฺู  ฿Ü    ณ ภู Û    ณ ภูÜÛ    ณ ฿ฤฤÜ    Version 6.63 - 09/2020 บ
บ ภฤÜÜÜÛ    ภÜÜÜÜÛ    ภÜÜÜÛ     ภÜÜÜÜÛ                           บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
บ                 Developped by ณ Geert Bryon                    บ
บ                               ณ Jean-Marc Paul                 บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
บ                         Tools ณ SCR/AL1(c) 1986-2020, JM.B.Paulบ
บ                      Web site ณ iode.plan.be                   บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤมฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
บ                                                                บ
บ                         &F10                                   บ
บ                                                                บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
}
    $BUTTONF10 "  Continue  "
}

GLOBAL CLOSEBOX YES
/* COMMENTS */
/* ======== */

COMMENT {
    line 29 col 0 nc 150 yellow center
}


/* LOCK SCREEN */
/* =========== */

ACTION ode_lock {
     C_FN SCR_lock_screen("I.O.D.E. 6.63");
}








