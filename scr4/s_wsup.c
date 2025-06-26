#include "scr4w.h"

#ifndef GENPROTO
/* DIRS
SCRSTAT *SCR_dir_edit(
char    *path,
char    *dir,
char    *file,
int     line,
int     col,
int     nl,
int     attr,
int     (*fn)()
)
{
    return((SCRSTAT *)0);
}
Programmé JMP 12-05-00 */

/* SCREEN and KEYB */
SCR_init_term() {return(0);}        /* JMP 05-05-00 */
SCR_end_term() {return(0);}         /* JMP 05-05-00 */
SCR_clear_screen() {return(0);}     /* JMP 05-05-00 */


SCR_lock_screen(U_ch * txt) {return(0);}
SCR_refresh()     {return(0);}
STATUS *SCR_save_status() {return((STATUS *)0);}
SCR_reset_status(STATUS *sta){return(0);}
SCR_cls()         {return(0);}
SCR_cursor_move(int line, int col) {return(0);}
SCR_write_text(int line, int col, U_ch *txt, int len)  {return(0);} /* Sous Windows, toujours write_im() */
SCR_map_box_2()   {return(0);}

/* ACTIONS, SYSTEM et ALARMS */
AB_edit(A_BAR *ab)           {return(0);}
AB_display(A_BAR *ab)        {return(0);}
PG_reedit(PAGE *pg)         {return(0);}
SCR_restart_alarm() {return(0);}
SCR_set_alarm(int sec, void  (*fn)()) {return(0);}
SCR_base_init()     {return(0);}
//sleep()             {return(0);}

#endif /* GENPROTO */

int CNF_REVERSE_TITLE = 0;

AP_undisplay       (APPL *ap) {return(0);}
AP_edit            (APPL *ap) {return(0);}
AP_display         (APPL *ap) {return(0);}

/* PG_display_slide(PAGE *pg)  {return(0);} */ /* BP_M 28-10-1999 */

PG_edit_field(PAGE *pg, int fld_nb)     {return(0);}

SCR_term_vkey(int val)     {return(0);}
SCR_app_key()       {return(0);}


/* STD* */
STD_doscpout(char *from, char *to, int unit, int type) {return(0);}
STD_subshell() {return(0);}
STD_dosformat(int unit, int type) {return(0);}
STD_doscpin(char *from, char *to, int unit, int type) {return(0);}

/* CONFIG */
SCR_config_screen() {return(0);}

/* S_DOS */
SCR_cursor_off() {return(0);}
SCR_cursor_on() {return(0);}

/* MISC */
/*system() {return(0);}*/
DT_calendar(long date, int bline, int bcol) {return(0);}
SCR_print_screen() {return(0);}

SCR_stop_alarm() {return(0);}


/* TAR */
#ifndef GENPROTO
AR_add(char *arname, char **files, int n) {return(0);}
AR_extract(char *arname, char **files, int n, int (*fn)()) {return(0);}
TarXtract(int a,char *b,char **c,int (*fn)()) {return(0);}
TarCopy(int a,char *b,char **c,int (*fn)()) {return(0);}
char *TarError() {return("Not imlemented");}
#endif

/* POUR MMT */
U_ch   *MMT_HLPF10 = 0;


/********* A l'étude .... **********/

SCR_mini_init()     {return(0);}
/*SCR_mini_end()    {return(0);} /* JMP 13-09-97 */
SCR_end()           {return(0);}
ACT_exec_system(char *cmd)   {return(0);}
